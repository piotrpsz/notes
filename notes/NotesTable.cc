// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.#pragma once
//
// Created by piotr on 09.04.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "NotesTable.hh"
#include "DeleteNoteDialog.hh"
#include "TreeDialog.hh"
#include "../model/note.hh"
#include "../model/category.hh"
#include "../common/EventController.hh"
#include <QTableWidgetItem>
#include <QDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <fmt/core.h>

NotesTable::NotesTable(QWidget* const parent) :
        QTableWidget(parent)
{
    setRowCount(0);
    setColumnCount(COLUMN_COUNT);
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Description"));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Category"));

    EventController::instance().append(this,
                                       event::CategorySelected,
                                       event::RemoveCurrentNoteRequest,
                                       event::MoveCurrentNoteRequest,
                                       event::NoteDatabaseChanged);


    // Użytkownik wybrał nowy wiersz.
    connect(this, &QTableWidget::currentItemChanged, [&] (QTableWidgetItem* const currentItem, auto){
        if (currentItem) {
            if (auto noteID = currentItem->data(NoteID); noteID.canConvert<int>())
                EventController::instance().send(event::NoteSelected, noteID.toInt());
        }
    });
    // Użytkownik dwa razy kliknął myszką wiersz.
    connect(this, &QTableWidget::cellDoubleClicked, [&] (auto const row, auto const col){
        if (auto selected_item = item(row, 0); selected_item) {
            auto noteID = selected_item->data(NoteID).toInt();
            EventController::instance().send(event::EditNoteRequest, noteID);
        }
    });
}

NotesTable::~NotesTable() {
    EventController::instance().remove(this);
}

void NotesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            clearContent();
            if (auto data = e->data(); not data.empty()) {
                auto const categoryID{data[0].toInt()};
                updateContentForCategoryWithID(categoryID);
                if (data.size() == 2) {
                    auto const noteID{data[1].toInt()};
                    if (auto item = rowWithID(noteID); item)
                        setCurrentItem(item);
                }
                else
                    selectRow(0);
            }
            break;
        case event::NoteDatabaseChanged:
            clearContent();
            if (auto data = e->data(); data.size() == 2) {
                updateContentForCategoryWithID(data[0].toInt());
                auto noteID = data[1].toInt();
                if (auto item = rowWithID(noteID); item)
                    setCurrentItem(item);
            }
            break;
        case event::RemoveCurrentNoteRequest:
            if (auto current_item = item(currentRow(), 0); current_item) {
                auto noteID = current_item->data(NoteID).toInt();
                deleteNoteWithID(noteID);
            }
            break;
        case event::MoveCurrentNoteRequest:
            if (auto item = currentItemWhenFocus(); item) {
                if (auto data = dataFromItem(item); data) {
                    auto [categoryID, noteID] = *data;
                    auto dialog = new TreeDialog(categoryID);
                    if (dialog->exec() == QDialog::Accepted) {
                        moveNoteToCategoryWithID(noteID, dialog->selectedCategoryID());
                    }
                }
            }
            break;
    }
}


/// Wyświetlamy wszystkie notatki dla wskazanej kategorii
/// oraz wszystkich jej podkategorii (jeśli istnieją).
void NotesTable::
updateContentForCategoryWithID(i64 const categoryID) noexcept {
    // Usunięcie wszystkich wierszy w tabeli.
    clearContent();

    if (auto ids = Category::idsSubchainFor(categoryID); not ids.empty()) {
        if (auto notes = Note::notes(std::move(ids)); not notes.empty()) {
            setRowCount(int(notes.size()));
            auto row = 0;
            for (auto const& note : notes) {
                auto const item0 = new QTableWidgetItem(note.qtitle());
                setItem(row, 0, item0);
                item0->setData(NoteID, note.id<qi64>());
                item0->setData(CategoryID, note.pid<qi64>());

                auto const item1 = new QTableWidgetItem(note.qdescription());
                setItem(row, 1, item1);

                auto const item2 = new QTableWidgetItem(note.qcategory());
                setItem(row, 2, item2);

                ++row;
            }
        }
    }
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    resizeColumnToContents(1);
    resizeColumnToContents(2);
//    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    categoryID_ = categoryID;

    update();
}

QTableWidgetItem* NotesTable::
rowWithID(qint64 id) const noexcept {
    int const rows = model()->rowCount();
    for (int i = 0; i < rows; ++i) {
        auto const row = item(i, 0);
        if (row->data(NoteID).toInt() == id)
            return row;
    }
    return nullptr;
}

void NotesTable::
deleteNoteWithID(qi64 const noteID) noexcept {
    if (auto note = Note::withID(noteID); note) {
        auto dialog = std::make_unique<DeleteNoteDialog>(*note, this);
        if (dialog->exec() == QDialog::Accepted) {
            auto row_nr = currentRow();
            if (Note::remove(noteID)) {
                updateContentForCategoryWithID(categoryID_);
                // Wybieramy wiersz o takim samym indeksie jeśli jest taki.
                // Lub ostatni wiersz.
                if (row_nr >= rowCount())
                    row_nr = rowCount() - 1;
                selectRow(row_nr);
            }
        }
    }
}

void NotesTable::
moveNoteToCategoryWithID(i64 noteID, i64 destinationCategoryID) noexcept {
    if (auto note = Note::withID(noteID); note) {
        // Nowa kategoria nie może już zawierać notatki o takim samym tytule.
        if (Note::containsParentTheNoteWithTitle(destinationCategoryID, (*note).title())) {
            QMessageBox::critical(this, "Illegal note title.", "The selected category already contains a note with this title.");
            return;
        }
        // Błąd zapisu do bazy danych.
        note->pid(destinationCategoryID);
        if (not note->save()) {
            QMessageBox::critical(this, "Database error", "Error updating note in database.");
            return;
        }
        EventController::instance().send(event::CategoryAndNoteToSelect,
                                         note->pid<qi64>(),
                                         note->id<qi64>());
    }
}

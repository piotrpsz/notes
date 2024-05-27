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
//

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
#include <fmt/core.h>

NotesTable::NotesTable(QWidget* const parent) :
        QTableWidget(parent)
{
    setRowCount(0);
    setColumnCount(2);
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);

    setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Description"));
//    horizontalHeader()->setStretchLastSection(false);

    EventController::instance().append(this,
                                       event::CategorySelected,
                                       event::RemoveCurrentNoteRequest,
                                       event::MoveCurrentNoteRequest,
                                       event::NoteDatabaseChanged);

    connect(this, &QTableWidget::currentItemChanged, [&] (auto current, auto){
        if (current) {
            auto noteID = current->data(NoteID).toInt();
            EventController::instance().send(event::NoteSelected, noteID);
        }
    });
    connect(this, &QTableWidget::cellDoubleClicked, [&] (auto row, auto col){
        if (auto selected_item = item(row, 0); selected_item) {
            auto noteID = selected_item->data(NoteID).toInt();
            EventController::instance().send(event::EditNoteRequest, noteID);
        }
    });
}

void NotesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            clear_content();
            if (auto data = e->data(); not data.empty()) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    update_content_for(value.toInt());
                    auto m = model();
                    selectRow(0);
                }
            }
            break;
        case event::NoteDatabaseChanged:
            clear_content();
            if (auto data = e->data(); data.size() == 2) {
                update_content_for(data[0].toInt());
                auto noteID = data[1].toInt();
                if (auto item = row_with_id(noteID); item)
                    setCurrentItem(item);
            }
            break;
        case event::RemoveCurrentNoteRequest:
            if (auto current_item = item(currentRow(), 0); current_item) {
                auto noteID = current_item->data(NoteID).toInt();
                delete_note(noteID);
            }
            break;
        case event::MoveCurrentNoteRequest:
            if (hasFocus()) {
                if (auto current_item = item(currentRow(), 0); current_item) {
                    if (auto ids = ids_from(current_item); ids) {
                        auto [categoryID, noteID] = *ids;
                        auto dialog = new TreeDialog(categoryID);
                        if (dialog->exec() == QDialog::Accepted) {

                        }
                    }
                }
            }
            break;
    }
}


/// Wyświetlamy wszystkie notatki dla wskazanej kategorii
/// oraz wszystkich jej podkategorii (jeśli istnieją).
void NotesTable::
update_content_for(i64 const category_id) noexcept {
    // Usunięcie wszystkich wierszy w tabeli.
    clear_content();

    if (auto ids = Category::ids_subchain_for(category_id); not ids.empty()) {
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
                ++row;
            }
        }
    }
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    resizeColumnToContents(1);
//    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    categoryID_ = category_id;

    update();
}

QTableWidgetItem* NotesTable::
row_with_id(qint64 id) const noexcept {
    int const rows = model()->rowCount();
    for (int i = 0; i < rows; ++i) {
        auto const row = item(i, 0);
        if (row->data(NoteID).toInt() == id)
            return row;
    }
    return nullptr;
}

void NotesTable::
delete_note(qi64 const noteID) noexcept {
    if (auto note = Note::with_id(noteID); note) {
        auto dialog = std::make_unique<DeleteNoteDialog>(*note, this);
        if (dialog->exec() == QDialog::Accepted) {
            auto row_nr = currentRow();
            if (Note::remove(noteID)) {
                update_content_for(categoryID_);
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
move_note(i64 noteID, i64 destinationCategoryID) const noexcept {

}

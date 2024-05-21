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
#include "../model/note.hh"
#include "../model/category.hh"
#include "../common/EventController.hh"
#include <QTableWidgetItem>
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
                                       event::NoteDatabaseChanged);

    connect(this, &QTableWidget::itemSelectionChanged, [&]{
        if (auto current_item = item(currentRow(), 0); current_item) {
            auto noteID = current_item->data(NoteID).toInt();
            EventController::instance().send(event::NoteSelected, noteID);
        }
    });
}

void NotesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            if (auto data = e->data(); not data.empty()) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    update_content_for(value.toInt());
                    auto m = model();
                    selectRow(0);
                }
            }
            break;
        case event::NoteDatabaseChanged:
            if (auto data = e->data(); data.size() == 2) {
                update_content_for(data[0].toInt());
                auto noteID = data[1].toInt();
                if (auto item = row_with_id(noteID); item)
                    setCurrentItem(item);
            }

    }
}


void NotesTable::update_content_for(i64 const category_id) noexcept {
    setRowCount(0);
    setColumnCount(2);

    if (auto ids = Category::ids_subchain_for(category_id); not ids.empty()) {
        if (auto notes = Note::notes(std::move(ids)); not notes.empty()) {
            setRowCount(int(notes.size()));
            auto row = 0;
            for (auto const& note : notes) {
                auto const item0 = new QTableWidgetItem(note.qtitle());
                auto const item1 = new QTableWidgetItem(note.qdescription());
                setItem(row, 0, item0);
                item0->setData(NoteID, note.id<qi64>());
                item0->setData(CategoryID, note.pid<qi64>());

                setItem(row, 1, item1);
                ++row;
            }
        }
    }
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
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

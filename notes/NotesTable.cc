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
#include "NotesTable.h"
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
    horizontalHeader()->setStretchLastSection(true);

    EventController::instance().append(this, event::CategorySelected);
}

void NotesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            if (auto data = e->data(); not data.empty()) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    update_content_for(value.toInt());
                }
            }
            break;
    }
}

void NotesTable::update_content_for(i64 const id) noexcept {
    setRowCount(0);
    setColumnCount(2);

    if (auto ids = Category::ids_subchain_for(id); not ids.empty()) {
        if (auto notes = Note::notes(std::move(ids)); not notes.empty()) {
            setRowCount(int(notes.size()));
        }
    }
    update();


//    if (ids.empty()) {
//        setRowCount(0);
//        setColumnCount(2);
//        setEditTriggers(NoEditTriggers);
//        setSelectionBehavior(SelectRows);
//        update();
//    }


//    for (auto const& id : ids)
//        fmt::print("{}\n", id);
//    fmt::print("--------------------------------\n");
}
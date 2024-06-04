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
// Created by Piotr Pszczółkowski 09.04.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "NotesWorkspace.hh"
#include "NotesTableWidget.hh"
#include "NoteWidget.hh"
#include "Settings.hh"
#include "../common/EventController.hh"
#include "EditDialog.hh"
#include <QVariant>
#include <QShowEvent>
#include <memory>
#include <fmt/core.h>

NotesWorkspace::NotesWorkspace(QWidget *const parent) : QSplitter(Qt::Vertical, parent) {
    setHandleWidth(SPLITTER_HANDLE_WIDTH);
    addWidget(new NotesTableWidget);
    addWidget(new NoteWidget);

    EventController::instance().append(this,
                                       event::NewNoteRequest,
                                       event::EditNoteRequest);
}

NotesWorkspace::~NotesWorkspace() {
    Settings sts;
    auto data = sizes();
    sts.save(SizesH0Key, data[0]);
    sts.save(SizesH1Key, data[1]);
}

void NotesWorkspace::showEvent(QShowEvent *const event) {
    QSplitter::showEvent(event);
    if (not firstTimeShow_) return;

    firstTimeShow_ = false;
    auto const s = size();
    auto h0 = int(s.width() * PERCENTAGE_WIDTH);
    auto h1 = int(s.width() - h0 - handleWidth());

    Settings sts;
    if (auto data = sts.read(SizesH0Key); data)
        h0 = data.value().toInt();
    if (auto data = sts.read(SizesH1Key); data)
        h1 = data.value().toInt();

    setSizes({h0, h1});
}

void NotesWorkspace::customEvent(QEvent* const event) {
    event->accept();
    auto const e = dynamic_cast<Event*>(event);

    switch (static_cast<int>(e->type())) {
        case event::NewNoteRequest:
            if (auto data = e->data(); data.size() == 1)
                newNoteForCategoryWithID(data[0].toInt());
            break;
        case event::EditNoteRequest:
            if (auto data = e->data(); data.size() == 1)
                editNoteWithID(data[0].toInt());
            break;
    }
}

void NotesWorkspace::newNoteForCategoryWithID(qi64 const categoryID) noexcept {
    auto const dialog = std::make_unique<EditDialog>(categoryID);
    if (dialog->exec() == QDialog::Accepted) {
        EventController::instance().send(event::NoteDatabaseChanged,
                                         categoryID,
                                         dialog->note_id<qi64>());
    }
}

void NotesWorkspace::editNoteWithID(qi64 const noteID) noexcept {
    if (auto note = Note::withID(noteID); note) {
        auto const dialog = std::make_unique<EditDialog>(std::move(*note));
        if (dialog->exec() == QDialog::Accepted) {
            auto updatedNote = dialog->get_note();
            EventController::instance().send(event::NoteDatabaseChanged,
                                             updatedNote.pid<qi64>(),
                                             noteID);
        }
    }
}

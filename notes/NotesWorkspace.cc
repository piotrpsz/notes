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
#include "NotesWorkspace.h"
#include "NotesTableWidget.h"
#include "NoteWidget.h"
#include "Settings.h"
#include "../common/EventController.hh"
#include "EditDialog.hh"
#include <QVariant>
#include <QShowEvent>
#include <memory>
#include <fmt/core.h>

NotesWorkspace::NotesWorkspace(QWidget *const parent) : QSplitter(Qt::Vertical, parent) {
    setHandleWidth(1);
    addWidget(new NotesTableWidget);
    addWidget(new NoteWidget);

    EventController::instance().append(this,
                                       event::NewNoteRequest,
                                       event::EditNoteRequest,
                                       event::RemoveNoteRequest);
}

NotesWorkspace::~NotesWorkspace() {
    Settings sts;
    auto data = sizes();
    sts.save(SizesH0Key, data[0]);
    sts.save(SizesH1Key, data[1]);
}

void NotesWorkspace::showEvent(QShowEvent *const event) {
    QSplitter::showEvent(event);
    if (not first_show_) return;

    first_show_ = false;
    auto const s = size();
    auto h0 = int(60. * s.width() / 100.);
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
        case event::NewNoteRequest: {
            if (auto args = e->data(); not args.empty()) {
                if (auto value = args[0]; value.canConvert<qi64>())
                    new_note(value.toInt());
            }
            break;
        }

    }
}

void NotesWorkspace::new_note(qi64 const category_id) noexcept {
    auto dialog = std::make_unique<EditDialog>(category_id);
    if (dialog->exec() == QDialog::Accepted) {
        std::integral auto note_id = dialog->note_id<qi64>();
        EventController::instance().send(event::NoteDatabaseChanged, category_id, note_id);
    }
}

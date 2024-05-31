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
// Created by Piotr Pszczółkowski on 21.05.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "Browser.hh"
#include "../common/EventController.hh"
#include "../model/note.hh"
#include <QEvent>
#include <fmt/core.h>

Browser::Browser(QWidget* const parent) : QTextEdit(parent) {
    setAcceptRichText(true);
    setReadOnly(true);

    // ustawienie odstępu pomiędzy liniami
    auto block_fmt = textCursor().blockFormat();
    block_fmt.setLineHeight(5, QTextBlockFormat::LineDistanceHeight);
    textCursor().setBlockFormat(block_fmt);

    // ustawinie długości TAB
    setTabStopDistance(5 * fontMetrics().horizontalAdvance('-') - 2.0);

    EventController::instance().append(this,
                                       event::NoteSelected,
                                       event::CategorySelected);
}

Browser::~Browser() {
    EventController::instance().remove(this);
}

void Browser::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
        case event::NoteSelected:
            if (auto data = e->data(); data.size() == 1) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    auto noteID = value.toInt();
                    if (auto note = Note::withID(noteID); note) {
                        auto html = (*note).qcontent();
                        setHtml(html);
                    }
                }
            }
            break;
        case event::CategorySelected:
            setPlainText("");
            break;
    }
}

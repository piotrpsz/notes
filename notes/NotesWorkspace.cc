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
#include "NotesTable.h"
#include "NoteWidget.h"
#include "Settings.h"
#include <QVariant>
#include <QShowEvent>
#include <fmt/core.h>

NotesWorkspace::NotesWorkspace(QWidget *const parent) : QSplitter(Qt::Vertical, parent) {
    setHandleWidth(1);
    addWidget(new NotesTable);
    addWidget(new NoteWidget);
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
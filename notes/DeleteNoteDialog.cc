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
#include "DeleteNoteDialog.hh"
#include "../model/note.hh"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

DeleteNoteDialog::DeleteNoteDialog(Note const& note, QWidget *const parent) :
        QDialog(parent)
{
    setWindowTitle("Remove Note");

    auto const buttons{new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No)};
    auto const yes_button = buttons->button(QDialogButtonBox::Yes);
    auto const no_button = buttons->button(QDialogButtonBox::No);
    connect(yes_button, &QPushButton::clicked, [&] { accept(); });
    connect(no_button, &QPushButton::clicked, [&] { reject(); });

    auto const layout{new QVBoxLayout};
    layout->addWidget(new QLabel("Would you like to remove selected note?"));
    layout->addWidget(buttons);
    setLayout(layout);
}

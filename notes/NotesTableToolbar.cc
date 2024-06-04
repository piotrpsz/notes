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
// Created by Piotr Pszczółkowski on 27.04.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "NotesTableToolbar.hh"
#include "Tools.hh"
#include "../common/EventController.hh"
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <fmt/core.h>


NotesTableToolbar::NotesTableToolbar(QWidget* const parent) :
    QToolBar(parent),
    categoryChain_{},
    categoryChainLabel_{new QLabel}
{
    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    setIconSize(QSize(16, 16));

    auto const moveAction = new QAction(QIcon::fromTheme("folder-new"), "Move");
    auto const newAction = new QAction(QIcon::fromTheme("list-add"), "New");
    auto const edtAction = new QAction(QIcon::fromTheme("accessories-text-editor"), "Edit");
    auto const delAction = new QAction(QIcon::fromTheme("list-remove"), "Delete");

    newAction->setToolTip("Create new note");
    edtAction->setToolTip("Edit selected note");
    delAction->setToolTip("Remove selected note");
    moveAction->setToolTip("Move selected note to another category");

    addWidget(categoryChainLabel_);

    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    addAction(newAction);
    addAction(edtAction);
    addAction(delAction);
    addAction(moveAction);

    // Użytkownik kliknął przycisk 'Add'.
    connect(newAction, &QAction::triggered, [this]() {
        if (currentCategoryID_ > 0)
            EventController::instance().send(event::NewNoteRequest, qint64(currentCategoryID_));
    });
    // Użytkownik kliknął przycisk 'Edit'.
    connect(edtAction, &QAction::triggered, []() {
        EventController::instance().send(event::EditNoteRequest);
    });
    // Użytkownik kliknął przycisk 'Delete'.
    connect(delAction, &QAction::triggered, []() {
        EventController::instance().send(event::RemoveCurrentNoteRequest);
    });
    // Użytkownik kliknął przycisk 'Move'.
    connect(moveAction, &QAction::triggered, [] {
        EventController::instance().send(event::MoveCurrentNoteRequest);
    });

    EventController::instance().append(this, event::CategorySelected);
}

void NotesTableToolbar::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            if (auto data = e->data(); not data.empty()) {
                currentCategoryID_ = data[0].toInt();
                categoryChain_ = Tools::categoriesChainInfo(currentCategoryID_);
                categoryChainLabel_->setText(qstr::fromStdString(*categoryChain_));
            }
            break;
    }
}

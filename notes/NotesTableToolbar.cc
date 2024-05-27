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
    category_chain_{},
    chain_info_{new QLabel}
{
    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    setIconSize(QSize(16, 16));

    auto const move_action = new QAction(QIcon::fromTheme("folder-new"), "Move");
    auto const add_action = new QAction(QIcon::fromTheme("list-add"), "Add");
    auto const edt_action = new QAction(QIcon::fromTheme("accessories-text-editor"), "Edit");
    auto const del_action = new QAction(QIcon::fromTheme("list-remove"), "Delete");

    add_action->setToolTip("Create new category note");
    edt_action->setToolTip("Edit selected note");
    del_action->setToolTip("Remove selected note");

    addWidget(chain_info_);

    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidget(spacer);

    addAction(add_action);
    addAction(edt_action);
    addAction(del_action);
    addAction(move_action);

    connect(add_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::NewNoteRequest, qint64(current_category_id_));
    });
    connect(edt_action, &QAction::triggered, []() {
        EventController::instance().send(event::EditNoteRequest);
    });
    connect(del_action, &QAction::triggered, []() {
        EventController::instance().send(event::RemoveCurrentNoteRequest);
    });
    connect(move_action, &QAction::triggered, [] {
        EventController::instance().send(event::MoveCurrentNoteRequest);
    });

    EventController::instance().append(this, event::CategorySelected);
}

void NotesTableToolbar::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategorySelected:
            if (auto data = e->data(); not data.empty()) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    current_category_id_ = value.toInt();
                    category_chain_ = Tools::chain_info(current_category_id_);
                    chain_info_->setText(qstr::fromStdString(*category_chain_));
                }
            }
            break;
    }
}

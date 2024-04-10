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
#include "CategoryTree.h"
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QDialog>
#include <QAction>
#include <QLineEdit>
#include <QHeaderView>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QDialogButtonBox>
#include <memory>
#include <fmt/core.h>

CategoryTree::CategoryTree(QWidget* const parent) :
        QTreeWidget(parent),
        root_{new QTreeWidgetItem(this)}
{
    root_->setText(0, "Categories");
    root_->setData(0, IdRole, 0);
    root_->setData(0, PidRole, 0);

    auto p = palette();
    p.setColor(QPalette::Base, QColor{60, 60, 60, 255});
    setAutoFillBackground(true);
    setPalette(p);

    header()->hide();
    setColumnCount(1);
    setHorizontalScrollMode(ScrollPerPixel);
}

void CategoryTree::mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton) {
        auto const action = new QAction("New main category");
        connect(action, &QAction::triggered, this, &CategoryTree::add_new_main_category);
        auto const menu = new QMenu(this);
        menu->addAction(action);
        menu->popup(viewport()->mapToGlobal(event->pos()));
    }
}

void CategoryTree::add_new_main_category() noexcept {
    if (auto opt = category_dialog(Category{}); opt) {
        auto [id, pid, name] = opt.value();
        auto const item = new QTreeWidgetItem(root_);
        item->setText(0,name);
        item->setData(0, IdRole, id);
        item->setData(0, PidRole, pid);
        root_->setExpanded(true);
    }
}

// Run the dialog for category edition.
std::optional<CategoryTree::Category> CategoryTree::category_dialog(Category&& category) noexcept {
    auto const parent_layout = new QHBoxLayout;
    if (category.pid == 0)
        parent_layout->addWidget(new QLabel("No parents, this will be the new main category."));

    auto const editor = new QLineEdit;
    editor->setText(category.name);

    auto const edit_layout = new QHBoxLayout;
    edit_layout->addWidget(new QLabel{"Category name:"});
    edit_layout->addWidget(editor);

    auto const separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto const dialog = std::make_unique<QDialog>();
    auto button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(button_box, &QDialogButtonBox::accepted, dialog.get(), &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, dialog.get(), &QDialog::reject);

    auto const main_layout = new QVBoxLayout;
    main_layout->addLayout(parent_layout);
    main_layout->addWidget(separator);
    main_layout->addSpacing(10);
    main_layout->addLayout(edit_layout);
    main_layout->addSpacing(10);
    main_layout->addWidget(button_box);
    dialog->setLayout(main_layout);
    dialog->setWindowTitle("Add new main category");

    if (dialog->exec() and not editor->text().isEmpty()) {
        category.name = std::move(editor->text());
        return std::move(category);
    }
    return {};
}

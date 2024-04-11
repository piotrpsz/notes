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
#include "../sqlite/sqlite.h"
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

    add_items_for(root_);
    root_->setExpanded(true);
}

// Handle mouse clicks.
void CategoryTree::mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton) {
        auto const new_action = new QAction("New");
        auto const edit_action = new QAction("Rename");
        auto const remove_action = new QAction("Delete");
        connect(new_action, &QAction::triggered, this, &CategoryTree::new_item);
        connect(edit_action, &QAction::triggered, this, &CategoryTree::edit_item);
        connect(remove_action, &QAction::triggered, this, &CategoryTree::remove_item);
        auto const menu = new QMenu(this);
        menu->addAction(new_action);
        menu->addSeparator();
        menu->addAction(edit_action);
        menu->addAction(remove_action);
        menu->popup(viewport()->mapToGlobal(event->pos()));
        event->accept();
    }
    QTreeWidget::mousePressEvent(event);
}

/// Add new category/subcategory.
void CategoryTree::new_item() noexcept {
    Category category{};

    // Get current selected item (parent item).
    QTreeWidgetItem* current_item = currentItem();
    if (current_item) {
        category.id = current_item->data(0, IdRole).toInt();
        category.pid = current_item->data(0, PidRole).toInt();
        category.name = current_item->text(0);
    }

    if (auto opt = category_dialog(std::move(category)); opt) {
        auto [id, pid, name] = opt.value();
        id  = SQLite::instance().insert("INSERT INTO category (pid, name) VALUES (?,?)", pid, name.toStdString());
        if (id not_eq -1) {
            auto const item = new QTreeWidgetItem(current_item ? current_item : root_);
            item->setText(0, name);
            item->setData(0, IdRole, id);
            item->setData(0, PidRole, pid);
            setCurrentItem(item);
        }
    }
}

void CategoryTree::remove_item() noexcept {
    auto item = currentItem();
    if (item == nullptr) return;
    auto category= category_from(item);

    // TODO check if are notes for this category (and subcategories)
    static std::string const query = "DELETE FROM category WHERE id=?";
    if (auto ok = SQLite::instance().exec(query, category.id); ok) {
        item->parent()->removeChild(item);
        delete item;
    }
}

void CategoryTree::edit_item() noexcept {
    auto item = currentItem();
    if (item == nullptr) return;
    auto category= category_from(item);

}

// Converts text and data of QTreeWidgetItem to Category.
CategoryTree::Category CategoryTree::category_from(QTreeWidgetItem const* const item) noexcept {
    Category category{};
    if (auto v = item->data(0, IdRole); v.canConvert<int>())
        category.id = v.toInt();
    if (auto v = item->data(0, PidRole); v.canConvert<int>())
        category.pid = v.toInt();
    if (auto v = item->text(0); not v.isEmpty())
        category.name = v;
    return std::move(category);
}

// Run the dialog for category edition.
std::optional<CategoryTree::Category> CategoryTree::
category_dialog(Category&& category, bool const rename) noexcept {
    fmt::print("CategoryTree::category_dialog: {}, {}, {}\n",
               category.id,
               category.pid,
               category.name.toStdString());

    auto const parent_layout = new QHBoxLayout;
    if (category.id == 0)
        parent_layout->addWidget(new QLabel("No parents, this will be the new main category."));
    else {
        // TODO fetch all parent categories
        auto str = fmt::format("Categories > {}", category.name.toStdString());
        parent_layout->addWidget(new QLabel(qstr::fromStdString(str)));
    }

    auto const editor = new QLineEdit;
    if (rename)
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
    if (category.id == 0)
        dialog->setWindowTitle("Add new main category");
    else
        dialog->setWindowTitle("Add new subcategory");

    if (dialog->exec() and not editor->text().isEmpty()) {
        Category result{.name = editor->text()};
        if (not rename)
            result.pid = category.id;
        return std::move(result);
    }
    return {};
}

/// Read all children (with sub-children) for passed item.
void CategoryTree::add_items_for(QTreeWidgetItem* const parent) noexcept {
    static std::string const query = "SELECT * FROM category WHERE pid=? ORDER BY name";

    auto pid = parent->data(0, IdRole).toInt();
    if (auto opt = SQLite::instance().select(query, pid); opt)
        if (auto result = opt.value(); not result.empty())
            for (auto&& row : result) {
                auto const item = new QTreeWidgetItem(parent);
                item->setText(0, qstr::fromStdString(row["name"].value().value().str()));
                item->setData(0, IdRole, row["id"].value().value().isize());
                item->setData(0, PidRole, row["pid"].value().value().isize());
                add_items_for(item);
            }
}

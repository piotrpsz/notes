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
#include "../common/EventController.h"
#include "CategoryTree.h"
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QDialog>
#include <QAction>
#include <QLineEdit>
#include <QHeaderView>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QTreeWidgetItem>
#include <QDialogButtonBox>
#include <memory>
#include <string>
#include <fmt/core.h>

/*------- local constants:
-------------------------------------------------------------------*/
std::string const CategoryTree::InsertQuery{"INSERT INTO category (pid, name) VALUES (?,?)"};
std::string const CategoryTree::UpdateQuery{"UPDATE category SET name=? WHERE id=?"};
std::string const CategoryTree::DeleteQuery{"DELETE FROM category WHERE id=?"};
std::string const CategoryTree::CountQuery{"SELECT COUNT(*) as count FROM category WHERE pid=? AND name=?"};
std::string const CategoryTree::SubcategoriesCountQuery{"SELECT COUNT(*) as count FROM category WHERE pid=?"};

static char const* const RemoveTitle = "The category cannot be deleted.";
static char const* const RemoveMessage = "This category cannot be deleted because it has subcategories!";

/*------- forward declarations:
-------------------------------------------------------------------*/
std::optional<int> count(i64 pid, std::string const& name) noexcept;
std::optional<bool> has_subcategories(i64 id) noexcept;
bool acceptable(i64 pid, std::string const& name) noexcept;


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

    connect(this, &QTreeWidget::itemDoubleClicked, this, &CategoryTree::item_double_clicked);
}

// Handle mouse clicks.
void CategoryTree::mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton) {
        auto const item = itemAt(event->pos());
        auto const main_item = (item == nullptr) or (item == root_);
        // actions
        auto const new_action = new QAction(main_item ? "New main category" : "New subcategory");
        auto const edit_action = new QAction("Rename");
        auto const remove_action = new QAction("Delete");
        // connections
        if (main_item)
            connect(new_action, &QAction::triggered, this, &CategoryTree::new_main_category);
        else
            connect(new_action, &QAction::triggered, this, &CategoryTree::new_subcategory);
        connect(edit_action, &QAction::triggered, this, &CategoryTree::edit_item);
        connect(remove_action, &QAction::triggered, this, &CategoryTree::remove_category);
        // menu
        auto const menu = new QMenu(this);
        menu->addAction(new_action);
        menu->addSeparator();
        menu->addAction(edit_action);
        menu->addAction(remove_action);
        menu->popup(viewport()->mapToGlobal(event->pos()));
        // finish
        event->accept();
    }
    QTreeWidget::mousePressEvent(event);
}

void CategoryTree::item_double_clicked(QTreeWidgetItem* const item, int) noexcept {
    bool ok_id{}, ok_pid{};
    int const id = item->data(0, IdRole).toInt(&ok_id);
    int const pid = item->data(0, PidRole).toInt(&ok_pid);

    if (ok_id and ok_pid)
        EventController::instance().send(event::CategorySelected, id, pid);
}

/// Add new main category.
void CategoryTree::new_main_category() noexcept {
    if (auto category_opt = category_dialog(Category{}); category_opt) {
        auto new_category = std::move(*category_opt);
        auto [_, pid, qname] = new_category;
        auto name = qname.toStdString();

        if (acceptable(pid, name)) {
            auto const id = SQLite::instance().insert(InsertQuery, pid, name);
            if (id not_eq SQLite::InvalidRowid) {
                new_category.id = id;
                auto const item = child_from_category(root_, std::move(new_category));
                setCurrentItem(item);
            }
        }
    }
}

/// Add new subcategory.
void CategoryTree::new_subcategory() noexcept {
    // Get current selected item (parent item).
    QTreeWidgetItem* current_item = currentItem();
    auto category = category_from(current_item);

    if (auto category_opt = category_dialog(std::move(category)); category_opt) {
        auto new_category = std::move(*category_opt);
        auto [_, pid, qname] = new_category;
        auto name = qname.toStdString();

        if (acceptable(pid, name)) {
            auto const id = SQLite::instance().insert(InsertQuery, pid, name);
            if (id not_eq SQLite::InvalidRowid) {
                new_category.id = id;
                auto const item = child_from_category(current_item, std::move(new_category));
                setCurrentItem(item);
            }
        }
    }
}

/// Delete current category/item.
void CategoryTree::remove_category() noexcept {
    auto item = currentItem();
    if (item == nullptr) return;

    auto [id, pid, _] = category_from(item);
    if (auto ok = has_subcategories(id); ok and ok.value()) {
        QMessageBox::warning(QApplication::activeWindow(), RemoveTitle, RemoveMessage);
        return;
    }
    // TODO check if category contains notes

    if (auto ok = SQLite::instance().exec(DeleteQuery, id); ok) {
        item->parent()->removeChild(item);
        delete item;
    }
}

void CategoryTree::
edit_item() noexcept {
    if (auto item = currentItem(); item)
        if (auto category = category_dialog(category_from(item), true)) {
            auto [id, pid, qname] = category.value();
            auto name = qname.toStdString();

            if (acceptable(pid, name))
                if (SQLite::instance().update(UpdateQuery, name, id)) {
                    item->setText(0, qname);
                    item->parent()->sortChildren(0, Qt::AscendingOrder);
                }
        }
}

QTreeWidgetItem* CategoryTree::
child_from_category(QTreeWidgetItem* const parent, Category&& category) noexcept {
    auto const item = new QTreeWidgetItem(parent);
    item->setText(0, category.name);
    item->setData(0, IdRole, qint64(category.id));
    item->setData(0, PidRole, qint64(category.pid));
    // Since we added a new subcategory-item, we need to sort them again
    parent->sortChildren(0, Qt::AscendingOrder);
    return item;
}

// Converts text and data of QTreeWidgetItem to Category.
CategoryTree::Category CategoryTree::
category_from(QTreeWidgetItem const* const item) noexcept {
    Category category{};
    if (auto v = item->data(0, IdRole); v.isValid())
        category.id = v.toInt();
    if (auto v = item->data(0, PidRole); v.isValid())
        category.pid = v.toInt();
    if (auto v = item->text(0); not v.isEmpty())
        category.name = v;
    return category;
}

// Run the dialog for category edition.
std::optional<CategoryTree::Category> CategoryTree::
category_dialog(Category&& category, bool const rename) noexcept {
    auto const parent_layout = new QHBoxLayout;
    if (category.id == 0)
        parent_layout->addWidget(new QLabel("No parents, this will be the new main category."));
    else {
        // TODO fetch all parent categories
        auto str = fmt::format("Categories > {}", category.name.toStdString());
        parent_layout->addWidget(new QLabel(qstr::fromStdString(str)));
    }

    auto const editor = new QLineEdit;
    if (rename) {
        editor->setText(category.name);
        editor->selectAll();
    }

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
        if (rename) {
            result.id = category.id;
            result.pid = category.pid;
        }
        else
            result.pid = category.id;
        return result;
    }
    return {};
}

/// Read all children (with sub-children) for passed item.
void CategoryTree::
add_items_for(QTreeWidgetItem* const parent) noexcept {
    static std::string const query = "SELECT * FROM category WHERE pid=? ORDER BY name";

    auto pid = parent->data(0, IdRole).toInt();
    if (auto result_opt = SQLite::instance().select(query, pid); result_opt)
        if (auto result = *result_opt; not result.empty())
            for (auto&& row : result) {
                auto const item = new QTreeWidgetItem(parent);
                item->setText(0, qstr::fromStdString(row["name"].value().value().str()));
                item->setData(0, IdRole, row["id"].value().value().isize());
                item->setData(0, PidRole, row["pid"].value().value().isize());
                add_items_for(item);
            }
}

bool acceptable(i64 const pid, std::string const& name) noexcept {
    auto n = count(pid, name);

    if (not n.has_value()) {
        QMessageBox::critical(QApplication::activeWindow(), "Database Error", "Database operation error.");
        return {};     // Database error
    }

    if (n.value() not_eq 0) {
        // Category with the name already exists
        auto msg = fmt::format("The subcategory '{}' already exists!", name);
        QMessageBox::warning(QApplication::activeWindow(), "Unacceptable category name", QString::fromStdString(msg));
        return {};
    }

    return true;
}

std::optional<int> count(i64 pid, std::string const& name) noexcept {
    if (auto oresult  = SQLite::instance().select(CategoryTree::CountQuery, pid, name); oresult)
        if (auto result = *oresult; not result.empty())
            if (auto ocount = result[0]["count"]; ocount)
                return ocount.value().value().int64();
    return {};
}

std::optional<bool> has_subcategories(i64 const id) noexcept {
    if (auto result = SQLite::instance().select(CategoryTree::SubcategoriesCountQuery, id); result and not result.value().empty())
        if (auto count = result.value()[0]["count"]; count)
            return count.value().value().int64() not_eq 0;

    return {};
}

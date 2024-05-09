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
#include "../sqlite/sqlite.hh"
#include "../common/EventController.hh"
#include "CategoryTree.h"
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QTimer>
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
        root_{new QTreeWidgetItem(this)},
        timer_{new QTimer(this)},
        story_{new StoreCategory(this)}
{
    story_->print();

    // Konfiguracja timera.
    // Timer jest nadawcą zdarzenia z informacją, że ulegla zmianie wybrana kategoria.
    // Ale timer wyśle zdarzenie(event) jeśli od zmiany minie 500 milisekund (pół sekundy),
    // zapobiega to zbyt szybkim i częstym uaktualnieniom tabeli z notatkami.
    timer_->setInterval(500);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, [this]() {
        if (auto current_item = currentItem(); current_item) {
            auto const id{current_item->data(0, IdRole).toInt()};
            EventController::instance().send(event::CategorySelected, id);
        }
    });

    // Ustawiamy kategorię 'root', która jest rodzicem
    // wszystkich innych kategorii.
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
    connect(this, &QTreeWidget::currentItemChanged, [this](auto, auto) {
        timer_->stop();
        timer_->start();
    });
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
    bool ok_id{};
    int const id = item->data(0, IdRole).toInt(&ok_id);

    if (ok_id)
        EventController::instance().send(event::CategorySelected, id);
}


/// Add new main category.
void CategoryTree::new_main_category() noexcept {
    if (auto opt = category_dialog(Category{}); opt) {
        auto category = opt.value();

        if (acceptable(category.pid(), category.name())) {
            auto const id = SQLite::instance().insert(InsertQuery, category.pid(), category.name());
            if (id not_eq SQLite::InvalidRowid) {
                category.id(id);
                auto const item = child_from_category(root_, std::move(category));
                setCurrentItem(item);
            }
        }
    }
}

/// Add new subcategory.
void CategoryTree::new_subcategory() noexcept {
    // Get current selected item (parent item).
    QTreeWidgetItem* current_item = currentItem();
    auto oryginal_category = category_from(current_item);

    if (auto opt = category_dialog(std::move(oryginal_category)); opt) {
        auto category = opt.value();

        if (acceptable(category.pid(), category.name())) {
            auto const id = SQLite::instance().insert(InsertQuery, category.pid(), category.name());
            if (id not_eq SQLite::InvalidRowid) {
                category.id(id);
                auto const item = child_from_category(current_item, std::move(category));
                setCurrentItem(item);
            }
        }
    }
}

// Usunięcie aktualnej kategorii.
void CategoryTree::remove_category() noexcept {
    if (auto item = currentItem(); item) {
        auto category = category_from(item);

        // Jeśli kategoria zawiera pod-kategorje prosimy użytkownika
        // o potwierdzenie czy rzeczywiście tego chce.
        if (auto ok = has_subcategories(category.id()); ok and ok.value()) {
            QMessageBox::warning(QApplication::activeWindow(), RemoveTitle, RemoveMessage);
            return;
        }
        // TODO check if category contains notes

        if (auto ok = SQLite::instance().exec(DeleteQuery, category.id()); ok) {
            item->parent()->removeChild(item);
            delete item;
        }
    }
}

// Edycja aktualnej kategorii.
void CategoryTree::
edit_item() noexcept {
    if (auto item = currentItem(); item)
        if (auto opt = category_dialog(category_from(item), true); opt) {
            auto category = opt.value();
            if (acceptable(category.pid(), category.name()))
                if (SQLite::instance().update(UpdateQuery, category.name(), category.id())) {
                    item->setText(0, category.qname());
                    item->parent()->sortChildren(0, Qt::AscendingOrder);
                }
        }
}

QTreeWidgetItem* CategoryTree::
child_from_category(QTreeWidgetItem* const parent, Category&& category) noexcept {
    auto const item = new QTreeWidgetItem(parent);
    item->setText(0, category.qname());
    item->setData(0, IdRole, category.qid());
    item->setData(0, PidRole, category.qpid());
    // Ponieważ dodaliśmy nową pod-kategorię to kategorię-rodzica musimy posortować.
    parent->sortChildren(0, Qt::AscendingOrder);
    return item;
}

// Converts text and data of QTreeWidgetItem to Category.
Category CategoryTree::
category_from(QTreeWidgetItem const* const item) noexcept {
    Category category{};
    if (auto v = item->data(0, IdRole); v.isValid())
        category.id(v.toInt());
    if (auto v = item->data(0, PidRole); v.isValid())
        category.pid(v.toInt());
    if (auto v = item->text(0); not v.isEmpty())
        category.name(v);
    return category;
}

// Run the dialog for category edition.
std::optional<Category> CategoryTree::
category_dialog(Category&& category, bool const rename) noexcept {
    auto const parent_layout = new QHBoxLayout;
    if (category.id() == 0)
        parent_layout->addWidget(new QLabel("No parents, this will be the new main category."));
    else {
        // TODO fetch all parent categories
        auto str = fmt::format("Categories > {}", category.name());
        parent_layout->addWidget(new QLabel(qstr::fromStdString(str)));
    }

    auto const editor = new QLineEdit;
    if (rename) {
        editor->setText(category.qname());
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
    if (category.id() == 0)
        dialog->setWindowTitle("Add new main category");
    else
        dialog->setWindowTitle("Add new subcategory");

    if (dialog->exec() and not editor->text().isEmpty()) {
        Category result{};
        result.name(editor->text());
        if (rename) result.id(category.id()).pid(category.pid());
        else result.pid(category.id());
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

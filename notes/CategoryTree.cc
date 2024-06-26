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
// Created by Piotr Pszczółkowski on 09.04.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "../sqlite/sqlite.hh"
#include "../common/EventController.hh"
#include "CategoryTree.hh"
#include "Tools.hh"
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QString>
#include <QDialog>
#include <QAction>
#include <QLineEdit>
#include <QHeaderView>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QDialogButtonBox>
#include <memory>
#include <string>
#include <fmt/core.h>

/*------- local constants:
-------------------------------------------------------------------*/
std::string const CategoryTree::InsertQuery{"INSERT INTO category (pid, name) VALUES (?,?)"};
std::string const CategoryTree::UpdateQuery{"UPDATE category SET name=? WHERE id=?"};
std::string const CategoryTree::DeleteQuery{"DELETE FROM category WHERE id=?"};

static char const* const RemoveTitle = "The category cannot be deleted.";
static char const* const RemoveMessage = "This category cannot be deleted because it has subcategories!";

CategoryTree::CategoryTree(QWidget* const parent) :
    QTreeWidget(parent),
    timer_(new QTimer(this))
{
    // !!! Konfiguracja timera !!!
    // Timer jest nadawcą zdarzenia z informacją, że kategoria ulegla zmianie.
    // Ale timer wyśle zdarzenie(event) jeśli od zmiany minie 500 milisekund (pół sekundy),
    // zapobiega to zbyt szybkim i częstym uaktualnieniom tabeli z notatkami.
    timer_->setInterval(500);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, [this]() {
        if (auto item = currentItem(); item) {
            auto const categoryID{item->data(0, IdRole).toInt()};
            if (noteID_ != -1) {
                EventController::instance().send(event::CategorySelected, categoryID, qi64(noteID_));
                noteID_ = -1;
            }
            else {
                EventController::instance().send(event::CategorySelected, categoryID);
            }
        }
    });

    auto p = palette();
    p.setColor(QPalette::Base, QColor{60, 60, 60, 255});
    setAutoFillBackground(true);
    setPalette(p);

    header()->hide();
    setColumnCount(1);
    setHorizontalScrollMode(ScrollPerPixel);

    connect(this, &QTreeWidget::currentItemChanged, [&](auto, auto) {
        timer_->stop();
        timer_->start();
    });

    updateContent();
    setCurrentItem(root_);
    EventController::instance().append(this, event::CategoryAndNoteToSelect);
}

CategoryTree::~CategoryTree() {
    EventController::instance().remove(this);
}

void CategoryTree::
customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event *>(event);
    switch (int(e->type())) {
        case event::CategoryAndNoteToSelect:
            if (auto data = e->data(); data.size() == 2) {
                auto const categoryID{data[0].toInt()};
                auto const noteID{data[1].toInt()};
                expandAndSelectChildFor(categoryID, noteID);
            }
            break;
    }
}

/// Utworzenie drzewa kategorii od nowa.
/// Od nowa tzn. odczytujemy najpierw bazę danych kategorii.
void CategoryTree::
updateContent() noexcept {
    clear();

    delete store_;
    store_ = new StoreCategory(this);

    // Ustawiamy kategorię 'root', która jest rodzicem
    // wszystkich innych kategorii.
    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Categories");
    root_->setData(0, IdRole, 0);
    root_->setData(0, PidRole, 0);

    addItemsFor(root_);
    root_->setExpanded(true);
}

/// Obsługa prawego klawisza myszu (menu kontekstowe)
void CategoryTree::
mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton) {
        auto const item = itemAt(event->pos());
        auto const main_item = (item == nullptr) or (item == root_);
        // actions
        auto const new_action = new QAction(main_item ? "New main category" : "New subcategory");
        auto const edit_action = new QAction("Rename");
        auto const remove_action = new QAction("Delete");
        // connections
        if (main_item)
            connect(new_action, &QAction::triggered, this, &CategoryTree::newMainCategory);
        else
            connect(new_action, &QAction::triggered, this, &CategoryTree::newSubcategory);
        connect(edit_action, &QAction::triggered, this, &CategoryTree::editItem);
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

/// Dodanie nowej kategorii głównej,
void CategoryTree::
newMainCategory() noexcept {
    if (auto category = categoryDialog(Category{}); category) {
        // w ramach kategorii żadne dwie bezpośrednie(!) podkategorie nie mogą się tak samo nazywać
        if (not alreadyExist(category->pid(), category->name())) {
            // zapis podanej nazwy kategorii do bazy danych
            auto const childID = SQLite::instance().insert(InsertQuery, category->pid(), category->name());
            if (childID not_eq SQLite::InvalidRowid) {
                auto expanded = fetchExpandedItems();
                updateContent();
                restoreExpandedItems(std::move(expanded));
                setCurrentItem(childWithID(root_, childID));
            }
        }
    }
}

/// Dodanie nowej podkategorii do aktualnie wybranej w drzewie kategorii.
void CategoryTree::
newSubcategory() noexcept {
    // wyznaczenie aktualnie wybranej w drzewie kategorii.
    // to będzie tak zwany parent-item.
    QTreeWidgetItem* parentItem = currentItem();
    if (parentItem == nullptr) return;
    auto parentCategory = categoryFrom(parentItem);

    if (auto opt = categoryDialog(std::move(parentCategory)); opt) {
        auto category = opt.value();
        // w ramach kategorii żadne dwie bezpośrednie(!) podkategorie nie mogą się tak samo nazywać
        if (not alreadyExist(category.pid(), category.name())) {
            auto const id = SQLite::instance().insert(InsertQuery, category.pid(), category.name());
            if (id not_eq SQLite::InvalidRowid) {
                auto expanded = fetchExpandedItems();
                updateContent();
                restoreExpandedItems(std::move(expanded));
                expandAndSelectChildFor(id);
            }
        }
    }
}

void CategoryTree::
expandAndSelectChildFor(i64 const categoryID, i64 const noteID) noexcept {
    if (auto item = childWithID(root_, categoryID); item) {
        // Rozwijamy parent, aby nasza kategoria była widziana.
        auto parent = item->parent();
        while (parent) {
            if (!parent->isExpanded())
                expandItem(parent);
            parent = parent->parent();
        }
        if (noteID != -1)
            noteID_ = noteID;
        setCurrentItem(item);
    }
}

/// Usunięcie aktualnej kategorii.
void CategoryTree::
remove_category() noexcept {
    if (auto item = currentItem(); item) {
        if (item == root_) return;
        auto category = categoryFrom(item);

        // Jeśli kategoria zawiera pod-kategorje prosimy użytkownika
        // o potwierdzenie czy rzeczywiście tego chce.
        if (store_->hasSubcategories(category.id())) {
            QMessageBox::warning(QApplication::activeWindow(), RemoveTitle, RemoveMessage);
            return;
        }
        // TODO check if category contains notes

        if (auto ok = SQLite::instance().exec(DeleteQuery, category.id()); ok) {
            // Co by tu wybrać po usunięciu aktualnej kategorii?
            i64 next_selected_id = 0;
            // Spróbuj przesunąć się do góry
            if (auto item_above = itemAbove(item); item_above && item_above != root_)
                next_selected_id = item_above->data(0, IdRole).toInt();
            // Jeśli nie można do góry, spróbuj przesunąć się w dół.
            else if (auto item_below = itemBelow(item); item_below && item_below != root_)
                next_selected_id = itemBelow(item)->data(0, IdRole).toInt();

            auto expanded = fetchExpandedItems();
            updateContent();
            restoreExpandedItems(std::move(expanded));
            setCurrentItem(childWithID(root_, next_selected_id));
        }
    }
}

/// Edycja nazwy aktualnej kategorii.
void CategoryTree::
editItem() noexcept {
    if (auto item = currentItem(); item) {
        if (auto opt = categoryDialog(categoryFrom(item), true); opt) {
            auto category{*opt};
            if (not alreadyExist(category.pid(), category.name())) {
                if (SQLite::instance().update(UpdateQuery, category.name(), category.id())) {
                    auto expanded = fetchExpandedItems();
                    updateContent();
                    restoreExpandedItems(std::move(expanded));
                    setCurrentItem(childWithID(root_, category.id()));
                }
            }
        }
    }
}

/// Konwersja danych tree-item na strukturę 'Category'.
Category CategoryTree::
categoryFrom(QTreeWidgetItem const* const item) noexcept {
    Category category{};
    if (auto v = item->data(0, IdRole); v.isValid())
        category.id(v.toInt());
    if (auto v = item->data(0, PidRole); v.isValid())
        category.pid(v.toInt());
    if (auto v = item->text(0); not v.isEmpty())
        category.name(v);
    return category;
}

/// Uruchomienie dialogu z edycją danych kategorii.
std::optional<Category> CategoryTree::
categoryDialog(Category&& category, bool const rename) noexcept {
    auto const parent_layout = new QHBoxLayout;
    if (category.id() == 0)
        parent_layout->addWidget(new QLabel("No parent category. This will be the new main category."));
    else {
        // TODO fetch all parent categories
//        auto str = fmt::format("Categories > {}", category.name());
        auto str = Tools::categoriesChainInfo(category.id());
        parent_layout->addWidget(new QLabel(qstr::fromStdString(str)));
    }

    auto const editor = new QLineEdit;
    if (rename) {
        editor->setText(category.qname());
        editor->selectAll();
    }

    auto const edit_layout = new QHBoxLayout;
    edit_layout->addWidget(new QLabel{category.id() ? "Subcategory name:" : "Category name:"});
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

    editor->setFocus();

    if (dialog->exec() and not editor->text().isEmpty()) {
        Category result{};
        result.name(editor->text());
        if (rename) result.id(category.id()).pid(category.pid());
        else result.pid(category.id());
        return result;
    }
    return {};
}

/// Add all children (with sub-children) for passed item.
void CategoryTree::
addItemsFor(QTreeWidgetItem* const parent) noexcept {
    auto const pid = parent->data(0, IdRole).toInt();
    auto childs = store_->childsForParentWithID(pid);
    std::ranges::sort(childs, [](auto const& a, auto const& b) {
       return QString::compare(a.qname(), b.qname(), Qt::CaseInsensitive) < 0;
    });
    std::ranges::for_each(childs, [parent, this](auto const& category) {
        auto const item = new QTreeWidgetItem(parent);
        item->setText(0, category.qname());
        item->setData(0, IdRole, category.qid());
        item->setData(0, PidRole, category.qpid());
        addItemsFor(item);
    });
}

/// Sprawdzenie czy kategoria już ma podkategorię o wskazanej nazwie.
bool CategoryTree::
alreadyExist(i64 const pid, std::string const& name) const noexcept {
    if (not store_->exist(pid, name))
        return {};

    // Podkategoria o takiej nazwie już istnieje.
    auto msg = fmt::format("The subcategory '{}' already exists!", name);
    QMessageBox::warning(QApplication::activeWindow(), "Unacceptable category name", QString::fromStdString(msg));
    return true;
}

/// Zwraca tree-item kategorii ze wskazanym numer ID.
/// Lub nullptr jesli nie znaleziono.
QTreeWidgetItem* CategoryTree::
childWithID(QTreeWidgetItem* const parent, i64 const id) noexcept {
    for (auto it = QTreeWidgetItemIterator{parent}; *it; ++it) {
        auto const item = *it;
        i64 const item_id = item->data(0, IdRole).toInt();
        if (item_id == id)
            return item;
    }
    return nullptr;
}

/// Zwraca wektor z numerami ID aktualnie rozwiniętych kategorii.
std::unordered_set<i64> CategoryTree::
fetchExpandedItems() const noexcept {
    std::unordered_set<i64> ids{};

    for (auto it = QTreeWidgetItemIterator{root_}; *it; ++it) {
        auto const item{*it};
        if (item->isExpanded())
            ids.insert(item->data(0, IdRole).toInt());
    }

    return ids;
}

/// Rozwinicię kategorii których numery ID znajdują się we wskazanym zbiorze.
void CategoryTree::
restoreExpandedItems(std::unordered_set<i64>&& ids) noexcept {
    if (not ids.empty()) {
        for (auto it = QTreeWidgetItemIterator{root_}; *it; ++it) {
            auto const item{*it};
            if (ids.contains(item->data(0, IdRole).toInt()))
                item->setExpanded(true);
        }
    }
}

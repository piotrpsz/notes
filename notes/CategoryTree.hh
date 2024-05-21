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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "../shared.hh"
#include "../model/StoreCategory.hh"
#include "types.hh"
#include <QTreeWidget>
#include <optional>
#include <unordered_set>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QTimer;
class QMouseEvent;
class QTreeWidgetItem;

/*------- class:
-------------------------------------------------------------------*/
class CategoryTree : public QTreeWidget {
    Q_OBJECT
    enum { IdRole = Qt::UserRole+1, PidRole};
public:
//    struct Category { i64 id{}; i64 pid{}; qstr name{}; };
public:
    explicit CategoryTree(QWidget* = nullptr);
    ~CategoryTree() override = default;

private:
    void mousePressEvent(QMouseEvent*) override;

    static std::optional<Category> category_dialog(Category&& category, bool rename = false) noexcept;
    void add_items_for(QTreeWidgetItem* item) noexcept;
    static Category category_from(QTreeWidgetItem const*) noexcept;
    [[nodiscard]] bool already_exist(i64 pid, std::string const& name) const noexcept;
    static QTreeWidgetItem* child_with_id_for(QTreeWidgetItem* parent, i64 id) noexcept;
    void update_content() noexcept;
    [[nodiscard]] std::unordered_set<i64> expanded_items() const noexcept;
    void expanded_items(std::unordered_set<i64>&& ids) noexcept;

private slots:
    void new_subcategory() noexcept;
    void new_main_category() noexcept;
    void remove_category() noexcept;
    void edit_item() noexcept;

private:
    QTreeWidgetItem* root_{};
    QTimer* const timer_;
    StoreCategory* store_{};

    static std::string const InsertQuery;
    static std::string const DeleteQuery;
    static std::string const UpdateQuery;
public:
    static std::string const CountQuery;
    static std::string const SubcategoriesCountQuery;
};

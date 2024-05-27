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
// SOFTWARE.
//
// Created by Piotr Pszczółkowski on 27/05/2024.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QTreeWidget>
#include "../shared.hh"

/*------- forward declarations:
-------------------------------------------------------------------*/
class StoreCategory;
class QTreeWidgetItem;


class CategoryTreeBrowser : public QTreeWidget {
    Q_OBJECT
    enum { IdRole = Qt::UserRole+1, PidRole};
    StoreCategory* store_{};
    QTreeWidgetItem* root_{};
public:
    explicit CategoryTreeBrowser(i64 categoryID, QWidget* = nullptr);
    ~CategoryTreeBrowser() override = default;

private:
    void populate_content() noexcept;
    void add_items_for(QTreeWidgetItem* item) noexcept;
    i64 selectedID() const noexcept;
    static QTreeWidgetItem* child_with_id_for(QTreeWidgetItem* parent, i64 id) noexcept;
};

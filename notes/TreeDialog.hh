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
#include <QDialog>
#include "../shared.hh"
#include <optional>

/*------- forward declaration:
-------------------------------------------------------------------*/
class CategoryTreeBrowser;
class QShowEvent;

class TreeDialog : public QDialog {
    Q_OBJECT
    CategoryTreeBrowser* const tree_;
    i64 selectedCategoryID_{-1};
public:
    explicit TreeDialog(i64 categoryID, QWidget* = nullptr);
    ~TreeDialog() override = default;

    i64 selectedCategoryID() const noexcept {
        return selectedCategoryID_;
    }
private:
    void showEvent(QShowEvent*) override;
};

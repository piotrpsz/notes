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

/*------- include files:
-------------------------------------------------------------------*/
#include "TreeDialog.hh"
#include "CategoryTreeBrowser.hh"
#include <QBoxLayout>
#include <QPushButton>
#include <QApplication>

TreeDialog::TreeDialog(i64 categoryID, QWidget* const parent) :
        QDialog(parent),
        tree_{ new CategoryTreeBrowser{categoryID}}
{
    setWindowTitle("Category Tree");

    auto const cancel{new QPushButton{"Cancel"}};
    auto const select{new QPushButton{"Select"}};
    connect(cancel, &QPushButton::clicked, [this] {
       reject();
    });
    connect(select, &QPushButton::clicked, [this] {

        accept();
    });

    auto const buttons{new QHBoxLayout};
    buttons->setContentsMargins(0, 0, 0, 0);
    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(select);

    auto const layout = new QVBoxLayout;
    layout->setContentsMargins(16, 16, 16, 6);
    layout->setSpacing(6);
    layout->addWidget(tree_);
    layout->addLayout(buttons);
    setLayout(layout);
}

void TreeDialog::
showEvent(QShowEvent*) {
    auto const parent = QApplication::activeWindow();
    auto const size = parent->size();
    auto const parent_width = size.width();
    auto const parent_height = size.height();
    auto const width = parent_width * .4;
    auto const height = parent_height * .7;
    resize(int(width), int(height));

    auto const x = parent->pos().x() + (parent_width - width)/2;
    auto const y = parent->pos().y() + (parent_height - height)/2;
    move(int(x), int(y));
}

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
#include "MainWindow.h"
#include "Settings.h"
#include "NotesWorkspace.h"
#include "CategoryTree.hh"
#include <QApplication>
#include <QSplitter>
#include <fmt/core.h>

MainWindow::MainWindow(QWidget *const parent) :
        QMainWindow(parent),
        splitter_{new QSplitter(Qt::Horizontal)}
{
    auto app_name = QCoreApplication::applicationName().toStdString();
    auto app_ver = QCoreApplication::applicationVersion().toStdString();
    auto title = fmt::format("{} ver. {}", app_name, app_ver);
    setWindowTitle(qstr::fromStdString(title));

    splitter_->setHandleWidth(1);
    splitter_->addWidget(new CategoryTree);
    splitter_->addWidget(new NotesWorkspace);
    setCentralWidget(splitter_);
}

void MainWindow::showEvent(QShowEvent *const event) {
    QMainWindow::showEvent(event);
    if (not first_show_) return;

    first_show_ = false;
    Settings sts;

    if (auto data = sts.read(MainWindowSizeKey); data)
        resize(data.value().toSize());
    if (auto data = sts.read(MainWindowPosKey); data)
        move(data.value().toPoint());
    if (auto data = sts.read(MainWindowStateKey); data)
        restoreState(data.value().toByteArray());

    auto const s = size();
    auto w0 = int(20. * s.width() / 100.);
    auto w1 = int(s.width() - w0 - splitter_->handleWidth());

    if (auto data = sts.read(MainWindowW0Key); data)
        w0 = data.value().toInt();
    if (auto data = sts.read(MainWindowW1Key); data)
        w1 = data.value().toInt();

    splitter_->setSizes({w0, w1});
}

void MainWindow::closeEvent(QCloseEvent *const event) {
    Settings sts;
    sts.save(MainWindowStateKey, saveState());
    sts.save(MainWindowPosKey, pos());
    sts.save(MainWindowSizeKey, size());

    auto data = splitter_->sizes();
    sts.save(MainWindowW0Key, data[0]);
    sts.save(MainWindowW1Key, data[1]);

    QMainWindow::closeEvent(event);
}

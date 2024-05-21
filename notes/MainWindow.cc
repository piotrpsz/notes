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
#include "MainWindow.hh"
#include "Settings.hh"
#include "NotesWorkspace.hh"
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

/// Wyświetlenie okna programu - odczyt i zastosowanie zapamiętanej
/// geometrii okna z poprzedniego uruchomienia programu.
void MainWindow::showEvent(QShowEvent *const event) {
    if (not first_show_) return;

    first_show_ = false;
    Settings sts;

    // Odtwórz rozmiar okna
    if (auto data = sts.read(MainWindowSizeKey); data)
        resize(data.value().toSize());
    // jego pozycję
    if (auto data = sts.read(MainWindowPosKey); data)
        move(data.value().toPoint());
    // i stan.
    if (auto data = sts.read(MainWindowStateKey); data)
        restoreState(data.value().toByteArray());

    // Ustaw proporcje splittera.
    auto const width = size().width();
    if (auto data = sts.read(MainWindowW0Key); data) {
        auto const w0 = (*data).toDouble() * width / 100.;
        auto const w1 = width - splitter_->handleWidth() - w0;
        splitter_->setSizes({static_cast<int>(w0), static_cast<int>(w1)});
    } else {
        auto const w0 = 20. * width / 100.;
        auto const w1 = width - splitter_->handleWidth() - w0;
        splitter_->setSizes({static_cast<int>(w0), static_cast<int>(w1)});
    }
}

/// Zamknięcie okna - zapamiętanie geometrii okna programu.
void MainWindow::closeEvent(QCloseEvent *const event) {
    Settings sts;

    // Zapisz stan okna, jego pozycję i rozmiar.
    sts.save(MainWindowStateKey, saveState());
    sts.save(MainWindowPosKey, pos());
    sts.save(MainWindowSizeKey, size());

    // Zapisz propocje komponentów splittera.
    auto const width{size().width()};
    auto const data{splitter_->sizes()};
    sts.save(MainWindowW0Key, double(data[0] * 100. / width));
}

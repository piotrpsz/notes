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
#include <QMainWindow>
#include "types.h"

/*------- forward declarations:
-------------------------------------------------------------------*/
class QShowEvent;
class QCloseEvent;
class QSplitter;

/*------- class:
-------------------------------------------------------------------*/
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* = nullptr);
    ~MainWindow() override = default;

private:
    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    bool first_show_{true};
    QSplitter* const splitter_;


    static inline qstr const MainWindowSizeKey = "MainWindow/Size";
    static inline qstr const MainWindowPosKey = "MainWindow/Position";
    static inline qstr const MainWindowStateKey = "MainWindow/State";
};

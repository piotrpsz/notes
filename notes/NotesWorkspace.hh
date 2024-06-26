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
// Created by Piotr Pszczółkowski 09.04.24.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "types.hh"
#include <QSplitter>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QShowEvent;
class QCloseEvent;
class QEvent;

/*------- class:
-------------------------------------------------------------------*/
class NotesWorkspace : public QSplitter {
    Q_OBJECT
    static double constexpr PERCENTAGE_WIDTH = 60. / 100.;
    static int constexpr SPLITTER_HANDLE_WIDTH = 1;
public:
    explicit NotesWorkspace(QWidget* = nullptr);
    ~NotesWorkspace() override;

private:
    void showEvent(QShowEvent*) override;
    void customEvent(QEvent*) override;
    static void newNoteForCategoryWithID(qi64 categoryID) noexcept;
    static void editNoteWithID(qi64 noteID) noexcept;

    bool firstTimeShow_{true};
    static inline qstr const SizesH0Key = "NotesWorkspace/Sizes/H0";
    static inline qstr const SizesH1Key = "NotesWorkspace/Sizes/H1";
};


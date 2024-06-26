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
// Created by Piotr Pszczółkowski on 30.04.24.

/*------- include files:
-------------------------------------------------------------------*/
#include "Editor.hh"
#include "../common/EventController.hh"
#include <QFont>
#include <QKeyEvent>
#include <QFontDialog>
#include <QColorDialog>
#include <fmt/core.h>

Editor::Editor(QWidget* const parent) : EditorBase(parent) {
    setUndoRedoEnabled(true);

    EventController::instance().append(this,
                                       event::CopyRequest,
                                       event::CutRequest,
                                       event::PasteRequest,
                                       event::UndoRequest,
                                       event::RedoRequest,
                                       event::SelectAllRequest,
                                       event::SelectColorRequest,
                                       event::SelectFontRequest);

    // informacje pomocniecze o tekście po zmianie pozycji kursora
//    connect(this, &QTextEdit::cursorPositionChanged, [&] {
//        auto const cursor = textCursor();
//        auto cf = cursor.charFormat();
//        auto c = cf.foreground().color();
//        auto f = cf.font();
//
//        fmt::print("r: {}, g: {}, b: {}, a: {} - {}\n", c.red(), c.green(), c.black(), c.alpha(), f.toString().toStdString());
//    });
}

Editor::~Editor() {
    EventController::instance().remove(this);
}

void Editor::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
        case event::SelectFontRequest:
            select_font();
            e->accept();
            break;
        case event::SelectColorRequest:
            select_color();
            e->accept();
            break;
        case event::CopyRequest:
            if (textCursor().hasSelection())
                copy();
            e->accept();
            break;
        case event::CutRequest:
            if (textCursor().hasSelection())
                cut();
            e->accept();
            break;
        case event::PasteRequest:
            if (canPaste())
                paste();
            e->accept();
            break;
        case event::SelectAllRequest:
            selectAll();
            e->accept();
            break;
        case event::UndoRequest:
            undo();
//            e->accept();
            break;
        case event::RedoRequest:
            redo();
            e->accept();
            break;
    }
}

void Editor::keyPressEvent(QKeyEvent* const event) {
    auto const flag = event->modifiers();
    switch (event->key()) {
        case Qt::Key_U:
            if (flag.testFlag(Qt::ControlModifier)) {
                setFontUnderline(not fontUnderline());
                event->accept();
            }
            break;
        case Qt::Key_I:
            if (flag.testFlag(Qt::ControlModifier)) {
                setFontItalic(not fontItalic());
                event->accept();
            }
            break;
        case Qt::Key_B:
            if (flag.testFlag(Qt::ControlModifier)) {
                auto const is_bold = fontWeight() == QFont::Bold;
                setFontWeight(is_bold ? QFont::Normal : QFont::Bold);
                event->accept();
            }
            break;

    }
    QTextEdit::keyPressEvent(event);
}

void Editor::select_font() noexcept {
    // https://doc.qt.io/qt-6/qfontdatabase.html#applicationFontFamilies
//    for (auto const& family : QFontDatabase::families()) {
//        fmt::print("{}\n", family.toStdString());
//        for (auto const& style : QFontDatabase::styles(family))
//            fmt::print("\t{}\n", style.toStdString());
//    }

    auto const cursor = textCursor();
    auto const cf = cursor.charFormat();
    bool ok;
    auto const fnt = QFontDialog::getFont(&ok, cf.font());
    if (ok)
        setCurrentFont(fnt);
}

void Editor::select_color() noexcept {
    auto const cursor = textCursor();
    auto cf = cursor.charFormat();
//    auto c = cf.foreground().color();
//    auto f = cf.font();

    auto const color = QColorDialog::getColor(cf.foreground().color());
    setTextColor(color);
}
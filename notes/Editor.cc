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
// Created by piotr on 30.04.24.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "Editor.h"
#include "../common/EventController.h"
#include <QFont>
#include <QKeyEvent>
#include <QFontDialog>
#include <QColorDialog>
#include <QFontDatabase>
#include <fmt/core.h>

Editor::Editor(QWidget* const parent) :
        QTextEdit(parent)
{
    setAcceptRichText(true);

    // ustawienie odstępu pomiędzy liniami
    auto block_fmt = textCursor().blockFormat();
    block_fmt.setLineHeight(5, QTextBlockFormat::LineDistanceHeight);
    textCursor().setBlockFormat(block_fmt);

    // ustawinie fontu
    QFont font;
    font.setFamily("Menlo");
    font.setKerning(true);
//    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);


    EventController::instance().append(this,
                                       event::CopyRequest,
                                       event::CutRequest,
                                       event::PasteRequest,
                                       event::SelectColorRequest,
                                       event::SelectFontRequest);
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
            fmt::print("copy request\n");
            e->accept();
            break;
        case event::CutRequest:
            fmt::print("cut request\n");
            e->accept();
            break;
        case event::PasteRequest:
            fmt::print("patse request\n");
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

    bool ok;
    auto const fnt = QFontDialog::getFont(&ok, this);
    if (ok)
        setCurrentFont(fnt);
}

void Editor::select_color() noexcept {
    auto const color = QColorDialog::getColor();
    setTextColor(color);
}
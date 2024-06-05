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
// Created by Piotr Pszczółkowski on 05.06.24.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QTextEdit>
#include "Settings.hh"

class EditorBase : public QTextEdit {
    Q_OBJECT
public:
    explicit EditorBase(QWidget* const parent = nullptr) : QTextEdit(parent) {
        setAcceptRichText(true);

        // ustawienie odstępu pomiędzy liniami
        auto format = textCursor().blockFormat();
        format.setLineHeight(settings::DEFAULT_LINE_DISTANCE, QTextBlockFormat::LineDistanceHeight);
        textCursor().setBlockFormat(format);

        // ustawinie fontu
        QFont font;
        font.setFamily(settings::DEFAULT_FONT_FAMILY.c_str());    // "Noto Sans Regular"
        font.setKerning(true);
        font.setPointSize(settings::DEFAULT_FONT_SIZE);
        setFont(font);

        // ustawinie długości TAB
        setTabStopDistance(settings::DEFAULT_TAB_STOP * fontMetrics().horizontalAdvance('-') - 2.0);
    }
    ~EditorBase() override = default;
};

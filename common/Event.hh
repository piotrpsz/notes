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
// Created by Piotr Pszczółkowski on 24.04.2024.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QEvent>
#include <QVector>
#include <QVariant>
#include <fmt/core.h>

/*------- class:
-------------------------------------------------------------------*/
class Event : public QEvent {
    QVector<QVariant> data_{};
public:
    template<typename... T>
    explicit Event(int const id, T... args) : QEvent(static_cast<QEvent::Type>(id)) {
        (..., data_.push_back(args));
    }

    QVector<QVariant> data() && {
        fmt::print("events data moved\n");
        return std::move(data_);
    }
    [[nodiscard]] QVector<QVariant> const& data() const& {
        return data_;
    }
};

/*------- user's events:
-------------------------------------------------------------------*/
namespace event {
    enum {
        CategorySelected = (QEvent::User + 1),
        CategoryAndNoteToSelect,
//        NoteToSelect,/
        NewNoteRequest,
        EditNoteRequest,
        RemoveCurrentNoteRequest,
        MoveCurrentNoteRequest,

        SelectFontRequest,
        BoldRequest,
        ItalicRequest,
        Underline,
        SelectColorRequest,
        CopyRequest,
        CutRequest,
        PasteRequest,
        UndoRequest,
        RedoRequest,
        SelectAllRequest,
        NoteDatabaseChanged,
        NoteSelected,
    };
}

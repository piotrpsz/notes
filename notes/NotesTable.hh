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
#include "../shared.hh"
#include <QTableWidget>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QEvent;
class QTableWidgetItem;

/*------- class:
-------------------------------------------------------------------*/
class NotesTable : public QTableWidget {
    Q_OBJECT
    enum { NoteID = Qt::UserRole+1, CategoryID};
public:
    explicit NotesTable(QWidget* = nullptr);
    ~NotesTable() override = default;
private:
    /// Odbieranie zdefiniowanych w programie zdarzeń.
    /// \param event - zdarzenie
    void customEvent(QEvent* event) override;

    void delete_note(qi64 noteID) noexcept;

    /// Uaktualnienie tabeli notatek dla wskazanej kategorii.
    /// \param id - numer ID kategorii, której notatki mają być wyświetlone.
    void update_content_for(i64 id) noexcept;

    QTableWidgetItem* row_with_id(qint64 id) const noexcept;
};

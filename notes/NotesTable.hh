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
#include <optional>
#include <utility>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QEvent;

class QTableWidgetItem;

/*------- class:
-------------------------------------------------------------------*/
class NotesTable : public QTableWidget {
    Q_OBJECT
    enum { NoteID = Qt::UserRole + 1, CategoryID };
    i64 categoryID_{};
public:
    explicit NotesTable(QWidget * = nullptr);

    ~NotesTable() override;

private:
    /// Odbieranie zdefiniowanych w programie zdarzeń.
    /// \param event - zdarzenie
    void customEvent(QEvent *event) override;

    void deleteNoteWithID(qi64 noteID) noexcept;

    /// Uaktualnienie tabeli notatek dla wskazanej kategorii.
    /// \param id - numer ID kategorii, której notatki mają być wyświetlone.
    void updateContentForCategoryWithID(i64 id) noexcept;

    void clearContent() noexcept {
        clearContents();
        setRowCount(0);
    }

    [[nodiscard]] QTableWidgetItem* currentItemWhenFocus() const noexcept {
        if (hasFocus())
            return item(currentRow(), 0);
        return {};
    }

    static std::optional<std::pair<i64, i64>>
    dataFromItem(QTableWidgetItem *const item) noexcept {
        i64 noteID{-1}, categoryID{-1};

        if (auto data = item->data(CategoryID); data.isValid() && data.canConvert<int>())
            categoryID = data.toInt();
        if (auto data = item->data(NoteID); data.isValid() && data.canConvert<int>())
            noteID = data.toInt();

        if (noteID not_eq -1 and categoryID not_eq -1)
            return std::make_pair(categoryID, noteID);
        return {};
    }

    [[nodiscard]] QTableWidgetItem* rowWithID(qint64 id) const noexcept;

    void moveNoteToCategoryWithID(i64 noteID, i64 destinationCategoryID) noexcept;
};

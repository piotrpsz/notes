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
// Created by Piotr Pszczółkowski on 27.04.24.

#include "EditDialog.hh"
#include "Editor.hh"
#include "../common/EventController.hh"
#include <QIcon>
#include <QFrame>
#include <QLabel>
#include <QAction>
#include <QToolBar>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QMessageBox>
#include <QStandardItem>
#include <fmt/core.h>

/// Edycja istniejącej notatki. \n
/// Przy wywołaniu przekazujemy na własność notatkę, która ma być edytowana. \n
/// Po zakończeniu należy pobrać nową (zmodyfikowaną) notatkę przez 'get_note'.
/// \param note - notatka do edycji.
EditDialog::EditDialog(Note&& note, QWidget *const parent) : EditDialog(parent) {
    setWindowTitle("Edit note");
    note_ = std::move(note);

    title_->setText(note_.value().qtitle());
    description_->setText(note_.value().qdescription());
    editor_->setHtml(note_.value().qcontent());

    connect(accept_btn_, &QPushButton::clicked, [&]() {
        auto new_note = note_.value();
        new_note.title(title_->text())
                .description((description_->text()))
                .content(editor_->toHtml());
        note_ = std::move(new_note);

        if (auto ok = note_.value().save(); not ok) {
            // Jesli zapis do bazy był nie udany, to dajemy użytkownikowi
            // możliwość dalszej edycji.
            QMessageBox::critical(this, "Error", "Error writing to database.");
            return;
        }
        accept();
    });

    connect(cancel_btn_, &QPushButton::clicked, [this]() {
        reject();
    });
}


/// Nowa notatka dla wskazanej kategorii.
EditDialog::EditDialog(qi64 const categoryID, QWidget *const parent) : EditDialog(parent) {
    setWindowTitle("New note");

    // Próba zapisania nowej notatki do bazy danych.
    connect(accept_btn_, &QPushButton::clicked, [this, categoryID] {
        Note note = Note{}
                .pid(categoryID)
                .title(title_->text())
                .description(description_->text())
                .content(editor_->toHtml());
        if (auto ok = note.save(); not ok) {
            QMessageBox::critical(this, "Error", "Error writing to database.");
            return;
        }
        else
            note_id_ = note.id<i64>();
        accept();
    });

    // Bye, bye
    connect(cancel_btn_, &QPushButton::clicked, [this]() {
        reject();
    });
}

/********************************************************************
 *                                                                  *
 *                         P R I V A T E                            *
 *                                                                  *
 ********************************************************************/

EditDialog::EditDialog(QWidget *const parent) :
        QDialog(parent),
        title_{new QLineEdit},
        description_{new QLineEdit},
        editor_{new Editor},
        size_cbox_{new QComboBox},
        font_face_cbx_{new QComboBox},
        accept_btn_{new QPushButton{"Accept"}},
        cancel_btn_{new QPushButton{"Cancel"}} {
    setSizeGripEnabled(true);
    populate_font_size_cbx();
    populate_font_face_cbx();

    auto const buttons{new QHBoxLayout};
    buttons->setContentsMargins(0, 0, 0, 0);
    buttons->addStretch();
    buttons->addWidget(cancel_btn_);
    buttons->addWidget(accept_btn_);
    accept_btn_->setDefault(true);

    auto const separator{new QFrame};
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto layout = new QGridLayout;
    layout->setContentsMargins(8, 8, 8, 4);
    layout->addWidget(new QLabel("Tilte"), 0, 0);
    layout->addWidget(title_, 0, 1);
    layout->addWidget(new QLabel("Description"), 1, 0);
    layout->addWidget(description_, 1, 1);
    layout->addWidget(separator, 2, 0, 1, 2);
    layout->addLayout(editor_layout(), 3, 0, 1, 2);
    layout->addLayout(buttons, 4, 0, 1, 2);

    setLayout(layout);
}

void EditDialog::
showEvent(QShowEvent *e) {
    auto const parent = QApplication::activeWindow();
    auto const size = parent->size();
    resize(int(size.width() * .7), int(size.height() * .7));
    QDialog::showEvent(e);
}

QVBoxLayout *EditDialog::
editor_layout() const noexcept {
    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html

    // Ikony po będą po prawej stronie. Aby tak się stało dodamy
    // z lewej strony spacer który zepchnie je do prawej strony.
    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto const toolbar{new QToolBar};
    toolbar->setIconSize(QSize(16, 16));

    auto const copy_action = new QAction(QIcon::fromTheme("edit-copy"), "Copy");
    connect(copy_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::CopyRequest);
    });
    auto const cut_action = new QAction(QIcon::fromTheme("edit-cut"), "Cut");
    connect(cut_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::CutRequest);
    });
    auto const paste_action = new QAction(QIcon::fromTheme("edit-paste"), "Paste");
    connect(paste_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::PasteRequest);
    });
    auto const font_action = new QAction(QIcon::fromTheme("preferences-desktop-font"), "Font");
    connect(font_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::SelectFontRequest);
    });
    auto const color_action = new QAction(QIcon::fromTheme("color"), "Color");
    connect(color_action, &QAction::triggered, [this]() {
        EventController::instance().send(event::SelectColorRequest);
    });

    toolbar->addWidget(new QLabel("Font settings: "));
    toolbar->addWidget(size_cbox_);
    toolbar->addWidget(font_face_cbx_);
    toolbar->addWidget(spacer);
    toolbar->addAction(copy_action);
    toolbar->addAction(cut_action);
    toolbar->addAction(paste_action);
    toolbar->addAction(font_action);
    toolbar->addAction(color_action);

    auto const layout{new QVBoxLayout};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolbar);
    layout->addWidget(editor_);
    return layout;
}

void EditDialog::
populate_font_size_cbx() const noexcept {
    size_cbox_->setEditable(false);
    for (int i = 8; i < 26; ++i)
        size_cbox_->addItem(qstr::fromStdString(fmt::format("{}", i)));
    size_cbox_->setCurrentText("10");
}

void EditDialog::
populate_font_face_cbx() const noexcept {
    auto const model = new QStandardItemModel(3, 1);

    auto const root = new QStandardItem("face");
    model->setItem(0, root);

    auto const bold = new QStandardItem("bold");
    bold->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    bold->setData(Qt::Unchecked, Qt::CheckStateRole);
    bold->setData(Bold, FaceRole);
    model->setItem(1, bold);

    auto const italic = new QStandardItem("italic");
    italic->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    italic->setData(Qt::Unchecked, Qt::CheckStateRole);
    italic->setData(Italic, FaceRole);
    model->setItem(2, italic);

    auto const underline = new QStandardItem("underline");
    underline->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    underline->setData(Qt::Unchecked, Qt::CheckStateRole);
    underline->setData(Underline, FaceRole);
    model->setItem(3, underline);

    font_face_cbx_->setModel(model);
    font_face_cbx_->setCurrentIndex(0);
    font_face_cbx_->setMinimumWidth(font_face_cbx_->fontMetrics().boundingRect("underline").width() + 60);

    connect(model, &QStandardItemModel::itemChanged, [bold, italic, underline](auto item) {
        u8 state = Normal;
        if (bold->checkState() & Qt::Checked) state |= Bold;
        if (italic->checkState() & Qt::Checked) state |= Italic;
        if (underline->checkState() & Qt::Checked) state |= Underline;
        // TODO send event?
    });
}



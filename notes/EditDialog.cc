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
// Created by piotr on 27.04.24.
//

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
#include <QDialogButtonBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <fmt/core.h>

EditDialog::EditDialog(Note&& note, QWidget* const parent) :
        QDialog(parent),
        note_{std::move(note)},
        title_{new QLineEdit},
        description_{new QLineEdit},
        editor_{new Editor},
        size_cbox_{new QComboBox},
        font_face_cbx_{new QComboBox}
{
    size_cbox_->setEditable(false);
    for (int i = 8, row = 0; i < 26; ++i, ++row)
        size_cbox_->addItem(qstr::fromStdString(fmt::format("{}", i)));
    size_cbox_->setCurrentText("10");

    populate_font_face_cbx();

    title_->setText(note_.value().qtitle());
    description_->setText(note_.value().qdescription());
    editor_->setHtml(note_.value().qcontent());

    setWindowTitle("Edit note");
    setSizeGripEnabled(true);

    auto const separator{new QFrame};
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto const buttons{new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel)};
    auto const apply_button = buttons->button(QDialogButtonBox::Apply);
    auto const cancel_button = buttons->button(QDialogButtonBox::Cancel);

    auto layout = new QGridLayout;
    layout->addWidget(new QLabel("Tilte"), 0, 0);
    layout->addWidget(title_, 0, 1);
    layout->addWidget(new QLabel("Description"), 1, 0);
    layout->addWidget(description_, 1, 1);
    layout->addWidget(separator, 2, 0, 1, 2);
    layout->addLayout(editor_layout(), 3, 0, 1, 2);
    layout->addWidget(buttons, 4, 0, 1, 2);
    setLayout(layout);

    connect(apply_button, &QPushButton::clicked, [&]() {
        auto new_note = std::move(*note_);
        new_note.title(title_->text());
        new_note.description((description_->text()));
        new_note.content(editor_->toHtml());
        note_ = std::move(new_note);

        if (auto ok = note_.value().save(); not ok) {
            // Jesli zapis do bazy był nie udany, to dajemy użytkownikowi
            // możliwość dalszej edycji.
            QMessageBox::critical(this, "Error", "Error writing to database.");
            return;
        }
        accept();
    });

    connect(cancel_button, &QPushButton::clicked, [this]() {
        reject();
    });
}

/// Nowa notatka dla wskazanej kategorii.
EditDialog::EditDialog(qi64 const category_id, QWidget* const parent) :
    QDialog(parent),
    title_{new QLineEdit},
    description_{new QLineEdit},
    editor_{new Editor},
    category_id_{category_id},
    size_cbox_{new QComboBox},
    font_face_cbx_{new QComboBox}
{
    size_cbox_->setEditable(false);
    for (int i = 8; i < 26; ++i)
        size_cbox_->addItem(qstr::fromStdString(fmt::format("{}", i)));
    size_cbox_->setCurrentText("10");

    setWindowTitle("New note");
    setSizeGripEnabled(true);

    auto const separator{new QFrame};
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto const buttons{new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel)};
    auto const apply_button = buttons->button(QDialogButtonBox::Apply);

    connect(apply_button, &QPushButton::clicked, [this]() {
        Note note = note_.value_or(Note())
                .pid(category_id_)
                .title(title_->text())
                .description(description_->text())
                .content(editor_->toHtml());
        if (auto ok = note.save(); not ok)
            QMessageBox::critical(this, "Error", "Error writing to database.");
        else {
            note_id_ = note.id<i64>();
        }
        accept();
    });
    auto const cancel_button = buttons->button(QDialogButtonBox::Cancel);
    connect(cancel_button, &QPushButton::clicked, [this]() {
        reject();
    });

    auto layout = new QGridLayout;
    layout->addWidget(new QLabel("Tilte"), 0, 0);
    layout->addWidget(title_, 0, 1);
    layout->addWidget(new QLabel("Description"), 1, 0);
    layout->addWidget(description_, 1, 1);
    layout->addWidget(separator, 2, 0, 1, 2);
    layout->addLayout(editor_layout(), 3, 0, 1, 2);
    layout->addWidget(buttons, 4, 0, 1, 2);

    setLayout(layout);
}

void EditDialog::showEvent(QShowEvent* e) {
    auto const parent = QApplication::activeWindow();
    auto const size = parent->size();
    resize(int(size.width() * .7), int(size.height() * .7));
    QDialog::showEvent(e);
}

QVBoxLayout* EditDialog::editor_layout() noexcept {
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
    layout->addWidget(toolbar);
    layout->addWidget(editor_);
    return layout;
}

void EditDialog::populate_font_face_cbx() const noexcept {
    font_face_cbx_->setEditable(false);

    auto const model = new QStandardItemModel(3, 1);
    {
        auto const item = new QStandardItem("face");
        model->setItem(0, item);
    }
    {
        auto const item = new QStandardItem("bold");
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        model->setItem(1, item);
    }
    {
        auto const item = new QStandardItem("italic");
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        model->setItem(2, item);
    }
    {
        auto const item = new QStandardItem("underline");
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        model->setItem(3, item);
    }
    font_face_cbx_->setModel(model);
    font_face_cbx_->setCurrentIndex(0);
    font_face_cbx_->setMinimumWidth(font_face_cbx_->fontMetrics().boundingRect("underline").width() + 60);
}

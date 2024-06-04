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
// Created by Piotr Pszczółkowski on 27.04.24.

#include "EditDialog.hh"
#include "Editor.hh"
#include "Tools.hh"
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
    chainInfoLabel_->setText(qstr::fromStdString(Tools::categoriesChainInfo(note_->pid<i64>())));

    title_->setText(note_.value().qtitle());
    description_->setText(note_.value().qdescription());
    editor_->setHtml(note_.value().qcontent());

    connect(acceptPushButton_, &QPushButton::clicked, [&]() {
        // Sprawdź czy stosowne pola mają zawartość.
        if (not valid())
            return;

        // Utwórz nową wersję notatki.
        auto new_note = note_.value();
        new_note.title(title_->text().trimmed())
                .description((description_->text().trimmed()))
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

    connect(cancelPushButton_, &QPushButton::clicked, [this]() {
        reject();
    });
}


/// Nowa notatka dla wskazanej kategorii.
EditDialog::EditDialog(qi64 const categoryID, QWidget *const parent) : EditDialog(parent) {
    setWindowTitle("New note");
    chainInfoLabel_->setText(qstr::fromStdString(Tools::categoriesChainInfo(categoryID)));

    // Próba zapisania nowej notatki do bazy danych.
    connect(acceptPushButton_, &QPushButton::clicked, [this, categoryID] {
        // Sprawdź czy stosowne pola mają zawartość.
        if (not valid())
            return;
        // Utwórz obiekt nowej notatki
        Note note = Note{}
                .pid(categoryID)
                .title(title_->text().trimmed())
                .description(description_->text().trimmed())
                .content(editor_->toHtml());
        // i ją zapisz do bazy danych.
        if (auto ok = note.save(); not ok) {
            QMessageBox::critical(this, "Error", "Error writing to database.");
            return;
        }
        else
            noteID_ = note.id<i64>();
        accept();
    });

    // Bye, bye
    connect(cancelPushButton_, &QPushButton::clicked, [this]() {
        reject();
    });
}

/********************************************************************
 *                                                                  *
 *                         P R I V A T E                            *
 *                                                                  *
 ********************************************************************/

/// Bazowy kontruktor. Pozostałe konstruktory go wywołują.
EditDialog::EditDialog(QWidget *const parent) :
        QDialog(parent),
        title_{new QLineEdit},
        description_{new QLineEdit},
        editor_{new Editor},
        sizesComboBox_{new QComboBox},
        facesComboBox_{new QComboBox},
        colorsComboBox_{new QComboBox},
        acceptPushButton_{new QPushButton{"Accept"}},
        cancelPushButton_{new QPushButton{"Cancel"}},
        chainInfoLabel_{new QLabel}
{
    setSizeGripEnabled(true);
    populateSizesComboBox();
    populateFacesComboBox();
    populateColorsComboBox();

    auto const buttons{new QHBoxLayout};
    buttons->setContentsMargins(0, 0, 0, 0);
    buttons->addStretch();
    buttons->addWidget(cancelPushButton_);
    buttons->addWidget(acceptPushButton_);
    acceptPushButton_->setDefault(true);

    auto const separator{new QFrame};
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);

    auto layout = new QGridLayout;
    layout->setContentsMargins(8, 8, 8, 4);

    int row = 0;
    layout->addWidget(chainInfoLabel_, row++, 0, 1, 2, Qt::AlignHCenter);

    layout->addWidget(new QLabel("Tilte"), row, 0);
    layout->addWidget(title_, row++, 1);
    layout->addWidget(new QLabel("Description"), row, 0);
    layout->addWidget(description_, row++, 1);
    layout->addWidget(separator, row++, 0, 1, 2);
    layout->addLayout(editor_layout(), row++, 0, 1, 2);
    layout->addLayout(buttons, row, 0, 1, 2);

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

    auto const copyAction = new QAction(QIcon::fromTheme("edit-copy"), "Copy");
    connect(copyAction, &QAction::triggered, [] {
        EventController::instance().send(event::CopyRequest);
    });
    auto const cutAction = new QAction(QIcon::fromTheme("edit-cut"), "Cut");
    connect(cutAction, &QAction::triggered, [] {
        EventController::instance().send(event::CutRequest);
    });
    auto const pasteAction = new QAction(QIcon::fromTheme("edit-paste"), "Paste");
    connect(pasteAction, &QAction::triggered, [] {
        EventController::instance().send(event::PasteRequest);
    });
    auto const fontAction = new QAction(QIcon::fromTheme("preferences-desktop-font"), "Font");
    connect(fontAction, &QAction::triggered, [] {
        EventController::instance().send(event::SelectFontRequest);
    });
    auto const colorAction = new QAction(QIcon::fromTheme("color"), "Color");
    connect(colorAction, &QAction::triggered, [] {
        EventController::instance().send(event::SelectColorRequest);
    });

    toolbar->addWidget(new QLabel("Font settings: "));
    toolbar->addWidget(sizesComboBox_);
    toolbar->addWidget(facesComboBox_);
    toolbar->addWidget(colorsComboBox_);
    toolbar->addWidget(spacer);
    toolbar->addAction(copyAction);
    toolbar->addAction(cutAction);
    toolbar->addAction(pasteAction);
    toolbar->addAction(fontAction);
    toolbar->addAction(colorAction);

    auto const layout{new QVBoxLayout};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(toolbar);
    layout->addWidget(editor_);
    return layout;
}

void EditDialog::
populateSizesComboBox() const noexcept {
    sizesComboBox_->setEditable(false);
    for (int i = 8; i < 26; ++i)
        sizesComboBox_->addItem(qstr::fromStdString(fmt::format("{}", i)));
    sizesComboBox_->setCurrentText("10");
}

void EditDialog::
populateColorsComboBox() const noexcept {
    colorsComboBox_->addItem("Colors");
    colorsComboBox_->addItem("example");
    colorsComboBox_->addItem("example");
    colorsComboBox_->addItem("example");
    colorsComboBox_->addItem("example");

    colorsComboBox_->setItemData(1, color0, Qt::ForegroundRole);
    colorsComboBox_->setItemData(2, color1, Qt::ForegroundRole);
    colorsComboBox_->setItemData(3, color2, Qt::ForegroundRole);
    colorsComboBox_->setItemData(4, color3, Qt::ForegroundRole);
}

void EditDialog::
populateFacesComboBox() const noexcept {
    auto const model = new QStandardItemModel(3, 1);

    auto const root = new QStandardItem("Faces");
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

    facesComboBox_->setModel(model);
    facesComboBox_->setCurrentIndex(0);
    facesComboBox_->setMinimumWidth(facesComboBox_->fontMetrics().boundingRect("underline").width() + 60);

    connect(model, &QStandardItemModel::itemChanged, [bold, italic, underline](auto item) {
        u8 state = Normal;
        if (bold->checkState() & Qt::Checked) state |= Bold;
        if (italic->checkState() & Qt::Checked) state |= Italic;
        if (underline->checkState() & Qt::Checked) state |= Underline;
        // TODO send event?
    });
}

bool EditDialog::
valid() noexcept {
    if (title_->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Warning", "The note must have a title!");
        title_->setFocus();
        return  false;
    }
    if (editor_->toPlainText().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Warning", "The note must contain some content!");
        editor_->setFocus();
        return  false;
    }
    return true;
}


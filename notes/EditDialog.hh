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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>
#include <QFrame>
#include "../shared.hh"
#include "../model/note.hh"
#include <optional>

/*------- forward declarations:
-------------------------------------------------------------------*/
class Editor;
class QLineEdit;
class QVBoxLayout;
class QComboBox;
class QPushButton;
class QLabel;

class EditDialog : public QDialog {
    Q_OBJECT
    enum { FaceRole = Qt::UserRole+1, ColorRole = Qt::UserRole+1 };
    enum { Normal = 0x0, Bold = 0x1, Italic = 0x2, Underline = 0x4 };
    QLineEdit* const title_;
    QLineEdit* const description_;
    Editor* const editor_;
    std::optional<Note> note_{};
    i64 noteID_{};
    QComboBox* const sizesComboBox_;
    QComboBox* const facesComboBox_;
    QComboBox* const colorsComboBox_;
    QPushButton* const acceptPushButton_;
    QPushButton* const cancelPushButton_;
    QLabel* const chainInfoLabel_;

public:
    explicit EditDialog(qi64 category_id, QWidget* = nullptr);
    explicit EditDialog(Note&& note, QWidget* = nullptr);
    ~EditDialog() override = default;

    Note get_note() {
        return std::move(note_.value());
    }

    template<std::integral T>
    T note_id() const noexcept {
        return static_cast<T>(noteID_);
    }

private:
    explicit EditDialog(QWidget* = nullptr);
    bool valid() noexcept;

    void showEvent(QShowEvent*) override;
    void populateSizesComboBox() const noexcept;
    void populateFacesComboBox() const noexcept;
    void populateColorsComboBox() const noexcept;
    [[nodiscard]] QVBoxLayout* editorLayout() const noexcept;

    static inline QWidget* spacer() noexcept {
        auto spacer = new QWidget;
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        return spacer;
    }
    static inline QFrame* horizontalSeparator() noexcept {
        auto const separator{new QFrame};
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        return separator;
    }

    static inline QColor const color0{0xcf, 0x8e, 0x6d};
    static inline QColor const color1{0x2a, 0xac, 0xb8};
    static inline QColor const color2{0x5a, 0xab, 0x73};
    static inline QColor const color3{0xff, 0xc6, 0x6d};
};

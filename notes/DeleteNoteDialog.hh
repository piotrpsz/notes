//
// Created by piotr on 21.05.24.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>
#include "../shared.hh"

class Note;

class DeleteNoteDialog : public QDialog {
public:
    explicit DeleteNoteDialog(Note const& note, QWidget* = nullptr);
    ~DeleteNoteDialog() override = default;
};

//
// Created by piotr on 21.05.24.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QDialog>
#include "../shared.hh"

class DeleteNoteDialog : public QDialog {
public:
    DeleteNoteDialog(qi64 noteID, QWidget* = nullptr);
    ~DeleteNoteDialog() override = default;
};

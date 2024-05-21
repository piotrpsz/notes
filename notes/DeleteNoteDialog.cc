//
// Created by piotr on 21.05.24.
//

#include "DeleteNoteDialog.hh"
#include "../model/note.hh"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

DeleteNoteDialog::DeleteNoteDialog(Note const& note, QWidget *const parent) :
        QDialog(parent)
{
    setWindowTitle("Remove Note");

    auto const buttons{new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No)};
    auto const yes_button = buttons->button(QDialogButtonBox::Yes);
    auto const no_button = buttons->button(QDialogButtonBox::No);
    connect(yes_button, &QPushButton::clicked, [&] { accept(); });
    connect(no_button, &QPushButton::clicked, [&] { reject(); });

    auto const layout{new QVBoxLayout};
    layout->addWidget(new QLabel("Would you like to remove selected note?"));
    layout->addWidget(buttons);
    setLayout(layout);
}

//
// Created by piotr on 27.04.24.
//

#include "NotesTableWidget.hh"
#include "NotesTableToolbar.hh"
#include "NotesTable.hh"
#include <QVBoxLayout>

NotesTableWidget::NotesTableWidget(QWidget *const parent)
    : QWidget(parent)
{
    auto const layout = new QVBoxLayout();
    layout->addWidget(new NotesTableToolbar);
    layout->addWidget(new NotesTable);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

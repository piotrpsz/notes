//
// Created by piotr on 21.05.24.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "Browser.hh"
#include "../common/EventController.hh"
#include "../model/note.hh"
#include <QEvent>
#include <fmt/core.h>

Browser::Browser(QWidget* const parent) : QTextEdit(parent) {
    setAcceptRichText(true);
    setReadOnly(true);

    // ustawienie odstępu pomiędzy liniami
    auto block_fmt = textCursor().blockFormat();
    block_fmt.setLineHeight(5, QTextBlockFormat::LineDistanceHeight);
    textCursor().setBlockFormat(block_fmt);

    // ustawinie długości TAB
    setTabStopDistance(5 * fontMetrics().horizontalAdvance('-') - 2.0);

    EventController::instance().append(this, event::NoteSelected);
}

Browser::~Browser() {
    EventController::instance().remove(this);
}

void Browser::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
        case event::NoteSelected:
            if (auto data = e->data(); data.size() == 1) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    auto noteID = value.toInt();
                    if (auto note = Note::with_id(noteID); note) {
                        auto html = (*note).qcontent();
                        setHtml(html);
                    }
                }
            }
            break;
    }
}
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
    EventController::instance().append(this, event::NoteSelected);
}

void Browser::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
        case event::NoteSelected:
            if (auto data = e->data(); data.size() == 1) {
                if (auto value = data[0]; value.canConvert<int>()) {
                    auto noteID = value.toInt();
                    fmt::print("note id: {}\n", noteID);
                    if (auto note = Note::with_id(noteID); note) {
                        auto html = (*note).qcontent();
                        setHtml(html);
                    }
                }
            }
            break;
    }
}
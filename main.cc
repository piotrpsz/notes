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
// Created by piotr on 09.04.24.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "model/category.hh"
#include "model/note.hh"
#include "sqlite/sqlite.hh"
#include "notes/MainWindow.hh"
#include <QApplication>
#include <fmt/core.h>
#include <string>


bool create_cmds(SQLite const& db, std::vector<std::string> const& commands) noexcept {
    for (auto const& cmd : commands) {
        auto const ok = db.exec(cmd);
        if (not ok) return {};
    }
    return true;
}


bool open_or_create_database() noexcept {
    using namespace std::string_literals;
    static auto const DatabasePath = "/home/piotr/notes.sqlite"s;

    // Try to open.
    if (SQLite::instance().open(DatabasePath))
        return true;

    // Can't open so create one.
    return SQLite::instance().create(DatabasePath, [](SQLite const& db){
        // Create tables.
        return create_cmds(db, Category::CreationCmd) and create_cmds(db, Note::CreationCmd);
    }, false);
}


int main(int argc, char *argv[]) {
    if (not open_or_create_database()) {
        fmt::print(stderr, "Database creation error\n");
        return 1;
    }

    QCoreApplication::setApplicationName("notes");
    QCoreApplication::setApplicationVersion("0.0.1");
    QCoreApplication::setOrganizationName("Piotr Pszczółkowski");
    QCoreApplication::setOrganizationDomain("beesoft.pl");

    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return QApplication::exec();
}

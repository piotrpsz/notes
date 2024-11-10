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
#include "notes/Settings.hh"
#include "shared.hh"
#include <QApplication>
#include <QDir>
#include <fmt/core.h>
#include <string>
#include <iostream>
#include <format>
using namespace std;

bool create_cmds(SQLite const& db, std::vector<std::string> const& commands) noexcept {
    for (auto const& cmd : commands) {
        auto const ok = db.exec(cmd);
        if (not ok) return {};
    }
    return true;
}

/// Open the database, if that fails create a new database.
bool open_or_create_database() noexcept {
    auto const database_dir = shared::home_dir() + "/.beesoft";
    if (!shared::create_dirs(database_dir))
        return {};

    auto const database_path = database_dir + "/notes.sqlite";

    // Try to open.
    if (SQLite::instance().open(database_path))
        return true;

    // Can't open so create one.
    return SQLite::instance().create(database_path, [](SQLite const& db){
        // Create tables.
        return create_cmds(db, Category::CreationCmd) and create_cmds(db, Note::CreationCmd);
    }, false);
}


int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName(shared::PROGRAM);
    QCoreApplication::setApplicationVersion(settings::appVersion().c_str());
    QCoreApplication::setOrganizationName(shared::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(shared::DOMAIN);

    if (not open_or_create_database()) {
        cout << format("Database could not be created. Exiting...\n");
        return 1;
    }

    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return QApplication::exec();
}

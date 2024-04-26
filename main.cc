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
#include "model/category.h"
#include "model/note.h"
#include "sqlite/sqlite.h"
#include "notes/MainWindow.h"
#include <QApplication>
#include <fmt/core.h>
#include <string>

#include "common/Datime.h"
#include <fmt/core.h>

bool open_or_create_database() noexcept {
    using namespace std::string_literals;
    static auto const DatabasePath = "/home/piotr/notes.sqlite"s;

    // Try to open.
    if (SQLite::instance().open(DatabasePath))
        return true;

    // Can't open so create one.
    return SQLite::instance().create(DatabasePath, [](SQLite const& db){
        // Create tables.
        return db.exec(Category::CreateTable) and
                db.exec(Category::CreateIndexId) and
                db.exec(Category::CreateIndexName) and
                db.exec(Note::Create);
    });
}


int main(int argc, char *argv[]) {
    Datime dt{"2024-04-26 21:55:38"};
    fmt::print("{}\n", dt.str());
    return 0;


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

//
// Created by piotr on 09.04.24.
//
#pragma once

#include <string>
#include <vector>

class Note {
public:
    static inline std::vector<std::string> const CreationCmd{
            {
            R"(
                CREATE TABLE note (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    pid INTEGER,
                    title TEXT NOT NULL COLLATE NOCASE,
                    content TEXT NOT NULL,
                    created DATETIME,
                    updated DATETIME
                );)"
            },
            {
            R"(
                CREATE TRIGGER insert_note AFTER INSERT ON note
                BEGIN
                    UPDATE note SET created = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                    UPDATE note SET updated = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                END;)"
            },
            {
            R"(
                CREATE TRIGGER update_note AFTER UPDATE ON note
                BEGIN
                    UPDATE note SET updated = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                END;)"
            },

    };


//    static inline std::string const Create = R"(
//        CREATE TABLE note (
//            id INTEGER PRIMARY KEY AUTOINCREMENT,
//            pid INTEGER,
//            title TEXT NOT NULL COLLATE NOCASE,
//            content TEXT NOT NULL
//        );
//    )";
};


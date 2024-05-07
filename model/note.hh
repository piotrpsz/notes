//
// Created by piotr on 09.04.24.
//
#pragma once

#include "../shared.hh"
#include "../common/Datime.hh"
#include "../sqlite/sqlite.hh"
#include <string>
#include <vector>
#include <optional>

class Note {
    i64 id_{};
    i64 pid_{};   // category id
    std::string title_{};
    std::string description_{};
    std::string content_{};
public:
    explicit Note(Row&& row);
    static std::optional<Note> with_id(i64 id, std::string const& fields = "*") noexcept;
    static std::vector<Note> notes(std::vector<i64> ids) noexcept;

public:
    static inline std::vector<std::string> const CreationCmd{
            {
            R"(
                CREATE TABLE note (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    pid INTEGER,
                    title TEXT NOT NULL COLLATE NOCASE,
                    description TEXT COLLATE NOCASE,
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
};


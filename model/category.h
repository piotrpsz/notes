//
// Created by piotr on 09.04.24.
//

#pragma once

#include <string>
#include <vector>

class Category {
public:
    static inline std::vector<std::string> const CreationCmd = {
            {
            R"(
                CREATE TABLE category (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    pid INTEGER,
                    created DATETIME,
                    updated DATETIME,
                    name TEXT NOT NULL COLLATE NOCASE
                );)"
            },
            {
                R"(CREATE UNIQUE INDEX category_pid_index ON category(pid, id);)"
            },
            {
                R"(CREATE UNIQUE INDEX category_pid_name ON category(pid, name);)"
            },
            {R"(
                CREATE TRIGGER insert_category AFTER INSERT ON category
                BEGIN
                    UPDATE category SET created = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                    UPDATE category SET updated = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                END;
            )"},
    };
};


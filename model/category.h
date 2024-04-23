//
// Created by piotr on 09.04.24.
//

#pragma once
#include <string>

class Category {
public:
    static inline std::string const CreateTable = R"(
        CREATE TABLE category (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pid INTEGER,
            name TEXT NOT NULL COLLATE NOCASE
        );
    )";
    static inline std::string const CreateIndexId = R"(
        CREATE UNIQUE INDEX category_pid_index ON category(pid, id);
    )";
    static inline std::string const CreateIndexName = R"(
        CREATE UNIQUE INDEX category_pid_name ON category(pid, name);
    )";

};


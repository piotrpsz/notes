//
// Created by piotr on 09.04.24.
//
#pragma once
#include <string>

class Note {
public:
    static inline std::string const Create = R"(
        CREATE TABLE note (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pid INTEGER,
            title TEXT NOT NULL COLLATE NOCASE,
            content TEXT NOT NULL
        );
    )";
};


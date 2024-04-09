//
// Created by piotr on 09.04.24.
//

#pragma once
#include <string>

class Category {
public:
    static inline std::string const Create = R"(
        CREATE TABLE category (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            pid INTEGER,
            name TEXT NOT NULL COLLATE NOCASE
        );
    )";
};


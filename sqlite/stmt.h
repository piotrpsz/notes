//
// Created by piotr on 08.04.24.
//
#pragma once

#include <sqlite3.h>
#include "query.h"

class Stmt {
    sqlite3* db_{};
    sqlite3_stmt* stmt_{};
public:
    Stmt() = delete;
    explicit Stmt(sqlite3* db) : db_{db} {}
    ~Stmt();

    Stmt(Stmt const&) = default;
    Stmt(Stmt&&) = default;
    Stmt& operator=(Stmt const&) = default;
    Stmt& operator=(Stmt&&) = default;

    bool exec(query_t const& query) noexcept;

};

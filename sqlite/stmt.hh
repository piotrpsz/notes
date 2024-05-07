//
// Created by piotr on 08.04.24.
//
#pragma once

#include <optional>
#include <sqlite3.h>
#include "query.hh"
#include "result.hh"

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

    bool exec_without_result(query_t const& query) noexcept;
    std::optional<Result> exec_with_result(query_t const& query) noexcept;
};

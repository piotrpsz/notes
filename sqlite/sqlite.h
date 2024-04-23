//
// Created by piotr on 08.04.24.
//
#pragma once

#include "logger.h"
#include "../shared.h"
#include "stmt.h"
#include "query.h"
#include <sqlite3.h>
#include <string>
#include <array>
#include <functional>

class SQLite {
    static inline std::array<u8, 16> Header = {
            0x53, 0x51, 0x4c, 0x69, 0x74, 0x65, 0x20, 0x66,
            0x6f, 0x72, 0x6d, 0x61, 0x74, 0x20, 0x33, 0x00
    };
    sqlite3 *db_ = nullptr;
public:
    static i64 const InvalidRowid = -1;
    static inline std::string InMemory{":memory:"};

    // singleton
    static SQLite& instance() noexcept {
        static SQLite db;
        return db;
    }

    // no copy
    SQLite(SQLite const&) = delete;
    SQLite& operator=(SQLite const&) = delete;
    // no move
    SQLite(SQLite&&) = delete;
    SQLite& operator=(SQLite&&) = delete;

    static std::string version() noexcept {
        return sqlite3_version;
    }

    bool close() noexcept;
    bool open(fs::path const &path, bool read_only = false) noexcept;
    bool create(fs::path const &path, std::function<bool(SQLite const&)> const& lambda, bool override = false) noexcept;

    //------- EXEC ----------------------------------------
    [[nodiscard]] bool exec(query_t const& query) const noexcept {
        return Stmt(db_).exec_without_result(query);
    }
    template<typename... T>
    [[nodiscard]] bool exec(std::string const& str, T... args) const noexcept {
        return exec(query_t{str, args...});
    }
    //------- INSERT --------------------------------------
    [[nodiscard]] i64 insert(query_t const& query) const noexcept {
        Stmt stmt(db_);
        if (stmt.exec_without_result(query))
            return sqlite3_last_insert_rowid(db_);
        return InvalidRowid;
    }
    template<typename... T>
    [[nodiscard]] i64 insert(std::string const& str, T... args) const noexcept {
        return insert(query_t{str, args...});
    }
    //------- UPDATE --------------------------------------
    [[nodiscard]] bool update(query_t const& query) const noexcept {
        return Stmt(db_).exec_without_result(query);
    }
    template<typename... T>
    [[nodiscard]] bool update(std::string const& str, T... args) const noexcept {
        return update(query_t{str, args...});
    }
    //------- SELECT --------------------------------------
    [[nodiscard]] std::optional<result_t> select(query_t const& query) const noexcept {
        return Stmt(db_).exec_with_result(query);
    }
    template<typename... T>
    [[nodiscard]] std::optional<result_t> select(std::string const& str, T... args) const noexcept {
        return select(query_t{str, args...});
    }

private:
    SQLite() {
        sqlite3_initialize();
    }
};

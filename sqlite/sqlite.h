//
// Created by piotr on 08.04.24.
//
#pragma once

#include "logger.h"
#include "../types.h"
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
    bool create(fs::path const &path, std::function<bool(SQLite*)> const& lambda, bool override = false) noexcept;

private:
    SQLite() {
        sqlite3_initialize();
    }
};

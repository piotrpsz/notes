//
// Created by piotr on 08.04.24.
//

#include "sqlite.hh"
#include <fmt/core.h>

// Close database (if possible).
bool SQLite::close() noexcept {
    if (db_) {
        if (sqlite3_close_v2(db_) != SQLITE_OK) {
            LOG_ERROR(db_);
            return false;
        }
        db_ = nullptr;
    }
    return true;
}

// Open database with given path.
bool SQLite::open(fs::path const &path, bool const read_only) noexcept {
    if (db_ not_eq nullptr) {
        fmt::print(stderr, "database is already opened\n");
        return false;
    }
    if (path == InMemory) {
        fmt::print(stderr, "database in memory can't be opened (use create)\n");
        return false;
    }

    auto const flags = read_only ? SQLITE_READONLY : SQLITE_OPEN_READWRITE;
    auto const database_path = path.string();
    if (SQLITE_OK == sqlite3_open_v2(database_path.c_str(), &db_, flags, nullptr)) {
        fmt::print("database opened: {}\n", path.string());
        return true;
    }
    LOG_ERROR(db_);
    db_ = nullptr;
    return false;
}

// Create a new database file.
bool SQLite::create(fs::path const &path, std::function<bool(SQLite const&)> const& lambda, bool override) noexcept {
    if (db_ not_eq nullptr) {
        fmt::print(stderr, "database is already opened\n");
        return false;
    }
    if (lambda == nullptr) {
        fmt::print(stderr, "operations to be performed on created database were not specified\n");
        return false;
    }

    std::error_code err{};
    // If it is not an in-memory database, create a new database file.
    // If the file already exists, delete it first if the user so desires.
    if (path not_eq InMemory) {
        if (fs::exists(path, err)) {
            if (override) { // is this what the user wants?
                if (not fs::remove(path)) {
                    fmt::print(stderr, "database file could not be deleted\n");
                    return false;
                }
            }
        }
        else if (err)
            fmt::print(stderr, "database already exist {}\n", err.message());
    }

    auto const flags = SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE;
    auto const database_path = path.string();
    if (SQLITE_OK == sqlite3_open_v2(database_path.c_str(), &db_, flags, nullptr)) {
        if (not lambda(*this))
            return false;
        fmt::print("The database created successfully: {}\n", path.string());
        return true;
    }
    LOG_ERROR(db_);
    return false;
}

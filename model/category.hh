//
// Created by piotr on 09.04.24.
//

#pragma once

#include "../shared.hh"
#include "../common/Datime.hh"
#include "../sqlite/sqlite.hh"

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <fmt/core.h>
#include <QString>


class Category {
    i64 id_{};
    i64 pid_{};
    std::string name_{};

public:
    explicit Category(Row row);
    Category() = default;

    // Getters
    [[nodiscard]] qi64 qid() const noexcept { return qi64(id_); }
    [[nodiscard]] i64 id() const noexcept { return id_; }
    [[nodiscard]] qi64 qpid() const noexcept { return qi64(pid_); }
    [[nodiscard]] i64 pid() const noexcept { return pid_; }
    [[nodiscard]] qstr qname() const noexcept { return qstr::fromStdString(name_); }
    [[nodiscard]] std::string const& name() const noexcept { return name_; }
    // Setters
    Category& id(std::integral auto const value) noexcept {
        id_ = i64(value);
        return *this;
    }
    Category& pid(std::integral auto  const value) noexcept {
        pid_ = i64(value);
        return *this;
    }
    Category& name(qstr const& value) noexcept {
        name_ = value.toStdString();
        return *this;
    }
    Category& name(std::string const& value) noexcept {
        name_ = value;
        return *this;
    }

    static std::optional<Category> with_id(i64 id, std::string const& fields = "*") noexcept;
    static std::vector<Category> with_pid(i64 pid, std::string const& fields = "*") noexcept;
    static std::optional<std::string> name_with_id(i64 id) noexcept;
    static std::optional<std::vector<Category>> all() noexcept;



    [[nodiscard]] std::string str() const noexcept {
        return fmt::format("id:{}, pid:{}, name:{}", id_, pid_, name_);
    }
    static std::optional<std::pair<std::vector<i64>, std::vector<std::string>>> chain_for(i64 id) noexcept;
    static std::optional<std::vector<std::string>> names_chain_for(i64 id) noexcept;
    static std::vector<i64> ids_subchain_for(i64 id) noexcept;

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


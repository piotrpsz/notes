//
// Created by piotr on 09.04.24.
//

#pragma once

#include "../shared.h"
#include "../common/Datime.h"
#include "../sqlite/sqlite.h"
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <fmt/core.h>


class Category {
    i64 id_{};
    i64 pid_{};
    DateTime created_{};
    DateTime updated_{};
    std::string name_{};

public:
    explicit Category(Row row);
    static std::optional<Category> with_id(i64 id, std::string const& fields = "*") noexcept;
    static std::optional<std::string> name_with_id(i64 id) noexcept;

    [[nodiscard]] std::string str() const noexcept {
        return fmt::format("id:{}, pid:{}, name:{}, created:{}, updated:{}", id_, pid_, name_, created_.str(), updated_.str());
    }
    static std::optional<std::pair<std::vector<i64>, std::vector<std::string>>> chain_for(i64 id) noexcept;
    static std::optional<std::vector<std::string>> names_chain_for(i64 id) noexcept;

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


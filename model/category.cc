//
// Created by piotr on 09.04.24.
//

#include "category.hh"

Category::Category(Row row) {
    if (auto field = row["id"]; field)
        id_  = field.value().value().int64();
    if (auto field = row["pid"]; field)
        pid_  = field.value().value().int64();
    if (auto field = row["name"]; field)
        name_  = field.value().value().str();
}

std::optional<Category> Category::
with_id(i64 const id, std::string const& fields) noexcept {
    auto cmd = fmt::format("SELECT {} FROM category WHERE id=?", fields);
    if (auto result = SQLite::instance().select(cmd, id); result)
        if (auto data = result.value(); not data.empty())
            return Category(data[0]);

    return {};
}

std::vector<Category> Category::
with_pid(i64 const pid, std::string const& fields) noexcept {
    std::vector<Category> data{};

    auto cmd = fmt::format("SELECT {} FROM category WHERE pid=?", fields);
    if (auto opt = SQLite::instance().select(cmd, pid); opt)
        if (auto result = opt.value(); not result.empty())
            for (auto const& row : result)
                data.emplace_back(row);

    data.shrink_to_fit();
    return data;
}

std::optional<std::string> Category::
name_with_id(i64 id) noexcept {
    if (auto opt = SQLite::instance().select("SELECT name FROM category WHERE id=?", id); opt) {
        if (auto result = opt.value(); not result.empty()) {
            auto row = result[0];
            if (auto field = row["name"]; field)
                return field.value().value().str();
        }
    }
    return {};
}

std::optional<std::pair<std::vector<i64>, std::vector<std::string>>> Category::
chain_for(i64 const id) noexcept {
    std::vector<i64> ids{};
    std::vector<std::string> names{};

    auto current_id = id;
    while (auto opt = with_id(current_id)) {
        auto category = opt.value();
        ids.push_back(category.id_);
        names.push_back(std::move(category.name_));
        current_id = category.pid_;
        if (current_id == 0) break;
    }
    if (ids.empty() and names.empty())
        return {};

    std::reverse(ids.begin(), ids.end());
    std::reverse(names.begin(), names.end());
    return std::make_pair(ids, names);
}

std::optional<std::vector<std::string>> Category::
names_chain_for(i64 id) noexcept {
    std::vector<std::string> names{};
    auto current_id = id;

    while (auto opt = with_id(current_id, "pid, name")) {
        auto category = opt.value();
        names.push_back(std::move(category.name_));
        if (category.pid_ == 0) break;
        current_id = category.pid_;
    }

    if (names.empty())
        return {};

    std::reverse(names.begin(), names.end());
    return names;
}

std::vector<i64> Category::
ids_subchain_for(i64 const id) noexcept {
    std::vector<i64> ids{id};

    for (auto const& child : with_pid(id)) {
        auto data = ids_subchain_for(child.id_);
        std::copy(data.begin(), data.end(), std::back_inserter(ids));
    }

    ids.shrink_to_fit();
    std::sort(ids.begin(), ids.end());
    return ids;
}
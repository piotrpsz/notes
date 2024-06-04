//
// Created by piotr on 09.04.24.
//

#include "category.hh"

using namespace std;

Category::Category(Row&& row) {
    if (auto field = row["id"]; field)
        id_  = field.value().value().int64();
    if (auto field = row["pid"]; field)
        pid_  = field.value().value().int64();
    if (auto field = row["name"]; field)
        name_  = field.value().value().str();
}

optional<vector<Category>> Category::
all() noexcept {
    using namespace std::string_literals;
    auto query{"SELECT * FROM category"s};

    if (auto selected = SQLite::instance().select(query); selected) {
        if (auto result = selected.value(); not result.empty()) {
            vector<Category> vec{};
            vec.reserve(result.size());
            for (auto&& row: result)
                vec.emplace_back(std::move(row));
            return vec;
        }
    }

    return {};
}

std::optional<Category> Category::
withID(i64 const id, std::string const& fields) noexcept {
    auto query = fmt::format("SELECT {} FROM category WHERE id=?", fields);
    if (auto result = SQLite::instance().select(query, id); result)
        if (auto data = result.value(); not data.empty())
            return Category(data[0]);

    return {};
}

std::vector<Category> Category::
withPID(i64 const pid, std::string const& fields) noexcept {
    std::vector<Category> data{};

    auto query = fmt::format("SELECT {} FROM category WHERE pid=?", fields);
    if (auto opt = SQLite::instance().select(query, pid); opt)
        if (auto result = opt.value(); not result.empty())
            for (auto&& row : result)
                data.emplace_back(std::move(row));

    data.shrink_to_fit();
    return data;
}

std::optional<std::string> Category::
nameWithID(i64 id) noexcept {
    if (auto opt = SQLite::instance().select("SELECT name FROM category WHERE id=?", id); opt) {
        if (auto result = opt.value(); result.size() == 1) {
            if (auto field = result[0]["name"]; field)
                return (*field).value().str();
        }
    }
    return {};
}

std::optional<std::vector<std::string>> Category::
namesChainFor(i64 const id) noexcept {
    std::vector<std::string> names{};
    auto currentID = id;

    while (auto opt = withID(currentID, "pid, name")) {
        auto category = *opt;
        names.push_back(std::move(category.name_));
        if (category.pid_ == 0) break;
        currentID = category.pid_;
    }

    if (names.empty())
        return {};

    std::reverse(names.begin(), names.end());
    return names;
}

std::vector<i64> Category::
idsSubchainFor(i64 const id) noexcept {
    std::vector<i64> ids{id};

    for (auto const& child : withPID(id)) {
        auto data = idsSubchainFor(child.id_);
        std::copy(data.begin(), data.end(), std::back_inserter(ids));
    }

    ids.shrink_to_fit();
    std::sort(ids.begin(), ids.end());
    return ids;
}

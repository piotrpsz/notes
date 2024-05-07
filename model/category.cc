//
// Created by piotr on 09.04.24.
//

#include "category.hh"

Category::Category(Row row) {
    if (auto field = row["id"]; field)
        id_  = field.value().value().int64();
    if (auto field = row["pid"]; field)
        pid_  = field.value().value().int64();
    if (auto field = row["created"]; field)
        created_  = DateTime(field.value().value().str());
    if (auto field = row["updated"]; field)
        updated_  = DateTime(field.value().value().str());
    if (auto field = row["name"]; field)
        name_  = field.value().value().str();
}

std::optional<Category> Category::
with_id(i64 const id) noexcept {
    if (auto result = SQLite::instance().select("SELECT * FROM category WHERE id=?", id); result)
        if (auto data = result.value(); not data.empty())
            return Category(data[0]);

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

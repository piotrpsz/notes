//
// Created by piotr on 09.04.24.
//

#include "note.hh"
#include "../sqlite/sqlite.hh"
#include <numeric>
#include <string>
#include <fmt/core.h>
#include <range/v3/range.hpp>

Note::Note(Row &&row) {
    if (auto f = row["id"]; f)
        id_ = (*f).value().int64();
    if (auto f = row["pid"]; f)
        pid_ = (*f).value().int64();
    if (auto f = row["title"]; f)
        title_ = (*f).value().str();
    if (auto f = row["description"]; f)
        description_ = (*f).value().str();
    if (auto f = row["content"]; f)
        content_ = (*f).value().str();
}

std::optional<Note> Note::
with_id(i64 const id, std::string const &fields) noexcept {
    auto cmd = fmt::format("SELECT {} FROM note WHERE id=?", fields);
    if (auto result = SQLite::instance().select(cmd, id); result)
        if (auto data = result.value(); not data.empty())
            return Note(data[0]);

    return {};
}

bool Note::
insert() noexcept {
    using namespace std::string_literals;
    auto cmd{"INSERT INTO note (pid, title, description, content) VALUES (?,?,?,?)"s};
    if (auto id = SQLite::instance().insert(cmd, pid_, title_, description_, content_); id > 0) {
        id_ = id;
        return true;
    }
    return {};
}

bool Note::
update() noexcept {
    using namespace std::string_literals;
    auto cmd{"UPDATE note SET title=?, description=?, content=? WHERE id=?"s};
    return SQLite::instance().update(cmd, title_, description_, content_);
}

// Odczyt z bazy danych notatek których numery ID są podane jako argument w wektorze 'ids'.
std::vector<Note> Note::
notes(std::vector<i64> ids) noexcept {
    // konwersja liczb na tekst
    auto data = ids |
               ranges::views::transform([](i64 const i) { return std::to_string(i); }) |
               ranges::to<std::vector>();

    // połączenie (join) tekstów z użyciem przecinka
    auto acc = std::accumulate(
            std::next(data.begin()),
            data.end(),
            data[0],
            [](auto a, auto b) {
                return fmt::format("{},{}", a, b);
            });



    std::vector<Note> vec{};
    auto cmd = fmt::format("SELECT * FROM note WHERE pid IN ({})", acc);

    if (auto opt = SQLite::instance().select(cmd); opt) {
        for (auto row : opt.value()) {
            vec.emplace_back(std::move(row));
        }
    }

    vec.shrink_to_fit();
    return vec;
}

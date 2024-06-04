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
    if (auto f = row["name"]; f)
        category_ = (*f).value().str();
}

bool Note::
remove(i64 const id) noexcept {
    return SQLite::instance().exec("DELETE FROM note WHERE id=?", id);
}

/// Sprawdzenie czy wskazama kategoria posiada notatkę z podanum tytułem.
/// \param categoryID - numer ID sprawdzanej kategorii
/// \param title - tytuł poszukiwanej notatki.
/// \return True jeśli znaleziono notatkę, False w przeciwnym przypadku
bool Note::
containsParentTheNoteWithTitle(i64 categoryID, std::string const& title) noexcept {
    auto countQuery = "SELECT COUNT(*) as count FROM note WHERE pid=? AND title=?";
    if (auto result = SQLite::instance().select(countQuery, categoryID, title); result) {
        if (auto data = *result; data.size() == 1) {
            auto row = data[0];
            if (auto field = row["count"]; field) {
                if (auto n = (*field).value().int64(); n > 0)
                    return true;
            }
        }
    }
    return {};
}

/// Odczyt danych notatki posiadającej wskazany numer ID.
std::optional<Note> Note::
withID(i64 const noteID, std::string const &fields) noexcept {
    auto selectQuery = fmt::format("SELECT note.*, category.name FROM note INNER JOIN category ON category.id=note.pid WHERE note.id=?");
    if (auto result = SQLite::instance().select(selectQuery, noteID); result)
        if (auto data = *result; data.size() == 1)
            return Note(data[0]);

    return {};
}

/// Dodanie do bazy danych wiersza z nową notatką.
/// \return True jeśli zapis zakończył się sukcesem, False w przeciwnym przypadku.
/// \remark Po zapisie zakończonym sukcesem zapamiętujemy numer ID.
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

/// Uaktualnienie danych notatki. \n
/// Wiersz z danymi notatki identyfikowany jest przez jej numer ID.
bool Note::
update() noexcept {
    using namespace std::string_literals;
    auto cmd{"UPDATE note SET pid=?, title=?, description=?, content=? WHERE id=?"s};
    return SQLite::instance().update(cmd, pid_, title_, description_, content_, id_);
}

/// Odczyt z bazy danych notatek których numery ID są podane jako argument w wektorze 'ids'.
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


// "SELECT note.*, category.name FROM note INNER JOIN category ON category.id=note.pid WHERE note.id=?"
    std::vector<Note> vec{};
    auto cmd = fmt::format("SELECT note.*, category.name FROM note INNER JOIN category ON category.id=note.pid WHERE note.pid IN ({})", acc);

    if (auto opt = SQLite::instance().select(cmd); opt) {
        for (auto row : opt.value()) {
            vec.emplace_back(std::move(row));
        }
    }

    vec.shrink_to_fit();
    return vec;
}

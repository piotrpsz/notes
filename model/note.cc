//
// Created by piotr on 09.04.24.
//

#include "note.hh"

Note::Note(Row&& row) {
    if (auto f = row["id"]; f)
        id_ = (*f).value().int64();
    if (auto f = row["pid"]; f)
        pid_ = (*f).value().int64();
    if (auto f = row["title"]; f)
        title_  = (*f).value().str();
    if (auto f = row["description"]; f)
        description_  = (*f).value().str();
    if (auto f = row["content"]; f)
        content_  = (*f).value().str();
}

std::optional<Note> Note::
with_id(i64 const id, std::string const& fields) noexcept {
    auto cmd = fmt::format("SELECT {} FROM category WHERE id=?", fields);
    if (auto result = SQLite::instance().select(cmd, id); result)
        if (auto data = result.value(); not data.empty())
            return Note(data[0]);

    return {};
}

std::vector<Note> Note::
notes(std::vector<i64>&& ids) noexcept {

}

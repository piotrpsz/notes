//
// Created by piotr on 09.04.24.
//
#pragma once

#include "../shared.hh"
#include "../common/Datime.hh"
#include "../sqlite/sqlite.hh"
#include "../notes/types.hh"
#include <QString>
#include <string>
#include <vector>
#include <optional>

class Note {
    i64 id_{};
    i64 pid_{};   // category id
    std::string title_{};
    std::string description_{};
    std::string content_{};
    std::string category_{};
public:
    Note() = default;
    explicit Note(Row&& row);
    Note(Note&&) = default;
    Note& operator=(Note&&) = default;
    Note(Note const&) = default;
    Note& operator=(Note const&) = default;
    ~Note() = default;

    static bool remove(i64 id) noexcept;
    static std::optional<Note> withID(i64 id, std::string const& fields = "*") noexcept;
    static std::vector<Note> notes(std::vector<i64> ids) noexcept;
    static bool containsParentTheNoteWithTitle(i64 categoryID, std::string const& title) noexcept;

    [[nodiscard]] std::string const& title() const noexcept { return title_; }
    [[nodiscard]] QString qtitle() const noexcept { return QString::fromStdString(title_); }
    [[nodiscard]] QString qdescription() const noexcept { return QString::fromStdString(description_); }
    [[nodiscard]] QString qcontent() const noexcept { return QString::fromStdString(content_); }
    [[nodiscard]] QString qcategory() const noexcept { return QString::fromStdString(category_); }

    template<std::integral T>
    T id() const noexcept {
        return static_cast<T>(id_);
    }
    template<std::integral T>
    T pid() const noexcept {
        return static_cast<T>(pid_);
    }

    Note& pid(qi64 const value) noexcept {
        pid_ = i64(value);
        return *this;
    }

    Note& title(qstr const& txt) noexcept {
        title_ = txt.toStdString();
        return *this;
    }
    Note& description(qstr const& txt) noexcept {
        description_ = txt.toStdString();
        return *this;
    }
    Note& content(qstr const& txt) noexcept {
        content_ = txt.toStdString();
        return *this;
    }

    bool save() noexcept { return (id_ == 0) ? insert() : update(); }
    bool insert() noexcept;
    bool update() noexcept;

public:
    static inline std::vector<std::string> const CreationCmd{
            {
            R"(
                CREATE TABLE note (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    pid INTEGER,
                    title TEXT NOT NULL COLLATE NOCASE,
                    description TEXT COLLATE NOCASE,
                    content TEXT NOT NULL,
                    created DATETIME,
                    updated DATETIME
                );)"
            },
            {
                    R"(CREATE UNIQUE INDEX note_pid_index ON note(pid, title, description);)"
            },
            {
            R"(
                CREATE TRIGGER insert_note AFTER INSERT ON note
                BEGIN
                    UPDATE note SET created = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                    UPDATE note SET updated = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                END;)"
            },
            {
            R"(
                CREATE TRIGGER update_note AFTER UPDATE ON note
                BEGIN
                    UPDATE note SET updated = DATETIME('NOW', 'localtime') WHERE ROWID = new.ROWID;
                END;)"
            },

    };
};


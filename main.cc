#include "shared.h"
#include "model/category.h"
#include "model/note.h"
#include "sqlite/sqlite.h"
#include <fmt/core.h>
#include <string>

int main(int argc, char *argv[]) {
    auto const ok = SQLite::instance().create("/home/piotr/notes.sqlite", [](SQLite const& db){
        using namespace std::string_literals;
        if (db.exec(Category::Create)) {
            if (db.exec(Note::Create)) {
                if (db.insert("INSERT INTO category (pid, name) VALUES (?,?)", 1959, "Piotrek"s) == -1)
                    return false;
                if (db.insert("INSERT INTO category (pid, name) VALUES (?,?)", 1987, "Artur & Błażej"s) == -1)
                    return false;
                if (db.insert("INSERT INTO note (pid, title, content) VALUES(?, ?, ?)", 2043, "tuuł", "notatka") == -1)
                    return false;
            }
        }
        return true;
    }, true);

    if (not ok) {
        fmt::print("ERROR\n");
        return 1;
    }

    {

        if (SQLite::instance().update("UPDATE category SET pid=?, name=? WHERE id=?", 2099, "Yolanda", 2))
            fmt::print("updated\n");

        if (auto result = SQLite::instance().select("SELECT * FROM category"); result) {
            auto data = result.value();
            for (auto it = data.begin(); it != data.end(); ++it)
                fmt::print("{}\n", it->description());
        }
    }



//    SQLite::instance().create(SQLite::InMemory, [](SQLite const& db){
//        if (not db.exec(Category::Create)) return false;
//        return true;
//    });


//    QApplication a(argc, argv);
//    QPushButton button("Hello world!", nullptr);
//    button.resize(200, 100);
//    button.show();
//    return QApplication::exec();
    return 0;
}

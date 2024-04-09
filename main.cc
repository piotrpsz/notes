#include "shared.h"
#include "model/category.h"
#include "model/note.h"
#include "sqlite/sqlite.h"
#include <fmt/core.h>
#include <string>


bool open_or_create_database() noexcept {
    using namespace std::string_literals;
    static auto const DatabasePath = "/home/piotr/notes.sqlite"s;

    // Try to open.
    if (SQLite::instance().open(DatabasePath))
        return true;

    // Can't open so create one.
    return SQLite::instance().create(DatabasePath, [](SQLite const& db){
        // Create tables.
        return db.exec(Category::Create) and db.exec(Note::Create);
    });
}


int main(int argc, char *argv[]) {
    if (not open_or_create_database()) {
        fmt::print(stderr, "Database creation error\n");
        return 1;
    }

//    QApplication a(argc, argv);
//    QPushButton button("Hello world!", nullptr);
//    button.resize(200, 100);
//    button.show();
//    return QApplication::exec();
    return 0;
}

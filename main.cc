#include "shared.h"
#include "sqlite/sqlite.h"
#include <fmt/core.h>

int main(int argc, char *argv[]) {
    std::string str{"ala , bela  , gamma  , omega  "};
    auto vs = shared::split(str, ',');
    for (auto const& item : vs)
        fmt::print("|{}|\n", item);

    auto rs = shared::join(vs);
    fmt::print("|{}|\n\n", rs);

    auto bytes = shared::random_bytes(10);

    fmt::print("{}\n", shared::bytes2str(bytes));


//    QApplication a(argc, argv);
//    QPushButton button("Hello world!", nullptr);
//    button.resize(200, 100);
//    button.show();
//    return QApplication::exec();
    return 0;
}

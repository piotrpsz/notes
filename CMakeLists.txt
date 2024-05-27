cmake_minimum_required(VERSION 3.27)
project(notes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(SQLite3 REQUIRED)
find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)

#======== date Howard Hinnant
find_package(date REQUIRED)

include(FetchContent)
FetchContent_Declare(
        glaze
        GIT_REPOSITORY https://github.com/stephenberry/glaze.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(notes main.cc
        sqlite/sqlite.cc
        sqlite/sqlite.hh
        sqlite/logger.hh
        sqlite/stmt.cc
        sqlite/stmt.hh
        shared.hh
        sqlite/value.hh
        sqlite/field.hh
        sqlite/row.hh
        sqlite/result.hh
        sqlite/query.hh
        model/category.cc
        model/category.hh
        model/note.cc
        model/note.hh
        notes/MainWindow.cc
        notes/MainWindow.hh
        notes/Settings.hh
        notes/types.hh
        notes/CategoryTree.cc
        notes/CategoryTree.hh
        notes/NotesWorkspace.cc
        notes/NotesWorkspace.hh
        notes/NotesTable.cc
        notes/NotesTable.hh
        notes/NoteWidget.cc
        notes/NoteWidget.hh
        common/Event.hh
        common/EventController.hh
        common/Datime.hh
        notes/NotesTableWidget.cc

        notes/NotesTableWidget.hh
        notes/NotesTableToolbar.cc
        notes/NotesTableToolbar.hh
        notes/EditDialog.cc
        notes/EditDialog.hh
        notes/Editor.cc
        notes/Editor.hh
        model/StoreCategory.cc
        model/StoreCategory.hh
        notes/Browser.cc
        notes/Browser.hh
        notes/DeleteNoteDialog.cc
        notes/DeleteNoteDialog.hh
        common/EventController.cc
        notes/Tools.hh
        notes/TreeDialog.cpp
        notes/TreeDialog.hh
        notes/CategoryTreeBrowser.cpp
        notes/CategoryTreeBrowser.hh
)
target_link_libraries(notes
        Qt::Core Qt::Gui Qt::Widgets
        sqlite3
        fmt::fmt
        range-v3::meta range-v3::concepts range-v3::range-v3
        date::date
        date::date-tz
        glaze::glaze
        curl
)

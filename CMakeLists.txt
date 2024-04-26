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
#include(FetchContent)
#FetchContent_Declare(date_src
#        GIT_REPOSITORY https://github.com/HowardHinnant/date.git
#        GIT_TAG master
#)
#FetchContent_MakeAvailable(date_src)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(notes main.cc
        sqlite/sqlite.cc
        sqlite/sqlite.h
        sqlite/logger.h
        sqlite/stmt.cc
        sqlite/stmt.h
        shared.h
        sqlite/value.h
        sqlite/field.h
        sqlite/row.h
        sqlite/result.h
        sqlite/query.h
        model/category.cc
        model/category.h
        model/note.cc
        model/note.h
        notes/MainWindow.cc
        notes/MainWindow.h
        notes/Settings.h
        notes/types.h
        notes/CategoryTree.cc
        notes/CategoryTree.h
        notes/NotesWorkspace.cc
        notes/NotesWorkspace.h
        notes/NotesTable.cc
        notes/NotesTable.h
        notes/NoteWidget.cc
        notes/NoteWidget.h
        common/Event.h
        common/EventController.h
        common/Datime.h
)
target_link_libraries(notes
        Qt::Core Qt::Gui Qt::Widgets
        sqlite3
        fmt::fmt
        range-v3::meta range-v3::concepts range-v3::range-v3
        date::date
        date::date-tz
        curl
)

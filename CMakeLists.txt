cmake_minimum_required(VERSION 3.26.4)

project(dbscratch)

set(CMAKE_CXX_STANDARD 11)

add_executable(dbscratch
        src/main.cpp
        src/table.h
        src/table.cc
        src/row.h
        src/row.cc
        src/meta_command.h
        src/meta_command.cc
        src/statement.h
        src/statement.cc
        src/pager.h
        src/pager.cc
        src/db.h
        src/db.cc
)

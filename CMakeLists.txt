cmake_minimum_required(VERSION 3.26.4)

project(dbscratch)

set(CMAKE_CXX_STANDARD 11)

# add_executable(dbscratch
#     _src/db.cc
#     _src/db.h
# )

add_executable(dbscratch
        src/main.cpp
        src/table.h
        src/constants.h
        src/meta_command.h
        src/statement.h
        src/pager.h
        src/db.h
)

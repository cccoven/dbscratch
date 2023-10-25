cmake_minimum_required(VERSION 3.26.4)

project(dbscratch)

set(CMAKE_CXX_STANDARD 11)

# add_executable(dbscratch
#     _src/db.cc
#     _src/db.h
# )

add_executable(dbscratch
        src/db.cc
        src/db.h
)


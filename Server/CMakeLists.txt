cmake_minimum_required(VERSION 3.0)

project(DARKPINES_SERVER)

add_executable(server main.c Common/utils.c Common/db.c Common/persistence.c Service/msg_service.c)

include_directories(/opt/homebrew/Cellar/cjson/1.7.15/include)
include_directories(/opt/homebrew/opt/sqlite/include)

link_directories(/opt/homebrew/Cellar/cjson/1.7.15/lib)
link_directories(/opt/homebrew/opt/sqlite/lib)

find_library(CJSON_LIBRARY NAMES cjson PATHS /opt/homebrew/Cellar/cjson/1.7.15/lib)
target_link_libraries(server ${CJSON_LIBRARY} sqlite3)

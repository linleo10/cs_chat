#include "db.h"

void openDatabase() {
    int rc = sqlite3_open("../data.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

void closeDatabase() {
    sqlite3_close(db);
}

void initDatabase() {
    openDatabase();

    char *create_msgs_table_sql = "CREATE TABLE IF NOT EXISTS msgs (id INTEGER PRIMARY KEY AUTOINCREMENT, nickname TEXT, from_uid INTEGER, to_uid INTEGER);";
    char *create_users_table_sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, uid TEXT NOT NULL, username TEXT NOT NULL, password TEXT NOT NULL, nickname TEXT NOT NULL, last_online_at INTEGER NOT NULL);";
    int rc_msgs = sqlite3_exec(db, create_msgs_table_sql, 0, 0, 0);
    int rc_users = sqlite3_exec(db, create_users_table_sql, 0, 0, 0);
    if (rc_msgs != SQLITE_OK || rc_users != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(db));
    }

    closeDatabase();
}
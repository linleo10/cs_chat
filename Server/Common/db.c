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

    char *create_db_sql = "CREATE TABLE IF NOT EXISTS msgs (id INTEGER PRIMARY KEY AUTOINCREMENT, nickname TEXT, from_uid INTEGER, to_uid INTEGER);";
    int rc = sqlite3_exec(db, create_db_sql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %s\n", sqlite3_errmsg(db));
    }

    closeDatabase();
}
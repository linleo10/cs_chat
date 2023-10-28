#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

sqlite3 *db;

void openDatabase();
void closeDatabase();
void initDatabase();

#endif 
/*
database classe
Hugh Smith
*/
#pragma once
#include <iostream>
#include <sqlite3.h>

class Database
{
private:
    const char* file;
    sqlite3 *db;
public:
    void close();
    const char* get_item(const char* command);

  Database(const char* data_base_file)
    {
    /*constructor*/
        file = data_base_file;
        sqlite3_open(file, &db);
    }
};

const char* Database::get_item(const char* command)
{
    sqlite3_stmt* statement;
    sqlite3_prepare_v2(db,command, -1, &statement, NULL);
    sqlite3_step(statement);
    const char* output;
    //sqlite3_bind_text(statement, 1, output, -1, SQLITE_TRANSIENT);
    output = (const char *)sqlite3_column_text(statement, 1);
    sqlite3_finalize(statement);
    return output;
}

void Database::close()
{
    /*close the data base*/
    sqlite3_close_v2(db);
}

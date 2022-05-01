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
public:
    void close();
    std::string get_item(const char* command);

  Database(const char* data_base_file)
    {
    /*constructor*/
        file = data_base_file;
    }
};

std::string Database::get_item(const char* command)
{
    /*perform an action to the data base
    takes a string*/
    sqlite3 *db;
	sqlite3_open(file, &db);
	sqlite3_stmt* statement;
    sqlite3_prepare_v2(db,command, -1, &statement, NULL);
    sqlite3_step(statement);
    const char* output;
    output = (const char *)sqlite3_column_text(statement, 0);
    sqlite3_step(statement);
    string output_sting = string(output);
    sqlite3_finalize(statement);
	sqlite3_close_v2(db);
    return output_sting;
}

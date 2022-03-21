#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <sqlite3.h>
#include <vector>
#include <string>

class Database {
private:
    sqlite3* DB;
public:
    Database();
    ~Database();
};


#endif //DATABASEC_DATABASE_H

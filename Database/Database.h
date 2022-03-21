#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <functional>
#include <sqlite3.h>
#include <vector>
#include <string>

class Database {
private:
    sqlite3* DB;
public:
    Database();
};


#endif //DATABASEC_DATABASE_H

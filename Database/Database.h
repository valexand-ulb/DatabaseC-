#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include <vector>
#include <string>

class Database {
private:
    sqlite3* DB;
public:
    // Constructeurs
    Database();
    ~Database();
    // Méthodes

    // Bool Méthodes
    bool createNewAccount(std::string username, std::string password);
    bool createFriendship(std::string username1, std::string username2);
    bool isUserinDB(std::string username);
    bool doesFriendshipExists(std::string username1, std::string username2);
    bool deleteFriendship(std::string username1, std::string username2);

};


#endif //DATABASEC_DATABASE_H

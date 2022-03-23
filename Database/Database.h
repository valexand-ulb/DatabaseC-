#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include <vector>
#include <string>

struct UserScore{
    int GamesPlayed;
    int GamesWon;
};

class Database {
private:
    sqlite3* DB;
public:
    // Constructeurs
    Database();
    ~Database();
    // Méthodes

    // Bool Méthodes
    //write
    bool createNewAccount(std::string username, std::string password);
    bool createFriendship(std::string username1, std::string username2);

    //modify
    bool deleteFriendship(std::string username1, std::string username2);
    bool addGamePlayed(std::string username, bool win);
    void resetTables();

    //read
    bool isUserinDB(std::string username);
    bool doesFriendshipExists(std::string username1, std::string username2);
    UserScore getScore(std::string username);

};


#endif //DATABASEC_DATABASE_H

#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iterator>
#include <sqlite3.h>
#include <vector>
#include <string>

struct UserScore{
    int GamesPlayed;
    int GamesWon;
    int GameLose = GamesPlayed-GamesWon;
};


struct FriendsInfo{
    std::vector<std::string> FriendsList;
    std::vector<std::string> FriendsToAddList;
};

class Database {
private:
    sqlite3* DB;
    FriendsInfo userInfo;
public:
    // Constructeurs
    Database();
    ~Database();
    // Méthodes

    //getter setter
    std::string getPassword(std::string username);
    UserScore getScore(std::string username);
    void setUserFriendsList(std::string username);
    void setUserFriendsToAddList(std::string username);

    //methodes statiques
    static void stringToVect(std::string inputString, std::vector<std::string> &vectAddr);
    static bool isStringinVect(std::string inputStr, const std::vector<std::string> &vect);

    //write
    bool createNewAccount(std::string username, std::string password);
    bool askFriend(std::string username1, std::string username2);
    bool transferFriend(std::string username, std::string validationString);

    //modify
    bool deleteFriendship(std::string username1, std::string username2);
    bool addGamePlayed(std::string username, bool win);
    void resetTables();


    //read
    bool isUserinDB(std::string username);
    bool doesFriendshipExists(std::string username1, std::string username2);
    bool checkPassword(std::string username, std::string password);



};


#endif //DATABASEC_DATABASE_H
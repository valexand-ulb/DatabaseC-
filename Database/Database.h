#ifndef DATABASEC_DATABASE_H
#define DATABASEC_DATABASE_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iterator>
#include <sqlite3.h>
#include <vector>
#include <algorithm>
#include <string>

struct UserScore{
    int GamesPlayed;
    int GamesWon;
    int GameLose = GamesPlayed-GamesWon;
};


struct UserInfo{
    std::string username;
    std::vector<std::string> FriendsList;
    std::vector<std::string> FriendsToAddList;
    UserScore score;
};

class Database {
private:
    sqlite3* DB;
    UserInfo userInfo;
public:
    // Constructeurs
    Database();
    ~Database();
    // Méthodes

    // initialisation obligatoire avant d'utiliser certaine méthode
    bool connect(const std::string & username);

    // write / read from database
    bool createNewAccount(const std::string& username, const std::string& password);
    std::string  getPassword(const std::string& username);
    UserScore getScore(const std::string& username);
    UserInfo getUserInfo(){return userInfo;}
    void setUserFriendsList(const std::string& username);
    void setUserFriendsToAddList(const std::string& username);
    void writeFriendsList();
    void writeFriendsToAddList();

    //methodes statiques
    static void stringToVect(const std::string& inputString, std::vector<std::string> &vectAddr);
    static std::string VectTostring(std::vector<std::string> &vect);
    static bool isStringinVect(const std::string& inputStr,std::vector<std::string> &vect);

    //write

    bool askFriend(const std::string& username);
    bool transferFriend(const std::string& username);


    //modify
    void deleteFriendship(const std::string& username);
    bool addGamePlayed(const std::string& username, bool win);
    void resetTables();


    //read
    bool isUserinDB(const std::string& username);
    bool doesFriendshipExists(const std::string& username2);
    bool checkPassword(const std::string& username, const std::string& password);



};


#endif //DATABASEC_DATABASE_H
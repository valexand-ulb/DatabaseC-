#include "Database.h"

Database::Database() {
    sqlite3_open("./test.db", &this->DB);
    char* messageError;

    std::string sql1 = "CREATE TABLE IF NOT EXISTS UserEntry("
                       "username        TEXT,"
                       "password        TEXT,"
                       "PRIMARY KEY (username))";
    sqlite3_exec(this->DB, sql1.c_str(), NULL, 0, &messageError);

    std::string sql2 = "CREATE TABLE IF NOT EXISTS ScoreEntry ("
                       "username           TEXT,"
                       "nameOfLevel       TEXT,"
                       "score             INTEGER,"
                       "PRIMARY KEY (username, nameOfLevel)"
                       "FOREIGN KEY (username) REFERENCES UserEntry(username))";
    sqlite3_exec(this->DB, sql2.c_str(), NULL, 0, &messageError);

    std::string sql3 = "CREATE TABLE IF NOT EXISTS FriendshipEntry ("
                       "username1	     TEXT,"
                       "username2	     TEXT,"
                       "PRIMARY KEY (username1, username2)"
                       "FOREIGN KEY (username1) REFERENCES UserEntry(username),"
                       "FOREIGN KEY (username2) REFERENCES UserEntry(username))";
    sqlite3_exec(this->DB, sql3.c_str(), NULL, 0, &messageError);

    std::string sql4 = "CREATE TABLE IF NOT EXISTS CustomizedLevel("
                       "creatorUsername      TEXT,"
                       "nameOfLevel          TEXT,"
                       "listOfEnnemies       TEXT,"
                       "listOfPositions      TEXT,"
                       "listOfSpawnTime      TEXT,"
                       "numberOfLikes        INT,"
                       "nameOfLikers         TEXT,"
                       "PRIMARY KEY (nameOfLevel)"
                       "FOREIGN KEY (creatorUsername) REFERENCES UserEntry(username))";
    sqlite3_exec(this->DB, sql4.c_str(), NULL, 0, &messageError);
}

int main(){
    return 0;
}
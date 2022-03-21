#include "Database.h"

Database::Database() {
    sqlite3_open("test.db", &this->DB);
    char* messageError;

    std::string sql1 = "CREATE TABLE IF NOT EXISTS UserData ("
                       "Id INTEGER NOT NULL UNIQUE,"
                       "Username TEXT NOT NULL UNIQUE,"
                       "Password TEXT NOT NULL,"
                       "PRIMARY KEY( Id AUTOINCREMENT))";
    sqlite3_exec(this->DB, sql1.c_str(), NULL, 0, &messageError);


    std::string sql2 = "CREATE TABLE IF NOT EXISTS GameScore ("
                       "Username TEXT,"
                       "Score INTEGER,"
                       "PRIMARY KEY (Username)"
                       "FOREIGN KEY (Username) REFERENCES UserData(Username))";
    sqlite3_exec(this->DB, sql2.c_str(), NULL, 0, &messageError);

    std::string sql3 = "CREATE TABLE IF NOT EXISTS FriendshipEntry ("
                       "Username1 TEXT,"
                       "Username2 TEXT,"
                       "PRIMARY KEY (Username1, Username2)"
                       "FOREIGN KEY (Username1) REFERENCES UserData(Username),"
                       "FOREIGN KEY (Username2) REFERENCES UserData(Username))";
    sqlite3_exec(this->DB, sql3.c_str(), NULL, 0, &messageError);
}

int main(){
    Database db;
    return 0;
}
#include "Database.h"

Database::Database() {
    sqlite3_open("test.db", &this->DB);
    char* messageError;

    std::string sql1 = "CREATE TABLE IF NOT EXISTS UserData ("
                       "Username TEXT NOT NULL UNIQUE,"
                       "Password TEXT NOT NULL,"
                       "PRIMARY KEY(Username))";
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

bool Database::createNewAccount(std::string username, std::string password) {
    std::hash<std::string> hsh;
    if (isUserinDB(username)){
        return false;
    }

    const std::string sqlRequest = "INSERT INTO UserData(username,password) VALUES ('" + username + "', '" + std::to_string(hsh(password)) + "')";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

    return true;
}

bool Database::createFriendship(const std::string username1, const std::string username2)
{
    if (doesFriendshipExists(username1, username2))
    {
        return false;
    }

    const std::string sqlRequest = "INSERT INTO FriendshipEntry(Username1, Username2) VALUES ('" + username1 + "', '" + username2 + "')";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

    return true;
}

bool Database::isUserinDB(std::string username) {
    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT username FROM UserData WHERE Username = '" + username + "'";

    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        if (username == std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))))
        {
            return true;
        }
    }
    return false;
}

bool Database::doesFriendshipExists(const std::string username1, const std::string username2)
{
    sqlite3_stmt* stmt;

    const std::string sqlRequest = "SELECT * FROM FriendshipEntry WHERE (Username1 = '" + username1 + "' AND Username2 = '" + username2 + "')";

    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    bool found = false;

    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        found = true;
    }
    return found;
}

int main(){
    Database DB;
    DB.createNewAccount("Alex", "Alex");
    DB.createNewAccount("Theo", "Theo");
    DB.createFriendship("Alex","Theo");
    return 0;
}
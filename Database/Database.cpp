#include "Database.h"

Database::Database() {
    sqlite3_open("test.db", &this->DB);
    char *messageError;

    std::string sql1 = "CREATE TABLE IF NOT EXISTS UserData ("
                       "Username TEXT NOT NULL UNIQUE,"
                       "Password TEXT NOT NULL,"
                       "PRIMARY KEY(Username))";
    sqlite3_exec(this->DB, sql1.c_str(), NULL, 0, &messageError);


    std::string sql2 = "CREATE TABLE IF NOT EXISTS GameScore ("
                       "Username TEXT NOT NULL UNIQUE,"
                       "GamesPlayed INTEGER DEFAULT 0,"
                       "GamesWon INTEGER DEFAULT 0,"
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
    if (isUserinDB(username)) {
        return false;
    }

    const std::string sqlRequest =
            "INSERT INTO UserData(username,password) VALUES ('" + username + "', '" + std::to_string(hsh(password)) +
            "')";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

    const std::string sqlRequest2 = "INSERT INTO GameScore(Username) VALUES('"+username+"')";
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);

    return true;
}

bool Database::createFriendship(std::string username1, std::string username2) {
    if (doesFriendshipExists(username1, username2) or doesFriendshipExists(username2, username1) or
        username1 == username2) {
        return false;
    }
    for (int i = 0; i < 2; i++) {
        const std::string sqlRequest =
                "INSERT INTO FriendshipEntry(Username1, Username2) VALUES ('" + username1 + "', '" + username2 + "')";
        sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);
        std::string temp = username1;
        username1 = username2;
        username2 = temp;
    }
    return true;
}

bool Database::isUserinDB(std::string username) {
    sqlite3_stmt *stmt;
    const std::string sqlRequest = "SELECT username FROM UserData WHERE Username = '" + username + "'";

    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (username == std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)))) {
            return true;
        }
    }
    return false;
}

bool Database::doesFriendshipExists(const std::string username1, const std::string username2) {
    sqlite3_stmt *stmt;

    const std::string sqlRequest =
            "SELECT * FROM FriendshipEntry WHERE (Username1 = '" + username1 + "' AND Username2 = '" + username2 + "')";

    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    bool found = false;

    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        found = true;
    }
    return found;
}

bool Database::deleteFriendship(std::string username1, std::string username2) {

    if (doesFriendshipExists(username1, username2)) {
        for(int i = 0;i<2;i++) {
            const std::string sqlRequest =
                    "DELETE FROM FriendshipEntry WHERE (username1 = '" + username1 + "' AND username2 = '" + username2 +
                    "')";
            sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);
            std::string temp = username1;
            username1 = username2;
            username2 = temp;
        }
        return true;
    }
    return false;
}

Database::~Database() {
    sqlite3_close(this->DB);
}

bool Database::addGamePlayed(std::string username, bool win) {
    if(!isUserinDB(username))
        return false;
    sqlite3_stmt* stmt;

    const std::string sqlRequest = "SELECT GamesPlayed, GamesWon FROM GameScore WHERE (Username = '" + username + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;

    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        int GamesPlayed = sqlite3_column_int(stmt, 0);
        int GamesWon = sqlite3_column_int(stmt, 1);
        GamesPlayed++;
        if(win)
            GamesWon++;
        const std::string sqlRequest2 = "UPDATE GameScore SET GamesPlayed = '" + std::to_string(GamesPlayed) + "', GamesWon = '" + std::to_string(GamesWon) + "' WHERE Username = '" + username + "'";
        sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);
    }
    return true;
}

UserScore Database::getScore(std::string username) {

    UserScore score{-1,-1};

    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT GamesPlayed, GamesWon FROM GameScore WHERE (Username = '" + username + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;

    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        score.GamesPlayed = sqlite3_column_int(stmt, 0);
        score.GamesWon = sqlite3_column_int(stmt, 1);
    }
    return score;
}

void Database::resetTables(){
    char* messageError;
    sqlite3_stmt* stmt;
    std::string sqlRequest1 = "DELETE FROM GameScore;";
    const std::string sqlRequest2 = "DELETE FROM FriendshipEntry;";
    const std::string sqlRequest3 = "DELETE FROM UserData;";
    sqlite3_exec(this->DB, sqlRequest1.c_str(), NULL, 0, &messageError);
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, &messageError);
    sqlite3_exec(this->DB, sqlRequest3.c_str(), NULL, 0, &messageError);



}

int main() {
    Database DB;
    DB.resetTables();
    DB.createNewAccount("Alex", "Alex");
    DB.createNewAccount("Theo", "Theo");
    DB.createNewAccount("Mark", "123");
    DB.createFriendship("Alex", "Theo");
    DB.createFriendship("Mark", "Alex");
    DB.deleteFriendship("Alex", "Theo");
    DB.addGamePlayed("Alex", true);
    DB.addGamePlayed("Alex", false);
    UserScore alexScore = DB.getScore("Alex");
    std::cout<<"Parties jouées : "<< alexScore.GamesPlayed<<", Parties gagnées : "<<alexScore.GamesWon<<std::endl;
    return 0;
}


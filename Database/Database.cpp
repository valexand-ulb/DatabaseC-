#include "Database.h"

//constructeurs destructeur
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
                       "Username TEXT,"
                       "FriendString TEXT,"
                       "PRIMARY KEY (Username)"
                       "FOREIGN KEY (Username) REFERENCES UserData(Username))";
    sqlite3_exec(this->DB, sql3.c_str(), NULL, 0, &messageError);

    std::string sql4 = "CREATE TABLE IF NOT EXISTS ToAddFriends("
                       "Username TEXT,"
                       "FriendToAddString TEXT,"
                       "PRIMARY KEY (Username)"
                       "FOREIGN KEY (Username) REFERENCES UserData(Username))";
    sqlite3_exec(this->DB, sql4.c_str(), NULL, 0, &messageError);
}

Database::~Database() {
    sqlite3_close(this->DB);
}

//getter setter
void Database::setUserFriendsList(std::string username) {
    // récupere
    std::string FriendsString;
    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT FriendString FROM FriendshipEntry WHERE (Username = '" + username + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        FriendsString = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    stringToVect(FriendsString,userInfo.FriendsList);
}
void Database::setUserFriendsToAddList(std::string username) {
    // récupere
    std::string FriendsToAddString;
    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT FriendToAddString FROM ToAddFriends WHERE (Username = '" + username + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        FriendsToAddString = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    stringToVect(FriendsToAddString,userInfo.FriendsToAddList);
}

/**
 * @brief retourne le score affilié à un username
 * @param username : string du nom utilisateur
 * @return struct Userscore permettant d'avoir les partie totale perdue et gagnée
 */
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

/**
 * @brief retourne le hash du mot de passe associé à un utilisateur
 * @param username : string du nom utilisateur
 * @return hash du mot de passe
 */
std::string Database::getPassword(std::string username) {
    std::string password;
    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT Password FROM UserData WHERE (Username = '" + username + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        password =  std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    return password;
}

// write
/**
 * Crée un compte dans la Base de donnée
 * @param username : string du nom utilisateur
 * @param password : string du mot de passe
 * @return
 */
bool Database::createNewAccount(std::string username, std::string password) {
    std::hash<std::string> hsh;
    std::string NoneStr="";
    if (isUserinDB(username)) return false;

    // insert une entrée Username et psw dans la UserData
    const std::string sqlRequest =
            "INSERT INTO UserData(username,password) VALUES ('" + username + "', '" + std::to_string(hsh(password)) +
            "')";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);
    // insert une entrée Username dans la table Gamescore
    const std::string sqlRequest2 = "INSERT INTO GameScore(Username) VALUES('"+username+"')";
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);
    // insert une entrée Username et FriendStr dans la table FriendshipEntry
    const std::string sqlRequest3 = "INSERT INTO FriendshipEntry(Username,FriendString) VALUES('"+username+"', '"+ NoneStr +"')";
    sqlite3_exec(this->DB, sqlRequest3.c_str(), NULL, 0, NULL);
    // insert une entrée Username et FriendStr dans la table ToAddFriends
    const std::string sqlRequest4 = "INSERT INTO ToAddFriends(Username,FriendToAddString) VALUES('"+username+"', '"+ NoneStr +"')";
    sqlite3_exec(this->DB, sqlRequest4.c_str(), NULL, 0, NULL);
    return true;
}

/**
 * @brief Username2 demande en ami Username1
 * @param username1 : string du nom utilisateur
 * @param username2 : string du nom utilisateur
 * @return true si opération effectuée, false sinon
 */
bool Database::askFriend(std::string username1, std::string username2){
    // ! user2 demande user 1 en ami
    if (doesFriendshipExists(username1,username2) or username1==username2) return false;
    std::string FriendsString;
    sqlite3_stmt* stmt;
    const std::string sqlRequest = "SELECT FriendToAddString FROM ToAddFriends WHERE (Username = '" + username1 + "')";
    sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
    int ret_code;
    if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW){
        FriendsString = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    FriendsString += " "+username2;
    const std::string sqlRequest2 = "UPDATE ToAddFriends SET FriendToAddString = '" + FriendsString + "' WHERE Username = '" + username1 + "'";
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);
    return true;
}

bool Database::transferFriend(std::string username, std::string validationString) {
    return true;
}


// modify
bool Database::deleteFriendship(std::string username1, std::string username2) {
    //TODO
    return false;
}
/**
 * @brief met a jour le score de l'utilisateur
 * @param username : string du nom utilisateur
 * @param win : booléen si la partie a ete gagnée
 * @return
 */
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
/**
 * @brief Efface toute les tables
 */
void Database::resetTables(){
    char* messageError;
    std::string sqlRequest1 = "DELETE FROM GameScore;";
    const std::string sqlRequest2 = "DELETE FROM FriendshipEntry;";
    const std::string sqlRequest3 = "DELETE FROM UserData;";
    const std::string sqlRequest4 = "DELETE FROM ToAddFriends;";
    sqlite3_exec(this->DB, sqlRequest1.c_str(), NULL, 0, &messageError);
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, &messageError);
    sqlite3_exec(this->DB, sqlRequest3.c_str(), NULL, 0, &messageError);
    sqlite3_exec(this->DB, sqlRequest4.c_str(), NULL, 0, &messageError);
}
// read
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
    setUserFriendsList(username1);
    return isStringinVect(username2,userInfo.FriendsList);
}

bool Database::checkPassword(std::string username, std::string password) {
    std::string hashedDBPassword = getPassword(username);
    std::hash<std::string> h;
    std::string hashedInputPsw = std::to_string(h(password));
    return hashedDBPassword == hashedInputPsw;
}

void Database::stringToVect(std::string inputString, std::vector<std::string> &vectAddr) {
    std::stringstream ss(inputString);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    vectAddr =vstrings;
}

bool Database::isStringinVect(std::string inputStr, const std::vector<std::string> &vect) {
    for (std::string s: vect){
        if (s == inputStr) return true;
    }
    return false;
}

int main() {
    Database DB;
    //DB.resetTables();
    std::cout << DB.doesFriendshipExists("Alex","Alex") << std::endl;
    //DB.addGamePlayed("Alex", true);
    //DB.addGamePlayed("Alex", false);
    //UserScore alexScore = DB.getScore("Alex");
    //std::cout<<"Parties jouées : "<< alexScore.GamesPlayed<<", Parties gagnées : "<<alexScore.GamesWon<<std::endl;
    //std::cout<<DB.getPassword("Alex");
    return 0;
}
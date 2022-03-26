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

//initialise
bool Database::connect(const std::string& username) {
    if (!isUserinDB(username))return false;
    setUserFriendsList(username);
    setUserFriendsToAddList(username);
    userInfo.username = username;
    userInfo.score = getScore(username);
    return true;
}

//getter setter
void Database::setUserFriendsList(const std::string& username) {
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

void Database::setUserFriendsToAddList(const std::string& username) {
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
UserScore Database::getScore(const std::string& username) {

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
std::string Database::getPassword(const std::string& username) {
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

void Database::writeFriendsList() {
    std::string friendString = VectTostring(userInfo.FriendsList);
    const std::string sqlRequest = "UPDATE FriendshipEntry SET FriendString = '" + friendString +"' WHERE Username = '" + userInfo.username + "'";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);
}

void Database::writeFriendsToAddList() {
    std::string friendsToAddString = VectTostring(userInfo.FriendsToAddList);
    const std::string sqlRequest = "UPDATE ToAddFriends SET FriendToAddString = '" + friendsToAddString +"' WHERE Username = '" + userInfo.username + "'";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);
}

// write
/**
 * Crée un compte dans la Base de donnée
 * @param username : string du nom utilisateur
 * @param password : string du mot de passe
 * @return
 */
bool Database::createNewAccount(const std::string& username, const std::string& password) {
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
bool Database::askFriend(const std::string& username){
    // ! user2 demande user 1 en ami
    if (doesFriendshipExists(username) or userInfo.username==username) return false;
    userInfo.FriendsToAddList.push_back(username);
    writeFriendsToAddList();
    return true;
}

bool Database::transferFriend(const std::string& username) {
    if (!isStringinVect(username,userInfo.FriendsToAddList)) return false;
    userInfo.FriendsList.push_back(username);
    std::remove(userInfo.FriendsToAddList.begin(),userInfo.FriendsToAddList.end(),username);
    writeFriendsList();
    writeFriendsToAddList();
    return true;
}


// modify
void Database::deleteFriendship(const std::string& username) {
    std::remove(userInfo.FriendsList.begin(),userInfo.FriendsList.end(),username);
    writeFriendsList();
    writeFriendsToAddList();
}
/**
 * @brief met a jour le score de l'utilisateur
 * @param username : string du nom utilisateur
 * @param win : booléen si la partie a ete gagnée
 * @return
 */
bool Database::addGamePlayed(const std::string& username, bool win) {
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
bool Database::isUserinDB(const std::string& username) {
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

bool Database::doesFriendshipExists(const std::string& username) {
    return isStringinVect(username,userInfo.FriendsList);
}

bool Database::checkPassword(const std::string& username, const std::string& password) {
    std::string hashedDBPassword = getPassword(username);
    std::hash<std::string> h;
    std::string hashedInputPsw = std::to_string(h(password));
    return hashedDBPassword == hashedInputPsw;
}

void Database::stringToVect(const std::string& inputString, std::vector<std::string> &vectAddr) {
    std::stringstream ss(inputString);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    vectAddr =vstrings;
}

std::string Database::VectTostring(std::vector<std::string> &vect) {
    std::string final;
    for (auto &s:vect){
        final += s +" ";
    }
    return final;
}

bool Database::isStringinVect(const std::string& inputStr, std::vector<std::string> &vect) {
    for (std::string s: vect){
        if (s == inputStr) return true;
    }
    return false;
}

int main() {
    // création de la DB
    Database DB;

    // efface les ancienne table pour le test
    DB.resetTables();

    // créé les Comptes
    DB.createNewAccount("Alex","Alex");
    DB.createNewAccount("Mark","Mark");
    DB.createNewAccount("Theo","Theo");

    // connecte l'utilisateur alex
    DB.connect("Alex");
    // theo demande en amis alex
    DB.askFriend("Theo");
    // alex accepte theo
    DB.transferFriend("Theo");
    // mark demande en amis alex
    DB.askFriend("Mark");
    // alex accepte mark
    DB.transferFriend("Mark");
    // theo est restiré de la liste d'amis :sad:
    DB.deleteFriendship("Theo");

    return 0;
}
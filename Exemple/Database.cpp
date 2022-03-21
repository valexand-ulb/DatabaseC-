#include "Database.hpp"

Database::Database() {
  sqlite3_open("./ltype.db", &this->DB);
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


Database::~Database() {
    sqlite3_close(this->DB);
}


//WRITE
bool Database::newAccount(const std::string username, const std::string password)
{
  if (isUsernameInDatabase(username))
  {
    return false;
  }

  const std::string sqlRequest = "INSERT INTO UserEntry(username,password) VALUES ('" + username + "', '" + password + "')";
  sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

  return true;
}

bool Database::newFriendship(const std::string username1, const std::string username2)
{
  if (friendshipExists(username1, username2))
  {
    return false;
  }

  const std::string sqlRequest = "INSERT INTO FriendshipEntry(username1, username2) VALUES ('" + username1 + "', '" + username2 + "')";
  sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

  return true;
}

int Database::getScoreOfUserForLevel(std::string username, std::string nameOfLevel)
{
  sqlite3_stmt* stmt;
  const std::string sqlRequest = "SELECT score FROM ScoreEntry WHERE (username = '" + username + "' AND nameOfLevel = '" + nameOfLevel + "')";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;;
  int lastScore = -1;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    lastScore = sqlite3_column_int(stmt, 0);
  }

  return lastScore;
}

bool Database::newScore(const std::string username, const std::string nameOfLevel, const int score)
{
  int lastScore = getScoreOfUserForLevel(username, nameOfLevel);
  if (lastScore != -1)
  {
    if (lastScore < score)
    {
      modifyScoreOfUserForLevel(username, nameOfLevel, score);
      return true;
    }
    else
    {
      return false;
    }
  }

  const std::string sqlRequest = "INSERT INTO ScoreEntry (username, nameOfLevel, score) VALUES ('" + username + "','" + nameOfLevel + "','" + std::to_string(score) + "')";
  sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

  return true;

}

bool Database::modifyScoreOfUserForLevel(const std::string username, const std::string nameOfLevel, const int score)
{
  const std::string sqlRequest = "UPDATE ScoreEntry SET score = '" + std::to_string(score) + "' WHERE username = '" + username + "' AND nameOfLevel = '" + nameOfLevel + "'";
  sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

  return true;
} 

bool Database::newCustomizedLevel(const std::string username, const std::string nameOfLevel, const std::vector<DataBaseSpawnDS> vectorOfLevel)
{
  if (isCustomizedLevelNameAlreadyUsed(nameOfLevel))
  {
    return false;
  }

  int i = int(vectorOfLevel.size())-1;
  std::string stringOfEnnemies = std::to_string(vectorOfLevel[i].ennemyID);
  std::string stringOfPositions = std::to_string(vectorOfLevel[i].xOffSet);
  std::string stringOfTimeOfSpawn = std::to_string(vectorOfLevel[i].triggerPos);
  for (int j=i-1; j > -1; j--)
  {
    stringOfEnnemies = stringOfEnnemies + ";" + std::to_string(vectorOfLevel[j].ennemyID);
    stringOfPositions = stringOfPositions + ";" + std::to_string(vectorOfLevel[j].xOffSet);
    stringOfTimeOfSpawn = stringOfTimeOfSpawn + ";" + std::to_string(vectorOfLevel[j].triggerPos);
  }

  const std::string sqlRequest = "INSERT INTO CustomizedLevel (creatorUsername, nameOfLevel, listOfEnnemies, listOfPositions, listOfSpawnTime, numberOfLikes, nameOfLikers) VALUES ('" + username + "', '" + nameOfLevel + "', '" + stringOfEnnemies + "', '" + stringOfPositions + "','" + stringOfTimeOfSpawn + "', '0', '')";
  sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

  return true;
  
}

bool Database::deleteFriendship(const std::string username1, const std::string username2)
{

  if (friendshipExists(username1, username2))
  {
    const std::string sqlRequest = "DELETE FROM FriendshipEntry WHERE (username1 = '" + username1 + "' AND username2 = '" + username2 + "')";
    sqlite3_exec(this->DB, sqlRequest.c_str(), NULL, 0, NULL);

    return true;
  }
  return false;
}

bool Database::addLikeToLevel(const std::string nameOfLevel, const std::string usernameOfLiker)
{
  if (isLevelAlreadyLikedByUser(nameOfLevel, usernameOfLiker))
  {
    return false;
  }

  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT numberOfLikes, nameOfLikers FROM CustomizedLevel WHERE (nameOfLevel = '" + nameOfLevel + "')";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  
  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  { 
    int numberOfLikes = sqlite3_column_int(stmt, 0);
    std::string stringOfLikers = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

    numberOfLikes++;
    if (stringOfLikers == "")
    {
      stringOfLikers = usernameOfLiker;
    }
    else 
    {
      stringOfLikers = stringOfLikers + ";" + usernameOfLiker;
    }
    const std::string sqlRequest2 = "UPDATE CustomizedLevel SET numberOfLikes = '" + std::to_string(numberOfLikes) + "', nameOfLikers = '" + stringOfLikers + "' WHERE nameOfLevel = '" + nameOfLevel + "'";
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);
  }

  return true;
}

bool Database::removeLikeFromLevel(const std::string nameOfLevel, const std::string usernameOfLiker)
{
  if (!isLevelAlreadyLikedByUser(nameOfLevel, usernameOfLiker))
  {
    return false;
  }

  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM CustomizedLevel WHERE (nameOfLevel = '" + nameOfLevel + "')";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  
  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  { 
    int numberOfLikes = sqlite3_column_int(stmt, 3);
    numberOfLikes--;
  
    std::string stringOfLikers = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
    
    std::vector<std::string> vectorOfLikers = splitStringIntoVector(stringOfLikers);
    vectorOfLikers = removeFromVector(vectorOfLikers, usernameOfLiker);
    stringOfLikers = vectorToString(vectorOfLikers);

    const std::string sqlRequest2 = "UPDATE CustomizedLevel SET numberOfLikes = '" + std::to_string(numberOfLikes) + "', nameOfLikers = '" + stringOfLikers + "' WHERE nameOfLevel = '" + nameOfLevel + "'";
    sqlite3_exec(this->DB, sqlRequest2.c_str(), NULL, 0, NULL);
  }

  return true;

}

//READ
bool Database::isUsernameInDatabase(const std::string username)
{
  sqlite3_stmt* stmt;
  const std::string sqlRequest = "SELECT username FROM UserEntry WHERE username = '" + username + "'";

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

bool Database::isPasswordMatching(const std::string username, const std::string password)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM UserEntry WHERE username = '" + username + "'";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    if (password == std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))))
    {
      return true;
    }
  }
  return false;
}

bool Database::friendshipExists(const std::string username1, const std::string username2)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM FriendshipEntry WHERE (username1 = '" + username1 + "' AND username2 = '" + username2 + "')";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  bool found = false;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    found = true;
  }
  return found;
}

bool Database::isCustomizedLevelNameAlreadyUsed(const std::string nameOfLevel)
{
  sqlite3_stmt* stmt;
  const std::string sqlRequest = "SELECT nameOfLevel FROM CustomizedLevel WHERE (nameOfLevel = '" + nameOfLevel + "')";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    if (nameOfLevel == std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))))
    {
      return true;
    }
  }

  return false;
}

bool Database::isLevelAlreadyLikedByUser(const std::string nameOfLevel, std::string usernameOfLiker)
{
  sqlite3_stmt* stmt;
  const std::string sqlRequest = "SELECT nameOfLikers FROM CustomizedLevel WHERE (nameOfLevel = '" + nameOfLevel + "')";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  bool found = false;
  
  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    std::string stringOfLikers = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    std::vector<std::string> vectorOfLikers = splitStringIntoVector(stringOfLikers);
    int i = 0;
    while (!found && i < int(vectorOfLikers.size()))
    {
      if (usernameOfLiker == vectorOfLikers[i])
      {
        found = true;
      }
      else{
        i++;
      }
    }
  }

  return found;;

}

//GET
std::vector<std::string> Database::getFriendsListOf(const std::string username)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM FriendshipEntry WHERE username1 = '" + username + "'";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  std::vector<std::string> friendsList;

  while ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    friendsList.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))));
  }
return friendsList;
}

std::vector<ScoreEntry> Database::getAllScoresOfUser(const std::string username)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM ScoreEntry Where username = '" + username + "' ORDER BY score DESC";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  std::vector<ScoreEntry> scoreList;
  ScoreEntry scoreObject;

  while ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    scoreObject.username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    scoreObject.nameOfLevel = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    scoreObject.score = sqlite3_column_int(stmt, 2);
    scoreList.push_back(scoreObject);
  }
  return scoreList;
}


std::vector<ScoreEntry> Database::getTopXOfUser(const std::string username, const int x)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM ScoreEntry WHERE username = '" + username + "' ORDER BY score DESC";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code, i = 0;
  std::vector<ScoreEntry> scoreList;
  ScoreEntry scoreObject;

  while (((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) && i < x)
  {
    scoreObject.username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    scoreObject.nameOfLevel = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    scoreObject.score = sqlite3_column_int(stmt, 2);
    scoreList.push_back(scoreObject);
    i++;
  }
  return scoreList;
}

std::vector<ScoreEntry> Database::getGlobalTopTen()
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM ScoreEntry ORDER BY score DESC";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code, i=0;
  std::vector<ScoreEntry> topTenScoreList;
  ScoreEntry scoreObject;

  while (((ret_code = sqlite3_step(stmt)) == SQLITE_ROW) && i < 10)
  {
    scoreObject.username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    scoreObject.nameOfLevel = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    scoreObject.score = sqlite3_column_int(stmt, 2);
    topTenScoreList.push_back(scoreObject);
    i++;
  }
  return topTenScoreList;
}

std::vector<ScoreEntry> Database::getScoresOfLevel(const std::string nameOfLevel)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT * FROM ScoreEntry WHERE level = '" + nameOfLevel + "' ORDER BY score DESC";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;
  std::vector<ScoreEntry> scoreListOfLevel;
  ScoreEntry scoreObject;

  while ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    scoreObject.username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    scoreObject.nameOfLevel = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    scoreObject.score = sqlite3_column_int(stmt, 2);
    scoreListOfLevel.push_back(scoreObject);
  }
  return scoreListOfLevel;
}

std::vector<ScoreEntry> Database::getScoresOfFriends(const std::string username)
{
  std::vector<std::string> vectorOfFriends = getFriendsListOf(username);
  int numberOfFriends = int(vectorOfFriends.size());
  int i = 0;
  std::vector<ScoreEntry> scoresOfAllFriends;
  std::vector<ScoreEntry> scoresOfFriend;

  while (i < numberOfFriends)
  {
    scoresOfFriend = getAllScoresOfUser(vectorOfFriends[i]);
    for (int j = 0; j < int(scoresOfFriend.size()); j++) {
      scoresOfAllFriends.push_back(scoresOfFriend[j]);
    }
    i++;
  }

  return scoresOfAllFriends;
}

int Database::getNumberOfLikesForLevel(const std::string nameOfLevel)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT numberOfLikes FROM CustomizedLevel WHERE nameOfLevel = '" + nameOfLevel + "'";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    int numberOfLikes = sqlite3_column_int(stmt, 0);
    return numberOfLikes;
  }

}

std::vector<std::string> Database::getUsernameOfLikers(const std::string nameOfLevel)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT nameOfLikers FROM CustomizedLevel WHERE nameOfLevel = '" + nameOfLevel + "'";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    std::string nameOfLikers = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    std::vector<std::string> vectorOfLikers = splitStringIntoVector(nameOfLikers);
    return vectorOfLikers;
  }
}


//méthode à modifier quand hisao aura fait sa partie
std::stack<DataBaseSpawnDS> Database::getLevel(const std::string nameOfLevel)
{
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT listOfEnnemies, listOfPositions, listOfSpawnTime FROM CustomizedLevel WHERE nameOfLevel = '" + nameOfLevel + "'";
  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;

  if ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  {
    std::string stringOfEnnemies = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    std::string stringOfPosition = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    std::string stringOfTimeOfSpawn = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

    std::vector<std::string> vectorOfEnnemies = splitStringIntoVector(stringOfEnnemies);
    std::vector<std::string> vectorOfPosition = splitStringIntoVector(stringOfPosition);
    std::vector<std::string> vectorOfTimeOfSpawn = splitStringIntoVector(stringOfTimeOfSpawn);

    std::stack<DataBaseSpawnDS> ennemyStack;
    for (int i = 0; i < int(vectorOfEnnemies.size()); i++)
    {
      ennemyStack.push(DataBaseSpawnDS(vectorOfEnnemies[i], vectorOfTimeOfSpawn[i], vectorOfPosition[i]));
    } 

    return ennemyStack;
  }
}

std::vector<LevelInfo> Database::getAllLevels() {
  sqlite3_stmt* stmt;

  const std::string sqlRequest = "SELECT creatorUsername, nameOfLevel, numberOfLikes FROM CustomizedLevel";

  sqlite3_prepare_v2(this->DB, sqlRequest.c_str(), -1, &stmt, NULL);
  int ret_code;

  std::vector<LevelInfo> levels;

  while ((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
  { 
    levels.push_back(LevelInfo{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), sqlite3_column_int(stmt, 2)});
  }

  return levels;
}

//OTHERS
std::vector<std::string> Database::splitStringIntoVector(std::string stringToSplit)
{
  std::vector<std::string> splittedString;
  int i = 0;
  while (i < (stringToSplit.size()))
  {
    std::string nameOfUser;
    while (stringToSplit[i] != ';' && i < (stringToSplit.size()))
    {
      nameOfUser = nameOfUser + stringToSplit[i];
      i++;
    }
    i++;
    splittedString.push_back(nameOfUser);
  }
  return splittedString;
} 


std::vector<std::string> Database::removeFromVector(std::vector<std::string> vectorOfString, const std::string elementToRemove)
{
  bool found = false;
  int i = 0;
  while (!found && i < int(vectorOfString.size()))
  {
    if ( vectorOfString[i] == elementToRemove)
    {
      vectorOfString.erase(vectorOfString.begin() + i);
      found = true;
    }
    i++;
  }

  return vectorOfString;
}

std::string Database::vectorToString(std::vector<std::string> vectorOfString)
{
  std::string finalString = vectorOfString[0];
  for (int i = 1; i < int(vectorOfString.size()); i++)
  {
    finalString = finalString + ";" + vectorOfString[i];
  }

  return finalString;
}
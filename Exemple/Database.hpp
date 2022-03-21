#ifndef _DATABASE_H_
#define _DATABASE_H_
#include <iostream>
#include <stdio.h>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include "../../Client/src/Game/DataBaseSpawnDS.hpp"

struct ScoreEntry
{
	std::string username;
	std::string nameOfLevel;
	unsigned int score;
};

struct LevelInfo {
	std::string author, name;
	int likes;
};

class Database
{
	private:
	sqlite3* DB;

	public:
	//init
	Database();
	~Database();

	//WRITE
	bool newAccount(const std::string username, const std::string password);
	bool newFriendship(const std::string username1, const std::string username2);
	bool newScore(const std::string username, const std::string nameOfLevel, const int score);
	bool newCustomizedLevel(const std::string username, const std::string nameOfLevel,const std::vector<DataBaseSpawnDS> vectorOfLevel);


	//MODIFY
	bool deleteFriendship(const std::string username1, const std::string username2);
	bool addLikeToLevel(const std::string nameOfLevel, const std::string usernameOfLiker);
	bool removeLikeFromLevel(const std::string nameOfLevel, const std::string usernameOfLiker);
	bool modifyScoreOfUserForLevel(const std::string username, const std::string nameOfLevel, const int score);

	//READ
	bool isUsernameInDatabase(const std::string username);
	bool isPasswordMatching(const std::string username, const std::string password);
	bool friendshipExists(const std::string username1, const std::string username2);
	bool isCustomizedLevelNameAlreadyUsed(const std::string nameOfLevel);
	bool isLevelAlreadyLikedByUser(const std::string nameOfLevel, const std::string usernameOfLiker);

	//GET
	std::vector<std::string> getFriendsListOf(const std::string username);
	std::vector<ScoreEntry> getAllScoresOfUser(const std::string username);
	std::vector<ScoreEntry> getTopXOfUser(const std::string username, const int x);
	std::vector<ScoreEntry> getGlobalTopTen();
	std::vector<ScoreEntry> getScoresOfLevel(const std::string nameOflevel);
	std::vector<ScoreEntry> getScoresOfFriends(const std::string username);
	int getNumberOfLikesForLevel(const std::string nameOfLevel);
	std::vector<std::string> getUsernameOfLikers(const std::string nameOfLevel);
	std::stack<DataBaseSpawnDS> getLevel(const std::string nameOfLevel);
	std::vector<LevelInfo> getAllLevels();
	int getScoreOfUserForLevel(std::string username, std::string nameOfLevel);

	//OTHERS
	std::vector<std::string> splitStringIntoVector(std::string stringToSplit);
	std::vector<std::string> removeFromVector(std::vector<std::string> vectorOfString, const std::string elementToRemove);
	std::string vectorToString(std::vector<std::string>);
};
#endif
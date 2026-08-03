#pragma once
#include <tgbot/tgbot.h>
#include <unordered_map>
#include "DatabaseManager.h"

class TelegramManager {
public:
	enum class UserState {
		NotAuthorized,
		WaitingCode,
		Authorized
	};
	struct UserSession {
		UserState state;
	};
	TelegramManager(DatabaseManager& database, const std::string& token);
	void start();
	void sendMessage(int64_t chatId, const std::string& text);
private:
	void registerHandlers();
private:
	TgBot::Bot bot;
	DatabaseManager& dbManager;
	std::unordered_map<int64_t, UserSession> users;
};
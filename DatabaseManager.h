#pragma once
#include <pqxx/pqxx>
#include <iostream>
#include "Config.h"
#include "Notification.h"

class DatabaseManager {
public:
	DatabaseManager(const Config& config);
	bool checkLinkingByChatId(int64_t chatId);
	int getUserIdByLinkCode(int64_t linkCode);
	void addTelegramAccount(int64_t userId, int64_t chatId, std::string& username, bool notifications);
	std::vector<Notification> getNotifications();
	void markSent(int taskId);
private:
	pqxx::connection mainConn;
	pqxx::connection notifConn;
};
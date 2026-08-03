#pragma once
#include <thread>
#include <chrono>
#include <string>
#include "DatabaseManager.h"
#include "TelegramManager.h"
#include "Notification.h"

class NotificationService {
public:
	NotificationService(DatabaseManager& database, TelegramManager& telegram);
	void start();
private:
	DatabaseManager& dbManager;
	TelegramManager& tgManager;
};
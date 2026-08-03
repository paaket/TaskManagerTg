#include "NotificationService.h"

NotificationService::NotificationService(DatabaseManager& database, TelegramManager& telegram) : dbManager(database), tgManager(telegram) {}

void NotificationService::start() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::minutes(1));
		std::vector<Notification> result = dbManager.getNotifications();
		for (const auto& el : result) {
			tgManager.sendMessage(el.chatId, "The deadline for the task \"" + el.title + "\" expires in less than 1 hour.");
			dbManager.markSent(el.taskId);
		}
	}
}
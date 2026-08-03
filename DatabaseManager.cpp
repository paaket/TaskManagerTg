#include "DatabaseManager.h"

DatabaseManager::DatabaseManager(const Config& config) : mainConn(config.text), notifConn(config.text) {
	if (mainConn.is_open()) std::cout << "conn is open: " << mainConn.dbname() << std::endl;
}

bool DatabaseManager::checkLinkingByChatId(int64_t chatId) {
	pqxx::work tx(mainConn);
	bool res = tx.exec("SELECT 1 FROM telegram_accounts WHERE telegram_chat_id = $1;", pqxx::params{ chatId }).empty();
	tx.commit();
	return !res;
}

int DatabaseManager::getUserIdByLinkCode(int64_t linkCode) {
	pqxx::work tx(mainConn);
	pqxx::result res = tx.exec("SELECT user_id FROM telegram_link_codes WHERE code = $1 AND expires_at >= NOW();", pqxx::params{ linkCode });
	tx.commit();
	if (res.empty()) return 0;
	pqxx::row row = res[0];
	return row["user_id"].as<int>();
}

void DatabaseManager::addTelegramAccount(int64_t userId, int64_t chatId, std::string& username, bool notifications) {
	pqxx::work tx(mainConn);
	tx.exec("INSERT INTO telegram_accounts(user_id, telegram_chat_id, telegram_username, notifications_enabled) VALUES ($1, $2, $3, $4);",
		pqxx::params{ userId, chatId, username, notifications });
	 tx.exec("DELETE FROM telegram_link_codes WHERE user_id = $1;", pqxx::params{ userId });
	tx.commit();
}

void DatabaseManager::markSent(int taskId) {
	pqxx::work tx(notifConn);
	tx.exec("INSERT INTO task_notifications(task_id, sent) VALUES ($1, TRUE);", taskId);
	tx.commit();
}

std::vector<Notification> DatabaseManager::getNotifications() {
	pqxx::work tx(notifConn);
	pqxx::result res = tx.exec("WITH active_tg_accounts AS (SELECT user_id, telegram_chat_id FROM telegram_accounts WHERE notifications_enabled = '1'), pending_tasks AS (SELECT id AS task_id, user_id, title FROM tasks WHERE completed = '0' AND deadline > NOW() AND deadline <= NOW() + INTERVAL '1 hour') SELECT tg.telegram_chat_id, t.task_id, t.title FROM pending_tasks t INNER JOIN active_tg_accounts tg ON t.user_id = tg.user_id WHERE NOT EXISTS(SELECT 1 FROM task_notifications tn WHERE tn.task_id = t.task_id);");
	if (res.empty()) return {};

	std::vector<Notification> data;

	for (const auto& row : res) {
		int64_t tg_chat_id = row["telegram_chat_id"].as<int64_t>();
		int64_t task_id = row["task_id"].as<int64_t>();
		std::string title = row["title"].as<std::string>();

		data.push_back({ tg_chat_id, title, task_id });
	}
	tx.commit();
	return data;
}
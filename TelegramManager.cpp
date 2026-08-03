#include "TelegramManager.h"

TelegramManager::TelegramManager(DatabaseManager& database, const std::string& token) : bot(token), dbManager(database) {}

void TelegramManager::registerHandlers() {
    bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        sendMessage(message->chat->id, u8"Бот предназначен для получения уведомлений из приложения TaskManagerQt.\n\nДля привязки аккаунта отправьте временный код из приложения, который можно получить в Account->Link Telegram.\n\nЕсли у вас ещё нет аккаунта, скачайте приложение с https://github.com/paaket/TaskManagerQt и пройдите процесс регистрации.");
        });

    bot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        if (!dbManager.checkLinkingByChatId(message->chat->id) and users[message->chat->id].state == UserState::Authorized)
            users.erase(message->chat->id);

        if (!users.count(message->chat->id)) users.insert({ message->chat->id, UserSession{ UserState::NotAuthorized } });

        switch (users[message->chat->id].state) {
        case UserState::NotAuthorized: {
            bool res = dbManager.checkLinkingByChatId(message->chat->id);
            if (!res) {
                sendMessage(message->chat->id, u8"Вы не зарегистрированы, отправьте код регистрации:");
                users[message->chat->id].state = UserState::WaitingCode;
            }
            else {
                sendMessage(message->chat->id, u8"Вы зарегистрированы");
                users[message->chat->id].state = UserState::Authorized;
            }
            break;
        }
        case UserState::WaitingCode: {
            int code;
            try {
                code = std::stoi(message->text);
            }
            catch (...) {
                sendMessage(message->chat->id, u8"Некорректный код. введите шестизначный числовой код");
                break;
            }

            int64_t userId = dbManager.getUserIdByLinkCode(code);
            if (userId == 0) {
                sendMessage(message->chat->id, u8"Код не найден, возможно, срок его действия истёк. вернитесь в приложение и сгенерируйте новый код");
                break;
            }
            
            dbManager.addTelegramAccount(userId, message->chat->id, message->from->username, 1);
            sendMessage(message->chat->id, u8"Вы зарегистрированы");
            users[message->chat->id].state = UserState::Authorized;
            break;
        }
        case UserState::Authorized:
            sendMessage(message->chat->id, u8"Вы зарегистрированы");
            break;
        }
        });
}

void TelegramManager::start() {
    registerHandlers();

    TgBot::TgLongPoll longPoll(bot);
    while (true) longPoll.start();
}

void TelegramManager::sendMessage(int64_t chatId, const std::string& text) {
    bot.getApi().sendMessage(chatId, text);
}
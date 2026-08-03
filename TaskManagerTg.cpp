#include <tgbot/tgbot.h>
#include <iostream>
#include <thread>
#include "TelegramManager.h"
#include "DatabaseManager.h"
#include "Config.h"
#include "NotificationService.h"

int main() {
    setlocale(LC_ALL, "ru");
    try {
        Config config;
        DatabaseManager dbManager(config);

        TelegramManager telegramManager(dbManager, config.botApi);

        NotificationService notification(dbManager, telegramManager);

        std::thread thread(&NotificationService::start, &notification);

        telegramManager.start();
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
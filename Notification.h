#pragma once
#include <stdint.h>
#include <string>

struct Notification {
	int64_t chatId;
	std::string title;
	int64_t taskId;
};
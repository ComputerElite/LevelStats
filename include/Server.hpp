#pragma once

#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include <thread>
#include <optional>
#include <mutex>

class LevelStatsManager    {
private:
    Logger& logger;
    std::thread networkThread;

    void threadEntrypoint();
    bool runServer();
    bool createModuleJSON();
    std::string constructResponse();
    std::string handlePlaceholders(std::string str);
    std::string replaceAll(std::string& str, std::string key, std::string replacement);
public:
    std::string levelName = "";
    float energy = 1.0f;
    int combo = 0;
    float percentage = 0.0f;
    float bpm = 0.0f;
    float njs = 0.0f;
    std::string songAuthor = "";
    std::string id = "";
    std::string mapper = "";
    std::string levelSub = "";
    std::string rank = "";
    int difficulty = 0;
    int score = 0;
    int timePlayed = 0;
    int totalTime = 0;

    LevelStatsManager(Logger& logger);
};
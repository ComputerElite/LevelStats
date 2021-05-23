#define RAPIDJSON_HAS_STDSTRING 1 // Enable rapidjson's support for std::string
#include <thread>
#include <cmath>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Server.hpp"

#include "beatsaber-hook/shared/config/config-utils.hpp"

#define ADDRESS "0.0.0.0" // Binding to localhost
#define PORT 3501
#define CONNECTION_QUEUE_LENGTH 1 // How many connections to store to process

LevelStatsManager::LevelStatsManager(Logger& logger) : logger(logger) {
    logger.info("Starting network thread . . .");
    networkThread = std::thread(&LevelStatsManager::runServer, this); // Run the thread
}

void LevelStatsManager::threadEntrypoint()    {
    logger.info("Starting server");
    if(!runServer()) {
        logger.error("Failed to run server!");
    }
}

// Creates the JSON
std::string LevelStatsManager::constructResponse() {
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("levelName", LevelStatsManager::levelName, alloc);
    doc.AddMember("energy", LevelStatsManager::energy, alloc);
    doc.AddMember("combo", LevelStatsManager::combo, alloc);
    doc.AddMember("percentage", LevelStatsManager::percentage, alloc);
    doc.AddMember("songAuthor", LevelStatsManager::songAuthor, alloc);
    doc.AddMember("difficulty", LevelStatsManager::difficulty, alloc);
    doc.AddMember("score", LevelStatsManager::score, alloc);
    doc.AddMember("timePlayed", LevelStatsManager::timePlayed, alloc);
    doc.AddMember("totalTime", LevelStatsManager::totalTime, alloc);
    doc.AddMember("id", LevelStatsManager::id, alloc);
    doc.AddMember("levelSub", LevelStatsManager::levelSub, alloc);
    doc.AddMember("mapper", LevelStatsManager::mapper, alloc);
    doc.AddMember("rank", LevelStatsManager::rank, alloc);
    doc.AddMember("bpm", LevelStatsManager::bpm, alloc);
    doc.AddMember("njs", LevelStatsManager::njs, alloc);

    // Convert the document into a string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

// Replaces all occurunces of the key with the replacement inside of str
std::string LevelStatsManager::replaceAll(std::string& str, std::string key, std::string replacement) {
    int pos = str.find(key);
    while(pos != std::string::npos) {
        str.replace(pos, key.size(), replacement);
        pos = str.find(key, pos + replacement.size());
    }
    return str;
}

bool LevelStatsManager::runServer()   {
    // Make our IPv4 endpoint
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ADDRESS);

    int sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create the socket
    // Prevents the socket taking a while to close from causing a crash
    int iSetOption = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,sizeof(iSetOption));
    if(sock == -1)    {
        logger.error("Error creating socket: %s", strerror(errno));
        return false;
    }

    // Attempt to bind to our port
    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr))) {
        logger.error("Error binding to port %d: %s", PORT, strerror(errno));
        close(sock);
        return false;
    }

    logger.info("Listening on port %d", PORT);
    while(true) {
        if (listen(sock, CONNECTION_QUEUE_LENGTH) == -1) { // Return if an error occurs listening for a request
            logger.error("Error listening for request");
            continue;
        }

        socklen_t socklen = sizeof addr;
        int client_sock = accept(sock, (struct sockaddr*) &addr, &socklen); // Accept the next request
        if(client_sock == -1)   {
            logger.error("Error accepting request");
            continue;
        }        

        logger.info("Client connected with address: %s", inet_ntoa(addr.sin_addr));

        std::string stats = constructResponse();
        std::string response = "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(stats.length()) + "\nContent-Type: application/json\nAccess-Control-Allow-Origin: *\n\n" + stats;
        logger.info("Response: %s", response.c_str());

        int convertedLength = htonl(response.length());
        if(write(client_sock, response.c_str(), response.length()) == -1)    { // Then send the string
            logger.error("Error sending JSON: %s", strerror(errno));
            close(client_sock); continue;
        }

        close(client_sock); // Close the client's socket to avoid leaking resources
    }
    
    close(sock);
    return true;
}
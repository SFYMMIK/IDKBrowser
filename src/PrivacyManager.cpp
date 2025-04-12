#include "PrivacyManager.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PrivacyManager::PrivacyManager() : harmfulTrackerDetected(false) {
    loadTrackers();
}

void PrivacyManager::loadTrackers() {
    std::ifstream file("src/trackers.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open trackers.json" << std::endl;
        return;
    }

    json j;
    file >> j;

    for (const auto& tracker : j["harmful_trackers"]) {
        harmfulTrackers.insert(tracker.dump());
    }

    for (const auto& tracker : j["benign_trackers"]) {
        benignTrackers.insert(tracker.dump());
    }
}

bool PrivacyManager::checkTrackers(const std::string &html) {
    harmfulTrackerDetected = false;
    for (const auto& tracker : harmfulTrackers) {
        if (html.find(tracker) != std::string::npos) {
            harmfulTrackerDetected = true;
            return true;
        }
    }
    return false;
}

bool PrivacyManager::hasHarmfulTracker() const {
    return harmfulTrackerDetected;
}

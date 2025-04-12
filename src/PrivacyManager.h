#ifndef PRIVACYMANAGER_H
#define PRIVACYMANAGER_H

#include <string>
#include <vector>
#include <unordered_set>

class PrivacyManager {
public:
    PrivacyManager();
    void loadTrackers();
    bool checkTrackers(const std::string &html);
    bool hasHarmfulTracker() const;

private:
    std::unordered_set<std::string> harmfulTrackers;
    std::unordered_set<std::string> benignTrackers;
    bool harmfulTrackerDetected;
};

#endif // PRIVACYMANAGER_H

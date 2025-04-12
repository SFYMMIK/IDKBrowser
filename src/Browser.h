#ifndef BROWSER_H
#define BROWSER_H

#include "NetworkManager.h"
#include "PrivacyManager.h"
#include <string>
#include <vector>

class Browser {
public:
    Browser();
    ~Browser();

    void start();
    void updateUI();

    std::vector<std::pair<std::string, std::string>> loadBookmarks();
    void addBookmark(const std::string& title, const std::string& url);

private:
    PrivacyManager privacyManager;
    int trackersBlocked;

    void navigateTo(const std::string &url);
};

#endif // BROWSER_H

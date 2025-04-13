#include "Browser.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

static const std::string bookmarkFile = "bookmarks.json";

Browser::Browser() : trackersBlocked(0) {
    initialize_network_manager();
    // Ensure bookmark file exists
    if (!fs::exists(bookmarkFile)) {
        std::ofstream f(bookmarkFile);
        f << "[]";
    }
}

Browser::~Browser() {
    cleanup_network_manager();
}

void Browser::start() {
    std::string url;
    while (true) {
        std::cout << "Enter URL: ";
        std::cin >> url;
        if (url == "home") {
            updateUI();
        } else {
            navigateTo(url);
        }
    }
}

void Browser::navigateTo(const std::string &url) {
    char *html = fetch_page(url.c_str());
    if (!html) {
        std::cerr << "Failed to fetch page" << std::endl;
        return;
    }

    if (privacyManager.checkTrackers(html)) {
        std::cout << "This website contains harmful trackers. Do you wish to continue? (yes/no): ";
        std::string response;
        std::cin >> response;
        if (response == "no") {
            std::cout << "Returning to homepage..." << std::endl;
            free(html);
            return;
        }
    }

    if (privacyManager.hasHarmfulTracker()) {
        trackersBlocked++;
    }

    updateUI();
    std::cout << "Loading page..." << std::endl;

    free(html);
}

void Browser::updateUI() {
    std::ofstream uiFile("src/UI/ui.js");
    if (uiFile.is_open()) {
        uiFile << "updateStats(" << 0 << ", " << trackersBlocked << ");\n";
        uiFile.close();
    } else {
        std::cerr << "Failed to update UI" << std::endl;
    }
}

std::vector<std::pair<std::string, std::string>> Browser::loadBookmarks() {
    std::vector<std::pair<std::string, std::string>> bookmarks;
    if (!fs::exists(bookmarkFile)) return bookmarks;

    std::ifstream file(bookmarkFile);
    if (!file.is_open()) return bookmarks;

    try {
        json data;
        file >> data;
        for (const auto& item : data) {
            bookmarks.emplace_back(item["title"], item["url"]);
        }
    } catch (const std::exception &e) {
        std::cerr << "Bookmark parse error: " << e.what() << std::endl;
        // Recover the file
        std::ofstream reset(bookmarkFile);
        reset << "[]";
    }

    return bookmarks;
}

void Browser::addBookmark(const std::string& title, const std::string& url) {
    json data = json::array();

    if (fs::exists(bookmarkFile)) {
        std::ifstream in(bookmarkFile);
        try {
            in >> data;
        } catch (...) {
            // fallback to clean array
            data = json::array();
        }
    }

    data.push_back({ {"title", title}, {"url", url} });

    std::ofstream out(bookmarkFile);
    out << data.dump(4);
}

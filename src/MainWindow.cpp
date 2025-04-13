#include "MainWindow.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QRegularExpression>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QTabBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QPalette>
#include <QApplication>
#include <fstream>
#include <nlohmann/json.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    qApp->setStyle("Fusion");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::black);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    QWidget *central = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    QToolBar *navBar = new QToolBar("Navigation", this);
    QAction *backAction = new QAction("←", this);
    QAction *forwardAction = new QAction("→", this);
    QAction *refreshAction = new QAction("⟳", this);
    QAction *homeAction = new QAction("🏠", this);
    QAction *goAction = new QAction("Go", this);
    QAction *clearHistoryAction = new QAction("🗑️ Clear History", this);
    QAction *downloadListAction = new QAction("⬇ Downloads", this);
    QAction *viewHistoryAction = new QAction("📜 View History", this);

    urlBar = new QLineEdit;
    urlBar->setPlaceholderText("Search with Brave Search");
    urlBar->setStyleSheet("QLineEdit::placeholder { color: gray; }");

    navBar->addAction(backAction);
    navBar->addAction(forwardAction);
    navBar->addAction(refreshAction);
    navBar->addAction(homeAction);
    navBar->addWidget(urlBar);
    navBar->addAction(goAction);
    navBar->addAction(clearHistoryAction);
    navBar->addAction(downloadListAction);
    navBar->addAction(viewHistoryAction);

    connect(backAction, &QAction::triggered, this, &MainWindow::goBack);
    connect(forwardAction, &QAction::triggered, this, &MainWindow::goForward);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refreshPage);
    connect(homeAction, &QAction::triggered, this, &MainWindow::goHome);
    connect(goAction, &QAction::triggered, this, &MainWindow::onLoadPage);
    connect(urlBar, &QLineEdit::returnPressed, this, &MainWindow::onLoadPage);
    connect(clearHistoryAction, &QAction::triggered, this, &MainWindow::clearBrowsingHistory);
    connect(downloadListAction, &QAction::triggered, this, &MainWindow::showDownloadHistory);
    connect(viewHistoryAction, &QAction::triggered, this, &MainWindow::showBrowsingHistory);

    addToolBar(navBar);

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setDocumentMode(true);

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::handleCloseRequest);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    layout->addWidget(tabWidget);
    central->setLayout(layout);
    setCentralWidget(central);

    addNewTab();
    addPlusTab();
    setWindowTitle("SolarBrowser");
}

QWebEngineView* MainWindow::currentWebView() {
    int index = tabWidget->currentIndex();
    if (index >= 0 && tabWidget->tabText(index) != "+") {
        return qobject_cast<QWebEngineView*>(tabWidget->widget(index));
    }
    return nullptr;
}

void MainWindow::onLoadPage() {
    if (!currentWebView()) return;

    QString input = urlBar->text().trimmed();
    QUrl url;
    if (input.startsWith("http://") || input.startsWith("https://")) {
        url = QUrl(input);
    } else if (QRegularExpression(R"(([\w-]+\.[\w.-]+))").match(input).hasMatch()) {
        url = QUrl("https://" + input);
    } else {
        url = QUrl("https://search.brave.com/search?q=" + QUrl::toPercentEncoding(input));
    }

    logHistory(url.toString());
    currentWebView()->load(url);
}

void MainWindow::goHome() {
    if (!currentWebView()) return;

    QString path = QCoreApplication::applicationDirPath() + "/src/UI/homepage.html";
    currentWebView()->load(QUrl::fromLocalFile(path));
    urlBar->clear();
    urlBar->setPlaceholderText("Search with Brave Search");
}

void MainWindow::addNewTab() {
    auto *view = new QWebEngineView;
    view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    view->settings()->setAttribute(QWebEngineSettings::WebAttribute::ShowScrollBars, false);
    view->settings()->setAttribute(QWebEngineSettings::WebAttribute::DnsPrefetchEnabled, true);
    view->settings()->setAttribute(QWebEngineSettings::WebAttribute::LocalContentCanAccessRemoteUrls, true);

    QWebEngineProfile *profile = view->page()->profile();
    profile->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    profile->setHttpUserAgent(profile->httpUserAgent() + " SolarBrowser/1.0 (DarkMode)");

    connect(view, &QWebEngineView::urlChanged, this, [=](const QUrl &url) {
        if (view == currentWebView()) {
            if (url.toString().startsWith("file://") && url.toString().contains("homepage.html")) {
                urlBar->clear();
                urlBar->setPlaceholderText("Search with Brave Search");
            } else {
                urlBar->setText(url.toString());
                urlBar->setPlaceholderText("");
            }
        }
    });

    connect(profile, &QWebEngineProfile::downloadRequested, this, [=](QWebEngineDownloadRequest *download) {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Download Folder");
        if (!dir.isEmpty()) {
            QString fileName = download->downloadFileName();
            download->setDownloadDirectory(dir);
            download->setDownloadFileName(fileName);
            logDownload(QDir(dir).filePath(fileName));
            download->accept();
        }
    });

    int insertIndex = tabWidget->count();
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == "+") {
            insertIndex = i;
            break;
        }
    }

    tabWidget->insertTab(insertIndex, view, "New Tab");
    tabWidget->setCurrentIndex(insertIndex);
    goHome();
}

void MainWindow::addPlusTab() {
    QWidget *plusWidget = new QWidget;
    tabWidget->addTab(plusWidget, "+");
    tabWidget->tabBar()->setTabButton(tabWidget->count() - 1, QTabBar::RightSide, nullptr);
}

void MainWindow::closeCurrentTab(int index) {
    if (index >= 0 && tabWidget->tabText(index) != "+") {
        QWidget *widget = tabWidget->widget(index);
        tabWidget->removeTab(index);
        widget->deleteLater();
    }
}

void MainWindow::handleCloseRequest(int index) {
    int realTabs = 0;
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) != "+") realTabs++;
    }

    if (realTabs <= 1 && tabWidget->tabText(index) != "+") {
        QCoreApplication::quit();
    } else {
        closeCurrentTab(index);
    }
}

void MainWindow::goBack() {
    if (currentWebView()) currentWebView()->back();
}

void MainWindow::goForward() {
    if (currentWebView()) currentWebView()->forward();
}

void MainWindow::refreshPage() {
    if (currentWebView()) currentWebView()->reload();
}

void MainWindow::tabChanged(int index) {
    if (index >= 0 && tabWidget->tabText(index) == "+") {
        addNewTab();
    } else if (index >= 0) {
        QWebEngineView* view = qobject_cast<QWebEngineView*>(tabWidget->widget(index));
        if (view) {
            QString urlStr = view->url().toString();
            if (urlStr.startsWith("file://") && urlStr.contains("homepage.html")) {
                urlBar->clear();
                urlBar->setPlaceholderText("Search with Brave Search");
            } else {
                urlBar->setText(urlStr);
                urlBar->setPlaceholderText("");
            }
        } else {
            urlBar->clear();
            urlBar->setPlaceholderText("Search with Brave Search");
        }
    }
}

void MainWindow::logHistory(const QString &url) {
    nlohmann::json db;
    std::ifstream in("history.json");
    if (in.is_open()) in >> db;

    db.push_back({{"url", url.toStdString()}, {"timestamp", QDateTime::currentSecsSinceEpoch()}});

    qint64 thirtyDaysAgo = QDateTime::currentSecsSinceEpoch() - (60 * 60 * 24 * 30);
    db.erase(std::remove_if(db.begin(), db.end(), [=](const nlohmann::json &entry) {
        return entry["timestamp"].get<qint64>() < thirtyDaysAgo;
    }), db.end());

    std::ofstream out("history.json");
    out << db.dump(4);
}

void MainWindow::clearBrowsingHistory() {
    std::ofstream out("history.json");
    out << "[]";
    QMessageBox::information(this, "History Cleared", "Browsing history cleared.");
}

void MainWindow::logDownload(const QString &path) {
    nlohmann::json db;
    std::ifstream in("downloads.json");
    if (in.is_open()) in >> db;

    db.push_back({{"file", path.toStdString()}, {"timestamp", QDateTime::currentSecsSinceEpoch()}});

    qint64 sevenDaysAgo = QDateTime::currentSecsSinceEpoch() - (60 * 60 * 24 * 7);
    db.erase(std::remove_if(db.begin(), db.end(), [=](const nlohmann::json &entry) {
        return entry["timestamp"].get<qint64>() < sevenDaysAgo;
    }), db.end());

    std::ofstream out("downloads.json");
    out << db.dump(4);
}

void MainWindow::showDownloadHistory() {
    nlohmann::json db;
    std::ifstream in("downloads.json");
    if (!in.is_open()) return;
    in >> db;

    QStringList options;
    for (const auto &item : db) {
        options << QString::fromStdString(item["file"].get<std::string>());
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Download History", "Select file to open folder:", options, 0, false, &ok);
    if (ok && !selected.isEmpty()) {
        QFileInfo file(selected);
        QDesktopServices::openUrl(QUrl::fromLocalFile(file.absolutePath()));
    }
}

void MainWindow::showBrowsingHistory() {
    nlohmann::json db;
    std::ifstream in("history.json");
    if (!in.is_open()) return;
    in >> db;

    QStringList entries;
    for (const auto &item : db) {
        entries << QString::fromStdString(item["url"].get<std::string>());
    }

    bool ok;
    QString selected = QInputDialog::getItem(this, "Browsing History", "Select page to visit:", entries, 0, false, &ok);
    if (ok && !selected.isEmpty()) {
        urlBar->setText(selected);
        onLoadPage();
    }
}

MainWindow::~MainWindow() {}

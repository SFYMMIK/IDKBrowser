#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTabWidget>
#include <QWebEngineView>
#include <QString>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadPage();
    void goHome();
    void addPlusTab();
    void addNewTab();
    void closeCurrentTab(int index);
    void handleCloseRequest(int index);
    void goBack();
    void goForward();
    void refreshPage();
    void tabChanged(int index);
    void clearBrowsingHistory();
    void showDownloadHistory();
    void showBrowsingHistory();

private:
    QLineEdit *urlBar;
    QTabWidget *tabWidget;

    QWebEngineView* currentWebView();
    void logHistory(const QString &url);
    void logDownload(const QString &path);
};

#endif // MAINWINDOW_H

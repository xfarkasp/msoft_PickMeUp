#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DataHandler.h"

class QStackedWidget;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget* stackedWidget;

    QWidget* createLoginPage();
    QWidget* createRegistrationPage();
    QWidget* createMenuPage(const DataHandler::User& user);

private slots:
    void switchToLoginPage();
    void switchToRegistrationPage();
    void switchToMenuPage();
    void switchToFormularPage();
    void handleLogin(std::string mail, std::string password);
    void handleRegistration();
};

#endif // MAINWINDOW_H

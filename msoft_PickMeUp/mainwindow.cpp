#include "mainwindow.h"
#include "mainwindow.h"
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include "formularHandler.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), stackedWidget(new QStackedWidget(this))
{

    // Create pages
    QWidget* loginPage = createLoginPage();
    QWidget* registrationPage = createRegistrationPage();

    // Add pages to the stacked widget
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);

    // Set the stacked widget as the central widget
    setCentralWidget(stackedWidget);
}

MainWindow::~MainWindow() {}

QWidget* MainWindow::createLoginPage()
{
    QWidget* loginPage = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(loginPage);

    // Username
    QLabel* usernameLabel = new QLabel("Username:", loginPage);
    QLineEdit* usernameInput = new QLineEdit(loginPage);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);

    // Password
    QLabel* passwordLabel = new QLabel("Password:", loginPage);
    QLineEdit* passwordInput = new QLineEdit(loginPage);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);

    // Login Button
    QPushButton* loginButton = new QPushButton("Login", loginPage);
    layout->addWidget(loginButton);

    // Switch to Registration Button
    QPushButton* toRegisterButton = new QPushButton("Register Here", loginPage);
    layout->addWidget(toRegisterButton);

    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, [this, usernameInput, passwordInput]
        {
            QString username = usernameInput->text();  // Fetch username from QLineEdit
            QString password = passwordInput->text();  // Fetch password from QLineEdit
            handleLogin(username.toStdString(), password.toStdString());
        });
    connect(toRegisterButton, &QPushButton::clicked, this, &MainWindow::switchToRegistrationPage);

    return loginPage;
}

QWidget* MainWindow::createRegistrationPage() {
    QWidget* registrationPage = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(registrationPage);

    // Username
    QLabel* usernameLabel = new QLabel("Username:", registrationPage);
    QLineEdit* usernameInput = new QLineEdit(registrationPage);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);

    // Password
    QLabel* passwordLabel = new QLabel("Password:", registrationPage);
    QLineEdit* passwordInput = new QLineEdit(registrationPage);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);

    // Confirm Password
    QLabel* confirmPasswordLabel = new QLabel("Confirm Password:", registrationPage);
    QLineEdit* confirmPasswordInput = new QLineEdit(registrationPage);
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(confirmPasswordLabel);
    layout->addWidget(confirmPasswordInput);

    // Register Button
    QPushButton* registerButton = new QPushButton("Register", registrationPage);
    layout->addWidget(registerButton);

    // Switch to Login Button
    QPushButton* toLoginButton = new QPushButton("Back to Login", registrationPage);
    layout->addWidget(toLoginButton);

    // Connect signals
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::handleRegistration);
    connect(toLoginButton, &QPushButton::clicked, this, &MainWindow::switchToLoginPage);

    return registrationPage;
}

QWidget* MainWindow::createMenuPage(const DataHandler::User& user)
{
    QWidget* menuWidget = new QWidget(this);
    QGridLayout* gridLayout = new QGridLayout(menuWidget);

    if (user.m_userType == DataHandler::UserType::Basic)
    {
        QPushButton* sendParcelButton = new QPushButton("Send Parcel", menuWidget);
        QPushButton* receiveParcelButton = new QPushButton("PickUp Parcel", menuWidget);
        QPushButton* orderHistoryButton = new QPushButton("Order History", menuWidget);
        QPushButton* userSettingsButton = new QPushButton("User Settings", menuWidget);

        // Set tile styles (optional)
        QList<QPushButton*> buttons = { sendParcelButton, receiveParcelButton, orderHistoryButton, userSettingsButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150); // Set size
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;"); // Style
        }

        // Add tiles to the grid layout
        gridLayout->addWidget(sendParcelButton, 0, 0);
        gridLayout->addWidget(receiveParcelButton, 0, 1);
        gridLayout->addWidget(orderHistoryButton, 1, 0);
        gridLayout->addWidget(userSettingsButton, 1, 1);

        connect(sendParcelButton, &QPushButton::clicked, this, [this]
            {
                FormularHandler formUi;
                for (int i = 0; i < stackedWidget->count(); ++i) {
                    if (stackedWidget->widget(i)->objectName() == "ReceiveParcelPage") {
                        stackedWidget->setCurrentIndex(i);
                        return;
                    }
                }

                QWidget* receiveParcelPage = formUi.getForm(FormType::SendParcel, this);
                receiveParcelPage->setObjectName("ReceiveParcelPage");
                stackedWidget->addWidget(receiveParcelPage);
                stackedWidget->setCurrentWidget(receiveParcelPage);
            });

    }

    if (user.m_userType == DataHandler::UserType::FreelanceCourier)
    {
        QPushButton* sendParcelButton = new QPushButton("Task List", menuWidget);
        QPushButton* userSettingsButton = new QPushButton("User Settings", menuWidget);

        // Set tile styles (optional)
        QList<QPushButton*> buttons = { sendParcelButton, userSettingsButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150); // Set size
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;"); // Style
        }

        // Add tiles to the grid layout
        gridLayout->addWidget(sendParcelButton, 0, 0);
        gridLayout->addWidget(userSettingsButton, 0, 1);
    }

    return menuWidget;
}

void MainWindow::switchToLoginPage()
{
    stackedWidget->setCurrentIndex(0); // Switch to login page
}

void MainWindow::switchToRegistrationPage()
{
    stackedWidget->setCurrentIndex(1); // Switch to registration page
}

void MainWindow::switchToMenuPage()
{
    stackedWidget->setCurrentIndex(2); // Switch to registration page
}

void MainWindow::switchToFormularPage()
{
    stackedWidget->setCurrentIndex(3); // Switch to registration page
}

void MainWindow::handleLogin(std::string mail, std::string password) {
    DataHandler& userHandler = userHandler.getInstance();
    try
    {
        DataHandler::User user = userHandler.getUser(mail, password);
        QWidget* menuPage = createMenuPage(user);
        stackedWidget->addWidget(menuPage);

        switchToMenuPage();

    }
    catch (std::runtime_error& e)
    {
        QMessageBox::warning(this, "Login", "User does not exist");
    }
}

void MainWindow::handleRegistration() {
    QMessageBox::information(this, "Register", "Registration functionality not implemented yet!");
}

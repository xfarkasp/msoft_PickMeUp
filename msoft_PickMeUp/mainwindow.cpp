#include "mainwindow.h"
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include "formularHandler.h"
#include <QScrollArea>
#include "ConnectionHandler.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), stackedWidget(new QStackedWidget(this))
{
    QWidget* loginPage = createLoginPage();
    QWidget* registrationPage = createRegistrationPage();

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registrationPage);

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
            QString username = usernameInput->text();
            QString password = passwordInput->text();
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
        menuWidget->setObjectName("BasicMenu");
        QPushButton* sendParcelButton = new QPushButton("Send Parcel", menuWidget);
        QPushButton* receiveParcelButton = new QPushButton("PickUp Parcel", menuWidget);
        QPushButton* orderHistoryButton = new QPushButton("Order History", menuWidget);
        QPushButton* logOutButton = new QPushButton("Log out", menuWidget);

        QList<QPushButton*> buttons = { sendParcelButton, receiveParcelButton, orderHistoryButton, logOutButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150);
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;");
        }

        gridLayout->addWidget(sendParcelButton, 0, 0);
        gridLayout->addWidget(receiveParcelButton, 0, 1);
        gridLayout->addWidget(orderHistoryButton, 1, 0);
        gridLayout->addWidget(logOutButton, 1, 1);

        connect(sendParcelButton, &QPushButton::clicked, this, [this]
            {
                FormularHandler formUi;
                QWidget* receiveParcelPage = formUi.getForm(FormType::SendParcel, this);
                receiveParcelPage->setObjectName("ReceiveParcelPage");
                stackedWidget->addWidget(receiveParcelPage);
                stackedWidget->setCurrentWidget(receiveParcelPage);
            });

        connect(logOutButton, &QPushButton::clicked, this, [this]
            {
                switchToLoginPage();
            });

    }

    if (user.m_userType == DataHandler::UserType::FreelanceCourier)
    {
        menuWidget->setObjectName("FreelanceCourierMenu");
        QPushButton* taskListButton = new QPushButton("Task List", menuWidget);
        QPushButton* userSettingsButton = new QPushButton("User Settings", menuWidget);

        // Set tile styles (optional)
        QList<QPushButton*> buttons = { taskListButton, userSettingsButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150); // Set size
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;"); // Style
        }

        connect(taskListButton, &QPushButton::clicked, this, [this]
            {
                QWidget* taskPage = createTaskList();
                stackedWidget->addWidget(taskPage);
                stackedWidget->setCurrentWidget(taskPage);
            });

        // Add tiles to the grid layout
        gridLayout->addWidget(taskListButton, 0, 0);
        gridLayout->addWidget(userSettingsButton, 0, 1);
    }

    return menuWidget;
}

QWidget* MainWindow::createTaskList()
{
    DataHandler& dataHandler = DataHandler::getInstance();
    QWidget* containerWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(containerWidget);

    for (auto& taskPair : dataHandler.getTasks())
    {

        QVBoxLayout* layout = new QVBoxLayout(this);
        const auto& task = taskPair.second;
        // Create labels for task information
        QLabel* assigneeLabel = new QLabel("Assignee: " + QString::fromStdString(task.m_asignee), this);
        QLabel* creatorLabel = new QLabel("Creator: " + QString::fromStdString(task.m_creator), this);
        QLabel* locationLabel = new QLabel("Location: " + QString::fromStdString(task.m_location), this);
        QLabel* typeLabel = new QLabel("Type: " +  DataHandler::taskTypeToString(task.m_taskType));
        QLabel* statusLabel = new QLabel("State: " +  DataHandler::taskStatusToString(task.m_taskStatus));

        layout->addWidget(assigneeLabel);
        layout->addWidget(creatorLabel);
        layout->addWidget(locationLabel);
        layout->addWidget(typeLabel);
        layout->addWidget(statusLabel);

        mainLayout->addLayout(layout);

        QPushButton* actionButton = new QPushButton("Execute");
        layout->addWidget(actionButton);
        QObject::connect(actionButton, &QPushButton::clicked, this, [=]() {
            QString message = QString("Navigation system is navigating you.");
               
            QMessageBox::information(nullptr, "Task Details", message);
            ConnectionHandler connection;

            QWidget* connectionPage = connection.connectionGui(task.m_objectId, this);
            connectionPage->setObjectName("ReceiveParcelPage");
            stackedWidget->addWidget(connectionPage);
            stackedWidget->setCurrentWidget(connectionPage);
            });
    }

    containerWidget->setLayout(mainLayout);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(false);

    return scrollArea;
}

void MainWindow::switchToLoginPage()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::switchToRegistrationPage()
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::handleLogin(std::string mail, std::string password) {
    DataHandler& userHandler = userHandler.getInstance();
    try
    {
        DataHandler::User user = userHandler.getUser(mail, password);
        QWidget* menuPage = createMenuPage(user);
        stackedWidget->addWidget(menuPage);
        stackedWidget->setCurrentWidget(menuPage);
    }
    catch (std::runtime_error& e)
    {
        QMessageBox::warning(this, "Login", "User does not exist");
    }
}

void MainWindow::handleRegistration() {
    QMessageBox::information(this, "Register", "Registration functionality not implemented yet!");
}
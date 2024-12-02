#include "mainwindow.h"
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollArea>
#include "formularHandler.h"
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

// Creation of the login page with widgets
QWidget* MainWindow::createLoginPage()
{
    QWidget* loginPage = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(loginPage);

    QLabel* usernameLabel = new QLabel("Username:", loginPage);
    QLineEdit* usernameInput = new QLineEdit(loginPage);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);

    QLabel* passwordLabel = new QLabel("Password:", loginPage);
    QLineEdit* passwordInput = new QLineEdit(loginPage);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);

    QPushButton* loginButton = new QPushButton("Login", loginPage);
    layout->addWidget(loginButton);

    QPushButton* toRegisterButton = new QPushButton("Register Here", loginPage);
    layout->addWidget(toRegisterButton);

    connect(loginButton, &QPushButton::clicked, this, [this, usernameInput, passwordInput]
        {
            QString username = usernameInput->text();
            QString password = passwordInput->text();
            // find the user in the login datbase and check if password is correct
            handleLogin(username.toStdString(), password.toStdString());
        });
    connect(toRegisterButton, &QPushButton::clicked, this, &MainWindow::switchToRegistrationPage);

    return loginPage;
}

// Create the registration page
QWidget* MainWindow::createRegistrationPage() {
    QWidget* registrationPage = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(registrationPage);

    QLabel* usernameLabel = new QLabel("Username:", registrationPage);
    QLineEdit* usernameInput = new QLineEdit(registrationPage);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameInput);

    QLabel* passwordLabel = new QLabel("Password:", registrationPage);
    QLineEdit* passwordInput = new QLineEdit(registrationPage);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordInput);

    QLabel* confirmPasswordLabel = new QLabel("Confirm Password:", registrationPage);
    QLineEdit* confirmPasswordInput = new QLineEdit(registrationPage);
    confirmPasswordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(confirmPasswordLabel);
    layout->addWidget(confirmPasswordInput);

    QPushButton* registerButton = new QPushButton("Register", registrationPage);
    layout->addWidget(registerButton);

    QPushButton* toLoginButton = new QPushButton("Back to Login", registrationPage);
    layout->addWidget(toLoginButton);

    connect(registerButton, &QPushButton::clicked, this, &MainWindow::handleRegistration);
    connect(toLoginButton, &QPushButton::clicked, this, &MainWindow::switchToLoginPage);

    return registrationPage;
}

// Create the menu page
QWidget* MainWindow::createMenuPage(const DataHandler::User& user)
{
    QWidget* menuWidget = new QWidget(this);
    QGridLayout* gridLayout = new QGridLayout(menuWidget);

    // Create the menu page for the normal user mainly for sending and recieving packages
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
                // Create a form for sendign a package
                FormularHandler formUi;
                QWidget* receiveParcelPage = formUi.getForm(FormType::SendParcel, this);
                receiveParcelPage->setObjectName("ReceiveParcelPage");
                stackedWidget->addWidget(receiveParcelPage);
                stackedWidget->setCurrentWidget(receiveParcelPage);
            });

        connect(logOutButton, &QPushButton::clicked, this, [this]
            {
                // Log out the user
                switchToLoginPage();
            });

    }
    // Create the menu page for the normal FreeLance Courier, this menu will have task, which the currie can complete
    if (user.m_userType == DataHandler::UserType::FreelanceCourier)
    {
        menuWidget->setObjectName("FreelanceCourierMenu");
        QPushButton* taskListButton = new QPushButton("Task List", menuWidget);
        QPushButton* logOutButton = new QPushButton("Log out", menuWidget);

        QList<QPushButton*> buttons = { taskListButton, logOutButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150); // Set size
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;");
        }

        // Get the list ov available tasks
        connect(taskListButton, &QPushButton::clicked, this, [this]
            {
                // Create and switch to the task apge
                QWidget* taskPage = createTaskList();
                stackedWidget->addWidget(taskPage);
                stackedWidget->setCurrentWidget(taskPage);
            });

        connect(logOutButton, &QPushButton::clicked, this, [this]
            {
                switchToLoginPage();
            });

        gridLayout->addWidget(taskListButton, 0, 0);
        gridLayout->addWidget(logOutButton, 0, 1);
    }

    // Create the menu page for the Operator user to check automat reports and create report requests
    if (user.m_userType == DataHandler::UserType::Operator)
    {
        menuWidget->setObjectName("OperatorMenu");
        QPushButton* reportListButton = new QPushButton("Report List", menuWidget);
        QPushButton* logOutButton = new QPushButton("Logout", menuWidget);

        QList<QPushButton*> buttons = { reportListButton, logOutButton };
        for (QPushButton* button : buttons) {
            button->setFixedSize(150, 150);
            button->setStyleSheet("font-size: 16px; color: black; background-color: #f0f0f0; border-radius: 10px;");
        }

        // Return the list of available reports
        connect(reportListButton, &QPushButton::clicked, this, [this]
            {
                QWidget* reportPage = createReportList();
                stackedWidget->addWidget(reportPage);
                stackedWidget->setCurrentWidget(reportPage);

            });

        connect(logOutButton, &QPushButton::clicked, this, [this]
            {
                switchToLoginPage();
            });

        gridLayout->addWidget(reportListButton, 0, 0);
        gridLayout->addWidget(logOutButton, 0, 1);
    }

    return menuWidget;
}

// Create task list from the task database in DataHandler
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

// Create report list widget from reports in the TaskData
QWidget* MainWindow::createReportList()
{
    DataHandler& dataHandler = DataHandler::getInstance();
    QWidget* containerWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(containerWidget);

    for (auto& reportPair : dataHandler.getReports())
    {

        QVBoxLayout* layout = new QVBoxLayout(this);
        const auto& report = reportPair.second;

        time_t time = report.m_reportDate;
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(time));
        QString formattedDate = dateTime.toString("yyyy-MM-dd hh:mm:ss");

        QLabel* locationLabel = new QLabel("Location: " + QString::fromStdString(report.m_location), this);
        QLabel* reportDateLabel = new QLabel("Time of report: " + QString("Timestamp: ") + formattedDate, this);
        QLabel* statusLabel = new QLabel("Location: " + DataHandler::reportStatusToString(report.m_status), this);

        layout->addWidget(locationLabel);
        layout->addWidget(reportDateLabel);
        layout->addWidget(statusLabel);

        mainLayout->addLayout(layout);

        QPushButton* actionButton = new QPushButton("Request repair");
        layout->addWidget(actionButton);
        QObject::connect(actionButton, &QPushButton::clicked, this, [=]() {
            FormularHandler formUi;
            QWidget* receiveParcelPage = formUi.getForm(FormType::RequestRepair, this);
            receiveParcelPage->setObjectName("ReceiveParcelPage");
            stackedWidget->addWidget(receiveParcelPage);
            stackedWidget->setCurrentWidget(receiveParcelPage);
            });
    }

    containerWidget->setLayout(mainLayout);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(false);

    return scrollArea;
}

// Switch gui to login page
void MainWindow::switchToLoginPage()
{
    stackedWidget->setCurrentIndex(0);
}

// Switch gui to registratin page
void MainWindow::switchToRegistrationPage()
{
    stackedWidget->setCurrentIndex(1);
}

// Find the user in the database and check if the inserted password is correct
// if yes switch to login page, else show error messsage
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

// Fake registration proccess(this is not implemented)
void MainWindow::handleRegistration() {
    QMessageBox::information(this, "Register", "Registration functionality not implemented yet!");
}
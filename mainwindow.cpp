#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QApplication>
#include <QStatusBar>
#include "StatisticsDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbConnection(new DBConnection(this))
    , m_dbManager(nullptr)
    , m_flightModel(new FlightModel(this))
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    initializeData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate(2016, 8, 15));
    ui->dateEdit->setMaximumDate(QDate(2017, 9, 14));

    ui->flightsTableView->setModel(m_flightModel);
    ui->flightsTableView->horizontalHeader()->setStretchLastSection(true);

    ui->reconnectButton->setEnabled(false);

    // Инициализация статусбара
    handleConnectionChange(m_dbConnection->isConnected());
}

void MainWindow::setupConnections()
{
    connect(m_dbConnection, &DBConnection::connectionStatusChanged,
            this, &MainWindow::handleConnectionChange);

    connect(ui->reconnectButton, &QPushButton::clicked,
            this, &MainWindow::onReconnectClicked);

    connect(ui->showFlightsButton, &QPushButton::clicked,
            this, &MainWindow::loadFlights);

    connect(ui->showStatisticsButton, &QPushButton::clicked,
            this, &MainWindow::showStatistics);

    connect(m_dbConnection, &DBConnection::connectionError,
            this, &MainWindow::showConnectionError);
}

void MainWindow::initializeData()
{
    if (m_dbConnection->isConnected()) {
        m_dbManager = new DBManager(this);
        connect(m_dbManager, &DBManager::airportsLoaded, this, &MainWindow::onAirportsLoaded);
        m_dbManager->loadAirports();
    }
}

void MainWindow::showConnectionError(const QString &error)
{
    QMessageBox::critical(this, "Ошибка подключения",
                          "Не удалось подключиться к базе данных:\n" + error);
}

void MainWindow::handleConnectionChange(bool connected)
{
    if (connected) {
        ui->statusBar->showMessage("Подключено", 0);
        ui->statusBar->setStyleSheet("color: green;");
    } else {
        ui->statusBar->showMessage("Отключено", 0);
        ui->statusBar->setStyleSheet("color: red;");
    }

    ui->reconnectButton->setEnabled(!connected);

    if (connected && !m_dbManager) {
        m_dbManager = new DBManager(this);
        connect(m_dbManager, &DBManager::airportsLoaded, this, &MainWindow::onAirportsLoaded);
        m_dbManager->loadAirports();
    } else if (!connected) {
        lockInterface(true);
    }
}

void MainWindow::onAirportsLoaded(bool success)
{
    if (success) {
        loadAirports();
    } else {
        ui->statusBar->showMessage("Ошибка загрузки аэропортов", 3000);
    }
}

void MainWindow::onReconnectClicked()
{
    ui->statusBar->showMessage("Переподключение...", 0);
    ui->reconnectButton->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_dbConnection->forceReconnect();
    QTimer::singleShot(500, [](){ QApplication::restoreOverrideCursor(); });
}

void MainWindow::loadAirports()
{
    if (!m_dbManager) return;

    ui->airportComboBox->clear();
    auto airports = m_dbManager->getAirports();

    if (airports.isEmpty()) {
        ui->statusBar->showMessage("Не удалось загрузить аэропорты", 3000);
        return;
    }

    for (const auto &airport : airports) {
        ui->airportComboBox->addItem(airport.name, airport.code);
    }

    lockInterface(false);
}

void MainWindow::loadFlights()
{
    if (!m_dbConnection->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }

    QString airportCode = ui->airportComboBox->currentData().toString();
    QDate date = ui->dateEdit->date();
    bool isArrival = ui->arrivalRadio->isChecked();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QVector<DBManager::Flight> flights = m_dbManager->getFlights(airportCode, date, isArrival);
    QApplication::restoreOverrideCursor();

    m_flightModel->setFlights(flights, isArrival);

    if (flights.isEmpty()) {
        ui->statusBar->showMessage("Нет рейсов для выбранных параметров", 3000);
    }
}

void MainWindow::showStatistics()
{
    if (!m_dbConnection->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }

    if (ui->airportComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите аэропорт");
        return;
    }

    QString airportName = ui->airportComboBox->currentText();
    QString airportCode = ui->airportComboBox->currentData().toString();

    StatisticsDialog dialog(airportName, airportCode, m_dbManager, this);
    dialog.exec();
}

void MainWindow::lockInterface(bool lock)
{
    ui->airportComboBox->setEnabled(!lock);
    ui->dateEdit->setEnabled(!lock);
    ui->arrivalRadio->setEnabled(!lock);
    ui->departureRadio->setEnabled(!lock);
    ui->showFlightsButton->setEnabled(!lock);
    ui->showStatisticsButton->setEnabled(!lock);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbConnection(new DBConnection(this))
    , m_dbManager(nullptr)
    , m_flightModel(new FlightModel(this))
{
    ui->setupUi(this);

    // Настройка интерфейса
    setupUI();
    setupConnections();

    // Инициализация данных
    initializeData();
}

void MainWindow::setupUI()
{
    // Настройка элементов интерфейса
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate(2016, 8, 15));
    ui->dateEdit->setMaximumDate(QDate(2017, 9, 14));

    // Настройка таблицы
    ui->flightsTableView->setModel(m_flightModel);
    ui->flightsTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::setupConnections()
{
    // Подключение сигналов БД
    connect(m_dbConnection, &DBConnection::connectionStatusChanged,
            this, &MainWindow::handleConnectionChange);

    // Кнопки интерфейса
    connect(ui->showFlightsButton, &QPushButton::clicked,
            this, &MainWindow::loadFlights);
    connect(ui->showStatisticsButton, &QPushButton::clicked,
            this, &MainWindow::showStatistics);
}

void MainWindow::initializeData()
{
    if (m_dbConnection->isConnected()) {
        m_dbManager = new DBManager(this);
        loadAirports();
    } else {
        showDemoData();
    }
}

void MainWindow::handleConnectionChange(bool connected)
{
    // Обновление статусбара
    QString status = connected ? "Подключено к БД" : "Отключено: " + m_dbConnection->lastError();
    ui->statusBar->showMessage(status);

    // Блокировка/разблокировка интерфейса
    ui->airportComboBox->setEnabled(connected);
    ui->dateEdit->setEnabled(connected);
    ui->arrivalRadio->setEnabled(connected);
    ui->departureRadio->setEnabled(connected);
    ui->showFlightsButton->setEnabled(connected);
    ui->showStatisticsButton->setEnabled(connected);

    if (connected && !m_dbManager) {
        m_dbManager = new DBManager(this);
        loadAirports();
    }
}

void MainWindow::loadAirports()
{
    if (!m_dbManager) return;

    ui->airportComboBox->clear();
    auto airports = m_dbManager->getAirports();

    if (airports.isEmpty()) {
        ui->airportComboBox->addItem("Нет данных", "");
        return;
    }

    for (const auto &airport : airports) {
        ui->airportComboBox->addItem(airport.name, airport.code);
    }
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

    QVector<DBManager::Flight> flights = isArrival
                                             ? m_dbManager->getArrivals(airportCode, date)
                                             : m_dbManager->getDepartures(airportCode, date);

    m_flightModel->setFlights(flights, isArrival);
}

void MainWindow::showDemoData()
{
    // Заполнение демо-данными при отсутствии подключения
    ui->airportComboBox->clear();
    ui->airportComboBox->addItem("Демо-аэропорт (режим оффлайн)", "DEMO");

    QMessageBox::information(this, "Информация",
                             "Работаем в демо-режиме. Функционал ограничен.\n"
                             "Ошибка подключения: " + m_dbConnection->lastError());
}

void MainWindow::showStatistics()
{
    if (!m_dbConnection->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }
    // ... реализация показа статистики
}

MainWindow::~MainWindow()
{
    delete ui;
}

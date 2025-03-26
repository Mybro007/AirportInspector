#include "dbconnection.h"
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

DBConnection::DBConnection(QObject *parent) : QObject(parent)
{
    // Настройка таймера переподключения
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000); // 5 секунд
    connect(m_reconnectTimer, &QTimer::timeout, this, &DBConnection::attemptReconnect);

    // Проверка доступности драйвера
    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        m_lastError = "Драйвер PostgreSQL (QPSQL) не доступен.\nУстановите необходимые драйверы.";
        qCritical() << m_lastError;
        return;
    }

    // Первоначальная настройка подключения
    setupDatabase();
    connectToDatabase();
}

void DBConnection::setupDatabase()
{
    // Закрываем предыдущее подключение, если было
    if (m_db.isOpen()) {
        m_db.close();
    }

    // Настройка параметров подключения
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("981757-ca08998.tmweb.ru");
    m_db.setPort(5432);
    m_db.setDatabaseName("demo");
    m_db.setUserName("netology_usr_cpp");
    m_db.setPassword("CppNeto3");
}

void DBConnection::connectToDatabase()
{
    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qCritical() << "Ошибка подключения:" << m_lastError;
        m_reconnectTimer->start();
    } else {
        m_reconnectTimer->stop();
        qDebug() << "Успешное подключение к БД";
    }
    emit connectionStatusChanged(isConnected());
}

void DBConnection::attemptReconnect()
{
    qDebug() << "Попытка переподключения...";
    if (m_db.isOpen()) {
        m_db.close();
    }
    connectToDatabase();
}

void DBConnection::forceReconnect()
{
    qDebug() << "Принудительное переподключение";
    m_reconnectTimer->stop();
    attemptReconnect();
}

bool DBConnection::isConnected() const
{
    return m_db.isOpen() && m_db.isValid();
}

QString DBConnection::lastError() const
{
    return m_lastError;
}

DBConnection::~DBConnection()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

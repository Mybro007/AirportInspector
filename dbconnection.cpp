#include "dbconnection.h"
#include <QSqlError>
#include <QDebug>

DBConnection::DBConnection(QObject *parent) : QObject(parent)
{
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000);
    connect(m_reconnectTimer, &QTimer::timeout, this, &DBConnection::attemptReconnect);

    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        m_lastError = "Драйвер PostgreSQL не доступен";
        qCritical() << m_lastError;
        return;
    }

    setupDatabase();
    attemptReconnect();
}

DBConnection::~DBConnection()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void DBConnection::setupDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }

    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("981757-ca08998.tmweb.ru");
    m_db.setPort(5432);
    m_db.setDatabaseName("demo");
    m_db.setUserName("netology_usr_cpp");
    m_db.setPassword("CppNeto3");
}

void DBConnection::attemptReconnect()
{
    qDebug() << "Попытка подключения к БД...";

    if (m_db.open()) {
        m_reconnectTimer->stop();
        qDebug() << "Подключение успешно";
    } else {
        m_lastError = m_db.lastError().text();
        qCritical() << "Ошибка подключения:" << m_lastError;
        m_reconnectTimer->start();
    }
    updateConnectionState();
}

void DBConnection::forceReconnect()
{
    qDebug() << "Принудительное переподключение...";
    m_reconnectTimer->stop();
    if (m_db.isOpen()) {
        m_db.close();
    }
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

void DBConnection::updateConnectionState()
{
    emit connectionStatusChanged(isConnected());
}

#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>

class DBConnection : public QObject
{
    Q_OBJECT

public:
    explicit DBConnection(QObject *parent = nullptr);
    ~DBConnection();

    bool isConnected() const;
    QString lastError() const;
    void forceReconnect();

    // Геттеры параметров подключения
    QString host() const { return m_db.hostName(); }
    int port() const { return m_db.port(); }
    QString databaseName() const { return m_db.databaseName(); }
    QString username() const { return m_db.userName(); }

signals:
    void connectionStatusChanged(bool isConnected);

private slots:
    void attemptReconnect();

private:
    QSqlDatabase m_db;
    QTimer *m_reconnectTimer;
    QString m_lastError;

    void setupDatabase();
    void updateConnectionState();
};

#endif // DBCONNECTION_H

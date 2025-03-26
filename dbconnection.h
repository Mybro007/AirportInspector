#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QTimer>
#include <QMessageBox>
#include <QSqlError>

class DBConnection : public QObject
{
    Q_OBJECT
public:
    explicit DBConnection(QObject *parent = nullptr);
    ~DBConnection();

    bool isConnected() const;
    QString lastError() const;

public slots:
    void connectToDatabase();
    void forceReconnect();

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
#endif

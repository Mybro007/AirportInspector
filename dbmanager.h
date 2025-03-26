#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QDate>
#include <QRegularExpression>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = nullptr);

    struct Airport {
        QString code;
        QString name;
    };

    struct Flight {
        QString flightNo;
        QDateTime time;
        QString airport;
    };

    struct StatisticsItem {
        QDate date;
        int count;
    };

    // Основные методы
    bool checkPostgreSQLVersion();
    QVector<Airport> getAirports();
    QVector<Flight> getArrivals(const QString &airportCode, const QDate &date);
    QVector<Flight> getDepartures(const QString &airportCode, const QDate &date);
    QVector<StatisticsItem> getYearStatistics(const QString &airportCode);
    QVector<StatisticsItem> getMonthStatistics(const QString &airportCode, int month);

    // Методы для работы с подключением
    QString lastError() const;
    bool testConnection();

private:
    QSqlDatabase m_db;

    QSqlQuery prepareQuery(const QString &query);
    bool executeQuery(QSqlQuery &query);
};

#endif // DBMANAGER_H

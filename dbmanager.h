#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QDate>

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
        QDateTime scheduledDeparture;
        QDateTime scheduledArrival;
        QString departureAirport;
        QString arrivalAirport;
        QString status;
    };

    struct StatisticsItem {
        QDate date;
        int count;
    };

    bool loadAirports();
    QVector<Flight> getFlights(const QString &airportCode, const QDate &date, bool arrivals);
    QVector<Airport> getAirports() const;
    QVector<StatisticsItem> getYearStatistics(const QString &airportCode);
    QVector<StatisticsItem> getMonthStatistics(const QString &airportCode, int month);

    const QVector<Airport>& airports() const { return m_airports; }
    QString lastError() const { return m_lastError; }
    bool hasAirportsData() const { return !m_airports.isEmpty(); }

signals:
    void airportsLoaded(bool success);

private:
    QSqlDatabase m_db;
    QVector<Airport> m_airports;
    QString m_lastError;

    bool executeQuery(const QString &query, QSqlQuery &result);
    QString extractAirportName(const QVariant &jsonData) const;
};

#endif // DBMANAGER_H

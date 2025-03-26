#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::database();
}

bool DBManager::checkPostgreSQLVersion()
{
    QSqlQuery query("SELECT version()");
    if (!executeQuery(query) || !query.next()) {
        return false;
    }

    QString versionStr = query.value(0).toString();
    QRegularExpression re("PostgreSQL (\\d+)\\.\\d+");
    QRegularExpressionMatch match = re.match(versionStr);

    return match.hasMatch() && match.captured(1).toInt() >= 15;
}

bool DBManager::executeQuery(QSqlQuery &query)
{
    if (!query.exec()) {
        qWarning() << "SQL Error:" << query.lastError().text();
        qWarning() << "Query:" << query.lastQuery();
        return false;
    }
    return true;
}

QString DBManager::lastError() const
{
    return m_db.lastError().text();
}

bool DBManager::testConnection()
{
    return m_db.isOpen() && checkPostgreSQLVersion();
}

QVector<DBManager::Airport> DBManager::getAirports()
{
    QVector<Airport> airports;
    QSqlQuery query("SELECT airport_name->>'ru' as name, airport_code FROM bookings.airports_data ORDER BY name");

    if (executeQuery(query)) {
        while (query.next()) {
            airports.append({
                query.value("airport_code").toString(),
                query.value("name").toString()
            });
        }
    }

    return airports;
}

QVector<DBManager::Flight> DBManager::getArrivals(const QString &airportCode, const QDate &date)
{
    QVector<Flight> flights;
    QSqlQuery query;
    query.prepare(
        "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as airport_name "
        "FROM bookings.flights f "
        "JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport "
        "WHERE f.arrival_airport = :airportCode AND DATE(scheduled_arrival) = :date "
        "ORDER BY scheduled_arrival"
        );
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);

    if (executeQuery(query)) {
        while (query.next()) {
            flights.append({
                query.value("flight_no").toString(),
                query.value("scheduled_arrival").toDateTime(),
                query.value("airport_name").toString()
            });
        }
    }

    return flights;
}

QVector<DBManager::Flight> DBManager::getDepartures(const QString &airportCode, const QDate &date)
{
    QVector<Flight> flights;
    QSqlQuery query;
    query.prepare(
        "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as airport_name "
        "FROM bookings.flights f "
        "JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport "
        "WHERE f.departure_airport = :airportCode AND DATE(scheduled_departure) = :date "
        "ORDER BY scheduled_departure"
        );
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);

    if (executeQuery(query)) {
        while (query.next()) {
            flights.append({
                query.value("flight_no").toString(),
                query.value("scheduled_departure").toDateTime(),
                query.value("airport_name").toString()
            });
        }
    }

    return flights;
}

QVector<DBManager::StatisticsItem> DBManager::getYearStatistics(const QString &airportCode)
{
    QVector<StatisticsItem> stats;
    QSqlQuery query;
    query.prepare(
        "SELECT DATE_TRUNC('month', scheduled_departure) AS month, COUNT(*) AS flights_count "
        "FROM bookings.flights "
        "WHERE (departure_airport = :airportCode OR arrival_airport = :airportCode) "
        "AND scheduled_departure BETWEEN '2016-08-15' AND '2017-09-14' "
        "GROUP BY month ORDER BY month"
        );
    query.bindValue(":airportCode", airportCode);

    if (executeQuery(query)) {
        while (query.next()) {
            stats.append({
                query.value("month").toDate(),
                query.value("flights_count").toInt()
            });
        }
    }

    return stats;
}

QVector<DBManager::StatisticsItem> DBManager::getMonthStatistics(const QString &airportCode, int month)
{
    QVector<StatisticsItem> stats;
    QSqlQuery query;
    query.prepare(
        "SELECT DATE(scheduled_departure) AS day, COUNT(*) AS flights_count "
        "FROM bookings.flights "
        "WHERE (departure_airport = :airportCode OR arrival_airport = :airportCode) "
        "AND EXTRACT(MONTH FROM scheduled_departure) = :month "
        "AND scheduled_departure BETWEEN '2016-08-15' AND '2017-09-14' "
        "GROUP BY day ORDER BY day"
        );
    query.bindValue(":airportCode", airportCode);
    query.bindValue(":month", month);

    if (executeQuery(query)) {
        while (query.next()) {
            stats.append({
                query.value("day").toDate(),
                query.value("flights_count").toInt()
            });
        }
    }

    return stats;
}

#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::database(); // Используем существующее подключение
}

bool DBManager::executeQuery(const QString &query, QSqlQuery &result)
{
    if (!result.exec(query)) {
        m_lastError = result.lastError().text();
        qCritical() << "SQL Error:" << m_lastError;
        qCritical() << "Failed query:" << query;
        return false;
    }
    return true;
}

QString DBManager::extractAirportName(const QVariant &jsonData) const
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toByteArray());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("ru")) {
            return obj["ru"].toString();
        }
    }
    return QString();
}

bool DBManager::loadAirports()
{
    m_airports.clear();
    QSqlQuery query;
    QString sql = "SELECT airport_code, airport_name FROM bookings.airports_data ORDER BY airport_name->>'ru'";

    if (!executeQuery(sql, query)) {
        emit airportsLoaded(false);
        return false;
    }

    while (query.next()) {
        Airport airport;
        airport.code = query.value("airport_code").toString();
        airport.name = extractAirportName(query.value("airport_name"));

        if (!airport.name.isEmpty()) {
            m_airports.append(airport);
        }
    }

    emit airportsLoaded(!m_airports.isEmpty());
    return !m_airports.isEmpty();
}

QVector<DBManager::Flight> DBManager::getFlights(const QString &airportCode, const QDate &date, bool arrivals)
{
    QVector<Flight> flights;
    QSqlQuery query;

    QString sql = arrivals ?
                      QString(
                          "SELECT f.flight_no, f.scheduled_departure, f.scheduled_arrival, "
                          "f.departure_airport, f.arrival_airport, f.status "
                          "FROM bookings.flights f "
                          "WHERE f.arrival_airport = '%1' AND DATE(f.scheduled_arrival) = '%2' "
                          "ORDER BY f.scheduled_arrival"
                          ).arg(airportCode, date.toString(Qt::ISODate)) :
                      QString(
                          "SELECT f.flight_no, f.scheduled_departure, f.scheduled_arrival, "
                          "f.departure_airport, f.arrival_airport, f.status "
                          "FROM bookings.flights f "
                          "WHERE f.departure_airport = '%1' AND DATE(f.scheduled_departure) = '%2' "
                          "ORDER BY f.scheduled_departure"
                          ).arg(airportCode, date.toString(Qt::ISODate));

    if (!executeQuery(sql, query)) {
        return flights;
    }

    while (query.next()) {
        Flight flight;
        flight.flightNo = query.value("flight_no").toString();
        flight.scheduledDeparture = query.value("scheduled_departure").toDateTime();
        flight.scheduledArrival = query.value("scheduled_arrival").toDateTime();
        flight.departureAirport = query.value("departure_airport").toString();
        flight.arrivalAirport = query.value("arrival_airport").toString();
        flight.status = query.value("status").toString();
        flights.append(flight);
    }

    return flights;
}

QVector<DBManager::Airport> DBManager::getAirports() const
{
    return m_airports;
}

QVector<DBManager::StatisticsItem> DBManager::getYearStatistics(const QString &airportCode)
{
    QVector<StatisticsItem> result;
    QSqlQuery query;

    QString sql = QString(
                      "SELECT DATE_TRUNC('month', scheduled_departure) AS month, "
                      "COUNT(*) AS count "
                      "FROM bookings.flights f "
                      "WHERE (departure_airport = '%1' OR arrival_airport = '%1') "
                      "AND scheduled_departure BETWEEN '2016-08-15' AND '2017-09-14' "
                      "GROUP BY month "
                      "ORDER BY month"
                      ).arg(airportCode);

    if (executeQuery(sql, query)) {
        while (query.next()) {
            StatisticsItem item;
            item.date = query.value("month").toDate();
            item.count = query.value("count").toInt();
            result.append(item);
        }
    }

    return result;
}

QVector<DBManager::StatisticsItem> DBManager::getMonthStatistics(const QString &airportCode, int month)
{
    QVector<StatisticsItem> result;
    QSqlQuery query;

    QString sql = QString(
                      "SELECT DATE(scheduled_departure) AS day, "
                      "COUNT(*) AS count "
                      "FROM bookings.flights f "
                      "WHERE (departure_airport = '%1' OR arrival_airport = '%1') "
                      "AND EXTRACT(MONTH FROM scheduled_departure) = %2 "
                      "AND scheduled_departure BETWEEN '2016-08-15' AND '2017-09-14' "
                      "GROUP BY day "
                      "ORDER BY day"
                      ).arg(airportCode).arg(month);

    if (executeQuery(sql, query)) {
        while (query.next()) {
            StatisticsItem item;
            item.date = query.value("day").toDate();
            item.count = query.value("count").toInt();
            result.append(item);
        }
    }

    return result;
}

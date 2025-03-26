#include "flightmodel.h"

FlightModel::FlightModel(QObject *parent) : QAbstractTableModel(parent), m_isArrival(false) {}

int FlightModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_flights.size();
}

int FlightModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3; // Номер рейса, Время, Аэропорт
}

QVariant FlightModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_flights.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &flight = m_flights.at(index.row());

        switch (index.column()) {
        case 0: return flight.flightNo;
        case 1: return flight.time.toString("HH:mm");
        case 2: return flight.airport;
        }
    }

    return QVariant();
}

QVariant FlightModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
    case 0: return "Номер рейса";
    case 1: return m_isArrival ? "Время прилета" : "Время вылета";
    case 2: return m_isArrival ? "Аэропорт отправления" : "Аэропорт назначения";
    default: return QVariant();
    }
}

void FlightModel::setFlights(const QVector<DBManager::Flight> &flights, bool isArrival)
{
    beginResetModel();
    m_flights = flights;
    m_isArrival = isArrival;
    endResetModel();
}

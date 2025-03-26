#ifndef FLIGHTMODEL_H
#define FLIGHTMODEL_H

#include <QAbstractTableModel>
#include "dbmanager.h"

class FlightModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FlightModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setFlights(const QVector<DBManager::Flight> &flights, bool isArrival);

private:
    QVector<DBManager::Flight> m_flights;
    bool m_isArrival;
};

#endif // FLIGHTMODEL_H

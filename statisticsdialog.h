#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QtCharts>
#include "dbmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class StatisticsDialog; }
QT_END_NAMESPACE

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(const QString &airportName, const QString &airportCode,
                              DBManager *dbManager, QWidget *parent = nullptr);
    ~StatisticsDialog();

private slots:
    void onMonthChanged(int index);

private:
    Ui::StatisticsDialog *ui;
    DBManager *m_dbManager;
    QString m_airportCode;

    void setupYearChart();
    void setupMonthChart();
    void loadMonthData(int month);
};

#endif // STATISTICSDIALOG_H

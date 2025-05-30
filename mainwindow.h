#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbconnection.h"
#include "dbmanager.h"
#include "flightmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleConnectionChange(bool connected);
    void onAirportsLoaded(bool success);
    void onReconnectClicked();
    void loadAirports();
    void loadFlights();
    void showStatistics();
    void showConnectionError(const QString &error);

private:
    Ui::MainWindow *ui;
    DBConnection *m_dbConnection;
    DBManager *m_dbManager;
    FlightModel *m_flightModel;

    void setupUI();
    void setupConnections();
    void initializeData();
    void lockInterface(bool lock);
};

#endif // MAINWINDOW_H

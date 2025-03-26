#ifndef DBSETTINGSDIALOG_H
#define DBSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class DBSettingsDialog;
}

class DBSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DBSettingsDialog(const QString &host, int port,
                              const QString &dbName,
                              const QString &user,
                              const QString &password,
                              QWidget *parent = nullptr);
    ~DBSettingsDialog();

    QString host() const;
    int port() const;
    QString dbName() const;
    QString user() const;
    QString password() const;

private:
    Ui::DBSettingsDialog *ui;
};

#endif // DBSETTINGSDIALOG_H

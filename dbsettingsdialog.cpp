#include "dbsettingsdialog.h"
#include "ui_dbsettingsdialog.h"

DBSettingsDialog::DBSettingsDialog(const QString &host, int port,
                                   const QString &dbName,
                                   const QString &user,
                                   const QString &password,
                                   QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DBSettingsDialog)
{
    ui->setupUi(this);

    // Установка текущих значений
    ui->hostEdit->setText(host);
    ui->portSpinBox->setValue(port);
    ui->dbNameEdit->setText(dbName);
    ui->userEdit->setText(user);
    ui->passwordEdit->setText(password); // Устанавливаем пароль

    // Настройка валидации
    ui->hostEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".+"), this));
    ui->dbNameEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".+"), this));
    ui->userEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".+"), this));
}

DBSettingsDialog::~DBSettingsDialog()
{
    delete ui;
}

QString DBSettingsDialog::host() const
{
    return ui->hostEdit->text().trimmed();
}

int DBSettingsDialog::port() const
{
    return ui->portSpinBox->value();
}

QString DBSettingsDialog::dbName() const
{
    return ui->dbNameEdit->text().trimmed();
}

QString DBSettingsDialog::user() const
{
    return ui->userEdit->text().trimmed();
}

QString DBSettingsDialog::password() const
{
    return ui->passwordEdit->text();
}

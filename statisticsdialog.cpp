#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(const QString &airportName, const QString &airportCode,
                                   DBManager *dbManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StatisticsDialog)
    , m_dbManager(dbManager)
    , m_airportCode(airportCode)
{
    ui->setupUi(this);
    setWindowTitle("Статистика загруженности: " + airportName);

    // Настройка вкладок
    setupYearChart();
    setupMonthChart();

    // Установка января по умолчанию
    ui->monthComboBox->setCurrentIndex(0);
    loadMonthData(1);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

void StatisticsDialog::setupYearChart()
{
    auto stats = m_dbManager->getYearStatistics(m_airportCode);

    QBarSeries *series = new QBarSeries();
    QBarSet *barSet = new QBarSet("Рейсов за месяц");

    QStringList categories;
    for (const auto &item : stats) {
        *barSet << item.count;
        categories << item.date.toString("MMM yyyy");
    }

    series->append(barSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Загруженность аэропорта за год");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->yearChartView->setChart(chart);
    ui->yearChartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsDialog::setupMonthChart()
{
    // Настройка выпадающего списка месяцев
    QStringList months;
    months << "Январь" << "Февраль" << "Март" << "Апрель"
           << "Май" << "Июнь" << "Июль" << "Август"
           << "Сентябрь" << "Октябрь" << "Ноябрь" << "Декабрь";
    ui->monthComboBox->addItems(months);

    connect(ui->monthComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StatisticsDialog::onMonthChanged);
}

void StatisticsDialog::loadMonthData(int month)
{
    auto stats = m_dbManager->getMonthStatistics(m_airportCode, month);

    QLineSeries *series = new QLineSeries();
    series->setName("Рейсов за день");

    int maxValue = 0;
    for (const auto &item : stats) {
        series->append(item.date.day(), item.count);
        if (item.count > maxValue) {
            maxValue = item.count;
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("Загруженность аэропорта за %1")
                        .arg(ui->monthComboBox->currentText()));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("День месяца");
    axisX->setLabelFormat("%d");
    axisX->setRange(1, 31);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Количество рейсов");
    axisY->setLabelFormat("%d");
    axisY->setRange(0, maxValue + 5);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->monthChartView->setChart(chart);
    ui->monthChartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsDialog::onMonthChanged(int index)
{
    loadMonthData(index + 1); // +1 так как месяцы с 1 до 12
}

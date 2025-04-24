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
    QDate minDate(2016, 8, 1);
    QDate maxDate(2017, 9, 1);

    // Создаем все месяцы в диапазоне
    for (QDate date = minDate; date <= maxDate; date = date.addMonths(1)) {
        bool found = false;
        for (const auto &item : stats) {
            if (item.date.toString("yyyy-MM") == date.toString("yyyy-MM")) {
                *barSet << item.count;
                found = true;
                break;
            }
        }
        if (!found) {
            *barSet << 0;
        }
        categories << date.toString("MMM yyyy");
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
    QDate monthDate(2016, month, 1);

    // Заполняем все дни месяца
    for (int day = 1; day <= monthDate.daysInMonth(); ++day) {
        bool found = false;
        for (const auto &item : stats) {
            if (item.date.day() == day) {
                series->append(day, item.count);
                if (item.count > maxValue) {
                    maxValue = item.count;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            series->append(day, 0);
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

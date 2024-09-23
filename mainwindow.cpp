#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "cmath"
#include <iostream>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

double calculateMean(const QVector<QPair<double, double>>& points) {
    double mean = 0.0;
    for (const auto &point : points) {
        mean += point.first * point.second;
    }
    return mean;
}

std::vector<double> calculateCentralMoments(const QVector<QPair<double, double>>& points, double mean, int maxOrder) {
    std::vector<double> centralMoments(maxOrder, 0.0);
    for (const auto &point : points) {
        double diff = point.first - mean;
        double power = 1.0;
        for (int j = 0; j < maxOrder; ++j) {
            power *= diff;
            centralMoments[j] += power * point.second;
        }
    }
    return centralMoments;
}

std::vector<double> calculateInitialMoments(const QVector<QPair<double, double>>& points, int maxOrder) {
    std::vector<double> initialMoments(maxOrder, 0.0);
    for (const auto &point : points) {
        double power = 1.0;
        for (int j = 0; j < maxOrder; ++j) {
            power *= point.first;
            initialMoments[j] += power * point.second;
        }
    }
    return initialMoments;
}

double calculateCumulativeProbability(const QVector<QPair<double, double>>& points, double x0) {
    double cumulativeProbability = 0.0;
    for (const auto &point : points) {
        if (point.first <= x0) {
            cumulativeProbability += point.second;
        }
    }
    return cumulativeProbability;
}

void MainWindow::on_pushButton_clicked()
{
    ui->textEdit->setPlainText("");
    ui->widget->xAxis->setRange(-4.0, 5);
    ui->widget->yAxis->setRange(0, 0.5);

    if(ui->radioButton->isChecked()){
        ui->widget->clearGraphs();
        points.clear();
        xData.clear();
        yData.clear();

        points.append(QPair<double, double>(-2.0, 0.1));
        points.append(QPair<double, double>(-1, 0.12));
        points.append(QPair<double, double>(0, 0.2));
        points.append(QPair<double, double>(1.0, 0.25));
        points.append(QPair<double, double>(2.0, 0.18));
        points.append(QPair<double, double>(3.0, 0.1));
        points.append(QPair<double, double>(4.0, 0.05));

        for (const auto &point : points) {
            xData.append(point.first);
            yData.append(point.second);
        }

        ui->widget->addGraph();
        ui->widget->graph(0)->addData(xData, yData);
        ui->widget->replot();
    }
    else if(ui->radioButton_2->isChecked()){
        ui->widget->clearGraphs();

        ui->widget->xAxis->setRange(-4.0, 5);
        ui->widget->yAxis->setRange(0, 1.2);

        QVector<double> cumulativeYData;
        double cumulative = 0.0;
        for (const auto &point : points) {
            cumulative += point.second;
            cumulativeYData.append(cumulative);
            xData.append(point.first);
        }

        ui->widget->addGraph();
        ui->widget->graph(0)->addData(xData, cumulativeYData);
        ui->widget->replot();
    }

    double mean = calculateMean(points);

    std::vector<double> centralMoments = calculateCentralMoments(points, mean, 4);
    std::vector<double> initialMoments = calculateInitialMoments(points, 4);

    double variance = centralMoments[1];
    double stdDev = std::sqrt(variance);
    double asymmetry = (centralMoments[2] / std::pow(stdDev, 3));
    double excess = 0.0;
    for (const auto &point : points) {
        excess += point.second * std::pow(point.first - mean, 4);
    }
    excess = excess / std::pow(stdDev, 4) - 3;

    double x0 = -1.0;
    double cumulativeProb = calculateCumulativeProbability(points, x0);

    double mode = 0.0;
    double maxCount = 0.0;
    for (const auto &point : points) {
        if (point.second > maxCount) {
            maxCount = point.second;
            mode = maxCount;
        }
    }

    QVector<double> sortedXData = xData;
    std::sort(sortedXData.begin(), sortedXData.end());
    double median = 0.0;
    int n = sortedXData.size();
    if (n % 2 == 0) {
        median = (sortedXData[n / 2 - 1] + sortedXData[n / 2]) / 2.0;
    } else {
        median = sortedXData[n / 2];
    }


    ui->textEdit->insertPlainText("нач-е моменты - ");
    for (double moment : initialMoments) {
        ui->textEdit->insertPlainText("\n");
        ui->textEdit->insertPlainText(QString::number(moment));
    }

    ui->textEdit->insertPlainText("\n\nцентр-е моменты - ");
    for (double moment : centralMoments) {
        ui->textEdit->insertPlainText("\n");
        ui->textEdit->insertPlainText(QString::number(moment));
    }

    ui->textEdit->insertPlainText("\n\nсреднеквадр. отклонение - ");
    ui->textEdit->insertPlainText(QString::number(mean));
    ui->textEdit->insertPlainText("\n\nмода - ");
    ui->textEdit->insertPlainText(QString::number(mode));
    ui->textEdit->insertPlainText("\n\nмедиана - ");
    ui->textEdit->insertPlainText(QString::number(median));
    ui->textEdit->insertPlainText("\n\nдисперсия - ");
    ui->textEdit->insertPlainText(QString::number(stdDev));
    ui->textEdit->insertPlainText("\n\nассиметрия - ");
    ui->textEdit->insertPlainText(QString::number(asymmetry));
    ui->textEdit->insertPlainText("\n\nэксцесс - ");
    ui->textEdit->insertPlainText(QString::number(excess));
    ui->textEdit->insertPlainText("\n\nвероятность P(X ≤ ) - ");
    ui->textEdit->insertPlainText(QString::number(cumulativeProb));
}



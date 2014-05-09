#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

using namespace std;

static const double h = 6.62606957e-34;
static const double c = 299792458.0;
static const double kb = 1.3806488e-23;

/* Planck function */
static inline double planck(const double temperature, const double freq) {
    double first = (2*h*freq*freq*freq)/(c*c);
    double discriminent = (exp((h * freq)/(kb * temperature))-1.0);
    double value = first / discriminent;
    return value;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->addPlot();
    this->redrawPlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::redrawPlots() {
    double x_bounds[2];
    double y_bounds[2];
    int steps = ui->spSteps->value();
    x_bounds[0] = ui->txtStartFreq->text().toDouble();
    x_bounds[1] = ui->txtEndFreq->text().toDouble();
    y_bounds[0] = 0.0;
    y_bounds[1] = 0.0;
    if(x_bounds[0] > x_bounds[1]) {
        double tmp = x_bounds[0];
        x_bounds[0] = x_bounds[1];
        x_bounds[1] = tmp;
    }


    QCustomPlot *customPlot = ui->customPlot;

    customPlot->clearGraphs();
    customPlot->xAxis->setLabel("Frequency [Hz]");
    customPlot->yAxis->setLabel("Planck fuction");
    customPlot->xAxis->setRange(x_bounds[0], x_bounds[1]);
    for(int i=0;i<temperatures.size();i++) {
        const double temperature = temperatures[i];
        const QColor color = colors[i];

        QVector<double> x(steps+1), y(steps+1);
        double increase = (x_bounds[1]-x_bounds[0]) / (double)(steps);
        double freq;
        for(int i=0;i<steps;i++) {
            freq = x_bounds[0] + increase * (double)i;
            const double value = planck(temperature, freq);
            x[i] = increase * (double)i;
            y[i] = value;
            if(value < y_bounds[0]) y_bounds[0] = value;
            if(value > y_bounds[1]) y_bounds[1] = value;
        }


        QCPGraph *graph = customPlot->addGraph();
        graph->setPen(QPen(color));
        graph->setData(x,y);
    }

    customPlot->yAxis->setRange(y_bounds[0], y_bounds[1]);
    customPlot->replot();

}

void MainWindow::changeColor() {
    int red = ui->slRed->value();
    int green = ui->slGreen->value();
    int blue = ui->slBlue->value();

    QColor color(red,green,blue);
    // TODO: Display current color somewhere
}

void MainWindow::refreshListModel() {
    const int plotcount = temperatures.size();
    QStandardItemModel *model = new QStandardItemModel(plotcount,2,this);
    for(int i=0;i<plotcount;i++) {
        const double temperature = temperatures[i];
        const QColor color = colors[i];
        model->setItem(i,0, new QStandardItem(QString::number(temperature)));
        model->setItem(i,1, new QStandardItem(QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue())));
    }
    ui->lstPlots->setModel(model);

}

void MainWindow::addPlot(const double temperature, const QColor& color) {
    temperatures.append(temperature);
    colors.append(color);
    this->refreshListModel();
}

void MainWindow::addPlot() {
    int r = ui->slRed->value();
    int g = ui->slGreen->value();
    int b = ui->slBlue->value();
    this->addPlot(ui->spTemperature->value(), QColor(r,g,b));

}

void MainWindow::on_btnAddPlot_pressed()
{
    int red = ui->slRed->value();
    int green = ui->slGreen->value();
    int blue = ui->slBlue->value();
    QColor color(red,green,blue);
    this->addPlot(ui->spTemperature->value(), color);
    this->redrawPlots();
}

void MainWindow::on_slRed_valueChanged(int value)
{
    this->changeColor();
}

void MainWindow::on_slGreen_valueChanged(int value)
{
    this->changeColor();
}

void MainWindow::on_slBlue_valueChanged(int value)
{
    this->changeColor();
}

void MainWindow::on_txtStartFreq_textChanged(const QString &arg1)
{

}

void MainWindow::on_txtEndFreq_returnPressed()
{
    this->redrawPlots();
}

void MainWindow::on_txtStartFreq_returnPressed()
{
    this->redrawPlots();
}

void MainWindow::on_spSteps_valueChanged(int arg1)
{
    this->redrawPlots();
}

void MainWindow::on_actionAdd_plot_triggered()
{
    int red = ui->slRed->value();
    int green = ui->slGreen->value();
    int blue = ui->slBlue->value();
    QColor color(red,green,blue);
    this->addPlot(ui->spTemperature->value(), color);
    this->redrawPlots();
}

void MainWindow::on_actionDelete_plot_triggered()
{
    QModelIndexList indices = ui->lstPlots->selectionModel()->selectedIndexes();
    if(indices.size() == 0) return;
    for(int i=0; i<indices.size();i++) {
        int index = indices[i].row();

        temperatures.remove(index);
        colors.remove(index);
    }

    this->refreshListModel();
    this->redrawPlots();
}

void MainWindow::on_actionClear_plots_triggered()
{
    temperatures.clear();
    colors.clear();
    this->refreshListModel();
    this->redrawPlots();
}

void MainWindow::on_txtEndFreq_editingFinished()
{
    this->redrawPlots();
}

void MainWindow::on_txtStartFreq_editingFinished()
{
    this->redrawPlots();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

void MainWindow::on_actionExport_triggered()
{
    exportPlot();
}


void MainWindow::exportPlot() {
    QString selectedFilter = tr("Portable network graphic (*.png)");
    QString filename = QFileDialog::getSaveFileName(this, "Export image", "", tr("Portable network graphic (*.png);; JPEG-compressed (*.jpg);; Bitmap (*.bmp);; Portable document file (*.pdf)"), &selectedFilter);
    if(filename.isEmpty()) {
        ui->statusBar->showMessage("Cancelled");
        return;
    }

    if(!filename.contains(".")) {
        ui->statusBar->showMessage("No known extension");
        return;
    }

    QString ext = filename.mid(filename.lastIndexOf("."));

    if(ext == ".png") {
        ui->statusBar->showMessage("Exporting as PNG ... ");
        ui->customPlot->savePng(filename);
    } else if (ext == ".jpg" || ext == ".jpeg") {
        ui->statusBar->showMessage("Exporting as JPG ... ");
        ui->customPlot->saveJpg(filename);
    } else if (ext == ".bmp") {
        ui->statusBar->showMessage("Exporting as Bitmap ... ");
        ui->customPlot->saveBmp(filename);
    } else if (ext == ".pdf") {
        ui->statusBar->showMessage("Exporting as PDF ... ");
        ui->customPlot->savePdf(filename);
    } else {
        ui->statusBar->showMessage("No known extension");
        return;
    }

    ui->statusBar->showMessage("Export completed");

}

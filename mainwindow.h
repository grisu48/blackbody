#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addPlot();
    void addPlot(const double, const QColor &);
    void redrawPlots();
    void refreshListModel();

    void changeColor();

private slots:

    void on_btnAddPlot_pressed();
    void on_slRed_valueChanged(int value);

    void on_slGreen_valueChanged(int value);

    void on_slBlue_valueChanged(int value);

    void on_txtStartFreq_textChanged(const QString &arg1);

    void on_txtEndFreq_returnPressed();

    void on_txtStartFreq_returnPressed();

    void on_spSteps_valueChanged(int arg1);

    void on_actionAdd_plot_triggered();

    void on_actionDelete_plot_triggered();

    void on_actionClear_plots_triggered();

    void on_txtEndFreq_editingFinished();

    void on_txtStartFreq_editingFinished();

private:
    Ui::MainWindow *ui;

    QVector<double> temperatures;
    QVector<QColor> colors;
};

#endif // MAINWINDOW_H

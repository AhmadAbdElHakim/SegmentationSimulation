#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QStackedBarSeries>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    QPushButton *exit = new QPushButton("Exit Program");

    ~MainWindow();

private:

    Ui::MainWindow *ui;
    int algorithmNumber = 0, itr1 = 0, itr2 = 0, itr3 = 0, itr4 = 0, processCounter = 0, allSegsItr = 0, holes = 0, segsStart;
    bool firstIsHole = 0, lastIsHole = 0;
    float m[100][4], holesSize = 0;
    double size = 0;
    QString segs[100][5], tempNames[100][2];
    QLabel *m1 = new QLabel("Please enter the total memory size:");
    QInputDialog *memorySize = new QInputDialog;
    QLabel *m2 = new QLabel("Please enter the number of holes:");
    QInputDialog *holesNo = new QInputDialog;
    QLabel *m3 = new QLabel("Please enter the number of segments for the next process:");
    QInputDialog *segmentsNo = new QInputDialog;
    QInputDialog *showSeg = new QInputDialog;
    QInputDialog *deReserveSeg = new QInputDialog;
    QInputDialog *deProcess = new QInputDialog;
    QGridLayout *layout = new QGridLayout;
    QStandardItemModel  *model = new QStandardItemModel;
    QStandardItemModel  *process = new QStandardItemModel;
    QStackedBarSeries *memory = new QStackedBarSeries;
    QTableView *showTable = new QTableView();
    QBarSet *s[100];
    QChart *chart = new QChart;
    QChartView *showMemory = new QChartView(chart);

    void createMainMenu();

public slots:

    void restart();
    void getSize(const double s);
    void getNumberOfHoles(const int holes);
    void getHole(const QModelIndex &index);
    void getProcess(const int rows);
    void getSegment(const QModelIndex &index);
    void getAlgorithm(const int algorithmNO);
    void getDeProcess(const int n);
    void getDeReserved(const int n);
    void showSegTable(const int n);
};
#endif // MAINWINDOW_H

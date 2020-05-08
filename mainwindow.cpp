#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QtCharts>
#include <QBarSet>
#include <QStackedBarSeries>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTableView>
#include <QDebug>
#include <QSpinBox>
#include <QInputDialog>
#include <QValueAxis>
#include <QRandomGenerator>
#include <QLegend>
#include <QLegendMarker>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE

using namespace std;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMainMenu();
}


void MainWindow::createMainMenu()
{
    setWindowTitle("Memory Allocation");

    QLabel *m1 = new QLabel("Please enter the total memory size:");
    QInputDialog *memorySize = new QInputDialog;
    //QLabel *m2 = new QLabel("Please enter the number of holes:");
    QInputDialog *holesNo = new QInputDialog;
    //QLabel *m3 = new QLabel("Please enter the number of segments for the next process:");
    QInputDialog *segmentsNo = new QInputDialog;
    //QPushButton *allocate = new QPushButton("Allocate");
    QLabel *m8 = new QLabel("Please choose the algorithm:");
    QComboBox *alg = new QComboBox;
    QPushButton *reset = new QPushButton("Reset");

    memorySize->setLabelText("");
    memorySize->setOption(QInputDialog::NoButtons);
    memorySize->setInputMode(QInputDialog::IntInput);
    QObject::connect(memorySize, SIGNAL(intValueChanged(int)), this, SLOT(getSize(int)));

    holesNo->setLabelText("");
    holesNo->setInputMode(QInputDialog::IntInput);
    holesNo->setCancelButtonText("");

    segmentsNo->setLabelText("");
    segmentsNo->setInputMode(QInputDialog::IntInput);
    segmentsNo->setCancelButtonText("");

    alg->addItem("First Fit");
    alg->addItem("Best Fit");

    layout->setVerticalSpacing(10);
    layout->setHorizontalSpacing(10);

    layout->setColumnMinimumWidth(0, 450);
    layout->setColumnMinimumWidth(1, 450);

    layout->addWidget(m1, 0, 0);
    layout->addWidget(memorySize, 0, 1);
    layout->addWidget(m2, 1, 0);
    layout->addWidget(holesNo, 1, 1);
    //layout->addWidget(allocate, 6, 0);
    layout->addWidget(m3, 7, 0);
    layout->addWidget(segmentsNo, 7, 1);
    layout->addWidget(m8, 8, 0);
    layout->addWidget(alg, 8, 1);
    layout->addWidget(reset, 23, 0);
    layout->addWidget(exit, 23, 1);

    QObject::connect(alg, SIGNAL(activated(int)), this, SLOT(getAlgorithm(int)));
    QObject::connect(holesNo, SIGNAL(intValueSelected(int)), this, SLOT(getNumberOfHoles(int)));
    QObject::connect(segmentsNo, SIGNAL(intValueSelected(int)), this, SLOT(getProcess(int)));
    QObject::connect(reset, SIGNAL(clicked()), this, SLOT(restart()));

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
    resize(1000,1000);
}

void MainWindow::getSize(const int s)
{
    size = s;
}

void MainWindow::getNumberOfHoles(const int holes)
{
    model->setRowCount(holes);

    for (int row = 0; row < model->rowCount(); ++row)
    {
        for (int column = 0; column < model->columnCount(); ++column)
        {
            QStandardItem *item = new QStandardItem();
            model->setItem(row, column, item);
        }
    }

    model->setHorizontalHeaderItem(0, new QStandardItem(tr("Base (starting address)")));
    model->setHorizontalHeaderItem(1, new QStandardItem(tr("Limit (size)")));

    showTable->setModel(model);
    showTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(showTable, 3, 0, 3, 1);
    //layout->addWidget(drawGantt, 7, 0, 1, 1);

    QObject::connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(getHole(QModelIndex)));
}

void MainWindow::getHole(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString number = index.data().toString();

        for(int i = 0, l = number.length(); i < l; i++)
        {
            if(!(number[i].isDigit() || number[i] == '.'))
            {
                QMessageBox enteredStirng;
                enteredStirng.setText("Please enter a number");
                enteredStirng.exec();
                restart();
                break;
            }
        }
        if(index.column() == 0)
        {
            baseFlag = 1;
            m[itr1][0] = index.data().toFloat();
            itr1++;
        }
        if(index.column() == 1)
        {
            limitFlag = 1;
            m[itr2][1] = index.data().toFloat();
            m[itr2][2] = -1;
            m[itr2][3] = index.row() - contiguousHoles + 1;
            itr2++;
        }
        if(baseFlag == 1 && limitFlag == 1)
        {
            baseFlag = 0;
            limitFlag = 0;

            if(basePlusLimit == m[itr1 - 1][0] && basePlusLimit != 0)
            {
                itr2 -=2;
                m[itr2][1] = m[itr2][1] + m[itr2 + 1][1];
                m[itr2][2] = -1;
                m[itr2][3] = index.row();
                itr1--;
                itr2++;
                contiguousHoles++;
            }
            else
            {
                basePlusLimit = m[itr1 - 1][0] + m[itr2 - 1][1];
            }
        }

        if(itr1 + contiguousHoles == model->rowCount() && itr2 + contiguousHoles == model->rowCount())
        {
            if(m[0][0] == 0)
            {
                firstIsHole = 1;
            }

            if(m[itr1 - 1][0] + m[itr2 - 1][1] == size)
            {
                lastIsHole = 1;
            }

            itr1--;
            itr2--;
            itr1 = itr1 * 2;
            itr2 = itr2 * 2;

            qDebug()<<firstIsHole<<lastIsHole;

            if(firstIsHole == 1 && lastIsHole == 1)
            {
                for(int i = itr1; i >= 2; i -= 2)
                {
                    m[i][0] = m[i / 2][0];
                    m[i][1] = m[i / 2][1];
                    m[i][2] = -1;
                    m[i][3] = m[i / 2][3];

                    m[i - 1][0] = m[(i / 2) - 1][0] + m[(i / 2) - 1][1];
                    m[i - 1][1] = m[i][0] - m[i - 1][0];
                    m[i - 1][2] = -2;
                    m[i - 1][3] = m[i][3] - 1;
                }
            }
            else if(firstIsHole == 0 && lastIsHole == 0)
            {
                for(int i = itr1 + 1; i >= 1; i -= 2)
                {
                    m[i][0] = m[i / 2][0];
                    m[i][1] = m[i / 2][1];
                    m[i][2] = -1;
                    m[i][3] = m[i / 2][3];

                    m[i - 1][0] = m[(i / 2) - 1][0] + m[(i / 2) - 1][1];
                    m[i - 1][1] = m[i][0] - m[i - 1][0];
                    m[i - 1][2] = -2;
                    m[i - 1][3] = m[i][3];
                }

                m[0][0] = 0;
                m[0][1] = m[1][0];
                m[0][2] = -2;
                m[0][3] = 1;

                m[itr1 + 2][0] = m[itr1 + 1][0] + m[itr1 + 1][1];
                m[itr1 + 2][1] = size - m[itr1 + 2][0];
                m[itr1 + 2][2] = -2;
                m[itr1 + 2][3] = m[itr1 + 1][3] + 1;
                itr1 += 2;
            }
            else if(firstIsHole == 1 && lastIsHole == 0)
            {
                for(int i = itr1; i >= 2; i -= 2)
                {
                    m[i][0] = m[i / 2][0];
                    m[i][1] = m[i / 2][1];
                    m[i][2] = -1;
                    m[i][3] = m[i / 2][3];

                    m[i - 1][0] = m[(i / 2) - 1][0] + m[(i / 2) - 1][1];
                    m[i - 1][1] = m[i][0] - m[i - 1][0];
                    m[i - 1][2] = -2;
                    m[i - 1][3] = m[i][3] - 1;
                }

                m[itr1 + 1][0] = m[itr1][0] + m[itr1][1];
                m[itr1 + 1][1] = size - m[itr1 + 1][0];
                m[itr1 + 1][2] = -2;
                m[itr1 + 1][3] = m[itr1][3];
                itr1++;
            }
            else if(firstIsHole == 0 && lastIsHole == 1)
            {
                for(int i = itr1 + 1; i >= 1; i -= 2)
                {
                    m[i][0] = m[i / 2][0];
                    m[i][1] = m[i / 2][1];
                    m[i][2] = -1;
                    m[i][3] = m[i / 2][3];

                    m[i - 1][0] = m[(i / 2) - 1][0] + m[(i / 2) - 1][1];
                    m[i - 1][1] = m[i][0] - m[i - 1][0];
                    m[i - 1][2] = -2;
                    m[i - 1][3] = m[i][3];
                }

                m[0][0] = 0;
                m[0][1] = m[1][0];
                m[0][2] = -2;
                m[0][3] = 1;
                itr1++;
            }

            //for(int i = itr1; i >= 0; i--)
            for(int i = 0; i <= itr1; i++)
            {
                s[i] = new QBarSet(QString("Hole%1").arg(m[i][3]));
                s[i]->append(m[i][1]);
                if(m[i][2] == -2)
                {
                    s[i]->setLabel(QString("Preallocated%1").arg(m[i][3]));
                    s[i]->setColor(Qt::black);
                }
                memory->append(s[i]);
            }

            chart->addSeries(memory);
            chart->setTitle("Memory Layout");
            chart->setAnimationOptions(QChart::SeriesAnimations);

            QStringList categoriesX;
            categoriesX << "Memory";

            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(categoriesX);
            chart->addAxis(axisX, Qt::AlignBottom);
            memory->attachAxis(axisX);

            QValueAxis *axisY = new QValueAxis();
            chart->addAxis(axisY, Qt::AlignLeft);
            memory->attachAxis(axisY);

            axisY->setReverse(true);
            axisY->setTickCount(size + 1);
            axisY->setLabelFormat("%.0f");
            axisY->setTitleText("Size");

            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignRight);

            showMemory->setRenderHint(QPainter::Antialiasing);
            layout->addWidget(showMemory, 0, 3, 12, 1);
        }
    }
}

void MainWindow::getProcess(const int rows)
{
    m2->hide();
    layout->addWidget(m3, 1, 0);
    process->setRowCount(rows);

    for (int row = 0; row < process->rowCount(); ++row)
    {
        for (int column = 0; column < process->columnCount(); ++column)
        {
            QStandardItem *item = new QStandardItem();
            process->setItem(row, column, item);
        }
    }

    process->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    process->setHorizontalHeaderItem(1, new QStandardItem(tr("Size")));

    showTable->setModel(process);
    showTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(showTable, 3, 0, 3, 1);

    QObject::connect(process, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(getSegment(QModelIndex)));
}

void MainWindow::getSegment(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString number = index.data().toString();

        if(index.column() == 0)
        {
            baseFlag = 1;
            m[itr1][0] = index.data().toFloat();
            itr1++;
        }
        if(index.column() == 1)
        {
            for(int i = 0, l = number.length(); i < l; i++)
            {
                if(!(number[i].isDigit() || number[i] == '.'))
                {
                    QMessageBox enteredStirng;
                    enteredStirng.setText("Please enter a number");
                    enteredStirng.exec();
                    restart();
                    break;
                }
            }

        }
    }
}

void MainWindow::getAlgorithm(const int algorithmNO)
{
    algorithmNumber = algorithmNO;

    /*
    if(algorithmNO == 1)
    {

    }
    else if(algorithmNO == 2)
    {

    }*/

    /*
    if(algorithmNO == 4 || algorithmNO == 5)
    {
        model->setColumnCount(3);
        model->setHorizontalHeaderItem(2, new QStandardItem(tr("Priority")));
    }
    else
    {
        model->setColumnCount(2);
    }

    if(algorithmNO == 2 || algorithmNO == 3 || algorithmNO == 5)
    {
        QInputDialog *Quantum = new QInputDialog;

        Quantum->setLabelText("Please enter the time Quantum:");
        Quantum->setInputMode(QInputDialog::IntInput);
        layout->addWidget(Quantum, 1, 0);
        QObject::connect(Quantum, SIGNAL(intValueSelected(int)), this, SLOT(getQuantum(int)));
    }*/
}

void MainWindow::restart()
{
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::appendBurst()
{
    drawGantt->hide();

    chart->addSeries(gantt);

    for(int i = 0; i < chart->legend()->markers().count(); i++)
    {
        for(int j = i; j < chart->legend()->markers().count(); j++)
        {
            if((chart->legend()->markers())[i]->label() == (chart->legend()->markers())[j]->label() && i != j)
            {
                (chart->legend()->markers())[j]->setVisible(0);
            }
        }
    }

    QLabel *m3 = new QLabel("Average turn around time:");
    QLabel *m4 = new QLabel("Average waiting time:");

    layout->addWidget(showGantt, 8, 0, 12, 2);
    layout->addWidget(m3, 21, 0, 1, 1);
    layout->addWidget(avgTurnaround, 21, 1);
    layout->addWidget(m4, 22, 0, 1, 1);
    layout->addWidget(avg, 22, 1);
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
*/
﻿#include "mainwindow.h"
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
#include <QTableWidget>

QT_CHARTS_USE_NAMESPACE

void swap(float *xp, float *yp);

using namespace std;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createMainMenu();
}


void MainWindow::createMainMenu()
{
    setWindowTitle("Memory Allocation");

    QInputDialog *holesNo = new QInputDialog;
    QComboBox *alg = new QComboBox;
    QLabel *m4 = new QLabel("Please choose the algorithm:");
    QLabel *m5 = new QLabel("Please enter the process number to show its segmentation table:");
    QLabel *m6 = new QLabel("Please enter the process number to deallocate:");
    QLabel *m7 = new QLabel("Please enter the reserved segment number to deallocate:");
    QPushButton *reset = new QPushButton("Reset");

    memorySize->setLabelText("");
    memorySize->setOption(QInputDialog::NoButtons);
    memorySize->setInputMode(QInputDialog::DoubleInput);
    memorySize->setDoubleMaximum(1000000000000);

    holesNo->setLabelText("");
    holesNo->setInputMode(QInputDialog::IntInput);
    holesNo->setCancelButtonText("");
    holesNo->setOkButtonText("");

    segmentsNo->setLabelText("");
    segmentsNo->setInputMode(QInputDialog::IntInput);
    segmentsNo->setCancelButtonText("");
    segmentsNo->setOkButtonText("");

    showSeg->setLabelText("");
    showSeg->setInputMode(QInputDialog::IntInput);
    showSeg->setCancelButtonText("");
    showSeg->setOkButtonText("");

    deProcess->setLabelText("");
    deProcess->setInputMode(QInputDialog::IntInput);
    deProcess->setCancelButtonText("");
    deProcess->setOkButtonText("");

    deReserveSeg->setLabelText("");
    deReserveSeg->setInputMode(QInputDialog::IntInput);
    deReserveSeg->setCancelButtonText("");
    deReserveSeg->setOkButtonText("");

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
    layout->addWidget(m3, 2, 0);
    layout->addWidget(segmentsNo, 2, 1);
    layout->addWidget(m4, 3, 0);
    layout->addWidget(alg, 3, 1);
    layout->addWidget(m5, 4, 0);
    layout->addWidget(showSeg, 4, 1);
    layout->addWidget(m6, 5, 0);
    layout->addWidget(deProcess, 5, 1);
    layout->addWidget(m7, 6, 0);
    layout->addWidget(deReserveSeg, 6, 1);
    layout->addWidget(reset, 7, 0);
    layout->addWidget(exit, 7, 1);

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
    axisY->setTickCount(13);
    axisY->setLabelFormat("%.1f");
    axisY->setTitleText("Size");

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    showMemory->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(showMemory, 0, 3, 8, 1);

    showTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(showTable, 0, 0, 2, 1);
    showTable->hide();

    QObject::connect(memorySize, SIGNAL(doubleValueChanged(double)), this, SLOT(getSize(double)));
    QObject::connect(holesNo, SIGNAL(intValueSelected(int)), this, SLOT(getNumberOfHoles(int)));
    QObject::connect(segmentsNo, SIGNAL(intValueSelected(int)), this, SLOT(getProcess(int)));
    QObject::connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(getHole(QModelIndex)));
    QObject::connect(process, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(getSegment(QModelIndex)));
    QObject::connect(alg, SIGNAL(activated(int)), this, SLOT(getAlgorithm(int)));
    QObject::connect(showSeg, SIGNAL(intValueSelected(int)), this, SLOT(showSegTable(int)));
    QObject::connect(deProcess, SIGNAL(intValueSelected(int)), this, SLOT(getDeProcess(int)));
    QObject::connect(deReserveSeg, SIGNAL(intValueSelected(int)), this, SLOT(getDeReserved(int)));
    QObject::connect(reset, SIGNAL(clicked()), this, SLOT(restart()));

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(layout);
    setCentralWidget(mainWidget);
}

void MainWindow::getSize(const double s)
{
    size = s;
}

void MainWindow::getNumberOfHoles(const int holes)
{
    m1->hide();
    memorySize->hide();
    m2->hide();
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
    showTable->show();
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
            m[itr1][0] = index.data().toFloat();
            holes++;
            itr1++;
        }
        if(index.column() == 1)
        {
            m[itr2][1] = index.data().toFloat();
            m[itr2][2] = -1;
            m[itr2][3] = index.row() + 1;
            holesSize += m[itr2][1];
            itr2++;
        }

        if(itr1 == model->rowCount() && itr2 == model->rowCount())
        {
            float basePlusLimit = 0;
            for(int i = 0; i < itr1; i++)
            {
                if(basePlusLimit == m[i][0] && basePlusLimit != 0)
                {
                    m[i - 1][1] = m[i - 1][1] + m[i][1];

                    for(int j = i; j < itr1; j++)
                    {
                        m[j][0] = m[j + 1][0];
                        m[j][1] = m[j + 1][1];
                        m[j][2] = m[j + 1][2];
                        m[j][3] = m[j + 1][3];
                    }
                    itr1--;
                    itr2--;
                    holes--;
                }
                else
                {
                    basePlusLimit = m[i][0] + m[i][1];
                }
            }

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

            for(int i = 0; i <= itr1; i++)
            {
                s[i] = new QBarSet(QString("Hole %1").arg(m[i][3]));
                s[i]->append(m[i][1]);
                if(m[i][2] == -2)
                {
                    s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                    s[i]->setColor(Qt::black);
                }
                memory->append(s[i]);
            }
            chart->axisY()->setRange(0, size);
        }
    }
}

void MainWindow::getProcess(const int rows)
{
    m2->hide();
    m3->hide();
    model->clear();
    process->clear();
    process->setRowCount(rows);

    processCounter++;

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
    showTable->show();
}

void MainWindow::getSegment(const QModelIndex &index)
{
    if(index.isValid())
    {
        QString number = index.data().toString();

        if(index.column() == 0)
        {
            segs[itr3][0] = index.data().toString();
            itr3++;
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
            segs[itr4][1] = index.data().toString();
            segs[itr4][2] = QString::number(processCounter);
            segs[itr4][3] = QString::number(index.row() + 1);
            itr4++;
        }
    }
    /*
    // Double Check
    if(itr3 - allSegsItr == process->rowCount() && itr4 - allSegsItr == process->rowCount())
    {
        float segsSize = 0;
        for(int i = allSegsItr; i < itr3; i++)
        {
            segsSize += segs[i][1].toFloat();
        }

        if(segsSize > holesSize)
        {
            QMessageBox enteredStirng;
            enteredStirng.setText("There is no enough space, please deallocate a process or a reserved segment.");
            enteredStirng.exec();
        }
    }*/
}

void MainWindow::getAlgorithm(const int algorithmNO)
{
    algorithmNumber = algorithmNO;

    int segsItr = 0, segsStart = allSegsItr;

    if(algorithmNO == 0)
    {
        while(allSegsItr < itr3)
        {
            bool fit = 0;
            for(int i = 0; i <= itr1; i++)
            {
                if(m[i][2] == -1)
                {
                    if(m[i][1] > segs[allSegsItr][1].toFloat())
                    {
                        for(int j = itr1 + 1; j > i + 1; j--)
                        {
                            m[j][0] = m[j - 1][0];
                            m[j][1] = m[j - 1][1];
                            m[j][2] = m[j - 1][2];
                            m[j][3] = m[j - 1][3];
                        }
                        m[i + 1][0] = m[i][0] + segs[allSegsItr][1].toFloat();
                        m[i + 1][1] = m[i][1] - segs[allSegsItr][1].toFloat();
                        m[i + 1][2] = -1;
                        m[i + 1][3] = m[i][3];

                        m[i][1] = segs[allSegsItr][1].toFloat();
                        m[i][2] = processCounter;
                        m[i][3] = segsItr + 1;
                        segs[allSegsItr][4] = QString::number(m[i][0]);

                        holesSize -= segs[allSegsItr][1].toFloat();
                        fit = 1;
                        itr1++;
                        break;
                    }
                    else if(m[i][1] == segs[allSegsItr][1].toFloat())
                    {
                        m[i][2] = processCounter;
                        m[i][3] = segsItr + 1;
                        segs[allSegsItr][4] = QString::number(m[i][0]);

                        for(int j = 0; j <= itr1 + 1; j++)
                        {
                            if(m[j][2] == -1 && m[j][3] > 1)
                            {
                                m[j][3]--;
                            }
                        }

                        holesSize -= segs[allSegsItr][1].toFloat();
                        fit = 1;
                        holes--;
                        break;
                    }
                    else if(m[i][1] < segs[allSegsItr][1].toFloat())
                    {
                        continue;
                    }
                }
            }

            if(fit == 0)
            {
                QMessageBox enteredStirng;
                enteredStirng.setText("There is no enough space, please deallocate a process or a reserved segment or change the algorithm");
                enteredStirng.exec();

                int holeIndex = 1, segsItr = 0, found = 0, justAnotherIterator = 0;
                QString justAnotherTemp[100][5], temp[100][2];

                for(int i = 0; i <= itr1; i++)
                {
                    if(m[i][2] == processCounter)
                    {
                        m[i][2] = -1;
                        holes++;
                        holesSize += m[i][1];

                        if(m[i + 1][2] == -1 && i < itr1 && m[i - 1][2] == -1 && i > 0)
                        {
                            m[i - 1][1] = m[i - 1][1] + m[i][1] + m[i + 1][1];

                            for(int j = i; j < itr1; j++)
                            {
                                m[j][0] = m[j + 2][0];
                                m[j][1] = m[j + 2][1];
                                m[j][2] = m[j + 2][2];
                                m[j][3] = m[j + 2][3];
                            }
                            itr1 -= 2;
                            holes -= 2;
                            i--;
                        }
                        else if(m[i - 1][2] == -1 && i > 0)
                        {
                            m[i - 1][1] = m[i - 1][1] + m[i][1];

                            for(int j = i; j < itr1; j++)
                            {
                                m[j][0] = m[j + 1][0];
                                m[j][1] = m[j + 1][1];
                                m[j][2] = m[j + 1][2];
                                m[j][3] = m[j + 1][3];
                            }
                            itr1--;
                            holes--;
                            i--;
                        }
                        else if(m[i + 1][2] == -1 && i < itr1)
                        {
                            m[i][1] = m[i][1] + m[i + 1][1];

                            for(int j = i + 1; j < itr1 + 1; j++)
                            {
                                m[j][0] = m[j + 1][0];
                                m[j][1] = m[j + 1][1];
                                m[j][2] = m[j + 1][2];
                                m[j][3] = m[j + 1][3];
                            }
                            itr1--;
                            holes--;
                        }
                        allSegsItr--;
                        found = 1;
                    }
                }

                if(found == 1)
                {
                    for(int i = 0; i <= itr1; i++)
                    {
                        if(m[i][2] == -1)
                        {
                            m[i][3] = holeIndex;
                            holeIndex++;
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        if(segs[i][2].toInt() == processCounter)
                        {
                            segs[i][0] = "";
                            segs[i][1] = "";
                            segs[i][2] = "";
                            segs[i][3] = "";
                            segs[i][4] = "";
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        if(segs[i][0] != "")
                        {
                           justAnotherTemp[justAnotherIterator][0] = segs[i][0];
                           justAnotherTemp[justAnotherIterator][1] = segs[i][1];
                           justAnotherTemp[justAnotherIterator][2] = segs[i][2];
                           justAnotherTemp[justAnotherIterator][3] = segs[i][3];
                           justAnotherTemp[justAnotherIterator][4] = segs[i][4];
                           justAnotherIterator++;
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        segs[i][0] = justAnotherTemp[i][0];
                        segs[i][1] = justAnotherTemp[i][1];
                        segs[i][2] = justAnotherTemp[i][2];
                        segs[i][3] = justAnotherTemp[i][3];
                        segs[i][4] = justAnotherTemp[i][4];
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        if(segs[i][0] != "")
                        {
                            temp[i][0] = segs[i][0];
                            temp[i][1] = segs[i][4];
                        }
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        for(int j = 0; j < allSegsItr - 1; j++)
                        {
                            float x = segs[j][4].toFloat(), y = segs[j + 1][4].toFloat();

                            if(x > y && segs[j][0] != "" && segs[j + 1][0] != "")
                            {
                                QString temp = segs[j][0];
                                segs[j][0] = segs[j + 1][0];
                                segs[j + 1][0] = temp;
                                temp = segs[j][4];
                                segs[j][4] = segs[j + 1][4];
                                segs[j + 1][4] = temp;
                            }
                        }
                    }

                    memory->clear();

                    for(int i = 0; i <= itr1; i++)
                    {
                        s[i] = new QBarSet("");

                        if(!(m[i][0] == size))
                        {
                            if(m[i][2] == -1)
                            {
                                s[i]->setLabel(QString("Hole %1").arg(m[i][3]));
                            }
                            else if(m[i][2] == -2)
                            {
                                s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                                s[i]->setColor(Qt::black);
                            }
                            else
                            {
                                s[i]->setLabel(QString("Process %1-%2").arg(m[i][2]).arg(segs[segsItr][0]));
                                segsItr++;
                            }
                        }
                        s[i]->append(m[i][1]);
                        memory->append(s[i]);
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        segs[i][0] = temp[i][0];
                        segs[i][4] = temp[i][1];
                    }
                    itr3 = allSegsItr;
                    itr4 = allSegsItr;
                    chart->axisY()->setRange(0, size);
                }
                processCounter--;
                break;
            }
            else
            {
                allSegsItr++;
                segsItr++;
            }
        }
    }

    else if(algorithmNO == 1)
    {
        while(allSegsItr < itr3)
        {
            int tempItr = 0;
            bool fit = 0;
            float tempHoles[holes][4];

            for(int i = 0; tempItr < holes; i++)
            {
                if(m[i][2] == -1)
                {
                    tempHoles[tempItr][0] = m[i][0];
                    tempHoles[tempItr][1] = m[i][1];
                    tempHoles[tempItr][2] = m[i][3];
                    tempHoles[tempItr][3] = i;
                    tempItr++;
                }
            }

            for(int i = 0; i < holes; i++)
            {
                for(int j = 0; j < holes - 1; j++)
                {
                    if(tempHoles[j][1] > tempHoles[j + 1][1])
                    {
                        swap(&tempHoles[j][0], & tempHoles[j + 1][0]);
                        swap(&tempHoles[j][1], & tempHoles[j + 1][1]);
                        swap(&tempHoles[j][2], & tempHoles[j + 1][2]);
                        swap(&tempHoles[j][3], & tempHoles[j + 1][3]);
                    }
                }
            }

            tempItr = 1;
            int i = tempHoles[0][3];

            for(int k = 0; k < holes; k++)
            {
                i = tempHoles[k][3];

                if(m[i][2] == -1)
                {
                    if(m[i][1] > segs[allSegsItr][1].toFloat())
                    {
                        for(int j = itr1 + 1; j > i + 1; j--)
                        {
                            m[j][0] = m[j - 1][0];
                            m[j][1] = m[j - 1][1];
                            m[j][2] = m[j - 1][2];
                            m[j][3] = m[j - 1][3];
                        }

                        m[i + 1][0] = m[i][0] + segs[allSegsItr][1].toFloat();
                        m[i + 1][1] = m[i][1] - segs[allSegsItr][1].toFloat();
                        m[i + 1][2] = -1;
                        m[i + 1][3] = m[i][3];

                        m[i][1] = segs[allSegsItr][1].toFloat();
                        m[i][2] = processCounter;
                        m[i][3] = segsItr + 1;
                        segs[allSegsItr][4] = QString::number(m[i][0]);

                        holesSize -= segs[allSegsItr][1].toFloat();
                        fit = 1;
                        itr1++;
                        break;
                    }
                    else if(m[i][1] == segs[allSegsItr][1].toFloat())
                    {
                        m[i][2] = processCounter;
                        m[i][3] = segsItr + 1;
                        segs[allSegsItr][4] = QString::number(m[i][0]);

                        for(int j = 0; j <= itr1 + 1; j++)
                        {
                            if(m[j][2] == -1 && m[j][3] > 1)
                            {
                                m[j][3]--;
                            }
                        }

                        holesSize -= segs[allSegsItr][1].toFloat();
                        fit = 1;
                        holes--;
                        break;
                    }
                    else if(m[i][1] < segs[allSegsItr][1].toFloat())
                    {
                        continue;
                    }
                }
            }


            if(fit == 0)
            {
                QMessageBox enteredStirng;
                enteredStirng.setText("There is no enough space, please deallocate a process or a reserved segment or change the algorithm");
                enteredStirng.exec();

                int holeIndex = 1, segsItr = 0, found = 0, justAnotherIterator = 0;
                QString justAnotherTemp[100][5], temp[100][2];

                for(int i = 0; i <= itr1; i++)
                {
                    if(m[i][2] == processCounter)
                    {
                        m[i][2] = -1;
                        holes++;
                        holesSize += m[i][1];

                        if(m[i + 1][2] == -1 && i < itr1 && m[i - 1][2] == -1 && i > 0)
                        {
                            m[i - 1][1] = m[i - 1][1] + m[i][1] + m[i + 1][1];

                            for(int j = i; j < itr1; j++)
                            {
                                m[j][0] = m[j + 2][0];
                                m[j][1] = m[j + 2][1];
                                m[j][2] = m[j + 2][2];
                                m[j][3] = m[j + 2][3];
                            }
                            itr1 -= 2;
                            holes -= 2;
                            i--;
                        }
                        else if(m[i - 1][2] == -1 && i > 0)
                        {
                            m[i - 1][1] = m[i - 1][1] + m[i][1];

                            for(int j = i; j < itr1; j++)
                            {
                                m[j][0] = m[j + 1][0];
                                m[j][1] = m[j + 1][1];
                                m[j][2] = m[j + 1][2];
                                m[j][3] = m[j + 1][3];
                            }
                            itr1--;
                            holes--;
                            i--;
                        }
                        else if(m[i + 1][2] == -1 && i < itr1)
                        {
                            m[i][1] = m[i][1] + m[i + 1][1];

                            for(int j = i + 1; j < itr1 + 1; j++)
                            {
                                m[j][0] = m[j + 1][0];
                                m[j][1] = m[j + 1][1];
                                m[j][2] = m[j + 1][2];
                                m[j][3] = m[j + 1][3];
                            }
                            itr1--;
                            holes--;
                        }
                        allSegsItr--;
                        found = 1;
                    }
                }

                if(found == 1)
                {
                    for(int i = 0; i <= itr1; i++)
                    {
                        if(m[i][2] == -1)
                        {
                            m[i][3] = holeIndex;
                            holeIndex++;
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        if(segs[i][2].toInt() == processCounter)
                        {
                            segs[i][0] = "";
                            segs[i][1] = "";
                            segs[i][2] = "";
                            segs[i][3] = "";
                            segs[i][4] = "";
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        if(segs[i][0] != "")
                        {
                           justAnotherTemp[justAnotherIterator][0] = segs[i][0];
                           justAnotherTemp[justAnotherIterator][1] = segs[i][1];
                           justAnotherTemp[justAnotherIterator][2] = segs[i][2];
                           justAnotherTemp[justAnotherIterator][3] = segs[i][3];
                           justAnotherTemp[justAnotherIterator][4] = segs[i][4];
                           justAnotherIterator++;
                        }
                    }

                    for(int i = 0; i <= itr3; i++)
                    {
                        segs[i][0] = justAnotherTemp[i][0];
                        segs[i][1] = justAnotherTemp[i][1];
                        segs[i][2] = justAnotherTemp[i][2];
                        segs[i][3] = justAnotherTemp[i][3];
                        segs[i][4] = justAnotherTemp[i][4];
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        if(segs[i][0] != "")
                        {
                            temp[i][0] = segs[i][0];
                            temp[i][1] = segs[i][4];
                        }
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        for(int j = 0; j < allSegsItr - 1; j++)
                        {
                            float x = segs[j][4].toFloat(), y = segs[j + 1][4].toFloat();

                            if(x > y && segs[j][0] != "" && segs[j + 1][0] != "")
                            {
                                QString temp = segs[j][0];
                                segs[j][0] = segs[j + 1][0];
                                segs[j + 1][0] = temp;
                                temp = segs[j][4];
                                segs[j][4] = segs[j + 1][4];
                                segs[j + 1][4] = temp;
                            }
                        }
                    }

                    memory->clear();

                    for(int i = 0; i <= itr1; i++)
                    {
                        s[i] = new QBarSet("");

                        if(!(m[i][0] == size))
                        {
                            if(m[i][2] == -1)
                            {
                                s[i]->setLabel(QString("Hole %1").arg(m[i][3]));
                            }
                            else if(m[i][2] == -2)
                            {
                                s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                                s[i]->setColor(Qt::black);
                            }
                            else
                            {
                                s[i]->setLabel(QString("Process %1-%2").arg(m[i][2]).arg(segs[segsItr][0]));
                                segsItr++;
                            }
                        }
                        s[i]->append(m[i][1]);
                        memory->append(s[i]);
                    }

                    for(int i = 0; i < allSegsItr; i++)
                    {
                        segs[i][0] = temp[i][0];
                        segs[i][4] = temp[i][1];
                    }
                    itr3 = allSegsItr;
                    itr4 = allSegsItr;
                    chart->axisY()->setRange(0, size);
                }
                processCounter--;
                break;
            }
            else
            {
                allSegsItr++;
                segsItr++;
            }
        }
    }

    for(int i = segsStart; i < allSegsItr; i++)
    {
        tempNames[i][0] = segs[i][0];
        tempNames[i][1] = segs[i][4];
    }

    for(int i = 0; i < allSegsItr; i++)
    {
        for(int j = 0; j < allSegsItr - 1; j++)
        {
            float x = segs[j][4].toFloat(), y = segs[j + 1][4].toFloat();

            if(x > y)
            {
                QString temp = segs[j][0];
                segs[j][0] = segs[j + 1][0];
                segs[j + 1][0] = temp;
                temp = segs[j][4];
                segs[j][4] = segs[j + 1][4];
                segs[j + 1][4] = temp;
            }
        }
    }

    memory->clear();
    segsItr = 0;

    for(int i = 0; i <= itr1; i++)
    {
        s[i] = new QBarSet("");

        if(!(m[i][0] == size))
        {
            if(m[i][2] == -1)
            {
                s[i]->setLabel(QString("Hole %1").arg(m[i][3]));
            }
            else if(m[i][2] == -2)
            {
                s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                s[i]->setColor(Qt::black);
            }
            else
            {
                s[i]->setLabel(QString("Process %1-%2").arg(m[i][2]).arg(segs[segsItr][0]));
                segsItr++;
            }
        }
        s[i]->append(m[i][1]);
        memory->append(s[i]);
    }

    for(int i = 0; i < allSegsItr; i++)
    {
        segs[i][0] = tempNames[i][0];
        segs[i][4] = tempNames[i][1];
    }
    chart->axisY()->setRange(0, size);
    showTable->hide();
    m3->show();
    segmentsNo->show();
}

void MainWindow::getDeReserved(const int n)
{
    int holeIndex = 1, preIndex = 1, segsItr = 0, found = 0;
    QString temp[100][2];

    for(int i = 0; i <= itr1; i++)
    {
        if(m[i][2] == -2 && m[i][3] == n)
        {
            m[i][2] = -1;
            holes++;
            holesSize += m[i][1];

            if(m[i + 1][2] == -1 && i < itr1 && m[i - 1][2] == -1 && i > 0)
            {
                m[i - 1][1] = m[i - 1][1] + m[i][1] + m[i + 1][1];

                for(int j = i; j < itr1; j++) // j = i
                {
                    m[j][0] = m[j + 2][0];
                    m[j][1] = m[j + 2][1];
                    m[j][2] = m[j + 2][2];
                    m[j][3] = m[j + 2][3];
                }
                itr1 -= 2;
                holes -= 2;
            }
            else if(m[i - 1][2] == -1 && i > 0)
            {
                m[i - 1][1] = m[i - 1][1] + m[i][1];

                for(int j = i; j < itr1; j++)
                {
                    m[j][0] = m[j + 1][0];
                    m[j][1] = m[j + 1][1];
                    m[j][2] = m[j + 1][2];
                    m[j][3] = m[j + 1][3];
                }
                itr1--;
                holes--;
            }
            else if(m[i + 1][2] == -1 && i < itr1)
            {
                m[i][1] = m[i][1] + m[i + 1][1];

                for(int j = i + 1; j < itr1; j++)
                {
                    m[j][0] = m[j + 1][0];
                    m[j][1] = m[j + 1][1];
                    m[j][2] = m[j + 1][2];
                    m[j][3] = m[j + 1][3];
                }
                itr1--;
                holes--;
            }

            found = 1;
            break;
        }
    }

    if(found == 0)
    {
        QMessageBox enteredStirng;
        enteredStirng.setText("There is no reserved area with this number, please enter a correct number");
        enteredStirng.exec();
    }
    else
    {
        for(int i = 0; i <= itr1; i++)
        {
            if(m[i][2] == -1)
            {
                m[i][3] = holeIndex;
                holeIndex++;
            }
            if(m[i][2] == -2)
            {
                m[i][3] = preIndex;
                preIndex++;
            }
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            temp[i][0] = segs[i][0];
            temp[i][1] = segs[i][4];
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            for(int j = 0; j < allSegsItr - 1; j++)
            {
                float x = segs[j][4].toFloat(), y = segs[j + 1][4].toFloat();

                if(x > y && segs[j][2].toFloat() == segs[j + 1][2].toFloat())
                {
                    QString temp = segs[j][0];
                    segs[j][0] = segs[j + 1][0];
                    segs[j + 1][0] = temp;
                    temp = segs[j][4];
                    segs[j][4] = segs[j + 1][4];
                    segs[j + 1][4] = temp;
                }
            }
        }

        memory->clear();
        for(int i = 0; i <= itr1; i++)
        {
            s[i] = new QBarSet("");

            if(!(m[i][0] == size))
            {
                if(m[i][2] == -1)
                {
                    s[i]->setLabel(QString("Hole %1").arg(m[i][3]));
                }
                else if(m[i][2] == -2)
                {
                    s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                    s[i]->setColor(Qt::black);
                }
                else
                {
                    s[i]->setLabel(QString("Process %1-%2").arg(m[i][2]).arg(segs[segsItr][0]));
                    segsItr++;
                }
            }
            s[i]->append(m[i][1]);
            memory->append(s[i]);
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            segs[i][0] = temp[i][0];
            segs[i][4] = temp[i][1];
        }
        chart->axisY()->setRange(0, size);
    }
    deReserveSeg->show();
}

void MainWindow::getDeProcess(const int n)
{
    int holeIndex = 1, segsItr = 0, found = 0, justAnotherIterator = 0;
    QString justAnotherTemp[100][5], temp[100][2];

    for(int i = 0; i <= itr1; i++)
    {
        if(m[i][2] == n)
        {
            m[i][2] = -1;
            holes++;
            holesSize += m[i][1];

            if(m[i + 1][2] == -1 && i < itr1 && m[i - 1][2] == -1 && i > 0)
            {
                m[i - 1][1] = m[i - 1][1] + m[i][1] + m[i + 1][1];

                for(int j = i; j < itr1; j++)
                {
                    m[j][0] = m[j + 2][0];
                    m[j][1] = m[j + 2][1];
                    m[j][2] = m[j + 2][2];
                    m[j][3] = m[j + 2][3];
                }
                itr1 -= 2;
                holes -= 2;
                i--;
            }
            else if(m[i - 1][2] == -1 && i > 0)
            {
                m[i - 1][1] = m[i - 1][1] + m[i][1];

                for(int j = i; j < itr1; j++)
                {
                    m[j][0] = m[j + 1][0];
                    m[j][1] = m[j + 1][1];
                    m[j][2] = m[j + 1][2];
                    m[j][3] = m[j + 1][3];
                }
                itr1--;
                holes--;
                i--;
            }
            else if(m[i + 1][2] == -1 && i < itr1)
            {
                m[i][1] = m[i][1] + m[i + 1][1];

                for(int j = i + 1; j < itr1 + 1; j++)
                {
                    m[j][0] = m[j + 1][0];
                    m[j][1] = m[j + 1][1];
                    m[j][2] = m[j + 1][2];
                    m[j][3] = m[j + 1][3];
                }
                itr1--;
                holes--;
            }
            allSegsItr--;
            found = 1;
        }
    }

    if(found == 0)
    {
        QMessageBox enteredStirng;
        enteredStirng.setText("There is no process with this number, please enter a correct process number");
        enteredStirng.exec();
    }
    else
    {
        for(int i = 0; i <= itr1; i++)
        {
            if(m[i][2] == -1)
            {
                m[i][3] = holeIndex;
                holeIndex++;
            }
        }

        for(int i = 0; i <= itr3; i++)
        {
            if(segs[i][2].toInt() == n)
            {
                segs[i][0] = "";
                segs[i][1] = "";
                segs[i][2] = "";
                segs[i][3] = "";
                segs[i][4] = "";
            }
        }

        for(int i = 0; i <= itr3; i++)
        {
            if(segs[i][0] != "")
            {
               justAnotherTemp[justAnotherIterator][0] = segs[i][0];
               justAnotherTemp[justAnotherIterator][1] = segs[i][1];
               justAnotherTemp[justAnotherIterator][2] = segs[i][2];
               justAnotherTemp[justAnotherIterator][3] = segs[i][3];
               justAnotherTemp[justAnotherIterator][4] = segs[i][4];
               justAnotherIterator++;
            }
        }

        for(int i = 0; i <= itr3; i++)
        {
            segs[i][0] = justAnotherTemp[i][0];
            segs[i][1] = justAnotherTemp[i][1];
            segs[i][2] = justAnotherTemp[i][2];
            segs[i][3] = justAnotherTemp[i][3];
            segs[i][4] = justAnotherTemp[i][4];
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            if(segs[i][0] != "")
            {
                temp[i][0] = segs[i][0];
                temp[i][1] = segs[i][4];
            }
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            for(int j = 0; j < allSegsItr - 1; j++)
            {
                float x = segs[j][4].toFloat(), y = segs[j + 1][4].toFloat();

                if(x > y && segs[j][0] != "" && segs[j + 1][0] != "")
                {
                    QString temp = segs[j][0];
                    segs[j][0] = segs[j + 1][0];
                    segs[j + 1][0] = temp;
                    temp = segs[j][4];
                    segs[j][4] = segs[j + 1][4];
                    segs[j + 1][4] = temp;
                }
            }
        }

        memory->clear();

        for(int i = 0; i <= itr1; i++)
        {
            s[i] = new QBarSet("");

            if(!(m[i][0] == size))
            {
                if(m[i][2] == -1)
                {
                    s[i]->setLabel(QString("Hole %1").arg(m[i][3]));
                }
                else if(m[i][2] == -2)
                {
                    s[i]->setLabel(QString("Reserved %1").arg(m[i][3]));
                    s[i]->setColor(Qt::black);
                }
                else
                {
                    s[i]->setLabel(QString("Process %1-%2").arg(m[i][2]).arg(segs[segsItr][0]));
                    segsItr++;
                }
            }
            s[i]->append(m[i][1]);
            memory->append(s[i]);
        }

        for(int i = 0; i < allSegsItr; i++)
        {
            segs[i][0] = temp[i][0];
            segs[i][4] = temp[i][1];
        }
        itr3 = allSegsItr;
        itr4 = allSegsItr;

        chart->axisY()->setRange(0, size);
    }
    deProcess->show();
}

void MainWindow::showSegTable(const int n)
{
    int noOfSegments = 0, processStart = 0, found = 0;

    for (int i = 0; i <= itr3; i++)
    {
        if(segs[i][2].toInt() == n)
        {
            noOfSegments++;
            if(segs[i][3].toInt() == 1 && processStart == 0)
            {
                processStart = i;
            }
            found = 1;
        }
    }

    if(found == 1)
    {
        QTableWidget *segTable = new QTableWidget(noOfSegments, 4);

        for (int row = 0; row < segTable->rowCount(); ++row)
        {
            QTableWidgetItem *segNo = new QTableWidgetItem(tr("%1").arg(segs[processStart][3]));
            QTableWidgetItem *name = new QTableWidgetItem(tr("%1").arg(segs[processStart][0]));
            QTableWidgetItem *limit = new QTableWidgetItem(tr("%1").arg(segs[processStart][1]));
            QTableWidgetItem *base = new QTableWidgetItem(tr("%1").arg(segs[processStart][4]));

            segNo->setFlags(segNo->flags() ^ Qt::ItemIsEditable);
            name->setFlags(name->flags() ^ Qt::ItemIsEditable);
            limit->setFlags(limit->flags() ^ Qt::ItemIsEditable);
            base->setFlags(base->flags() ^ Qt::ItemIsEditable);

            segTable->setItem(row, 0, segNo);
            segTable->setItem(row, 1, name);
            segTable->setItem(row, 2, limit);
            segTable->setItem(row, 3, base);
            processStart++;
        }

        segTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Segment No")));
        segTable->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Name")));
        segTable->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Limit")));
        segTable->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Base")));

        segTable->setWindowTitle("Segmentation Table");
        segTable->resize(400, 250);
        segTable->verticalHeader()->setVisible(false);
        segTable->show();
    }
    else
    {
        QMessageBox enteredStirng;
        enteredStirng.setText("There is no process with this number, please enter a correct process number");
        enteredStirng.exec();
    }
    showSeg->show();
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

void swap(float *xp, float *yp)
{
    float temp = *xp;
    *xp = *yp;
    *yp = temp;
}

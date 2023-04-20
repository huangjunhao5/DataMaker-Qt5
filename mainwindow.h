//
// Created by 18141 on 2023/4/20.
//

#ifndef DATAMAKER_MAINWINDOW_H
#define DATAMAKER_MAINWINDOW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;
private slots:
    void MakeData();
private:
    Ui::MainWindow *ui;
};


#endif //DATAMAKER_MAINWINDOW_H

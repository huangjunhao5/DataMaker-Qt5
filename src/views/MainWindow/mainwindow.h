//
// Created by 18141 on 2023/4/25.
//

#ifndef DATAMAKER_MAINWINDOW_H
#define DATAMAKER_MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;
private slots:
    void MakeData();
    void copyFile();
    void openHelp();
private:
    Ui::MainWindow *ui;
};


#endif //DATAMAKER_MAINWINDOW_H

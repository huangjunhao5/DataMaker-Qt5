//
// Created by 18141 on 2023/4/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include <QMessageBox>
#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "../../libs/DataMaker.h"
#include "../../libs/DataMakerFromText.h"


MainWindow::MainWindow(QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->make, SIGNAL(clicked()), this, SLOT(MakeData()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::MakeData() {

    try{
        DataMakerFromSourceText dataMaker;
        auto testNum = ui->lineEdit->text().toInt();
        if(testNum)dataMaker.setTestNum(testNum);
        std::string source = ui->source->toPlainText().toStdString();
        dataMaker.setSource(source);
        std::string inputMakeFun = ui->makeFunText->toPlainText().toStdString();
        dataMaker.setInputMakeSource(inputMakeFun);
        dataMaker.run();
    }catch(std::runtime_error e){
        std::cerr << "ERROR : " << e.what() << std::endl;
        QString info = QString("数据生成失败,错误原因:\n") + e.what();
        QMessageBox::critical(this,
                                 "错误",
                                 info);
        return ;
    }
    QMessageBox::information(this,
        "信息",
        "数据生成成功");
}

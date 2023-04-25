//
// Created by 18141 on 2023/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTextStream>
#include "../../libs/DataMaker.h"
#include "../../libs/DataMakerFromText.h"



MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->make, SIGNAL(clicked()), this, SLOT(MakeData()));
    connect(ui->openCppSource,&QPushButton::clicked,[&](bool){this->copyFile();});
    connect(ui->helpBtn, &QPushButton::clicked, [&](bool){this->openHelp();});
//    ui->statusbar->showMessage("就绪");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::MakeData() {
//    ui->statusbar->showMessage("正在生成数据");
    try {
        DataMakerFromSourceText dataMaker;
        auto testNum = ui->lineEdit->text().toInt();
        if (testNum)dataMaker.setTestNum(testNum);
        std::string source = ui->source->toPlainText().toStdString();
        dataMaker.setSource(source);
        std::string inputMakeFun = ui->makeFunText->toPlainText().toStdString();
        dataMaker.setInputMakeSource(inputMakeFun);
        dataMaker.run();
        QMessageBox::information(this,
                                 "信息",
                                 "数据生成成功, 生成位置为\n" + QString::fromStdString( dataMaker.getTestcasePath()));
//        ui->statusbar->showMessage("数据生成成功！");
    } catch (std::runtime_error e) {
        std::cerr << "ERROR : " << e.what() << std::endl;
        QString info = QString("数据生成失败,错误原因:\n") + e.what();
        QMessageBox::critical(this,
                              "错误",
                              info);
//        ui->statusbar->showMessage("生成失败！");
        return;
    }

}

void MainWindow::copyFile() {
    // 打开文件选择对话框，选择要打开的文件
    QString filePath = QFileDialog::getOpenFileName(nullptr, "选择文件", QDir::homePath(), "C++ Source Files (*.cpp)");

    if (filePath.isEmpty()) {
        QMessageBox::information(nullptr, "提示", "未选择文件");
        return;
    }

    // 打开文件
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString fileContent = in.readAll();
        file.close();

        // 将文件内容显示在文本框中
//        QTextEdit *textEdit = new QTextEdit;
        ui->source->setPlainText(fileContent);
//        textEdit->show();

        // 打开文件所在目录
        QString fileDirectory = QFileInfo(filePath).absolutePath();
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fileDirectory));
    } else {
        QMessageBox::warning(nullptr, "提示", "打开文件失败");
    }
}

void MainWindow::openHelp() {
    QDesktopServices::openUrl(QUrl("https://github.com/huangjunhao5/DataMaker-Qt5/tree/master",QUrl::TolerantMode));
}
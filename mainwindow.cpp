#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

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


void MainWindow::on_pushButtonSelectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "select file");
    if (fileName.isEmpty())
       return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
       return;
    QByteArray data = file.readAll();
    //_model->deserializeXML(QString(data));
    file.close();
}

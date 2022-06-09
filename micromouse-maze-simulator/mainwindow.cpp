#include "mainwindow.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonTest, SIGNAL(clicked()), this, SLOT(pushButtonTest_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButtonTest_clicked()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    cells[0][0] = new Cell(400, 200,scene);
}

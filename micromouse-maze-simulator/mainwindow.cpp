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
    ui->graphicsView->setGeometry(20, 20, 16 * (CELL_WIDTH - POST_WIDTH) + 10 , 16 * (CELL_HEIGHT - POST_HEIGHT) + 10);

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i] = new Cell(i * (CELL_WIDTH - POST_WIDTH), j * (CELL_HEIGHT - POST_HEIGHT), (j * 16) + i, scene);
        }
    }

    for(int i = 0; i < 16; i++) cells[i][15]->wallEast->setVisible(true);
    for(int i = 0; i < 16; i++) cells[i][0]->wallWest->setVisible(true);
    for(int i = 0; i < 16; i++) cells[0][i]->wallNorth->setVisible(true);
    for(int i = 0; i < 16; i++) cells[15][i]->wallSouth->setVisible(true);

}

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

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[i][j] = new Cell(i * (CELL_WIDTH - POST_WIDTH), j * (CELL_HEIGHT - POST_HEIGHT), scene);
        }
    }

    cells[8][8]->type = CELL_FINISH;
    cells[8][8]->SET_BRUSH();
    cells[8][7]->type = CELL_FINISH;
    cells[8][7]->SET_BRUSH();
    cells[7][7]->type = CELL_FINISH;
    cells[7][7]->SET_BRUSH();
    cells[7][8]->type = CELL_FINISH;
    cells[7][8]->SET_BRUSH();

    cells[0][15]->type = CELL_START;
    cells[0][15]->SET_BRUSH();

    for(int i = 0; i < 16; i++) cells[i][15]->wallSouth->setVisible(true);
    for(int i = 0; i < 16; i++) cells[i][0]->wallNorth->setVisible(true);
    for(int i = 0; i < 16; i++) cells[0][i]->wallWest->setVisible(true);
    for(int i = 0; i < 16; i++) cells[15][i]->wallEast->setVisible(true);
    cells[0][15]->wallWest->setVisible(true);
}

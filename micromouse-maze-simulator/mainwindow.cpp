#include "mainwindow.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStack>
#include <QList>
#include <QDir>

bool goal_reached = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonFloodFill,        SIGNAL(clicked()), this, SLOT(pushButtonFloodFill_clicked()));
    connect(ui->pushButtonClearWalls,       SIGNAL(clicked()), this, SLOT(pushButtonClearWalls_clicked()));
    connect(ui->pushButtonGenerate,         SIGNAL(clicked()), this, SLOT(pushButtonGenerate_clicked()));
    connect(ui->pushButtonAStar,            SIGNAL(clicked()), this, SLOT(pushButtonAStar_clicked()));
    connect(ui->pushButtonSaveMaze,         SIGNAL(clicked()), this, SLOT(pushButtonSaveMaze_clicked()));
    connect(ui->pushButtonLoadMaze,         SIGNAL(clicked()), this, SLOT(pushButtonLoadMaze_clicked()));
    connect(ui->pushButtonSerialConnect,    SIGNAL(clicked()), this, SLOT(pushButtonSerialConnect_clicked()));
    connect(ui->pushButtonSend,             SIGNAL(clicked()), this, SLOT(pushButtonSend_clicked()));

    connect(ui->pushButtonFWD,              SIGNAL(clicked()), this, SLOT(pushButtonFWD_clicked()));
    connect(ui->pushButtonN,                SIGNAL(clicked()), this, SLOT(pushButtonN_clicked()));
    connect(ui->pushButtonE,                SIGNAL(clicked()), this, SLOT(pushButtonE_clicked()));
    connect(ui->pushButtonS,                SIGNAL(clicked()), this, SLOT(pushButtonS_clicked()));
    connect(ui->pushButtonW,                SIGNAL(clicked()), this, SLOT(pushButtonW_clicked()));

    MAP_INIT_16x16();
    cell_start_conut = 0;
    cell_finish_count = 0;

    COMBO_BOX_MAZES_UPDATE();
}

MainWindow::~MainWindow()
{
    delete ui;
    serial.close();
}

void MainWindow::pushButtonFloodFill_clicked()
{
    int start_j = 0, start_i = 0;
    bool startFound = false;
    bool finishFound = false;
    int finishCount = 0;
    int finishIndexs[4];

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->type == CELL_START)
            {
              start_j = j;
              start_i = i;
              startFound = true;
              cells[j][i]->visited = true;
              cells[j][i]->solver_index = 0;
              cells[j][i]->solverIndexText->setPlainText(QString::number(0));
            }
            if(cells[j][i]->type == CELL_FINISH)
            {
              finishIndexs[finishCount] = cells[j][i]->index;
              finishCount++;
              finishFound = true;
            }
        }
    }
    goal_reached = false;

    if((startFound && finishFound) == true)
    {
        int finishIndex;

        switch (finishCount)
        {
        case 1:
           finishIndex = SOLVE_FLOOD_FILL(start_j, start_i, finishIndexs[0]);
        break;

        case 2:
           finishIndex = SOLVE_FLOOD_FILL(start_j, start_i, finishIndexs[0], finishIndexs[1]);
        break;

        case 3:
           finishIndex = SOLVE_FLOOD_FILL(start_j, start_i, finishIndexs[0], finishIndexs[1], finishIndexs[2]);
        break;

        case 4:
           finishIndex = SOLVE_FLOOD_FILL(start_j, start_i, finishIndexs[0], finishIndexs[1], finishIndexs[2], finishIndexs[3]);
        break;
        }

        SOLVE_FLOOD_GENERATE_PATH(finishIndex);
    }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You need to define start and finish cells!");
            msgBox.exec();
        }
}

void MainWindow::pushButtonClearWalls_clicked()
{
    MAP_CLEAR();
}

void MainWindow::pushButtonGenerate_clicked()
{
    goal_reached = false;
    MAP_CLEAR();
    MAP_GENERATE_ITERATIVE(0,0);
}

void MainWindow::pushButtonAStar_clicked()
{
    unsigned int startCellIndex, finishCellIndex;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->type == CELL_START)
            {
              startCellIndex = cells[j][i]->index;
            }
            if(cells[j][i]->type == CELL_FINISH)
            {
              finishCellIndex = cells[j][i]->index;
            }
        }
    }


    A_STAR_FIND_PATH(startCellIndex, finishCellIndex);
}

void MainWindow::pushButtonSaveMaze_clicked()
{
    QString dirPath("D:/github-repos/micromouse/micromouse-other/maze-files/");
    QString fileName = ui->lineEditSaveText->text();

    if(fileName.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please type maze name!");
        msgBox.exec();
    }
    else
    {
        QString path(dirPath + fileName + ".txt");

        QFile file(path);
        if(file.exists())
        {
            QMessageBox msgBox;
            msgBox.setText("Maze already exsist.");
            msgBox.setInformativeText("Do You want to overwrite?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();

            if(ret == QMessageBox::Yes)
            {
                file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
                QTextStream out(&file);

                    for(int i=0;i<16;i++)
                    {
                        for(int j=0;j<16;j++)
                        {
                            out << j << ";" << i << ";" << cells[j][i]->walls << "\n";
                        }
                    }
                   // optional, as QFile destructor will already do it:
                file.close();

            }

        }
        else
        {
            file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
            QTextStream out(&file);

                for(int i=0;i<16;i++)
                {
                    for(int j=0;j<16;j++)
                    {
                        out << j << ";" << i << ";" << cells[j][i]->walls << "\n";
                    }
                }
               // optional, as QFile destructor will already do it:
            file.close();
        }

        COMBO_BOX_MAZES_UPDATE();
    }
}

void MainWindow::pushButtonLoadMaze_clicked()
{
    MAP_CLEAR();

    QDir dir("D:/github-repos/micromouse/micromouse-other/maze-files/");
    QString fileName = ui->comboBoxLoad->currentText();
    QFileInfo path(dir, fileName);

    QFile file(path.canonicalFilePath());
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QList list = line.split(';');

        cells[list[0].toInt()][list[1].toInt()]->walls |= list[2].toInt();
    }
    MAP_WALLS_UPDATE();
    file.close();
}

void MainWindow::pushButtonSerialConnect_clicked()
{
    serial.setPortName("com9");
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    bool status = serial.open(QIODevice::ReadWrite);

    if(!status)
    {
        QMessageBox msgBox;
        msgBox.setText("Unable to connect!");
        msgBox.exec();
    }
    else
    {
        ui->pushButtonSend->setEnabled(true);
        ui->pushButtonFWD->setEnabled(true);
        ui->pushButtonN->setEnabled(true);
        ui->pushButtonE->setEnabled(true);
        ui->pushButtonS->setEnabled(true);
        ui->pushButtonW->setEnabled(true);
        connect(&serial, &QSerialPort::readyRead, this, &MainWindow::serialReceived);
    }

}

void MainWindow::pushButtonSend_clicked()
{
    serial.write(ui->plainTextEditSend->toPlainText().toUtf8());
}

void MainWindow::pushButtonFWD_clicked()
{
    serial.write("$MOVE=FWD,1#");
}

void MainWindow::pushButtonN_clicked()
{
    serial.write("$ROTATE=0#");
}

void MainWindow::pushButtonE_clicked()
{
    serial.write("$ROTATE=90#");
}

void MainWindow::pushButtonS_clicked()
{
    serial.write("$ROTATE=180#");
}

void MainWindow::pushButtonW_clicked()
{
    serial.write("$ROTATE=-90#");
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        for(int i=0;i<16;i++)
        {
            for(int j=0;j<16;j++)
            {
               if(cells[j][i]->wallNorth->isUnderMouse())
               {
                  cells[j][i]->wallNorth->setVisible(!cells[j][i]->wallNorth->isVisible());
                  cells[j][i]->walls ^= NORTH;
               }
               if(cells[j][i]->wallEast->isUnderMouse())
               {
                  cells[j][i]->wallEast->setVisible(!cells[j][i]->wallEast->isVisible());
                  cells[j][i]->walls ^= EAST;
               }
               if(cells[j][i]->wallSouth->isUnderMouse())
               {
                  cells[j][i]->wallSouth->setVisible(!cells[j][i]->wallSouth->isVisible());
                  cells[j][i]->walls ^= SOUTH;
               }
               if(cells[j][i]->wallWest->isUnderMouse())
               {
                  cells[j][i]->wallWest->setVisible(!cells[j][i]->wallWest->isVisible());
                  cells[j][i]->walls ^= WEST;
               }

               if(cells[j][i]->rect->isUnderMouse())
               {
                    if(cells[j][i]->type == CELL_START)
                    {
                        if(cell_finish_count < 4)
                        {
                            cells[j][i]->type = CELL_FINISH;
                            cell_finish_count++;
                            cell_start_conut--;
                        }
                        else
                        {
                            cells[j][i]->type = CELL_NULL;
                            cell_start_conut--;
                        }
                    }
                    else if(cells[j][i]->type == CELL_FINISH)
                    {
                        cells[j][i]->type = CELL_NULL;
                        cell_finish_count--;
                    }
                    else if(cells[j][i]->type == CELL_NULL)
                    {
                        if(cell_start_conut == 0)
                        {
                            cells[j][i]->type = CELL_START;
                            cell_start_conut++;
                        }
                        else if(cell_finish_count < 4)
                        {
                            cells[j][i]->type = CELL_FINISH;
                            cell_finish_count++;
                        }

                    }

                    cells[j][i]->SET_BRUSH();

                    qDebug() << "Cell index : " << cells[j][i]->index;
                    qDebug() << "[" << j << "]" << "[" << i << "]";
                    qDebug() << "Walls : " << cells[j][i]->walls;
                    qDebug() << "Solver index : " << cells[j][i]->solver_index << Qt::endl;
               }
            }
        }

    }
}

void MainWindow::COMBO_BOX_MAZES_UPDATE()
{
    QDir path("D:/github-repos/micromouse/micromouse-other/maze-files/");
    QStringList files = path.entryList(QDir::Files);
    files.sort();
    ui->comboBoxLoad->clear();
    ui->comboBoxLoad->addItems(files);
    ui->comboBoxLoad->show();
}

void MainWindow::serialReceived()
{
    QByteArray serialData = serial.readAll();
    QString receivedData = QString::fromStdString(serialData.toStdString());
    ui->textBrowserTerminal->append(receivedData);



    QStringList splitedData = receivedData.split('=');
    splitedData = splitedData.last().split(',');
    splitedData.last().remove(QRegularExpression("#"));

    ui->textBrowserTerminal->append(splitedData.last());
    ui->textBrowserTerminal->append(splitedData.first());

    int index = MAP_VALID_INDEX(splitedData.first().toInt());



    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->index == index)
            {
                cells[j][i]->walls = splitedData.last().toInt();
                MAP_WALLS_UPDATE();
            }
        }
    }

}

void MainWindow::MAP_INIT_16x16()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setGeometry(5, 5, 16 * (CELL_WIDTH - POST_WIDTH) + 10 , 16 * (CELL_HEIGHT - POST_HEIGHT) + 10);

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i] = new Cell(i * (CELL_WIDTH - POST_WIDTH), j * (CELL_HEIGHT - POST_HEIGHT), (j * 16) + i, scene);
        }
    }

    for(int i = 0; i < 16; i++)
    {
        cells[i][15]->wallEast->setVisible(true);
        cells[i][15]->walls ^= EAST;
    }
    for(int i = 0; i < 16; i++)
    {
        cells[i][0]->wallWest->setVisible(true);
        cells[i][0]->walls ^= WEST;
    }
    for(int i = 0; i < 16; i++)
    {
        cells[0][i]->wallNorth->setVisible(true);
        cells[0][i]->walls ^= NORTH;
    }
    for(int i = 0; i < 16; i++)
    {
        cells[15][i]->wallSouth->setVisible(true);
        cells[15][i]->walls ^= SOUTH;
    }
}

void MainWindow::MAP_CLEAR()
{
    cell_start_conut = 0;
    cell_finish_count = 0;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->type = CELL_NULL;
            cells[j][i]->walls = 0;
            cells[j][i]->visited = false;
            cells[j][i]->solver_index = 0;
            cells[j][i]->solverIndexText->setPlainText("");
            cells[j][i]->wallNorth->setVisible(false);
            cells[j][i]->wallEast->setVisible(false);
            cells[j][i]->wallSouth->setVisible(false);
            cells[j][i]->wallWest->setVisible(false);
            cells[j][i]->SET_BRUSH();
        }
    }

    for(int i = 0; i < 16; i++) cells[i][15]->wallEast->setVisible(true);
    for(int i = 0; i < 16; i++) cells[i][0]->wallWest->setVisible(true);
    for(int i = 0; i < 16; i++) cells[0][i]->wallNorth->setVisible(true);
    for(int i = 0; i < 16; i++) cells[15][i]->wallSouth->setVisible(true);
}

void MainWindow::MAP_GENERATE_ITERATIVE(unsigned int j, unsigned int i)
{
    // fill maze with all walls
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->walls = 15;
            cells[j][i]->wallNorth->setVisible(true);
            cells[j][i]->wallEast->setVisible(true);
            cells[j][i]->wallSouth->setVisible(true);
            cells[j][i]->wallWest->setVisible(true);
        }
    }

    //create stack
    QStack<Cell*> stack;
    Cell *current_cell;
    unsigned int unvisited_neighbour = 0;
    bool north_neighbour = false;
    bool east_neighbour = false;
    bool south_neighbour = false;
    bool west_neighbour = false;

    // 1. Choose the initial cell, mark it as visited and push it to the stack
    cells[j][i]->visited = true;
    stack.push(cells[j][i]);

    /*2. While the stack is not empty
        1. Pop a cell from the stack and make it a current cell
        2. If the current cell has any neighbours which have not been visited
           1. Push the current cell to the stack
           2. Choose one of the unvisited neighbours
           3. Remove the wall between the current cell and the chosen cell
           4. Mark the chosen cell as visited and push it to the stack
        */

    while (!stack.isEmpty()) // 2.
    {
        current_cell = stack.pop(); // 1.

        for(int i=0;i<16;i++) // 2.
        {
            for(int j=0;j<16;j++)
            {
                if(current_cell->index == cells[j][i]->index)
                {
                    unvisited_neighbour = 0;
                    north_neighbour = false;
                    east_neighbour = false;
                    south_neighbour = false;
                    west_neighbour = false;

                    if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited)
                    {
                        unvisited_neighbour++;
                        west_neighbour = true;
                    }
                    if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited)
                    {
                        unvisited_neighbour++;
                        east_neighbour = true;
                    }
                    if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited)
                    {
                        unvisited_neighbour++;
                        north_neighbour = true;
                    }
                    if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited)
                    {
                        unvisited_neighbour++;
                        south_neighbour = true;
                    }
                }
            }
        }

        if(unvisited_neighbour > 0) // 2.
        {
            stack.push(current_cell);



            for(int i=0;i<16;i++) // 2.
            {
                for(int j=0;j<16;j++)
                {
                    if(current_cell->index == cells[j][i]->index)
                    {
                        while(1)
                        {
                            int random;
                            random = random_in_range(0, 3);

                            if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && west_neighbour && random == 0)
                            {
                                // 3.
                              cells[j][i]->wallWest->setVisible(false);
                              cells[j][i]->walls ^= WEST;
                              cells[j][(i - 1 < 0) ? 0 : i - 1]->wallEast->setVisible(false);
                              cells[j][(i - 1 < 0) ? 0 : i - 1]->walls ^= EAST;
                                // 4.
                              cells[j][(i - 1 < 0) ? 0 : i - 1]->visited = true;
                              stack.push(cells[j][(i - 1 < 0) ? 0 : i - 1]);
                              break;
                            }
                            if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && east_neighbour && random == 1)
                            {
                              cells[j][i]->wallEast->setVisible(false);
                              cells[j][i]->walls ^= EAST;
                              cells[j][(i + 1 > 15) ? 15 : i + 1]->wallWest->setVisible(false);
                              cells[j][(i + 1 > 15) ? 15 : i + 1]->walls ^= WEST;
                              cells[j][(i + 1 > 15) ? 15 : i + 1]->visited = true;
                              stack.push(cells[j][(i + 1 > 15) ? 15 : i + 1]);
                              break;
                            }
                            if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && north_neighbour && random == 2)
                            {
                              cells[j][i]->wallNorth->setVisible(false);
                              cells[j][i]->walls ^= NORTH;
                              cells[(j - 1 < 0) ? 0 : j - 1][i]->wallSouth->setVisible(false);
                              cells[(j - 1 < 0) ? 0 : j - 1][i]->walls ^= SOUTH;
                              cells[(j - 1 < 0) ? 0 : j - 1][i]->visited = true;
                              stack.push(cells[(j - 1 < 0) ? 0 : j - 1][i]);
                              break;
                            }
                            if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && south_neighbour && random == 3)
                            {
                              cells[j][i]->wallSouth->setVisible(false);
                              cells[j][i]->walls ^= SOUTH;
                              cells[(j + 1 > 15) ? 15 : j + 1][i]->wallNorth->setVisible(false);
                              cells[(j + 1 > 15) ? 15 : j + 1][i]->walls ^= NORTH;
                              cells[(j + 1 > 15) ? 15 : j + 1][i]->visited = true;
                              stack.push(cells[(j + 1 > 15) ? 15 : j + 1][i]);
                              break;
                            }
                        }
                    }
                }
            }
        }


    }

    for(int i=0;i<16;i++)   // reset visit flag
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->visited = false;
        }
    }
}

void MainWindow::MAP_WALLS_UPDATE()
{
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if((cells[j][i]->walls & NORTH) != 0)   cells[j][i]->wallNorth->setVisible(true);
            if((cells[j][i]->walls & EAST) != 0)    cells[j][i]->wallEast->setVisible(true);
            if((cells[j][i]->walls & SOUTH) != 0)   cells[j][i]->wallSouth->setVisible(true);
            if((cells[j][i]->walls & WEST) != 0)    cells[j][i]->wallWest->setVisible(true);
        }
    }
}

int MainWindow::SOLVE_FLOOD_FILL(unsigned int j, unsigned int i, unsigned int finish_cell_first, const std::optional<unsigned int> &finish_cell_second, const std::optional<unsigned int> &finish_cell_third, const std::optional<unsigned int> &finish_cell_fourth)
{
    unsigned int current_cell_index;
    unsigned int *finish_cell_index;
    unsigned int finishCellSolverIndexs[4] = {256,256,256,256};
    unsigned int count = 0;
    QStack<Cell*> *stack = nullptr;
    stack = new QStack<Cell*>();

    QStack<Cell*> *temp_stack = nullptr;
    temp_stack = new QStack<Cell*>();


    stack->push(cells[j][i]);

    while(!stack->isEmpty())
    {

        if(stack->size() >= 2)
        {
            while(!stack->empty())
            {
                temp_stack->push(stack->pop());
            }

            while(!temp_stack->empty())
            {
                current_cell_index = temp_stack->pop()->index;

                if(current_cell_index == finish_cell_first || current_cell_index == finish_cell_second || current_cell_index == finish_cell_third || current_cell_index == finish_cell_fourth)
                {
                    goal_reached = true;

                    for(int i=0;i<16;i++)
                    {
                        for(int j=0;j<16;j++)
                        {
                            if(current_cell_index == cells[j][i]->index)
                            {
                              finishCellSolverIndexs[count] = cells[j][i]->solver_index;
                            }
                        }
                    }

                    count++;
                    continue;
                }

                for(int i=0;i<16;i++)
                {
                    for(int j=0;j<16;j++)
                    {
                        if(current_cell_index == cells[j][i]->index)
                        {
                          SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(j, i, stack);
                        }
                    }
                }
            }
        }
        else
        {
            current_cell_index = stack->pop()->index;

            if(current_cell_index == finish_cell_first || current_cell_index == finish_cell_second || current_cell_index == finish_cell_third || current_cell_index == finish_cell_fourth)
            {
                goal_reached = true;

                for(int i=0;i<16;i++)
                {
                    for(int j=0;j<16;j++)
                    {
                        if(current_cell_index == cells[j][i]->index)
                        {
                          finishCellSolverIndexs[count] = cells[j][i]->solver_index;
                        }
                    }
                }

                count++;
                continue;
            }


            for(int i=0;i<16;i++)
            {
                for(int j=0;j<16;j++)
                {
                    if(current_cell_index == cells[j][i]->index)
                    {
                      SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(j, i, stack);
                    }
                }
            }
        }





    }

    finish_cell_index = std::min_element(std::begin(finishCellSolverIndexs), std::end(finishCellSolverIndexs));

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->index == finish_cell_first || cells[j][i]->index == finish_cell_second || cells[j][i]->index == finish_cell_third || cells[j][i]->index == finish_cell_fourth)
            {
                if(cells[j][i]->solver_index == *finish_cell_index)
                {
                    *finish_cell_index = cells[j][i]->index;
                }
            }
        }
    }

    return *finish_cell_index;
}

void MainWindow::SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(int j, int i, QStack<Cell*> *stack)
{
    if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST())
    {
        cells[j][(i - 1 < 0) ? 0 : i - 1]->solver_index = cells[j][i]->solver_index + 1;
        cells[j][(i - 1 < 0) ? 0 : i - 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[j][(i - 1 < 0) ? 0 : i - 1]->visited = true;
        cells[j][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);

        stack->push(cells[j][(i - 1 < 0) ? 0 : i - 1]);
     }
    if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST())
    {
        cells[j][(i + 1 > 15) ? 15 : i + 1]->solver_index = cells[j][i]->solver_index + 1;
        cells[j][(i + 1 > 15) ? 15 : i + 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[j][(i + 1 > 15) ? 15 : i + 1]->visited = true;
        cells[j][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);

        stack->push(cells[j][(i + 1 > 15) ? 15 : i + 1]);
     }
    if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && !cells[j][i]->IS_WALL_SOUTH())
    {
        cells[(j + 1 > 15) ? 15 : j + 1][i]->solver_index = cells[j][i]->solver_index + 1;
        cells[(j + 1 > 15) ? 15 : j + 1][i]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[(j + 1 > 15) ? 15 : j + 1][i]->visited = true;
        cells[(j + 1 > 15) ? 15 : j + 1][i]->rect->setBrush(Qt::yellow);

        stack->push(cells[(j + 1 > 15) ? 15 : j + 1][i]);
     }
    if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && !cells[j][i]->IS_WALL_NORTH())
    {
        cells[(j - 1 < 0) ? 0 : j - 1][i]->solver_index = cells[j][i]->solver_index + 1;
        cells[(j - 1 < 0) ? 0 : j - 1][i]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[(j - 1 < 0) ? 0 : j - 1][i]->visited = true;
        cells[(j - 1 < 0) ? 0 : j - 1][i]->rect->setBrush(Qt::yellow);

        stack->push(cells[(j - 1 < 0) ? 0 : j - 1][i]);
    }

    QWidget::repaint();
}

void MainWindow::SOLVE_FLOOD_GENERATE_PATH(unsigned int finish_index)
{
     QStack<Cell*> stack;
     int current_cell_index;

     for(int i=0;i<16;i++)
     {
         for(int j=0;j<16;j++)
         {
             if(finish_index == cells[j][i]->index)
             {
               cells[j][i]->type = CELL_PATH;
               cells[j][i]->SET_BRUSH();
               stack.push(cells[j][i]);
             }
         }
     }

     while(!stack.isEmpty())
     {
        current_cell_index = stack.pop()->index;

        for(int i=0;i<16;i++)
        {
            for(int j=0;j<16;j++)
            {
                if(current_cell_index == cells[j][i]->index)
                {

                    if(cells[j][(i - 1 < 0) ? 0 : i - 1]->solver_index == (cells[j][i]->solver_index - 1) && !cells[j][i]->IS_WALL_WEST())
                    {
                        cells[j][(i - 1 < 0) ? 0 : i - 1]->type = CELL_PATH;
                        cells[j][(i - 1 < 0) ? 0 : i - 1]->SET_BRUSH();
                        stack.push(cells[j][(i - 1 < 0) ? 0 : i - 1]);
                     }
                    else if(cells[j][(i + 1 > 15) ? 15 : i + 1]->solver_index == (cells[j][i]->solver_index - 1) && !cells[j][i]->IS_WALL_EAST())
                    {
                        cells[j][(i + 1 > 15) ? 15 : i + 1]->type = CELL_PATH;
                        cells[j][(i + 1 > 15) ? 15 : i + 1]->SET_BRUSH();
                        stack.push(cells[j][(i + 1 > 15) ? 15 : i + 1]);
                     }
                    else if(cells[(j + 1 > 15) ? 15 : j + 1][i]->solver_index == (cells[j][i]->solver_index - 1) && !cells[j][i]->IS_WALL_SOUTH())
                    {
                        cells[(j + 1 > 15) ? 15 : j + 1][i]->type = CELL_PATH;
                        cells[(j + 1 > 15) ? 15 : j + 1][i]->SET_BRUSH();
                        stack.push(cells[(j + 1 > 15) ? 15 : j + 1][i]);
                     }
                    else if(cells[(j - 1 < 0) ? 0 : j - 1][i]->solver_index == (cells[j][i]->solver_index - 1) && !cells[j][i]->IS_WALL_NORTH())
                    {
                        cells[(j - 1 < 0) ? 0 : j - 1][i]->type = CELL_PATH;
                        cells[(j - 1 < 0) ? 0 : j - 1][i]->SET_BRUSH();
                        stack.push(cells[(j - 1 < 0) ? 0 : j - 1][i]);
                    }

                }
            }
        }

        QWidget::repaint();
     }




}

void MainWindow::A_STAR_FIND_PATH(unsigned int start_cell_index, unsigned int finish_cell_index)
{
    Cell startCell;
    Cell finishCell;

    QList<Cell> *openSet = new QList<Cell>();
    QList<Cell> *closeSet = new QList<Cell>();


    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(start_cell_index == cells[j][i]->index)
            {
                startCell = *cells[j][i];
            }

            if(finish_cell_index == cells[j][i]->index)
            {
                finishCell = *cells[j][i];
            }
        }
    }

    openSet->append(startCell);

    while(!openSet->empty())
    {
        Cell currentCell;

        currentCell = openSet->first();

        for(auto item : *openSet)
        {
            if(item.get_fCost() < currentCell.get_fCost() || (item.get_fCost() == currentCell.get_fCost() && item.hCost < currentCell.hCost))
            {
                currentCell = item;
            }
        }

       // openSet->removeOne(currentCell);

        QList<Cell>::iterator it = openSet->begin();
        while(it != openSet->end())
        {
            if(it->index == currentCell.index)
            {
                it = openSet->erase(it);
            }
            else
            {
                ++it;
            }
        }

        closeSet->append(currentCell);

        if(currentCell.index == finishCell.index)
        {
            return;
        }

    }


}

int MainWindow::random_in_range(int min, int max)
{
    static bool first = true;
       if (first)
       {
          srand( time(NULL) ); //seeding for the first time only!
          first = false;
       }
       return min + rand() % (( max + 1 ) - min);
}

int MainWindow::MAP_VALID_INDEX(int index)
{
    int ret = 0;

    switch (index) {
        case 0: ret = 240;  break;
        case 1: ret = 241;  break;
        case 2: ret = 242;  break;

        case 3: ret = 224;  break;
        case 4: ret = 225;  break;
        case 5: ret = 226;  break;

        case 6: ret = 208;  break;
        case 7: ret = 209;  break;
        case 8: ret = 210;  break;

        case 9: ret = 192;  break;
        case 10: ret = 193;  break;
        case 11: ret = 194;  break;

        case 12: ret = 176;  break;
        case 13: ret = 177;  break;
        case 14: ret = 178;  break;

        case 15: ret = 160;  break;
        case 16: ret = 161;  break;
        case 17: ret = 162;  break;
    }


    return ret;
}


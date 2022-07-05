#include "mainwindow.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStack>
#include <QList>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonFloodFill,        SIGNAL(clicked()), this, SLOT(pushButtonFloodFill_clicked()));
    connect(ui->pushButtonClearWalls,       SIGNAL(clicked()), this, SLOT(pushButtonClearWalls_clicked()));
    connect(ui->pushButtonClearPath,        SIGNAL(clicked()), this, SLOT(pushButtonClearPath_clicked()));
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

    connect(ui->comboBoxAstar,                   SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAStar_onChange()));
    connect(ui->comboBoxBFS,                     SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxBFS_onChange()));
    connect(ui->checkBoxAllowDiagonal,           SIGNAL(clicked()), this, SLOT(radioButtonAllowDiagonal_onChange()));
    connect(ui->checkBoxFloodFillBiDirectional,  SIGNAL(clicked()), this, SLOT(radioButtonAllowFloodFillBiDirectional_onChange()));
    connect(ui->checkBoxAStarBiDirectional,      SIGNAL(clicked()), this, SLOT(radioButtonAllowAStarBiDirectional_onChange()));
    connect(ui->checkBoxShowSearching,           SIGNAL(clicked()), this, SLOT(checkBoxShowSearching_onChange()));

    connect(ui->checkBoxAllowDiagonalBFS,        SIGNAL(clicked()), this, SLOT(checkBoxBFSAllowDiagonal_onChange()));
    connect(ui->pushButtonBFS,                   SIGNAL(clicked()), this, SLOT(pushButtonBFS_clicked()));

    MAP_INIT_16x16();
    cell_start_defined  = false;
    cell_finish_defined = false;


    aStarHeuristicOptions.insert("Manhattan distance", ASTAR_MANHATTAN_DISTANCE);
    aStarHeuristicOptions.insert("Euclidean distance", ASTAR_EUCLIDEAN_DISTANE);
    aStarHeuristicOptions.insert("Diagonal - Chebyshev distance", ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV);
    aStarHeuristicOptions.insert("Diagonal - Octile distance", ASTAR_DIAGONAL_DISTANCE_OCTILE);
    ui->comboBoxAstar->clear();
    ui->comboBoxAstar->addItems(aStarHeuristicOptions.keys());
    ui->comboBoxAstar->setCurrentIndex(3);
    ui->comboBoxAstar->show();
    heuristicType = ASTAR_MANHATTAN_DISTANCE;
    allowDiagonal = false;
    biDirectionalAStar = false;
    biDirectionalFloodFill = false;
    showSearching = true;

    BFSHeuristicOptions.insert("Manhattan distance", ASTAR_MANHATTAN_DISTANCE);
    BFSHeuristicOptions.insert("Euclidean distance", ASTAR_EUCLIDEAN_DISTANE);
    BFSHeuristicOptions.insert("Diagonal - Chebyshev distance", ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV);
    BFSHeuristicOptions.insert("Diagonal - Octile distance", ASTAR_DIAGONAL_DISTANCE_OCTILE);
    ui->comboBoxBFS->clear();
    ui->comboBoxBFS->addItems(BFSHeuristicOptions.keys());
    ui->comboBoxBFS->setCurrentIndex(3);
    ui->comboBoxBFS->show();
    heuristicTypeBFS = ASTAR_MANHATTAN_DISTANCE;
    allowDiagonalBFS = false;

    QPolygonF polygon;
    triangle = new QGraphicsPolygonItem();
    triangle->setPolygon(polygon);
    triangle->setBrush(Qt::red);
    scene->addItem(triangle);
    triangle->setVisible(false);

    COMBO_BOX_MAZES_UPDATE();
}
MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
    serial.close();
}

void MainWindow::DRAW_TRIANGLE(Cell *cell, int direction)
{
    int centerX = cell->x + ((POST_WIDTH + CELL_WIDTH) / 2);
    int centerY = cell->y + ((POST_HEIGHT + CELL_HEIGHT) / 2);

    triangle->polygon().clear();

    QPolygonF polygon;
    polygon << QPointF(centerX, centerY - 10) << QPointF(centerX + 5, centerY + 5) << QPointF(centerX - 5, centerY + 5);

    polygon =  QTransform().translate(centerX, centerY).rotate(direction).translate(-centerX, -centerY).map(polygon);
    triangle->setPolygon(polygon);
    triangle->setVisible(true);
}

void MainWindow::REMOVE_TRIANGLE()
{
    triangle->setVisible(false);
}

void MainWindow::pushButtonFloodFill_clicked()
{
    Cell *startCell;
    Cell *finishCell;
    bool startFound = false;
    bool finishFound = false;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->type == CELL_START)
            {
              startCell = cells[j][i];
              startFound = true;
              cells[j][i]->visited = true;
              cells[j][i]->solver_index = 0;
              cells[j][i]->solverIndexText->setPlainText(QString::number(0));
              lastStartIndex = cells[j][i]->index;
            }
            if(cells[j][i]->type == CELL_FINISH)
            {
              finishCell = cells[j][i];
              finishFound = true;
              lastFinishIndexs.append(cells[j][i]->index);
            }
        }
    }
    goal_reached = false;

    if((startFound && finishFound) == true)
    {

        SOLVE_FLOOD_FILL(startCell, finishCell);

        if(goal_reached)
        {
            int count = 0;
            for(int i=0;i<16;i++)
            {
                for(int j=0;j<16;j++)
                {
                    if(cells[j][i]->rect->brush() == Qt::yellow) count++;
                }
            }

            UPDATE_CELL_COUNT(count);
            ui->groupBoxSearchInfo->setEnabled(true);

            SOLVE_FLOOD_GENERATE_PATH(finishCell->index);
            ui->pushButtonClearPath->setEnabled(true);

            int countPath = 0;
            for(int i=0;i<16;i++)
            {
                for(int j=0;j<16;j++)
                {
                    if(cells[j][i]->rect->brush() == Qt::darkGreen) countPath++;
                }
            }
            UPDATE_PATH_COUNT(countPath);
        }
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

void MainWindow::pushButtonClearPath_clicked()
{
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->index == lastStartIndex)
            {
                cells[j][i]->type = CELL_START;
            }
            else if(lastFinishIndexs.contains(cells[j][i]->index))
            {
                cells[j][i]->type = CELL_FINISH;
            }
            else
            {
                cells[j][i]->type = CELL_NULL;
            }

            cells[j][i]->visited = false;
            cells[j][i]->solver_index = 0;
            cells[j][i]->gCost = std::numeric_limits<int>::max();;
            cells[j][i]->hCost = std::numeric_limits<int>::max();;
            cells[j][i]->solverIndexText->setPlainText("");
            cells[j][i]->gText->setPlainText("");
            cells[j][i]->hText->setPlainText("");
            cells[j][i]->SET_BRUSH();
        }
    }

    lastFinishIndexs.clear();
    ui->pushButtonClearPath->setEnabled(false);
    RESTART_SEARCH_COUNTS();

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
    bool startFound = false, finishFound = false;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->type == CELL_START)
            {
              startCellIndex = cells[j][i]->index;
              startFound = true;
              lastStartIndex = cells[j][i]->index;
            }
            if(cells[j][i]->type == CELL_FINISH)
            {
              finishCellIndex = cells[j][i]->index;
              finishFound = true;
              lastFinishIndexs.append(cells[j][i]->index);
            }
        }
    }

    if(startFound && finishFound)
    {
        A_STAR_FIND_PATH(startCellIndex, finishCellIndex);
        ui->pushButtonClearPath->setEnabled(true);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to define start and finish cells!");
        msgBox.exec();
    }
}

void MainWindow::pushButtonBFS_clicked()
{
    Cell *startCell, *finishCell;
    bool startFound = false, finishFound = false;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->type == CELL_START)
            {
              startCell = cells[j][i];
              startFound = true;
              lastStartIndex = cells[j][i]->index;
            }
            if(cells[j][i]->type == CELL_FINISH)
            {
              finishCell = cells[j][i];
              finishFound = true;
              lastFinishIndexs.append(cells[j][i]->index);
            }
        }
    }

    if(startFound && finishFound)
    {
        BFS_FIND_PATH(startCell, finishCell);
        ui->pushButtonClearPath->setEnabled(true);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You need to define start and finish cells!");
        msgBox.exec();
    }
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

void MainWindow::comboBoxAStar_onChange()
{
    heuristicType = aStarHeuristicOptions.value(ui->comboBoxAstar->currentText());
}

void MainWindow::comboBoxBFS_onChange()
{
    heuristicTypeBFS = BFSHeuristicOptions.value(ui->comboBoxBFS->currentText());
}

void MainWindow::radioButtonAllowDiagonal_onChange()
{
    if(ui->checkBoxAllowDiagonal->isChecked())
           {
            allowDiagonal = true;
           }
           else
           {
            allowDiagonal = false;
    }
}

void MainWindow::radioButtonAllowAStarBiDirectional_onChange()
{
    if(ui->checkBoxAStarBiDirectional->isChecked())
           {
            biDirectionalAStar = true;
           }
           else
           {
            biDirectionalAStar = false;
    }
}

void MainWindow::radioButtonAllowFloodFillBiDirectional_onChange()
{
    if(ui->checkBoxFloodFillBiDirectional->isChecked())
           {
            biDirectionalFloodFill = true;
           }
           else
           {
            biDirectionalFloodFill = false;
    }
}

void MainWindow::checkBoxBFSAllowDiagonal_onChange()
{
    if(ui->checkBoxAllowDiagonalBFS->isChecked())
           {
            allowDiagonalBFS = true;
           }
           else
           {
            allowDiagonalBFS = false;
    }
}

void MainWindow::checkBoxShowSearching_onChange()
{
    if(ui->checkBoxShowSearching->isChecked())
           {
            showSearching = true;
           }
           else
           {
            showSearching = false;
    }
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
                   qDebug() << cells[j][i]->type;

                    if(cells[j][i]->type == CELL_START)
                    {
                        if(!cell_finish_defined)
                        {
                            cells[j][i]->type = CELL_FINISH;
                            cell_finish_defined = true;
                            cell_start_defined = false;
                        }
                        else
                        {
                            cells[j][i]->type = CELL_NULL;
                            cell_start_defined = false;
                        }
                    }
                    else if(cells[j][i]->type == CELL_FINISH)
                    {
                        cells[j][i]->type = CELL_NULL;
                        cell_finish_defined = false;
                    }
                    else if(cells[j][i]->type == CELL_NULL)
                    {
                        if(!cell_start_defined)
                        {
                            cells[j][i]->type = CELL_START;
                            cell_start_defined = true;
                        }
                        else if(!cell_finish_defined)
                        {
                            cells[j][i]->type = CELL_FINISH;
                            cell_finish_defined = true;
                        }
                    }

                    cells[j][i]->SET_BRUSH();

                    qDebug() << "Cell index : " << cells[j][i]->index;
                    qDebug() << "[" << j << "]" << "[" << i << "]";
                    qDebug() << "x[" << cells[j][i]->x << "]" << "y[" << cells[j][i]->y << "]";
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

    if(splitedData.count() >= 3)
    {
        ui->textBrowserTerminal->append(splitedData.first() + "," + splitedData.at(1) + "," + splitedData.last());

        int index = MAP_VALID_INDEX(splitedData.at(0).toInt());
        int walls = splitedData.at(1).toInt();
        int direction = splitedData.at(2).toInt();
        int angle;

        if(direction == NORTH) angle = 0;
        else if(direction == EAST) angle = 90;
        else if(direction == SOUTH) angle = 180;
        else angle = -90;

        for(int i=0;i<16;i++)
        {
            for(int j=0;j<16;j++)
            {

                if(cells[j][i]->index == index)
                {
                    cells[j][i]->walls = walls;
                    cells[j][i]->rect->setBrush(Qt::yellow);
                    DRAW_TRIANGLE(cells[j][i], angle);
                    MAP_WALLS_UPDATE();
                }

                ui->graphicsView->repaint();
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
    cell_start_defined  = false;
    cell_finish_defined = false;
    lastFinishIndexs.clear();

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->type = CELL_NULL;
            cells[j][i]->walls = 0;
            cells[j][i]->visited = false;
            cells[j][i]->solver_index = 0;
            cells[j][i]->gCost = std::numeric_limits<int>::max();;
            cells[j][i]->hCost = std::numeric_limits<int>::max();;
            cells[j][i]->solverIndexText->setPlainText("");
            cells[j][i]->gText->setPlainText("");
            cells[j][i]->hText->setPlainText("");
            cells[j][i]->wallNorth->setVisible(false);
            cells[j][i]->wallEast->setVisible(false);
            cells[j][i]->wallSouth->setVisible(false);
            cells[j][i]->wallWest->setVisible(false);
            cells[j][i]->SET_BRUSH();
            REMOVE_TRIANGLE();
        }
    }

    for(int i = 0; i < 16; i++) cells[i][15]->wallEast->setVisible(true);
    for(int i = 0; i < 16; i++) cells[i][0]->wallWest->setVisible(true);
    for(int i = 0; i < 16; i++) cells[0][i]->wallNorth->setVisible(true);
    for(int i = 0; i < 16; i++) cells[15][i]->wallSouth->setVisible(true);

    RESTART_SEARCH_COUNTS();
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

void MainWindow::SOLVE_FLOOD_FILL(Cell *startCell, Cell *finishCell)
{
    unsigned int current_cell_index;

    QStack<Cell*> *stack = nullptr;
    stack = new QStack<Cell*>();

    QStack<Cell*> *temp_stack = nullptr;
    temp_stack = new QStack<Cell*>();

    stack->push(startCell);

    while(!stack->isEmpty() && !goal_reached)
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

                for(int i=0;i<16;i++)
                {
                    for(int j=0;j<16;j++)
                    {
                        if(current_cell_index == cells[j][i]->index)
                        {
                          SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(j, i, stack, finishCell);
                        }
                    }
                }
            }
        }
        else
        {
            current_cell_index = stack->pop()->index;

            for(int i=0;i<16;i++)
            {
                for(int j=0;j<16;j++)
                {
                    if(current_cell_index == cells[j][i]->index)
                    {
                      SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(j, i, stack, finishCell);
                    }
                }
            }
        }
    }

    delete(stack);
    delete(temp_stack);
}

void MainWindow::SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(int j, int i, QStack<Cell*> *stack, Cell *finishCell)
{
    if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST())
    {
        cells[j][(i - 1 < 0) ? 0 : i - 1]->solver_index = cells[j][i]->solver_index + 1;
        cells[j][(i - 1 < 0) ? 0 : i - 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[j][(i - 1 < 0) ? 0 : i - 1]->visited = true;
        cells[j][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);

        if(cells[j][(i - 1 < 0) ? 0 : i - 1]->index == finishCell->index)
        {
            goal_reached = true;
            return;
        }
        stack->push(cells[j][(i - 1 < 0) ? 0 : i - 1]);
     }
    if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST())
    {
        cells[j][(i + 1 > 15) ? 15 : i + 1]->solver_index = cells[j][i]->solver_index + 1;
        cells[j][(i + 1 > 15) ? 15 : i + 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[j][(i + 1 > 15) ? 15 : i + 1]->visited = true;
        cells[j][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);

        if(cells[j][(i + 1 > 15) ? 15 : i + 1]->index == finishCell->index)
        {
            goal_reached = true;
            return;
        }
        stack->push(cells[j][(i + 1 > 15) ? 15 : i + 1]);
     }
    if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && !cells[j][i]->IS_WALL_SOUTH())
    {
        cells[(j + 1 > 15) ? 15 : j + 1][i]->solver_index = cells[j][i]->solver_index + 1;
        cells[(j + 1 > 15) ? 15 : j + 1][i]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[(j + 1 > 15) ? 15 : j + 1][i]->visited = true;
        cells[(j + 1 > 15) ? 15 : j + 1][i]->rect->setBrush(Qt::yellow);

        if(cells[(j + 1 > 15) ? 15 : j + 1][i]->index == finishCell->index)
        {
            goal_reached = true;
            return;
        }
        stack->push(cells[(j + 1 > 15) ? 15 : j + 1][i]);
     }
    if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && !cells[j][i]->IS_WALL_NORTH())
    {
        cells[(j - 1 < 0) ? 0 : j - 1][i]->solver_index = cells[j][i]->solver_index + 1;
        cells[(j - 1 < 0) ? 0 : j - 1][i]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
        cells[(j - 1 < 0) ? 0 : j - 1][i]->visited = true;
        cells[(j - 1 < 0) ? 0 : j - 1][i]->rect->setBrush(Qt::yellow);

        if(cells[(j - 1 < 0) ? 0 : j - 1][i]->index == finishCell->index)
        {
            goal_reached = true;
            return;
        }
        stack->push(cells[(j - 1 < 0) ? 0 : j - 1][i]);
    }

    if(showSearching) ui->graphicsView->repaint();
}

void MainWindow::SOLVE_FLOOD_GENERATE_PATH(unsigned int finish_index)
{
     QStack<Cell*> stack;
     Cell *currentCell;
     int turnCount = 0;
     bool travelAlongX = false;
     bool travelAlongY = false;
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
        currentCell = stack.pop();

        for(int i=0;i<16;i++)
        {
            for(int j=0;j<16;j++)
            {
                if(currentCell->index == cells[j][i]->index)
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

        if(!stack.isEmpty())
        {
            if(currentCell->x == stack.first()->x && !travelAlongX && !travelAlongY)
            {
                travelAlongY = true;
            }
            else if(currentCell->y == stack.first()->y && !travelAlongX && !travelAlongY)
            {
                travelAlongX = true;
            }

            if(travelAlongX && currentCell->x == stack.first()->x)
            {
                turnCount++;
                travelAlongX = false;
                travelAlongY = true;
            }
            else if(travelAlongY && currentCell->y == stack.first()->y)
            {
                turnCount++;
                travelAlongX = true;
                travelAlongY = false;
            }
        }

        if(showSearching) ui->graphicsView->repaint();
     }

     UPDATE_TURN_COUNT(turnCount);
}

void MainWindow::A_STAR_FIND_PATH(unsigned int start_cell_index, unsigned int finish_cell_index)
{
    Cell *startCell;
    Cell *finishCell;

    QList<Cell*> openSet;
    QList<Cell*> closeSet;


    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(start_cell_index == cells[j][i]->index)
            {
                startCell = cells[j][i];
            }

            if(finish_cell_index == cells[j][i]->index)
            {
                finishCell = cells[j][i];
            }
        }
    }

    startCell->gCost = 0;
    startCell->hCost = GET_DISTANCE_BETWEEN_CELLS(*startCell,*finishCell);
    openSet.append(startCell);
    startCell->visited = true;

    while(!openSet.empty())
    {
        Cell *currentCell = nullptr;

        currentCell = openSet.first();

        for(auto cell : openSet)
        {
            if(cell->index == currentCell->index)
            {
                continue;
            }
            if(cell->get_fCost() < currentCell->get_fCost() || (cell->get_fCost() == currentCell->get_fCost() && cell->hCost < currentCell->hCost))
            {
                currentCell = cell;
            }
        }

        openSet.removeOne(currentCell);
        closeSet.append(currentCell);
        currentCell->rect->setBrush(Qt::magenta);
        if(showSearching) ui->graphicsView->repaint();

        if(currentCell->index == finishCell->index)
        {
            A_STAR_GENERATE_PATH(startCell, finishCell);
            return;
        }

        for(auto neighbour : A_STAR_GET_NEIGHBOURS(*currentCell))
        {
            int newMovmentCostToNeighbour = currentCell->gCost + GET_DISTANCE_BETWEEN_CELLS(*neighbour, *currentCell);

            if(closeSet.contains(neighbour))
            {
                continue;
            }

            if(newMovmentCostToNeighbour < neighbour->gCost || !openSet.contains(neighbour))
            {
                neighbour->gCost = newMovmentCostToNeighbour;
                neighbour->hCost = GET_DISTANCE_BETWEEN_CELLS(*finishCell, *neighbour);
                neighbour->solverIndexText->setPlainText("f" + QString::number(neighbour->get_fCost()));
                neighbour->gText->setPlainText("g" + QString::number(neighbour->gCost));
                neighbour->hText->setPlainText("h" + QString::number(neighbour->hCost));
                neighbour->parent = currentCell;
                neighbour->rect->setBrush(Qt::yellow);

                if(!openSet.contains(neighbour))
                {
                    openSet.append(neighbour);
                }

                if(showSearching) ui->graphicsView->repaint();
            }
        }
    }
}

QList<Cell*> MainWindow::A_STAR_GET_NEIGHBOURS(Cell cell)
{
    QList<Cell*> neighbours;

    int j, i;

    for(int x=0;x<16;x++)
    {
        for(int y=0;y<16;y++)
        {
            if(cell.index == cells[y][x]->index)
            {
                j = y;
                i = x;
            }
        }
    }

    if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST())
    {
        //cells[j][(i - 1 < 0) ? 0 : i - 1]->visited = true;
        //cells[j][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);
        neighbours.append(cells[j][(i - 1 < 0) ? 0 : i - 1]);
    }
    if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST())
    {
        //cells[j][(i + 1 > 15) ? 15 : i + 1]->visited = true;
        //cells[j][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);
        neighbours.append(cells[j][(i + 1 > 15) ? 15 : i + 1]);
    }
    if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && !cells[j][i]->IS_WALL_SOUTH())
    {
       // cells[(j + 1 > 15) ? 15 : j + 1][i]->visited = true;
        //cells[(j + 1 > 15) ? 15 : j + 1][i]->rect->setBrush(Qt::yellow);
        neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][i]);
    }
    if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && !cells[j][i]->IS_WALL_NORTH())
    {
       // cells[(j - 1 < 0) ? 0 : j - 1][i]->visited = true;
        //cells[(j - 1 < 0) ? 0 : j - 1][i]->rect->setBrush(Qt::yellow);
        neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][i]);
    }

    if(allowDiagonal)
    {
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH())
        {
           // cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited = true;
            //cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())
        {
         //   cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited = true;
            //cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH())
        {
           // cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited = true;
            //cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())
        {
          //  cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited = true;
            //cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]);
        }

    }

    if(showSearching) ui->graphicsView->repaint();

    return neighbours;
}

void MainWindow::A_STAR_GENERATE_PATH(Cell *startCell, Cell *finishCell)
{
    QList<Cell> path;
    Cell *currentCell;
    currentCell = finishCell;

    int turnCount = 0;
    bool travelAlongX = false;
    bool travelAlongY = false;

    int countCell = 0;
    int countPath = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() != Qt::lightGray) countCell++;
        }
    }

    UPDATE_CELL_COUNT(countCell - 1);
    ui->groupBoxSearchInfo->setEnabled(true);

    while(currentCell->index != startCell->index)
    {
        path.append(*currentCell);
        currentCell->type = CELL_PATH;
        currentCell->SET_BRUSH();

        if(currentCell->x == currentCell->parent->x && !travelAlongX && !travelAlongY)
        {
            travelAlongY = true;
        }
        else if(currentCell->y == currentCell->parent->y && !travelAlongX && !travelAlongY)
        {
            travelAlongX = true;
        }

        if(travelAlongX && currentCell->x == currentCell->parent->x)
        {
            turnCount++;
            travelAlongX = false;
            travelAlongY = true;
        }
        else if(travelAlongY && currentCell->y == currentCell->parent->y)
        {
            turnCount++;
            travelAlongX = true;
            travelAlongY = false;
        }

        currentCell = currentCell->parent;

        if(showSearching) ui->graphicsView->repaint();
    }

    currentCell->type = CELL_PATH;
    currentCell->SET_BRUSH();

    if(showSearching) ui->graphicsView->repaint();

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() == Qt::darkGreen) countPath++;
        }
    }
    UPDATE_PATH_COUNT(countPath);
    UPDATE_TURN_COUNT(turnCount);
}

int MainWindow::GET_DISTANCE_BETWEEN_CELLS(Cell cellA, Cell cellB)
{
    /*
     * distance between two cells normally is 1 * 10
     * distance between two cells diagonally is 1 * 14
    */

    int distanceX = abs(cellA.x - cellB.x) / 50;
    int distanceY = abs(cellA.y - cellB.y) / 50;

    switch (heuristicType)
    {
    case ASTAR_MANHATTAN_DISTANCE:
            return (distanceX + distanceY) * 10;

        break;

    case ASTAR_DIAGONAL_DISTANCE_OCTILE:
            return 10 * (distanceX + distanceY) + (14 - 2 * 10) * fmin(distanceX, distanceY);

        break;

    case ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV:
            return 10 * (distanceX + distanceY) + (10 - 2 * 10) * fmin(distanceX, distanceY);

        break;

    case ASTAR_EUCLIDEAN_DISTANE:
            return sqrt((distanceX * distanceX) + (distanceY * distanceY)) * 10;

        break;

    default:
            return (distanceX + distanceY) * 10;
        break;
    }
}

void MainWindow::BFS_FIND_PATH(Cell *startCell, Cell *finishCell)
{
    QList<Cell*> openSet;
    QList<Cell*> closeSet;


    startCell->hCost = BFS_GET_DISTANCE_BETWEEN_CELLS(*startCell,*finishCell);
    openSet.append(startCell);
    startCell->visited = true;

    while(!openSet.empty())
    {
        Cell *currentCell = nullptr;

        currentCell = openSet.first();

        for(auto cell : openSet)
        {
            if(cell->index == currentCell->index)
            {
                continue;
            }
            if(cell->hCost < currentCell->hCost)
            {
                currentCell = cell;
            }
        }

        openSet.removeOne(currentCell);
        closeSet.append(currentCell);
        currentCell->rect->setBrush(Qt::magenta);
        if(showSearching) ui->graphicsView->repaint();

        for(auto neighbour : BFS_GET_NEIGHBOURS(currentCell))
        {
            if(closeSet.contains(neighbour))
            {
                continue;
            }

            if(neighbour->index == finishCell->index)
            {
                neighbour->hCost = BFS_GET_DISTANCE_BETWEEN_CELLS(*neighbour,*finishCell);
                neighbour->hText->setPlainText("h" + QString::number(neighbour->hCost));
                neighbour->parent = currentCell;
                neighbour->rect->setBrush(Qt::yellow);
                BFS_GENERATE_PATH(startCell, finishCell);
                return;
            }
                if(!openSet.contains(neighbour))
                {
                neighbour->hCost = BFS_GET_DISTANCE_BETWEEN_CELLS(*neighbour,*finishCell);
                neighbour->hText->setPlainText("h" + QString::number(neighbour->hCost));
                neighbour->parent = currentCell;
                neighbour->rect->setBrush(Qt::yellow);
                openSet.append(neighbour);
                }

                if(showSearching) ui->graphicsView->repaint();
            }
    }
}

QList<Cell *> MainWindow::BFS_GET_NEIGHBOURS(Cell *cell)
{
    QList<Cell*> neighbours;

    int j, i;

    for(int x=0;x<16;x++)
    {
        for(int y=0;y<16;y++)
        {
            if(cell->index == cells[y][x]->index)
            {
                j = y;
                i = x;
            }
        }
    }

    if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST())
    {
        neighbours.append(cells[j][(i - 1 < 0) ? 0 : i - 1]);
    }
    if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST())
    {
        neighbours.append(cells[j][(i + 1 > 15) ? 15 : i + 1]);
    }
    if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && !cells[j][i]->IS_WALL_SOUTH())
    {
        neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][i]);
    }
    if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && !cells[j][i]->IS_WALL_NORTH())
    {
        neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][i]);
    }

    if(allowDiagonalBFS)
    {
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH())
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH())
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]);
        }

    }

    if(showSearching) ui->graphicsView->repaint();

    return neighbours;
}

void MainWindow::BFS_GENERATE_PATH(Cell *startCell, Cell *finishCell)
{
    QList<Cell> path;
    Cell *currentCell;
    currentCell = finishCell;

    int turnCount = 0;
    bool travelAlongX = false;
    bool travelAlongY = false;

    int countCell = 0;
    int countPath = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() != Qt::lightGray) countCell++;
        }
    }

    UPDATE_CELL_COUNT(countCell - 1);
    ui->groupBoxSearchInfo->setEnabled(true);

    while(currentCell->index != startCell->index)
    {
        path.append(*currentCell);
        currentCell->type = CELL_PATH;
        currentCell->SET_BRUSH();

        if(currentCell->x == currentCell->parent->x && !travelAlongX && !travelAlongY)
        {
            travelAlongY = true;
        }
        else if(currentCell->y == currentCell->parent->y && !travelAlongX && !travelAlongY)
        {
            travelAlongX = true;
        }

        if(travelAlongX && currentCell->x == currentCell->parent->x)
        {
            turnCount++;
            travelAlongX = false;
            travelAlongY = true;
        }
        else if(travelAlongY && currentCell->y == currentCell->parent->y)
        {
            turnCount++;
            travelAlongX = true;
            travelAlongY = false;
        }

        currentCell = currentCell->parent;

        if(showSearching) ui->graphicsView->repaint();
    }

    currentCell->type = CELL_PATH;
    currentCell->SET_BRUSH();

    if(showSearching) ui->graphicsView->repaint();

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() == Qt::darkGreen) countPath++;
        }
    }
    UPDATE_PATH_COUNT(countPath);
    UPDATE_TURN_COUNT(turnCount);
}

int MainWindow::BFS_GET_DISTANCE_BETWEEN_CELLS(Cell cellA, Cell cellB)
{
    int distanceX = abs(cellA.x - cellB.x) / 50;
    int distanceY = abs(cellA.y - cellB.y) / 50;

    switch (heuristicTypeBFS)
    {
    case ASTAR_MANHATTAN_DISTANCE:
            return (distanceX + distanceY) * 10;

        break;

    case ASTAR_DIAGONAL_DISTANCE_OCTILE:
            return 10 * (distanceX + distanceY) + (14 - 2 * 10) * fmin(distanceX, distanceY);

        break;

    case ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV:
            return 10 * (distanceX + distanceY) + (10 - 2 * 10) * fmin(distanceX, distanceY);

        break;

    case ASTAR_EUCLIDEAN_DISTANE:
            return sqrt((distanceX * distanceX) + (distanceY * distanceY)) * 10;

        break;

    default:
            return (distanceX + distanceY) * 10;
        break;
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

void MainWindow::UPDATE_CELL_COUNT(int count)
{
    ui->labelCellCount->setText(QString::number(count));
}

void MainWindow::UPDATE_PATH_COUNT(int count)
{
    ui->labelPathCount->setText(QString::number(count));
}

void MainWindow::UPDATE_TURN_COUNT(int count)
{
    ui->labelTurnCount->setText(QString::number(count));
}

void MainWindow::RESTART_SEARCH_COUNTS()
{
    ui->labelCellCount->setText("0");
    ui->labelPathCount->setText("0");
    ui->labelTurnCount->setText("0");
    ui->groupBoxSearchInfo->setEnabled(false);
}


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
    connect(ui->pushButtonNextStep,         SIGNAL(clicked()), this, SLOT(pushButtonNextStep_clicked()));
    connect(ui->pushButtonAuto,             SIGNAL(clicked()), this, SLOT(pushButtonAutoSearch_clicked()));
    connect(ui->pushButtonGenerate,         SIGNAL(clicked()), this, SLOT(pushButtonGenerate_clicked()));
    connect(ui->pushButtonAStar,            SIGNAL(clicked()), this, SLOT(pushButtonAStar_clicked()));
    connect(ui->pushButtonSaveMaze,         SIGNAL(clicked()), this, SLOT(pushButtonSaveMaze_clicked()));
    connect(ui->pushButtonLoadMaze,         SIGNAL(clicked()), this, SLOT(pushButtonLoadMaze_clicked()));

    connect(ui->comboBoxAstar,                   SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAStar_onChange()));
    connect(ui->comboBoxBFS,                     SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxBFS_onChange()));
    connect(ui->comboBoxDjikstra,                SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxDjikstra_onChange()));
    connect(ui->checkBoxAllowDiagonal,           SIGNAL(clicked()), this, SLOT(radioButtonAllowDiagonal_onChange()));
    connect(ui->checkBoxShowSearching,           SIGNAL(clicked()), this, SLOT(checkBoxShowSearching_onChange()));

    connect(ui->checkBoxAllowDiagonalBFS,        SIGNAL(clicked()), this, SLOT(checkBoxBFSAllowDiagonal_onChange()));
    connect(ui->checkBoxAllowDiagonalFlood,      SIGNAL(clicked()), this, SLOT(checkBoxFloodAllowDiagonal_onChange()));
    connect(ui->pushButtonBFS,                   SIGNAL(clicked()), this, SLOT(pushButtonBFS_clicked()));

    connect(ui->checkBoxAllowDiagonalDjikstra,   SIGNAL(clicked()), this, SLOT(checkBoxDjikstraAllowDiagonal_onChange()));
    connect(ui->pushButtonDjikstra,              SIGNAL(clicked()), this, SLOT(pushButtonDjikstra_clicked()));

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

    BFSHeuristicOptions.insert("Manhattan distance", ASTAR_MANHATTAN_DISTANCE);
    BFSHeuristicOptions.insert("Euclidean distance", ASTAR_EUCLIDEAN_DISTANE);
    BFSHeuristicOptions.insert("Diagonal - Chebyshev distance", ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV);
    BFSHeuristicOptions.insert("Diagonal - Octile distance", ASTAR_DIAGONAL_DISTANCE_OCTILE);
    ui->comboBoxBFS->clear();
    ui->comboBoxBFS->addItems(BFSHeuristicOptions.keys());
    ui->comboBoxBFS->setCurrentIndex(3);
    ui->comboBoxBFS->show();
    heuristicTypeBFS = ASTAR_MANHATTAN_DISTANCE;

    DjikstraHeuristicOptions.insert("Manhattan distance", ASTAR_MANHATTAN_DISTANCE);
    DjikstraHeuristicOptions.insert("Euclidean distance", ASTAR_EUCLIDEAN_DISTANE);
    DjikstraHeuristicOptions.insert("Diagonal - Chebyshev distance", ASTAR_DIAGONAL_DISTANCE_CHEBYSHEV);
    DjikstraHeuristicOptions.insert("Diagonal - Octile distance", ASTAR_DIAGONAL_DISTANCE_OCTILE);
    ui->comboBoxDjikstra->clear();
    ui->comboBoxDjikstra->addItems(DjikstraHeuristicOptions.keys());
    ui->comboBoxDjikstra->setCurrentIndex(3);
    ui->comboBoxDjikstra->show();
    heuristicTypeDjikstra = ASTAR_MANHATTAN_DISTANCE;

    allowDiagonalBFS = false;
    allowDiagonalDjikstra = false;
    allowDiagonal = false;
    showSearching = true;
    allowDiagonalFlood = false;
    ui->pushButtonAuto->setEnabled(true);
    ui->pushButtonNextStep->setEnabled(true);
    ui->pushButtonAuto->setChecked(true);
    autoSearch = true;

    COMBO_BOX_MAZES_UPDATE();
}
MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
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

            UPDATE_CELL_COUNT(count + 1);
            ui->groupBoxSearchInfo->setEnabled(true);

            SOLVE_FLOOD_GENERATE_PATH(finishCell->index);
            ui->pushButtonClearPath->setEnabled(true);         
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

void MainWindow::pushButtonNextStep_clicked()
{
    nextStep = true;
}

void MainWindow::pushButtonAutoSearch_clicked()
{
    if(ui->pushButtonAuto->isChecked())
           {
            autoSearch = true;
           }
           else
           {
            autoSearch = false;
           }
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

void MainWindow::pushButtonDjikstra_clicked()
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
        DJIKSTRA_FIND_PATH(startCell, finishCell);
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

void MainWindow::comboBoxAStar_onChange()
{
    heuristicType = aStarHeuristicOptions.value(ui->comboBoxAstar->currentText());
}

void MainWindow::comboBoxBFS_onChange()
{
    heuristicTypeBFS = BFSHeuristicOptions.value(ui->comboBoxBFS->currentText());
}

void MainWindow::comboBoxDjikstra_onChange()
{
    heuristicTypeDjikstra = DjikstraHeuristicOptions.value(ui->comboBoxDjikstra->currentText());
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

void MainWindow::checkBoxDjikstraAllowDiagonal_onChange()
{
    if(ui->checkBoxAllowDiagonalDjikstra->isChecked())
           {
            allowDiagonalDjikstra = true;
           }
           else
           {
            allowDiagonalDjikstra = false;
    }
}

void MainWindow::checkBoxFloodAllowDiagonal_onChange()
{
    if(ui->checkBoxAllowDiagonalFlood->isChecked())
           {
            allowDiagonalFlood = true;
           }
           else
           {
            allowDiagonalFlood = false;
    }
}

void MainWindow::checkBoxShowSearching_onChange()
{
    if(ui->checkBoxShowSearching->isChecked())
           {
            showSearching = true;
            ui->pushButtonAuto->setEnabled(true);
            ui->pushButtonNextStep->setEnabled(true);
           }
           else
           {
            showSearching = false;
            ui->pushButtonAuto->setEnabled(false);
            ui->pushButtonNextStep->setEnabled(false);
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
                    qDebug() << "j[" << j << "]" << "i[" << i << "]";
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

void MainWindow::MAP_INIT_16x16()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setGeometry(10, 20, 16 * (CELL_WIDTH - POST_WIDTH) + 10 , 16 * (CELL_HEIGHT - POST_HEIGHT) + 10);

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

        if(showSearching) ui->graphicsView->repaint();
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

        if(!autoSearch)
        {
        QEventLoop loop;
        connect(ui->pushButtonNextStep, &QPushButton::pressed, &loop, &QEventLoop::quit);
        loop.exec();
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

    if(allowDiagonalFlood)
    {
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited &&     // left top corner
                ((!cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH()) ||
                 (!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())))
        {
           cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->solver_index = cells[j][i]->solver_index + 1;
           cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
           cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited = true;
           cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);

           if(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->index == finishCell->index)
           {
               goal_reached = true;
               return;
           }
           stack->push(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited &&   // right top corner
                ((!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST()) ||
                 (!cells[j][i]->IS_WALL_EAST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_SOUTH())))
        {
            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->solver_index = cells[j][i]->solver_index + 1;
            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited = true;
            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);

            if(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->index == finishCell->index)
            {
                goal_reached = true;
                return;
            }
            stack->push(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && // right bottom corner
                ((!cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH()) ||
                 (!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())))
        {
            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->solver_index = cells[j][i]->solver_index + 1;
            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited = true;
            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);

            if(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->index == finishCell->index)
            {
                goal_reached = true;
                return;
            }
            stack->push(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited &&   // left bottom corner
                ((!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST()) ||
                 (!cells[j][i]->IS_WALL_WEST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_NORTH())))
        {
            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->solver_index = cells[j][i]->solver_index + 1;
            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->solverIndexText->setPlainText(QString::number(cells[j][i]->solver_index + 1));
            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited = true;
            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);

            if(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->index == finishCell->index)
            {
                goal_reached = true;
                return;
            }
            stack->push(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]);
        }

    }

    if(showSearching) ui->graphicsView->repaint();
}

void MainWindow::SOLVE_FLOOD_GENERATE_PATH(unsigned int finish_index)
{
     QStack<Cell*> stack;
     Cell *currentCell;
     int turnCount90 = 0;
     int turnCount45 = 0;
     int countPath = 0;
     int travelStraightCounter = 0;
     int travelDiagonalCounter = 0;
     bool travelAlongX = false;
     bool travelAlongY = false;
     bool travelDiagonal = false;

     bool travelDiagonalMM = false;
     bool travelDiagonalMP = false;
     bool travelDiagonalPP = false;
     bool travelDiagonalPM = false;

     bool prevTravelDiagonalMM = false;
     bool prevTravelDiagonalMP = false;
     bool prevTravelDiagonalPP = false;
     bool prevTravelDiagonalPM = false;

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
        countPath++;

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
                    else if(allowDiagonalFlood)
                    {
                        if((cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->solver_index == (cells[j][i]->solver_index - 1)) &&   // left top corner
                                ((!cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH()) ||
                                 (!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())))
                        {
                            cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->type = CELL_PATH;
                            cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->SET_BRUSH();
                            stack.push(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
                        }
                        else if((cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->solver_index == (cells[j][i]->solver_index - 1)) &&   // right top corner
                                ((!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST()) ||
                                 (!cells[j][i]->IS_WALL_EAST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_SOUTH())))
                        {
                            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->type = CELL_PATH;
                            cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->SET_BRUSH();
                            stack.push(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
                        }
                        else if((cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->solver_index == (cells[j][i]->solver_index - 1)) && // right bottom corner
                                ((!cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH()) ||
                                 (!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())))
                        {
                            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->type = CELL_PATH;
                            cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->SET_BRUSH();
                            stack.push(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
                        }
                        else if((cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->solver_index == (cells[j][i]->solver_index - 1)) &&   // left bottom corner
                                ((!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST()) ||
                                 (!cells[j][i]->IS_WALL_WEST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_NORTH())))
                        {
                            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->type = CELL_PATH;
                            cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->SET_BRUSH();
                            stack.push(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]);
                        }

                    }

                }
            }
        }

        if(!stack.isEmpty())
        {
            if(currentCell->x == stack.first()->x && !travelAlongX && !travelAlongY)
            {
                travelAlongY = true;
                travelDiagonal = false;
            }
            else if(currentCell->y == stack.first()->y && !travelAlongX && !travelAlongY)
            {
                travelAlongX = true;
                travelDiagonal = false;
            }
            else if((currentCell->y != stack.first()->y && currentCell->x != stack.first()->x) && !travelDiagonal)
            {
                travelDiagonal = true;
                travelAlongY = false;
                travelAlongX = false;
            }

            if(travelDiagonal)
            {
                prevTravelDiagonalMM = travelDiagonalMM;
                prevTravelDiagonalMP = travelDiagonalMP;
                prevTravelDiagonalPP = travelDiagonalPP;
                prevTravelDiagonalPM = travelDiagonalPM;

                if(currentCell->y > stack.first()->y && currentCell->x > stack.first()->x)
                {
                    travelDiagonalMM = false;
                    travelDiagonalMP = false;
                    travelDiagonalPP = true;
                    travelDiagonalPM = false;
                }
                else if(currentCell->y < stack.first()->y && currentCell->x < stack.first()->x)
                {
                    travelDiagonalMM = true;
                    travelDiagonalMP = false;
                    travelDiagonalPP = false;
                    travelDiagonalPM = false;
                }
                else if(currentCell->y < stack.first()->y && currentCell->x > stack.first()->x)
                {
                    travelDiagonalMM = false;
                    travelDiagonalMP = false;
                    travelDiagonalPP = false;
                    travelDiagonalPM = true;
                }
                else if(currentCell->y != stack.first()->y && currentCell->x != stack.first()->x)
                {
                    travelDiagonalMM = false;
                    travelDiagonalMP = true;
                    travelDiagonalPP = false;
                    travelDiagonalPM = false;
                }



                if((prevTravelDiagonalMM != travelDiagonalMM) || (prevTravelDiagonalMP != travelDiagonalMP) ||
                   (prevTravelDiagonalPP != travelDiagonalPP) || (prevTravelDiagonalPM != travelDiagonalPM))
                {
                    turnCount45 += 2;
                }
            }

            if(travelAlongX && currentCell->x == stack.first()->x)
            {
                turnCount90++;
                travelAlongX = false;
                travelAlongY = true;
                travelDiagonal = false;
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
            else if(travelAlongY && currentCell->y == stack.first()->y)
            {
                turnCount90++;
                travelAlongX = true;
                travelAlongY = false;
                travelDiagonal = false;
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
            else if(travelDiagonal && ((currentCell->y == stack.first()->y) || (currentCell->x == stack.first()->x)))
            {
                //turnCount45++;
                travelDiagonal = false;

                if(currentCell->y == stack.first()->y) travelAlongX = false;
                    else if(currentCell->x == stack.first()->x) travelAlongY = false;
            }

            if(travelAlongX || travelAlongY)
            {
                travelStraightCounter++;

                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
                else if(travelDiagonal) travelDiagonalCounter++;
        }



        if(showSearching) ui->graphicsView->repaint();
     }


     UPDATE_PATH_COUNT(countPath, travelStraightCounter, travelDiagonalCounter);
     UPDATE_TURN_COUNT(turnCount90, turnCount45);
     UPDATE_RUN_TIME(((turnCount90 * turnTime90) + (turnCount45 * turnTime45)) + ((travelStraightCounter * oneCellForwardTime) + (travelDiagonalCounter * oneCellDiagonalTime)));

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

        if(!autoSearch)
        {
        QEventLoop loop;
        connect(ui->pushButtonNextStep, &QPushButton::pressed, &loop, &QEventLoop::quit);
        loop.exec();
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

    /*
     * x----------x----------x----------x
     * |          |          |          |
     * |[j-1][i-1]| [j-1][i] |[j-1][i+1]|
     * |          |          |          |                        N
     * x----------x----------x----------x                   x----------x
     * |          |          |          |                   |          |
     * | [j][i-1] |  [j][i]  | [j][i+1] |                 W |          | E
     * |          |          |          |                   |          |
     * x----------x----------x----------x                   x----------x
     * |          |          |          |                        S
     * |[j+1][i-1]| [j+1][i] |[j+1][i+1]|
     * |          |          |          |
     * x----------x----------x----------x
     * */

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

    if(allowDiagonal)
    {
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited &&     // left top corner
                ((!cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH()) ||
                 (!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited &&   // right top corner
                ((!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST()) ||
                 (!cells[j][i]->IS_WALL_EAST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_SOUTH())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && // right bottom corner
                ((!cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH()) ||
                 (!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())))
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited &&   // left bottom corner
                ((!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST()) ||
                 (!cells[j][i]->IS_WALL_WEST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_NORTH())))
        {
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

    int turnCount90 = 0;
    int turnCount45 = 0;

    int travelStraightCounter = 0;
    int travelDiagonalCounter = 0;

    bool travelAlongX = false;
    bool travelAlongY = false;
    bool travelDiagonal = false;

    bool travelDiagonalMM = false;
    bool travelDiagonalMP = false;
    bool travelDiagonalPP = false;
    bool travelDiagonalPM = false;

    bool prevTravelDiagonalMM = false;
    bool prevTravelDiagonalMP = false;
    bool prevTravelDiagonalPP = false;
    bool prevTravelDiagonalPM = false;

    int countCell = 0;
    int countPath = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() != Qt::lightGray) countCell++;
        }
    }

    UPDATE_CELL_COUNT(countCell);
    ui->groupBoxSearchInfo->setEnabled(true);

    while(currentCell->index != startCell->index)
    {
        path.append(*currentCell);
        currentCell->type = CELL_PATH;
        currentCell->SET_BRUSH();

        if(currentCell->x == currentCell->parent->x && !travelAlongX && !travelAlongY)
        {
            travelAlongY = true;
            travelDiagonal = false;
        }
        else if(currentCell->y == currentCell->parent->y && !travelAlongX && !travelAlongY)
        {
            travelAlongX = true;
            travelDiagonal = false;
        }
        else if((currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x) && !travelDiagonal)
        {
            travelDiagonal = true;
            travelAlongY = false;
            travelAlongX = false;
        }

        if(travelDiagonal)
        {
            prevTravelDiagonalMM = travelDiagonalMM;
            prevTravelDiagonalMP = travelDiagonalMP;
            prevTravelDiagonalPP = travelDiagonalPP;
            prevTravelDiagonalPM = travelDiagonalPM;

            if(currentCell->y > currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = true;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x < currentCell->parent->x)
            {
                travelDiagonalMM = true;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = true;
            }
            else if(currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = true;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }



            if((prevTravelDiagonalMM != travelDiagonalMM) || (prevTravelDiagonalMP != travelDiagonalMP) ||
               (prevTravelDiagonalPP != travelDiagonalPP) || (prevTravelDiagonalPM != travelDiagonalPM))
            {
                turnCount45 += 2;
            }
        }

        if(travelAlongX && currentCell->x == currentCell->parent->x)
        {
            turnCount90++;
            travelAlongX = false;
            travelAlongY = true;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelAlongY && currentCell->y == currentCell->parent->y)
        {
            turnCount90++;
            travelAlongX = true;
            travelAlongY = false;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelDiagonal && ((currentCell->y == currentCell->parent->y) || (currentCell->x == currentCell->parent->x)))
        {
           // turnCount45++;
            travelDiagonal = false;

            if(currentCell->y == currentCell->parent->y) travelAlongX = false;
                else if(currentCell->x == currentCell->parent->x) travelAlongY = false;
        }

        currentCell = currentCell->parent;

        if(travelAlongX || travelAlongY)
        {
            travelStraightCounter++;

            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
            else if(travelDiagonal) travelDiagonalCounter++;

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
    UPDATE_PATH_COUNT(countPath, travelStraightCounter, travelDiagonalCounter);
    UPDATE_TURN_COUNT(turnCount90, turnCount45);
    //UPDATE_RUN_TIME(((turnCount90 * turnTime90) + (turnCount45 * turnTime45)) + ((countPath - (turnCount90 + turnCount45)) * oneCellForwardTime));
    UPDATE_RUN_TIME(((turnCount90 * turnTime90) + (turnCount45 * turnTime45)) + ((travelStraightCounter * oneCellForwardTime) + (travelDiagonalCounter * oneCellDiagonalTime)));
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

        if(!autoSearch)
        {
        QEventLoop loop;
        connect(ui->pushButtonNextStep, &QPushButton::pressed, &loop, &QEventLoop::quit);
        loop.exec();
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
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited &&     // left top corner
                ((!cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH()) ||
                 (!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited &&   // right top corner
                ((!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST()) ||
                 (!cells[j][i]->IS_WALL_EAST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_SOUTH())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && // right bottom corner
                ((!cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH()) ||
                 (!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())))
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited &&   // left bottom corner
                ((!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST()) ||
                 (!cells[j][i]->IS_WALL_WEST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_NORTH())))
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

    int turnCount90 = 0;
    int turnCount45 = 0;
    bool travelAlongX = false;
    bool travelAlongY = false;

    bool travelDiagonal = false;

    bool travelDiagonalMM = false;
    bool travelDiagonalMP = false;
    bool travelDiagonalPP = false;
    bool travelDiagonalPM = false;

    bool prevTravelDiagonalMM = false;
    bool prevTravelDiagonalMP = false;
    bool prevTravelDiagonalPP = false;
    bool prevTravelDiagonalPM = false;

    int countCell = 0;
    int countPath = 0;
    int travelStraightCounter = 0;
    int travelDiagonalCounter = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() != Qt::lightGray) countCell++;
        }
    }

    UPDATE_CELL_COUNT(countCell);
    ui->groupBoxSearchInfo->setEnabled(true);

    while(currentCell->index != startCell->index)
    {
        path.append(*currentCell);
        currentCell->type = CELL_PATH;
        currentCell->SET_BRUSH();

        if(currentCell->x == currentCell->parent->x && !travelAlongX && !travelAlongY)
        {
            travelAlongY = true;
            travelDiagonal = false;
        }
        else if(currentCell->y == currentCell->parent->y && !travelAlongX && !travelAlongY)
        {
            travelAlongX = true;
            travelDiagonal = false;
        }
        else if((currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x) && !travelDiagonal)
        {
            travelDiagonal = true;
            travelAlongY = false;
            travelAlongX = false;
        }

        if(travelDiagonal)
        {
            prevTravelDiagonalMM = travelDiagonalMM;
            prevTravelDiagonalMP = travelDiagonalMP;
            prevTravelDiagonalPP = travelDiagonalPP;
            prevTravelDiagonalPM = travelDiagonalPM;

            if(currentCell->y > currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = true;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x < currentCell->parent->x)
            {
                travelDiagonalMM = true;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = true;
            }
            else if(currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = true;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }



            if((prevTravelDiagonalMM != travelDiagonalMM) || (prevTravelDiagonalMP != travelDiagonalMP) ||
               (prevTravelDiagonalPP != travelDiagonalPP) || (prevTravelDiagonalPM != travelDiagonalPM))
            {
                turnCount45 += 2;
            }
        }

        if(travelAlongX && currentCell->x == currentCell->parent->x)
        {
            turnCount90++;
            travelAlongX = false;
            travelAlongY = true;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelAlongY && currentCell->y == currentCell->parent->y)
        {
            turnCount90++;
            travelAlongX = true;
            travelAlongY = false;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelDiagonal && ((currentCell->y == currentCell->parent->y) || (currentCell->x == currentCell->parent->x)))
        {
           // turnCount45++;
            travelDiagonal = false;

            if(currentCell->y == currentCell->parent->y) travelAlongX = false;
                else if(currentCell->x == currentCell->parent->x) travelAlongY = false;
        }

        currentCell = currentCell->parent;

        if(travelAlongX || travelAlongY)
        {
            travelStraightCounter++;

            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
            else if(travelDiagonal) travelDiagonalCounter++;

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
    UPDATE_PATH_COUNT(countPath, travelStraightCounter, travelDiagonalCounter);
    UPDATE_TURN_COUNT(turnCount90, turnCount45);
    UPDATE_RUN_TIME(((turnCount90 * turnTime90) + (turnCount45 * turnTime45)) + ((travelStraightCounter * oneCellForwardTime) + (travelDiagonalCounter * oneCellDiagonalTime)));
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

void MainWindow::DJIKSTRA_FIND_PATH(Cell *startCell, Cell *finishCell)
{
    QList<Cell*> openSet;
    QList<Cell*> closeSet;


    startCell->gCost = DJIKSTRA_GET_DISTANCE_BETWEEN_CELLS(*startCell,*startCell);
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
            if(cell->gCost < currentCell->gCost)
            {
                currentCell = cell;
            }
        }

        openSet.removeOne(currentCell);
        closeSet.append(currentCell);
        currentCell->rect->setBrush(Qt::magenta);
        if(showSearching) ui->graphicsView->repaint();

        for(auto neighbour : DJIKSTRA_GET_NEIGHBOURS(currentCell))
        {
            if(closeSet.contains(neighbour))
            {
                continue;
            }

            if(neighbour->index == finishCell->index)
            {
                neighbour->gCost = DJIKSTRA_GET_DISTANCE_BETWEEN_CELLS(*neighbour,*startCell);
                neighbour->gText->setPlainText("g" + QString::number(neighbour->gCost));
                neighbour->parent = currentCell;
                neighbour->rect->setBrush(Qt::yellow);
                DJIKSTRA_GENERATE_PATH(startCell, finishCell);
                return;
            }
                if(!openSet.contains(neighbour))
                {
                neighbour->gCost = DJIKSTRA_GET_DISTANCE_BETWEEN_CELLS(*neighbour,*startCell);
                neighbour->gText->setPlainText("g" + QString::number(neighbour->gCost));
                neighbour->parent = currentCell;
                neighbour->rect->setBrush(Qt::yellow);
                openSet.append(neighbour);
                }

                if(showSearching) ui->graphicsView->repaint();
            }

        if(!autoSearch)
        {
        QEventLoop loop;
        connect(ui->pushButtonNextStep, &QPushButton::pressed, &loop, &QEventLoop::quit);
        loop.exec();
        }
    }
}

QList<Cell *> MainWindow::DJIKSTRA_GET_NEIGHBOURS(Cell *cell)
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

    if(allowDiagonalDjikstra)
    {
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->visited &&     // left top corner
                ((!cells[j][i]->IS_WALL_WEST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_SOUTH()) ||
                 (!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i - 1 < 0) ? 0 : i - 1]);
        }
        if(!cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->visited &&   // right top corner
                ((!cells[j][i]->IS_WALL_NORTH() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST()) ||
                 (!cells[j][i]->IS_WALL_EAST() && !cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_SOUTH())))
        {
            neighbours.append(cells[(j - 1 < 0) ? 0 : j - 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->visited && // right bottom corner
                ((!cells[j][i]->IS_WALL_EAST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_NORTH()) ||
                 (!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]->IS_WALL_WEST())))
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i + 1 > 15) ? 15 : i + 1]);
        }
        if(!cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->visited &&   // left bottom corner
                ((!cells[j][i]->IS_WALL_SOUTH() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_EAST()) ||
                 (!cells[j][i]->IS_WALL_WEST() && !cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]->IS_WALL_NORTH())))
        {
            neighbours.append(cells[(j + 1 > 15) ? 15 : j + 1][(i - 1 < 0) ? 0 : i - 1]);
        }

    }

    if(showSearching) ui->graphicsView->repaint();

    return neighbours;
}

void MainWindow::DJIKSTRA_GENERATE_PATH(Cell *startCell, Cell *finishCell)
{
    QList<Cell> path;
    Cell *currentCell;
    currentCell = finishCell;

    int turnCount90 = 0;
    int turnCount45 = 0;
    bool travelAlongX = false;
    bool travelAlongY = false;

    bool travelDiagonal = false;

    bool travelDiagonalMM = false;
    bool travelDiagonalMP = false;
    bool travelDiagonalPP = false;
    bool travelDiagonalPM = false;

    bool prevTravelDiagonalMM = false;
    bool prevTravelDiagonalMP = false;
    bool prevTravelDiagonalPP = false;
    bool prevTravelDiagonalPM = false;

    int countCell = 0;
    int countPath = 0;
    int travelStraightCounter = 0;
    int travelDiagonalCounter = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(cells[j][i]->rect->brush() != Qt::lightGray) countCell++;
        }
    }

    UPDATE_CELL_COUNT(countCell);
    ui->groupBoxSearchInfo->setEnabled(true);

    while(currentCell->index != startCell->index)
    {
        path.append(*currentCell);
        currentCell->type = CELL_PATH;
        currentCell->SET_BRUSH();

        if(currentCell->x == currentCell->parent->x && !travelAlongX && !travelAlongY)
        {
            travelAlongY = true;
            travelDiagonal = false;
        }
        else if(currentCell->y == currentCell->parent->y && !travelAlongX && !travelAlongY)
        {
            travelAlongX = true;
            travelDiagonal = false;
        }
        else if((currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x) && !travelDiagonal)
        {
            travelDiagonal = true;
            travelAlongY = false;
            travelAlongX = false;
        }

        if(travelDiagonal)
        {
            prevTravelDiagonalMM = travelDiagonalMM;
            prevTravelDiagonalMP = travelDiagonalMP;
            prevTravelDiagonalPP = travelDiagonalPP;
            prevTravelDiagonalPM = travelDiagonalPM;

            if(currentCell->y > currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = true;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x < currentCell->parent->x)
            {
                travelDiagonalMM = true;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }
            else if(currentCell->y < currentCell->parent->y && currentCell->x > currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = false;
                travelDiagonalPP = false;
                travelDiagonalPM = true;
            }
            else if(currentCell->y != currentCell->parent->y && currentCell->x != currentCell->parent->x)
            {
                travelDiagonalMM = false;
                travelDiagonalMP = true;
                travelDiagonalPP = false;
                travelDiagonalPM = false;
            }



            if((prevTravelDiagonalMM != travelDiagonalMM) || (prevTravelDiagonalMP != travelDiagonalMP) ||
               (prevTravelDiagonalPP != travelDiagonalPP) || (prevTravelDiagonalPM != travelDiagonalPM))
            {
                turnCount45 += 2;
            }
        }

        if(travelAlongX && currentCell->x == currentCell->parent->x)
        {
            turnCount90++;
            travelAlongX = false;
            travelAlongY = true;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelAlongY && currentCell->y == currentCell->parent->y)
        {
            turnCount90++;
            travelAlongX = true;
            travelAlongY = false;
            travelDiagonal = false;
            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
        else if(travelDiagonal && ((currentCell->y == currentCell->parent->y) || (currentCell->x == currentCell->parent->x)))
        {
           // turnCount45++;
            travelDiagonal = false;

            if(currentCell->y == currentCell->parent->y) travelAlongX = false;
                else if(currentCell->x == currentCell->parent->x) travelAlongY = false;
        }

        currentCell = currentCell->parent;

        if(travelAlongX || travelAlongY)
        {
            travelStraightCounter++;

            travelDiagonalMM = false;
            travelDiagonalMP = false;
            travelDiagonalPP = false;
            travelDiagonalPM = false;
        }
            else if(travelDiagonal) travelDiagonalCounter++;

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
    UPDATE_PATH_COUNT(countPath, travelStraightCounter, travelDiagonalCounter);
    UPDATE_TURN_COUNT(turnCount90, turnCount45);
    UPDATE_RUN_TIME(((turnCount90 * turnTime90) + (turnCount45 * turnTime45)) + ((travelStraightCounter * oneCellForwardTime) + (travelDiagonalCounter * oneCellDiagonalTime)));
}

int MainWindow::DJIKSTRA_GET_DISTANCE_BETWEEN_CELLS(Cell cellA, Cell cellB)
{
    int distanceX = abs(cellA.x - cellB.x) / 50;
    int distanceY = abs(cellA.y - cellB.y) / 50;

    switch (heuristicTypeDjikstra)
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

void MainWindow::UPDATE_CELL_COUNT(int count)
{
    ui->labelCellCount->setText(QString::number(count));
}

void MainWindow::UPDATE_PATH_COUNT(int count, int countStraight, int countDiagonal)
{
    ui->labelPathCount->setText(QString::number(count));
    ui->labelPathCountStraight->setText(QString::number(countStraight));
    ui->labelPathCountDiagonal->setText(QString::number(countDiagonal));
}

void MainWindow::UPDATE_TURN_COUNT(int count90, int count45)
{
    ui->labelTurnCount90->setText(QString::number(count90));
    ui->labelTurnCount45->setText(QString::number(count45));
}

void MainWindow::UPDATE_RUN_TIME(float time)
{
     ui->labelRunTime->setText(QString::number(time) + "s");
}

void MainWindow::RESTART_SEARCH_COUNTS()
{
    ui->labelCellCount->setText("0");
    ui->labelPathCount->setText("0");
    ui->labelPathCountStraight->setText("0");
    ui->labelPathCountDiagonal->setText("0");
    ui->labelTurnCount90->setText("0");
    ui->labelTurnCount45->setText("0");
    ui->labelRunTime->setText("0");
    ui->groupBoxSearchInfo->setEnabled(false);
}


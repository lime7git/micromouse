#include "mainwindow.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"

#include <QStack>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonTest, SIGNAL(clicked()), this, SLOT(pushButtonTest_clicked()));
    connect(ui->pushButtonClearWalls, SIGNAL(clicked()), this, SLOT(pushButtonClearWalls_clicked()));
    connect(ui->pushButtonGenerate, SIGNAL(clicked()), this, SLOT(pushButtonGenerate_clicked()));

    MAP_INIT_16x16();
    cell_start_conut = 0;
    cell_finish_count = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButtonTest_clicked()
{
    cell_start_conut = 0;
    cell_finish_count = 0;
    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->type = CELL_NULL;
            if(cells[j][i]->index == 0) cells[j][i]->type = CELL_START;
            if(cells[j][i]->index == 119) cells[j][i]->type = CELL_FINISH;
            if(cells[j][i]->index == 120) cells[j][i]->type = CELL_FINISH;
            if(cells[j][i]->index == 135) cells[j][i]->type = CELL_FINISH;
            if(cells[j][i]->index == 136) cells[j][i]->type = CELL_FINISH;
            cells[j][i]->SET_BRUSH();
        }
    }
    cell_start_conut = 1;
    cell_finish_count = 4;

    SOLVE_FLOOD_FILL(0,0);
}

void MainWindow::pushButtonClearWalls_clicked()
{
    MAP_CLEAR();
}

void MainWindow::pushButtonGenerate_clicked()
{
    MAP_CLEAR();
    MAP_GENERATE_ITERATIVE(0,0);
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
                    qDebug() << "Solver index : " << cells[j][i]->solver_index << Qt::endl;
               }
            }
        }

    }
}


void MainWindow::MAP_INIT_16x16()
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
}

void MainWindow::SOLVE_FLOOD_FILL(unsigned int j, unsigned int i)
{
    unsigned int current_cell_index;

    cells[j][i]->solver_index = 0;
    current_cell_index = cells[j][i]->index;

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            cells[j][i]->visited = false;
        }
    }

    for(int i=0;i<16;i++)
    {
        for(int j=0;j<16;j++)
        {
            if(current_cell_index == cells[j][i]->index)
            {
                if(!cells[j][(i - 1 < 0) ? 0 : i - 1]->visited && !cells[j][i]->IS_WALL_WEST())
                {
                    cells[j][(i - 1 < 0) ? 0 : i - 1]->solver_index = cells[j][i]->solver_index + 1;
                    cells[j][(i - 1 < 0) ? 0 : i - 1]->visited = true;
                    cells[j][(i - 1 < 0) ? 0 : i - 1]->rect->setBrush(Qt::yellow);
                }
                if(!cells[j][(i + 1 > 15) ? 15 : i + 1]->visited && !cells[j][i]->IS_WALL_EAST())
                {
                    cells[j][(i + 1 > 15) ? 15 : i + 1]->solver_index = cells[j][i]->solver_index + 1;
                    cells[j][(i + 1 > 15) ? 15 : i + 1]->visited = true;
                    cells[j][(i + 1 > 15) ? 15 : i + 1]->rect->setBrush(Qt::yellow);
                }
                if(!cells[(j - 1 < 0) ? 0 : j - 1][i]->visited && !cells[j][i]->IS_WALL_NORTH())
                {
                    cells[(j - 1 < 0) ? 0 : j - 1][i]->solver_index = cells[j][i]->solver_index + 1;
                    cells[(j - 1 < 0) ? 0 : j - 1][i]->visited = true;
                    cells[(j - 1 < 0) ? 0 : j - 1][i]->rect->setBrush(Qt::yellow);
                }
                if(!cells[(j + 1 > 15) ? 15 : j + 1][i]->visited && !cells[j][i]->IS_WALL_SOUTH())
                {
                    cells[(j + 1 > 15) ? 15 : j + 1][i]->solver_index = cells[j][i]->solver_index + 1;
                    cells[(j + 1 > 15) ? 15 : j + 1][i]->visited = true;
                    cells[(j + 1 > 15) ? 15 : j + 1][i]->rect->setBrush(Qt::yellow);
                }
            }
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


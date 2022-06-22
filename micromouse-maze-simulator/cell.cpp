#include "cell.h"
#include "qbrush.h"
#include "qgraphicsscene.h"

Cell::Cell()
{

}

Cell::Cell(int x, int y, unsigned int index, QGraphicsScene *scene)
{
    QBrush wallBrush(Qt::black);
    QBrush postBrush(Qt::black);
    QBrush nullBrush(Qt::lightGray);
    QPen outlinePen(Qt::transparent);
    QPen postOutlinePen(Qt::transparent);

    rect = scene->addRect(x + POST_WIDTH, y + POST_HEIGHT, CELL_WIDTH - (2 * POST_WIDTH), CELL_HEIGHT - (2 * POST_HEIGHT), outlinePen, nullBrush);
    posts[0] = scene->addRect(x, y, POST_WIDTH, POST_HEIGHT, postOutlinePen, postBrush);
    posts[1] = scene->addRect(x + CELL_WIDTH - POST_WIDTH, y, POST_WIDTH, POST_HEIGHT, postOutlinePen, postBrush);
    posts[2] = scene->addRect(x, y + CELL_HEIGHT - POST_HEIGHT, POST_WIDTH, POST_HEIGHT, postOutlinePen, postBrush);
    posts[3] = scene->addRect(x + CELL_WIDTH - POST_WIDTH, y + CELL_HEIGHT - POST_HEIGHT, POST_WIDTH, POST_HEIGHT, postOutlinePen, postBrush);

    wallNorth   = scene->addRect(x + POST_WIDTH, y, WALL_HORIZONTAL_WIDTH, WALL_HORIZONTAL_HEIGHT, outlinePen, wallBrush);
    wallEast    = scene->addRect(x + CELL_WIDTH - POST_WIDTH, y + POST_HEIGHT, WALL_VERTICAL_WIDTH, WALL_VERTICAL_HEIGHT, outlinePen, wallBrush);
    wallSouth   = scene->addRect(x + POST_WIDTH, y + CELL_HEIGHT - POST_HEIGHT, WALL_HORIZONTAL_WIDTH, WALL_HORIZONTAL_HEIGHT, outlinePen, wallBrush);
    wallWest    = scene->addRect(x, y + POST_HEIGHT, WALL_VERTICAL_WIDTH, WALL_VERTICAL_HEIGHT, outlinePen, wallBrush);

    wallNorth->setVisible(false);
    wallEast->setVisible(false);
    wallSouth->setVisible(false);
    wallWest->setVisible(false);

    cellIndexText = scene->addText(QString::number(index));
    cellIndexText->setScale(0.8);
    cellIndexText->setPos(x + POST_WIDTH, y + POST_HEIGHT);

    solverIndexText = scene->addText("");
    solverIndexText->setScale(0.8);
    solverIndexText->setDefaultTextColor(Qt::green);
    QFont Font = solverIndexText->font();
    Font.setBold(true);
    solverIndexText->setFont(Font);
    solverIndexText->setPos(x + POST_WIDTH, y + (POST_HEIGHT * 3));

    gText = scene->addText("");
    gText->setScale(0.65);
    gText->setDefaultTextColor(Qt::blue);
    gText->setFont(Font);
    gText->setPos(x + POST_WIDTH, y + (POST_HEIGHT * 6));

    hText = scene->addText("");
    hText->setScale(0.65);
    hText->setDefaultTextColor(Qt::blue);
    hText->setFont(Font);
    hText->setPos(x + (POST_WIDTH * 6), y + (POST_HEIGHT * 6));

    this->index = index;
    solver_index = 0;
    type = CELL_NULL;
    visited = false;
    this->x = x;
    this->y = y;
    hCost = std::numeric_limits<int>::max();
    gCost = std::numeric_limits<int>::max();
    walls = 0; 
}

int Cell::get_fCost()
{
    return gCost + hCost;
}

bool Cell::IS_WALL_NORTH()
{
    return ((walls & NORTH) != 0);
}

bool Cell::IS_WALL_EAST()
{
    return ((walls & EAST) != 0);
}

bool Cell::IS_WALL_SOUTH()
{
    return ((walls & SOUTH) != 0);
}

bool Cell::IS_WALL_WEST()
{
    return ((walls & WEST) != 0);
}

void Cell::SET_BRUSH()
{
    QBrush nullBrush(Qt::lightGray);
    QBrush startBrush(Qt::green);
    QBrush finishBrush(Qt::red);
    QBrush pathBrush(Qt::darkGreen);

    switch(type)
    {
        case CELL_NULL      :   rect->setBrush(nullBrush);      break;
        case CELL_START     :   rect->setBrush(startBrush);     break;
        case CELL_FINISH    :   rect->setBrush(finishBrush);    break;
        case CELL_PATH      :   rect->setBrush(pathBrush);      break;
    }
}

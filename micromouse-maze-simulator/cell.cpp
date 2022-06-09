#include "cell.h"
#include "qbrush.h"

Cell::Cell()
{
    QBrush wallBrush(Qt::darkGray);
    QBrush postBrush(Qt::black);

    wallNorth->setBrush(wallBrush);
    wallEast->setBrush(wallBrush);
    wallSouth->setBrush(wallBrush);
    wallWest->setBrush(wallBrush);

    posts[0]->setBrush(postBrush);
    posts[1]->setBrush(postBrush);
    posts[2]->setBrush(postBrush);
    posts[3]->setBrush(postBrush);



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

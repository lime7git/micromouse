#ifndef CELL_H
#define CELL_H

#include <QGraphicsRectItem>


typedef enum
{
    CELL_NULL = 0,
    CELL_START,
    CELL_FINISH,
    CELL_PATH,
} eCELL_TYPE;

typedef enum
{
    CELL_OPENED = 0,
    CELL_CLOSED,
} eCELL_STATUS;

#define  WEST    1    // binary 0000 0001
#define  SOUTH   2    // binary 0000 0010
#define  EAST    4    // binary 0000 0100
#define  NORTH   8    // binary 0000 1000

class Cell
{
public:
    Cell();

    QGraphicsRectItem *rect;
    QGraphicsRectItem *wallNorth;
    QGraphicsRectItem *wallEast;
    QGraphicsRectItem *wallSouth;
    QGraphicsRectItem *wallWest;
    QGraphicsRectItem *posts[4];
    eCELL_TYPE type;
    eCELL_STATUS status;

    unsigned int index;
    bool visited;
    int x;
    int y;
    unsigned __int8 walls;

    bool IS_WALL_NORTH(void);
    bool IS_WALL_EAST(void);
    bool IS_WALL_SOUTH(void);
    bool IS_WALL_WEST(void);

    void SET_BRUSH();

};

#endif // CELL_H

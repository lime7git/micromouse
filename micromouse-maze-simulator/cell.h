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

#define CELL_WIDTH  30
#define CELL_HEIGHT 30

#define POST_WIDTH  5
#define POST_HEIGHT 5

#define WALL_HORIZONTAL_WIDTH   20
#define WALL_HORIZONTAL_HEIGHT  5

#define WALL_VERTICAL_WIDTH   5
#define WALL_VERTICAL_HEIGHT  20

class Cell
{
public:
    Cell(int x, int y, QGraphicsScene *scene);

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

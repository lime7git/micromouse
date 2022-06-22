#ifndef CELL_H
#define CELL_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QList>


typedef enum
{
    CELL_NULL = 0,
    CELL_START,
    CELL_FINISH,
    CELL_PATH,
} eCELL_TYPE;

#define  WEST    1    // binary 0000 0001
#define  SOUTH   2    // binary 0000 0010
#define  EAST    4    // binary 0000 0100
#define  NORTH   8    // binary 0000 1000

#define CELL_WIDTH  55
#define CELL_HEIGHT 55

#define POST_WIDTH  5
#define POST_HEIGHT 5

#define WALL_HORIZONTAL_WIDTH   (CELL_WIDTH - (2 * POST_WIDTH))
#define WALL_HORIZONTAL_HEIGHT  POST_HEIGHT

#define WALL_VERTICAL_WIDTH   POST_WIDTH
#define WALL_VERTICAL_HEIGHT  (CELL_HEIGHT - (2 * POST_HEIGHT))

class Cell
{
public:
    Cell();
    Cell(int x, int y, unsigned int index, QGraphicsScene *scene);

       bool operator==(const Cell& other)const
       {
           if(this->index==(other.index))
           {
               return true;
           }
           return false;
       }

    QGraphicsRectItem *rect;
    QGraphicsRectItem *wallNorth;
    QGraphicsRectItem *wallEast;
    QGraphicsRectItem *wallSouth;
    QGraphicsRectItem *wallWest;
    QGraphicsRectItem *posts[4];
    QGraphicsTextItem *cellIndexText;
    QGraphicsTextItem *solverIndexText;
    QGraphicsTextItem *gText;
    QGraphicsTextItem *hText;


    eCELL_TYPE type;

    bool visited;
    int x;
    int y;
    unsigned __int8 walls;

    unsigned __int8 index;

    // flood-fill
    unsigned int solver_index;

    // A star

    int gCost;
    int hCost;
    int get_fCost(void);

    Cell *parent;

    bool IS_WALL_NORTH(void);
    bool IS_WALL_EAST(void);
    bool IS_WALL_SOUTH(void);
    bool IS_WALL_WEST(void);

    void SET_BRUSH(void);
};

#endif // CELL_H

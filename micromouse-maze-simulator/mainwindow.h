#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QSerialPort>
#include "qgraphicsitem.h"
#include "cell.h"

typedef enum
{
    ASTAR_MANHATTAN_DISTANCE = 0,
    ASTAR_DIAGONAL_DISTANCE,
    ASTAR_EUCLIDEAN_DISTANE,
} eASTAR_HEURISTIC;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    void pushButtonFloodFill_clicked();
    void pushButtonClearWalls_clicked();
    void pushButtonClearPath_clicked();
    void pushButtonGenerate_clicked();
    void pushButtonAStar_clicked();
    void pushButtonSaveMaze_clicked();
    void pushButtonLoadMaze_clicked();
    void pushButtonSerialConnect_clicked();
    void pushButtonSend_clicked();

    void pushButtonFWD_clicked();
    void pushButtonN_clicked();
    void pushButtonE_clicked();
    void pushButtonS_clicked();
    void pushButtonW_clicked();

    void comboBoxAStar_onChange(void);
    void radioButtonAllowDiagonal_onChange(void);
    void radioButtonAllowAStarBiDirectional_onChange(void);
    void radioButtonAllowFloodFillBiDirectional_onChange(void);
    void checkBoxShowSearching_onChange(void);

    void serialReceived();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void COMBO_BOX_MAZES_UPDATE(void);


private:
    Ui::MainWindow *ui;

    QSerialPort serial;

    QGraphicsScene *scene;
    Cell *cells[16][16];

    bool showSearching;

    void MAP_INIT_16x16();
    void MAP_CLEAR();
    void MAP_GENERATE_ITERATIVE(unsigned int j, unsigned int i);
    void MAP_WALLS_UPDATE();

    int SOLVE_FLOOD_FILL(unsigned int j, unsigned int i, unsigned int finish_cell_first,
                          const std::optional<unsigned int>& finish_cell_second = std::nullopt,
                          const std::optional<unsigned int>& finish_cell_third  = std::nullopt,
                          const std::optional<unsigned int>& finish_cell_fourth = std::nullopt);
    void SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(int j, int i, QStack<Cell*> *stack);
    void SOLVE_FLOOD_GENERATE_PATH(unsigned int finish_index);

    void A_STAR_FIND_PATH(unsigned int start_cell_index, unsigned int finish_cell_index);
    QList<Cell*> A_STAR_GET_NEIGHBOURS(Cell cell);
    void A_STAR_GENERATE_PATH(Cell *startCell, Cell *finishCell);
    int GET_DISTANCE_BETWEEN_CELLS(Cell cellA, Cell cellB);

    QMap<QString, eASTAR_HEURISTIC> aStarHeuristicOptions;

    unsigned int cell_start_conut;
    unsigned int cell_finish_count;

    unsigned int lastStartIndex;
    QList<int> lastFinishIndexs;

    // A star
    eASTAR_HEURISTIC heuristicType;
    bool allowDiagonal;
    bool biDirectionalFloodFill;
    bool biDirectionalAStar;

    int random_in_range(int min, int max);
    int MAP_VALID_INDEX(int index);

};
#endif // MAINWINDOW_H

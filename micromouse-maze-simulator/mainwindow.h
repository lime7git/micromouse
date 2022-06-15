#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QSerialPort>
#include "qgraphicsitem.h"
#include "cell.h"

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
    void pushButtonGenerate_clicked();
    void pushButtonPath_clicked();
    void pushButtonSaveMaze_clicked();
    void pushButtonLoadMaze_clicked();
    void pushButtonSerialConnect_clicked();
    void pushButtonSend_clicked();

    void pushButtonFWD_clicked();
    void pushButtonN_clicked();
    void pushButtonE_clicked();
    void pushButtonS_clicked();
    void pushButtonW_clicked();


    void serialReceived();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;

    QSerialPort serial;

    QGraphicsScene *scene;
    Cell *cells[16][16];

    void MAP_INIT_16x16();
    void MAP_CLEAR();
    void MAP_GENERATE_ITERATIVE(unsigned int j, unsigned int i);
    void MAP_WALLS_UPDATE();

    void SOLVE_FLOOD_FILL(unsigned int j, unsigned int i, unsigned int finish_cell_first,
                          const std::optional<unsigned int>& finish_cell_second = std::nullopt,
                          const std::optional<unsigned int>& finish_cell_third  = std::nullopt,
                          const std::optional<unsigned int>& finish_cell_fourth = std::nullopt);
    void SOLVE_FLOOD_FILL_FILL_NEIGHBOURS(int j, int i, QStack<Cell*> *stack);

    unsigned int cell_start_conut;
    unsigned int cell_finish_count;

    int random_in_range(int min, int max);
    int MAP_VALID_INDEX(int index);

};
#endif // MAINWINDOW_H

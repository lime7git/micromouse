#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
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
    void pushButtonTest_clicked();
    void pushButtonClearWalls_clicked();
    void pushButtonGenerate_clicked();
    void pushButtonPath_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    Cell *cells[16][16];

    void MAP_INIT_16x16();
    void MAP_CLEAR();
    void MAP_GENERATE_ITERATIVE(unsigned int j, unsigned int i);

    void SOLVE_FLOOD_FILL(unsigned int j, unsigned int i);

    unsigned int cell_start_conut;
    unsigned int cell_finish_count;

    int random_in_range(int min, int max);

};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    Cell *cells[16][16];

    unsigned int cellStartIndex;
    unsigned int cellFinishIndex;



};
#endif // MAINWINDOW_H

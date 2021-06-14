#pragma once
#include <QObject>
#include <QPoint>
#include <QTcpSocket>
#include <QRegExp>
#include <QMessageBox>
#include <QFile>
#include <QRegExp>
#include "Model.h"
#include "Utility.h"

class Controller: public QWidget
{
    Q_OBJECT
public:
    int checkPlaceCells = 0;
    bool isEnemyShipPlace = false;

    int myCells = 19;
    int enemyCells = 19;

    int getMyCells()const;
    int getEnemyCells()const;
    void refreshValues();


    Controller( Model* model_ );
    ~Controller();
    void onMousePressed( const QPoint& position, bool setShip = true );
    void clearFields();
    virtual void refresh();
    void operator=(Controller& c);
    void randomField(bool whosShipsToPlase);
    State getState() const;

signals:
    void stateChanged();

private:
    bool isErrorPlacement(QPoint& pos);
    void placeShipAtRandom( int size , bool enemyShipsToPlase);
    void computerStep();
private:
    Model* model;
};

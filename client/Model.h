#pragma once
#include <QString>
#include <QObject>
#include "Field.h"

enum State
{
    ST_PLACING_SHIPS,
    ST_WAITING_ENEMY_STATEMENT,
    ST_MAKING_STEP,
    ST_GAME_END
};

class Model: public QObject
{
    Q_OBJECT
public:
    Model();
    ~Model();
    State getState() const;
    void setState( State );
    Cell getMyCell( int x, int y ) const;
    void setMyCell( int x, int y, Cell cell );
    QString getMyField() const;
    Cell getEnemyCell( int x, int y ) const;
    void setEnemyCell( int x, int y, Cell cell );
    QString getEnemyField() const;
    bool checkMyField() const;
    void clearEnemyField();
    void clearMyField();



    int shipNum( int size ) const;
    bool isShip( int size, int x, int y ) const;
    bool isNear(int x, int y ) const;
    bool isOnDiagonal(int x, int y ) const;
    bool canPlaceNewShip(int x, int y ) const;
    bool canAddDeck(int x, int y ) const;
    bool isEnemyShipDie(int x, int y ) const;
    void setEnemyShipDie(int x, int y);
    bool isMyShipDie(int x, int y ) const;
    void setMyShipDie(int x, int y);
private:

private:
    Field* myField, *enemyField;
    State state;
    QString login, pass;
};

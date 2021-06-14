#include <QAbstractSocket>
#include "Controller.h"

Controller::Controller( Model* model_ ):
    model( model_ )
{
}

Controller::~Controller()
{
}

void Controller::refreshValues()
{
    myCells = 19;
    enemyCells = 19;
}

int Controller::getMyCells()const
{
    return myCells;
}

int Controller::getEnemyCells()const
{
    return enemyCells;
}


void Controller::operator=(Controller& c)
{
    *this = c;
}


void Controller::refresh()
{
    myCells = 19;
    enemyCells = 19;
}

bool Controller::isErrorPlacement(QPoint& point)//Hardcore check ship placing trying
{
    bool isFirst =  (checkPlaceCells==0 || checkPlaceCells==4 ||checkPlaceCells==7 ||
                     checkPlaceCells==10 || checkPlaceCells==12 || checkPlaceCells==14||
                     checkPlaceCells==16|| checkPlaceCells==17|| checkPlaceCells==18 ||
                     checkPlaceCells==19);
    bool canPlaseNew = model->canPlaceNewShip(point.x(), point.y());

    bool canAddNewShip;

    if(isFirst && canPlaseNew)
        canAddNewShip=true;
    else
        canAddNewShip = false;

    bool canAddDeck;
    if(!isFirst && model->canAddDeck(point.x(), point.y()))
        canAddDeck=true;
    else
        canAddDeck=false;

    if(canAddDeck||canAddNewShip)
    {
        return false;
    }
    return true;
}

void Controller::onMousePressed( const QPoint& pos, bool setShip )
{
    if( model->getState() == ST_PLACING_SHIPS )
    {
        QPoint point = getMyFieldCoord( pos );

        if( point.x() == -1 || point.y() == -1 )
            return;

        if(isErrorPlacement(point) == true)
        {
            qDebug() << "Neet to place previos ship";
            return;
        }

        if(checkPlaceCells==19)
        {
            model->setState(ST_WAITING_ENEMY_STATEMENT);
            if(!isEnemyShipPlace)
            {
                randomField(true);
            }
        }


        qDebug() << "Ship at" << point.x() << point.y();

        checkPlaceCells++;
        qDebug() << "Cells count" << checkPlaceCells;

        model->setMyCell( point.x(), point.y(), setShip ? CL_SHIP : CL_CLEAR );
        emit stateChanged();
        return;
    }

    if( model->getState() == ST_MAKING_STEP)
    {
        QPoint point = getEnemyFieldCoord( pos );

        if( point.x() == -1 || point.y() == -1 )
            return;

        qDebug() << "My step at " << point.x() << point.y();
        Cell cell = model->getEnemyCell( point.x(), point.y() );

        if( cell == CL_CLEAR )
        {
            model->setEnemyCell( point.x(), point.y(), CL_DOT );
            //model->setEnemyCell( point.x(), point.y(), CL_SHIP );
            computerStep();
        }
        else if( cell == CL_SHIP )
        {
            model->setEnemyCell( point.x(), point.y(), CL_HALF );
            enemyCells--;
            if(model->isEnemyShipDie(point.x(), point.y()))
            {
                qDebug("Enemy ship is die_________________________________________");
                model->setEnemyShipDie(point.x(), point.y());
            }
        }

        if(enemyCells < 0)
        {
            model->setState(ST_GAME_END);
        }
        return;
    }
}

void Controller::computerStep()
{
    bool isHit = true;
    while(isHit && model->getState()!=ST_GAME_END)
    {
        int x = qrand() % 10;
        int y = qrand() % 10;

        Cell cell = model->getMyCell(x, y);
        qDebug()<<"Computer step at: x-"<<x<<"y-"<<y;

        if( cell == CL_CLEAR )
        {
            isHit=false;
            model->setMyCell(x, y, CL_DOT );
        }
        else if( cell == CL_SHIP )
        {
            model->setMyCell(x, y, CL_HALF );
            if(model->isMyShipDie(x, y))
            {
                qDebug("My ship is die_________________________________________");
                model->setMyShipDie(x, y);
            }
            myCells--;
            if(myCells < 0)
            {
                model->setState(ST_GAME_END);
            }
        }
    }

}

void Controller::randomField(bool enemyShipsToPlase)
{
    if( model->getState() == ST_MAKING_STEP )
        return;

    if(enemyShipsToPlase)
    {
        model->clearEnemyField();
    }
    else
    {
        model->clearMyField();
    }

    for( int i = 1, k = 4; i <= 4; i++, k-- )
        for( int j = 0; j < i; j++ )
            placeShipAtRandom( k , enemyShipsToPlase);
}

void Controller::placeShipAtRandom( int size, bool enemyShipsToPlase)
{
    int p;
    int q;
    bool r;
    bool isOk = true;

    while( isOk )
    {
        p = qrand() % ( 10 - size + 1 );
        q = qrand() % ( 10 - size + 1 );
        r = qrand() % 2;

        for(int k = r * p + !r * q - 1;k < (r * p + !r * q + size + 1);k++)
        {
            if(enemyShipsToPlase)
            {
                if(model->getEnemyCell(r * k + !r * p,r * q + !r * k) == CL_SHIP ||
                    model->getEnemyCell(r * k + !r * (p - 1),r * (q - 1) + !r * k) == CL_SHIP ||
                    model->getEnemyCell(r * k + !r * (p + 1), r * (q + 1) + !r * k) == CL_SHIP)
                    isOk = false;
            }
            else{
                if(model->getMyCell(r * k + !r * p,r * q + !r * k) == CL_SHIP ||
                    model->getMyCell(r * k + !r * (p - 1),r * (q - 1) + !r * k) == CL_SHIP ||
                    model->getMyCell(r * k + !r * (p + 1), r * (q + 1) + !r * k) == CL_SHIP)
                    isOk = false;
            }
        }

        isOk = ! isOk;
    }

    if(enemyShipsToPlase)//placing enemy ships
    {

        for(int k = r * p + !r * q; k < (r * p + !r * q + size);k++)
        {
            qDebug()<<"Enemy ship place in x: " << r * k + !r * p << "y: "<< r * q + !r * k ;
            model->setEnemyCell(r * k + !r * p, r * q + !r * k, CL_SHIP);
        }
        model->setState(ST_MAKING_STEP);
    }
    else//placing my ships
    {
        for(int k = r * p + !r * q; k < (r * p + !r * q + size);k++)
        {
            model->setMyCell(r * k + !r * p, r * q + !r * k, CL_SHIP);
        }
        randomField(true);
        model->setState(ST_MAKING_STEP);
    }
}

void Controller::clearFields()
{
    model->clearEnemyField();
    model->clearMyField();
    model->setState(ST_PLACING_SHIPS);
}

State Controller::getState() const
{
    return model->getState();
}

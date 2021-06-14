#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "Controller.h"

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    qsrand( QTime::currentTime().msec() );
    ui->setupUi( this );
    pictures.load();

    ui->labelStatus->setStyleSheet( "QLabel { color : #00157f; }" );
    model = new Model;
    controller = new Controller( model );
    this->redraw();
}

MainWindow::~MainWindow()
{
    delete controller;
    delete model;
    delete ui;
}

void MainWindow::setStatus( const QString& status )
{
    ui->labelStatus->setText( tr("Status: ") + status );
}

void MainWindow::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );
    const int deltaY = this->centralWidget()->y();

    QPainter painter( this );
    painter.drawImage(
        0,
        deltaY,
        pictures.get("field")
    );

    painter.drawImage( MYFIELD_X, MYFIELD_Y + deltaY, myFieldImage() );
    painter.drawImage( ENEMYFIELD_X, ENEMYFIELD_Y + deltaY, enemyFieldImage() );

    switch(controller->getState())
    {
    case ST_PLACING_SHIPS:
        setStatus("расстановка кораблей");
        break;

    case ST_MAKING_STEP:
        setStatus("сделайте шаг");
        break;

    case ST_WAITING_ENEMY_STATEMENT:
        setStatus("ожидание шага противника");
        break;
    case ST_GAME_END:
        qDebug("Game end!");
        if(controller->getMyCells()==0)
        {
            setStatus("поражение");
        }
        else
        {
            setStatus("победа!!!");
        }

        break;
    }
}

QImage MainWindow::myFieldImage()
{
    return getFieldImage( 0 );
}

QImage MainWindow::enemyFieldImage()
{
    return getFieldImage( 1 );
}

QImage MainWindow::getFieldImage( char fld )
{
    QImage image( FIELD_WIDTH, FIELD_HEIGHT, QImage::Format_ARGB32 );
    Cell cell;
    image.fill( 0 );
    QPainter painter( &image );

    double cfx = 1.0 * FIELD_WIDTH / 10.0;
    double cfy = 1.0 * FIELD_HEIGHT / 10.0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
        {
            if( fld == 0 )
                cell = model->getMyCell( i, j );
            else
                cell = model->getEnemyCell( i, j );

            switch( cell )
            {
            case CL_DOT:
                painter.drawImage( i * cfx, j * cfy, pictures.get("dot") );
                break;

            case CL_HALF:
                painter.drawImage(
                    i * cfx,
                    j * cfy,
                    fld ? pictures.get("half") : pictures.get("half")
                );
                break;

            case CL_SHIP:
                if(!fld)
                {
                    painter.drawImage(
                        i * cfx,
                        j * cfy,
                        pictures.get("full"));
                }
                break;

             case CL_DIED:
                painter.drawImage( i * cfx, j * cfy, pictures.get("hitfull") );
                break;
            default:
                break;
            }
        }

    return image;
}

void MainWindow::mousePressEvent( QMouseEvent* ev )
{
        QPoint pos = ev->pos();
        pos.setY( pos.y() - this->centralWidget()->y() );
        controller->onMousePressed( pos, ev->button() == Qt::LeftButton );
        redraw();
}

void MainWindow::redraw()
{
        this->update();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    close();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionClear_triggered()
{
    controller->clearFields();
    controller->refreshValues();
    this->update();
}

void MainWindow::on_actionRandom_triggered()
{
    controller->randomField(false);
    controller->refreshValues();
    this->update();
}

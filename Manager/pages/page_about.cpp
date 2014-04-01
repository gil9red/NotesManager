#include "page_about.h"
#include "ui_page_about.h"

#include "appinfo.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include "utils/func.h"

Page_About::Page_About( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::Page_About )
{
    ui->setupUi( this );

    const QString & strBuild = getBuildDateTime().toString( "dd.MM.yyyy hh:mm:ss" );
    ui->labelVersion->setText( ui->labelVersion->text().arg( App::version ).arg( strBuild ) );

    QFile file;
    QTextStream in( &file );
    in.setCodec( "utf8" );

    file.setFileName( ":/App/license" );
    if ( file.open( QIODevice::ReadOnly ) )
    {
        ui->licenseText->setHtml( in.readAll() );
        file.close();
        in.flush();
    }

    file.setFileName( ":/App/history_ru" );
    if ( file.open( QIODevice::ReadOnly ) )
    {
        ui->history->setText( in.readAll() );
        file.close();
        in.flush();
    }

//    ui->labelIconProgram->installEventFilter( this );
}

Page_About::~Page_About()
{
    delete ui;
}

void Page_About::setCurrentTab( int index )
{
    ui->tabWidget->setCurrentIndex( index );
}
int Page_About::currentTab()
{
    return ui->tabWidget->currentIndex();
}

//bool Page_About::eventFilter( QObject * object, QEvent * event )
//{
//    if ( object == ui->labelIconProgram )
//    {
//        if ( event->type() == QEvent::MouseButtonDblClick )
//        {
//            qDebug() << "dfdf";
//        }

//        return false;
//    }

//    return QMainWindow::eventFilter( object, event );
//}

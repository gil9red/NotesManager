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

    ui->labelName->setText( ui->labelName->text().arg( App::name ) );
    ui->labelVersion->setText( ui->labelVersion->text().arg( App::version ).arg( tr( "Build" ) + " " + strBuild ) );
    ui->labelDescription->setText( ui->labelDescription->text().arg( tr( "The program creates notes" ) ) );
    ui->labelCopyright->setText( ui->labelCopyright->text().arg( tr( "All rights reserved." ) ) );
    ui->labelTextVersion->setText( ui->labelTextVersion->text().arg( tr( "Version" ) ) );
    ui->labelTextAuthor->setText( ui->labelTextAuthor->text().arg( tr( "Author" ) ) );
    ui->labelTextMail->setText( ui->labelTextMail->text().arg( tr( "Mail" ) ) );
    ui->labelSourceCode->setText( ui->labelSourceCode->text().arg( tr( "Source code" ) ) );


    QFile file;
    QTextStream in( &file );
    in.setCodec( "utf8" );

    file.setFileName( ":/App/license" );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    ui->licenseText->setHtml( in.readAll() );
    file.close();
    in.flush();

    file.setFileName( ":/App/history" );
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    ui->history->setHtml( in.readAll() );
    file.close();

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

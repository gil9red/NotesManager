#include "page_notes.h"

#include <QMenu>
#include <QColorDialog>
#include <QDomDocument>
#include "appinfo.h"
#include "utils/func.h"
#include "noteeditor.h"
#include "RegisterNote.h"
#include "fullscreenshotcropper.h"
#include <QClipboard>
#include <QDesktopWidget>

#include "NavigationPanel/src/hierarchymodel.h"
#include "NavigationPanel/src/notebook.h"

#include "NavigationPanel/src/datenavigationwidget.h"

Page_Notes::Page_Notes( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    settings(0)
{
    ui->setupUi( this );
}
Page_Notes::~Page_Notes()
{
    delete ui;
}

void Page_Notes::setSettings( QSettings * s )
{
    settings = s;
}

void Page_Notes::read( QIODevice * device )
{
    Notebook::instance()->read( device );
    ui->tab_Notes->setModel( Notebook::instance()->hierarchyModel() );
    ui->tab_Tags->SetModel( Notebook::instance()->tagsModel() );
    ui->tab_Dates->SetCreationModel( Notebook::instance()->creationDateModel() );
    ui->tab_Dates->SetModificationModel( Notebook::instance()->modificationDateModel() );      
}
void Page_Notes::write( QIODevice * device )
{
    Notebook::instance()->write( device );
}

void Page_Notes::readSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    ui->splitter->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
    settings->endGroup();


    ui->tabWidget_Navigation->setCurrentWidget( ui->tab_Notes );
}
void Page_Notes::writeSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->endGroup();
    settings->sync();
}

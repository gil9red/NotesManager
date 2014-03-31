#include "page_notes.h"

#include <QMenu>
#include <QColorDialog>
#include <QDomDocument>
#include "appinfo.h"
#include "utils/func.h"
#include "fullscreenshotcropper.h"
#include <QClipboard>
#include <QDesktopWidget>
#include "NavigationPanel/src/hierarchymodel.h"
#include "NavigationPanel/src/datenavigationwidget.h"
#include "NavigationPanel/src/notebook.h"

Page_Notes::Page_Notes( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    settings(0)
{
    ui->setupUi( this );

    QObject::connect(ui->tab_Notes, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));
    QObject::connect(ui->tab_Tags, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));
    QObject::connect(ui->tab_Dates, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));
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
    QDomDocument xmlDomDocument;
    xmlDomDocument.setContent( device );

    // Корнем является тэг Notebook
    QDomElement root = xmlDomDocument.documentElement();

    // Загрузка заметок в модель
    {
        Notebook::instance()->read( root );

        ui->tab_Notes->setModel( Notebook::instance()->hierarchyModel() );
        ui->tab_Tags->SetModel( Notebook::instance()->tagsModel() );
        ui->tab_Dates->SetCreationModel( Notebook::instance()->creationDateModel() );
        ui->tab_Dates->SetModificationModel( Notebook::instance()->modificationDateModel() );
    }

    // Загрузка вкладок
    {
        QDomElement rootTabs = root.firstChildElement( "Tabs" );
        QDomElement tab = rootTabs.firstChildElement();
        while( !tab.isNull() )
        {
            const QString & id = tab.attribute( "id" );
            if ( id.isEmpty() )
            {
                tab = tab.nextSiblingElement();
                continue;
            }

            ui->tabWidget_EditNotes->openNote( Notebook::instance()->getNoteFromId( id ) );
            tab = tab.nextSiblingElement();
        }
        int index = rootTabs.attribute( "current_index", "-1" ).toInt();
        ui->tabWidget_EditNotes->setCurrentIndex( index );
    }
}
void Page_Notes::write( QIODevice * device )
{
    QDomDocument xmlDomDocument;

    // Корень - тэг Notebook
    QDomElement root = xmlDomDocument.createElement( "Notebook" );
    xmlDomDocument.appendChild( root );

    // Создаем xml, в котором будет описано иерархическое дерево
    {
        Notebook::instance()->write( root, xmlDomDocument );
    }

    // Сохранение вкладок
    {
        QDomElement rootTabs = xmlDomDocument.createElement( "Tabs" );
        rootTabs.setAttribute( "current_index", ui->tabWidget_EditNotes->currentIndex() );
        root.appendChild( rootTabs );

        // Делаем перебор всех вкладок
        for ( int index = 0; index < ui->tabWidget_EditNotes->count(); index++ )
        {
            Note * note = ui->tabWidget_EditNotes->note( index );

            // С помощью указателя получаем id заметки, а id заметки на момент написания коммента было названием папки заметки
            const QString & id_name = Notebook::instance()->getIdFromNote( note );

            // Создаем элемент и указываем id
            QDomElement element = xmlDomDocument.createElement( "Note" );
            element.setAttribute( "id", id_name );

            // Добавляем в xml дерево, в узел Tabs
            rootTabs.appendChild( element );
        }
    }

    const int indentSize = 4;
    QTextStream out( device );
    out.setCodec( "UTF-8" );
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlDomDocument.save( out, indentSize );
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

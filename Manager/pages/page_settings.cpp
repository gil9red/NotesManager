#include "page_settings.h"
#include "ui_page_settings.h"
#include "utils/func.h"
#include "utils/texttemplateparser.h"
#include "RichTextNote.h"

#include <QDebug>
#include <QMessageBox>
#include <QFontDialog>


Page_Settings::Page_Settings( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::Page_Settings ),
      settings( 0 )
{
    ui->setupUi( this );
    ui->stackedPages->setCurrentWidget( ui->pageInterface );

    ui->comboBoxLanguage->clear();
    ui->comboBoxLanguage->addItem( getTrDefaultLanguage(), getDefaultLanguage() );

    foreach ( const QFileInfo & fileInfo, QDir( getTrPath() ).entryInfoList( QStringList() << "*.qminfo" ) )
    {
        QSettings ini( fileInfo.absoluteFilePath(), QSettings::IniFormat );
        ini.setIniCodec( "utf8" );

        QString text = ini.value( "Name" ).toString();
        QVariant data = ini.value( "ShortName" );
        QPixmap pixmap;
        pixmap.loadFromData( ini.value( "Icon" ).toByteArray() );

        ui->comboBoxLanguage->addItem( pixmap, text, data );
    }

    ui->sBoxWidth->setMinimum( Note::minimalWidth );
    ui->sBoxHeight->setMinimum( Note::minimalHeight );
    ui->sBoxOpacity->setRange( Note::minimalOpacity * 100, Note::maximalOpacity * 100 );

    const QString & textTemplateParserToolTip = TextTemplateParser::descriptionToHtmlTable();
    ui->lEditTitle->setToolTip( textTemplateParserToolTip );
    ui->tEditText->setToolTip( textTemplateParserToolTip );

    QPalette palette = ui->lEditViewTitle->palette();
    palette.setColor( QPalette::Text, Qt::darkGray );
    ui->lEditViewTitle->setPalette( palette );

    ui->comboBoxActionsDoubleClickOnTitleNote->clear();
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "do nothing" ), Shared::DoNothing );
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "edit the title" ), Shared::EditTitle );
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "hide note" ), Shared::HideNote );
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "delete the note" ), Shared::DeleteNote );
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "save as" ), Shared::SaveAs );
    ui->comboBoxActionsDoubleClickOnTitleNote->addItem( tr( "print notes" ), Shared::PrintNotes );
}

Page_Settings::~Page_Settings()
{
    delete ui;
}

void Page_Settings::setSettings( QSettings * s )
{
    settings = s;
}

void Page_Settings::mapToSettings()
{
    QPoint position = mapSettings.value( "NewNote_Position", QPoint( 100, 100 ) ).toPoint();
    QSize size = mapSettings.value( "NewNote_Size", QSize( 100, 100 ) ).toSize();
    QFont fontTitle;
    fontTitle.fromString( mapSettings.value( "NewNote_FontTitle", QFont().toString() ).toString() );

    int index = ui->comboBoxLanguage->findData( mapSettings.value( "Language", getDefaultLanguage() ) );    
    ui->comboBoxLanguage->setCurrentIndex( index );

    ui->checkBoxAutosave->setChecked( mapSettings.value( "Autosave", true ).toBool() );
    ui->sBoxAutosaveInterval->setValue( mapSettings.value( "AutosaveInterval", 7 ).toInt() );
    ui->cBoxAskBeforeExiting->setChecked( mapSettings.value( "AskBeforeExiting", true ).toBool() );

    ui->cBoxMinimizeTrayWhenClosing->setChecked( mapSettings.value( "MinimizeTrayWhenClosing", false ).toBool() );
    ui->cBoxMinimizeTrayWhenMinimizing->setChecked( mapSettings.value( "MinimizeTrayWhenMinimizing", true ).toBool() );
    ui->cBoxMinimizeTrayOnStartup->setChecked( mapSettings.value( "MinimizeTrayOnStartup", false ).toBool() );

    ui->cBoxRandomColor->setChecked( mapSettings.value( "NewNote_RandomColor", true ).toBool() );
    ui->cBoxRandomPositionOnScreen->setChecked( mapSettings.value( "NewNote_RandomPositionOnScreen", true ).toBool() );
    ui->cBoxVisible->setChecked( mapSettings.value( "NewNote_Visible", true ).toBool() );
    ui->cBoxOnTop->setChecked( mapSettings.value( "NewNote_OnTop", true ).toBool() );
    ui->sBoxWidth->setValue( size.width() );
    ui->sBoxHeight->setValue( size.height() );
    ui->sBoxLeft->setValue( position.x() );
    ui->sBoxTop->setValue( position.y() );
    ui->sBoxOpacity->setValue( mapSettings.value( "NewNote_Opacity", Note::maximalOpacity ).toDouble() * 100 );
    ui->lEditTitle->setText( mapSettings.value( "NewNote_Title", tr( "New note" ) + " %td%" ).toString() );
    ui->lEditTitle->setFont( fontTitle );
    ui->tEditText->setText( mapSettings.value( "NewNote_Text", "" ).toString() );
    ui->tButtonColorTitle->setColor( QColor( mapSettings.value( "NewNote_ColorTitle", QColor( Qt::gray ).name() ).toString() ) );
    ui->tButtonColorBody->setColor( QColor( mapSettings.value( "NewNote_ColorBody", QColor( Qt::lightGray ).name() ).toString() ) );

    ui->checkBoxAutosaveNotes->setChecked( mapSettings.value( "Notes_Autosave", true ).toBool() );
    ui->sBoxAutosaveIntervalNotes->setValue( mapSettings.value( "Notes_AutosaveInterval", 7 ).toInt() );    
    index = ui->comboBoxActionsDoubleClickOnTitleNote->findData( mapSettings.value( "Notes_ActionDoubleClickOnTitle", Shared::EditTitle ) );
    ui->comboBoxActionsDoubleClickOnTitleNote->setCurrentIndex( index );
}
void Page_Settings::settingsToMap()
{
    mapSettings[ "Language" ] = ui->comboBoxLanguage->itemData( ui->comboBoxLanguage->currentIndex() );

    mapSettings[ "Autosave" ] = ui->checkBoxAutosave->isChecked();
    mapSettings[ "AutosaveInterval" ] = ui->sBoxAutosaveInterval->value();
    mapSettings[ "AskBeforeExiting" ] = ui->cBoxAskBeforeExiting->isChecked();

    mapSettings[ "MinimizeTrayWhenClosing" ] = ui->cBoxMinimizeTrayWhenClosing->isChecked();
    mapSettings[ "MinimizeTrayWhenMinimizing" ] = ui->cBoxMinimizeTrayWhenMinimizing->isChecked();
    mapSettings[ "MinimizeTrayOnStartup" ] = ui->cBoxMinimizeTrayOnStartup->isChecked();

    mapSettings[ "NewNote_RandomColor" ] = ui->cBoxRandomColor->isChecked();
    mapSettings[ "NewNote_RandomPositionOnScreen" ] = ui->cBoxRandomPositionOnScreen->isChecked();
    mapSettings[ "NewNote_Visible" ] = ui->cBoxVisible->isChecked();
    mapSettings[ "NewNote_Top" ] = ui->cBoxOnTop->isChecked();
    mapSettings[ "NewNote_Size" ] = QSize( ui->sBoxWidth->value(), ui->sBoxHeight->value() );
    mapSettings[ "NewNote_Position" ] = QPoint( ui->sBoxLeft->value(), ui->sBoxTop->value() );
    mapSettings[ "NewNote_Opacity" ] = ui->sBoxOpacity->value() / 100.0;
    mapSettings[ "NewNote_Title" ] = ui->lEditTitle->text();
    mapSettings[ "NewNote_FontTitle" ] = ui->lEditTitle->font().toString();
    mapSettings[ "NewNote_Text" ] = ui->tEditText->toHtml();
    mapSettings[ "NewNote_ColorTitle" ] = ui->tButtonColorTitle->color().name();
    mapSettings[ "NewNote_ColorBody" ] = ui->tButtonColorBody->color().name();

    mapSettings[ "Notes_Autosave" ] = ui->checkBoxAutosaveNotes->isChecked();
    mapSettings[ "Notes_AutosaveInterval" ] = ui->sBoxAutosaveIntervalNotes->value();
    mapSettings[ "Notes_ActionDoubleClickOnTitle" ] = ui->comboBoxActionsDoubleClickOnTitleNote->itemData( ui->comboBoxActionsDoubleClickOnTitleNote->currentIndex() );
}

QString Page_Settings::getDefaultLanguage()
{
    return "<systems language>";
}
QString Page_Settings::getTrDefaultLanguage()
{
    return "<" + tr( "systems language" ) + ">";
}
QString Page_Settings::getLanguage( QSettings * s )
{
    s->beginGroup( "Page_Settings" );
    const QVariant & value = s->value( "Settings" ).toMap().value( "Language", getDefaultLanguage() );
    s->endGroup();

    return value.toString();
}

void Page_Settings::readSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Settings" );    
    ui->splitter->restoreState( settings->value( "Splitter" ).toByteArray() );
    mapSettings = settings->value( "Settings" ).toMap();
    settings->endGroup();

    mapToSettings();
    RichTextNote::setDefaultSettingsFromMap( mapSettings );
}
void Page_Settings::writeSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Settings" );    
    settings->setValue( "Splitter", ui->splitter->saveState() );
    settings->setValue( "Settings", mapSettings );
    settings->endGroup();
    settings->sync();
}

void Page_Settings::on_treeWidgetMenu_itemClicked(QTreeWidgetItem *item, int column)
{
    const QString & name = item->statusTip( column );

    if ( name == "Interface" )
        ui->stackedPages->setCurrentWidget( ui->pageInterface );

    else if ( name == "System" )
        ui->stackedPages->setCurrentWidget( ui->pageSystem );

    else if ( name == "New note" )
        ui->stackedPages->setCurrentWidget( ui->pageNewNote );

    else if ( name == "Notes" )
        ui->stackedPages->setCurrentWidget( ui->pageNotes );
}

void Page_Settings::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole( button );

    // Если кликнули на кнопку "Принять"
    if ( role == QDialogButtonBox::ApplyRole )
    {
        const QString oldLanguage = mapSettings[ "Language" ].toString();
        settingsToMap();
        const QString newLanguage = mapSettings[ "Language" ].toString();

        // Если был выбран другой язык
        if ( oldLanguage != newLanguage )
            QMessageBox::information( this, tr( "Restart requires " ), tr( "The language change will take effect after a restart" ) );

        RichTextNote::setDefaultSettingsFromMap( mapSettings );

        writeSettings();
        emit acceptChangeSettings();
        emit message( tr( "Settings received and stored" ), 5000 );
    }
}
void Page_Settings::on_tButtonFontTitle_clicked()
{
    bool ok;
    const QFont & font = QFontDialog::getFont( &ok, ui->lEditTitle->font(), this );
    if ( !ok )
        return;

    ui->lEditTitle->setFont( font );
}
void Page_Settings::on_lEditTitle_textChanged( const QString & text )
{
    ui->lEditViewTitle->setText( TextTemplateParser::get( text ) );
}

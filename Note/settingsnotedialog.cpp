#include "settingsnotedialog.h"
#include "ui_settingsnotedialog.h"
#include <QDebug>
#include "NoteShared.h"
#include <QColorDialog>
#include <QFontDialog>

SettingsNoteDialog::SettingsNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsNoteDialog)
{
    ui->setupUi(this);
    ui->opacity->setRange( nm_Note::minimalOpacity * 100, nm_Note::maximalOpacity * 100 );

    setNote(0);
}
SettingsNoteDialog::~SettingsNoteDialog()
{
    delete ui;
}

void SettingsNoteDialog::setNote( AbstractNote * n )
{
    note = n;

    if ( note )
    {
        ui->title->setText( note->title() );

        titleFont = note->titleFont();
        ui->titleFont->setText( titleFont.family() );
        ui->titleColor->setText( note->titleColor().name() );
        ui->windowColor->setText( note->bodyColor().name() );
        ui->opacity->setValue( note->opacity() * 100 );
    }

    bool enabled = note != 0;
    ui->title->setEnabled( enabled );
    ui->titleFont->setEnabled( enabled );
    ui->titleColor->setEnabled( enabled );
    ui->windowColor->setEnabled( enabled );
    ui->opacity->setEnabled( enabled );
    ui->selectTitleColor->setEnabled( enabled );
    ui->selectWindowColor->setEnabled( enabled );
}

void SettingsNoteDialog::accept()
{
    if ( note )
    {
        note->setTitle( ui->title->text() );
        note->setTitleFont( titleFont );
        note->setTitleColor( QColor( ui->titleColor->text() ) );
        note->setBodyColor( QColor( ui->windowColor->text() ) );
        note->setOpacity( ui->opacity->text().toInt() / 100.0 );
    }

    QDialog::accept();
}

void SettingsNoteDialog::on_selectTitleColor_clicked()
{
    const QColor & initColor = QColor( ui->titleColor->text() );
    const QColor & color = QColorDialog::getColor( initColor, this );
    if ( !color.isValid() )
        return;

    ui->titleColor->setText( color.name() );
}
void SettingsNoteDialog::on_selectWindowColor_clicked()
{
    const QColor & initColor = QColor( ui->windowColor->text() );
    const QColor & color = QColorDialog::getColor( initColor, this );
    if ( !color.isValid() )
        return;

    ui->windowColor->setText( color.name() );
}
void SettingsNoteDialog::on_selectTitleFont_clicked()
{
    bool ok;
    const QFont & font = QFontDialog::getFont( &ok, titleFont, this );
    if ( !ok )
        return;

    titleFont = font;
    ui->titleFont->setText( titleFont.family() );
}

/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


#include "page_settings.h"
#include "ui_page_settings.h"

#include <QDebug>

Page_Settings::Page_Settings( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::Page_Settings ),
      settings( 0 )
{
    ui->setupUi( this );
    ui->stackedPages->setCurrentWidget( ui->pageInterface );

    ui->comboBoxLanguage->clear();
    ui->comboBoxLanguage->addItem( getTrDefaultLanguage(), getDefaultLanguage() );
    ui->comboBoxLanguage->addItem( "Ru", "ru" );
    ui->comboBoxLanguage->addItem( "En", "en" );
    ui->comboBoxLanguage->addItem( "Uk", "uk" );
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
    int index = ui->comboBoxLanguage->findData( mapSettings[ "Language" ] );
    if ( index == -1 )
        index = 0; // index == 0 -> "<systems language>"
    ui->comboBoxLanguage->setCurrentIndex( index );

    ui->checkBoxAutosave->setChecked( mapSettings[ "Autosave" ].toBool() );
    ui->sBoxAutosaveInterval->setValue( mapSettings[ "AutosaveInterval" ].toInt() );
    ui->cBoxAskBeforeExiting->setChecked( mapSettings[ "AskBeforeExiting" ].toBool() );

    ui->cBoxMinimizeTrayWhenClosing->setChecked( mapSettings[ "MinimizeTrayWhenClosing" ].toBool() );
    ui->cBoxMinimizeTrayWhenMinimizing->setChecked( mapSettings[ "MinimizeTrayWhenMinimizing" ].toBool() );
    ui->cBoxMinimizeTrayOnStartup->setChecked( mapSettings[ "MinimizeTrayOnStartup" ].toBool() );

    ui->cBoxRandomColor->setChecked( mapSettings[ "NewNote_Color" ].toBool() );
    ui->cBoxRandomPositionOnScreen->setChecked( mapSettings[ "NewNote_RandomPositionOnScreen" ].toBool() );
    ui->cBoxVisible->setChecked( mapSettings[ "NewNote_Visible" ].toBool() );
    ui->cBoxOnTop->setChecked( mapSettings[ "NewNote_OnTop" ].toBool() );

    ui->sBoxWidth->setValue( mapSettings[ "NewNote_Width" ].toInt() );
    ui->sBoxHeight->setValue( mapSettings[ "NewNote_Height" ].toInt() );
    ui->sBoxLeft->setValue( mapSettings[ "NewNote_Left" ].toInt() );
    ui->sBoxTop->setValue( mapSettings[ "NewNote_Top" ].toInt() );
}
void Page_Settings::settingsToMap()
{
    if ( ui->comboBoxLanguage->currentIndex() == 0 )
        mapSettings[ "Language" ] = getDefaultLanguage();
    else
        mapSettings[ "Language" ] = ui->comboBoxLanguage->itemData( ui->comboBoxLanguage->currentIndex() );

    mapSettings[ "Autosave" ] = ui->checkBoxAutosave->isChecked();
    mapSettings[ "AutosaveInterval" ] = ui->sBoxAutosaveInterval->value();
    mapSettings[ "AskBeforeExiting" ] = ui->cBoxAskBeforeExiting->isChecked();

    mapSettings[ "MinimizeTrayWhenClosing" ] = ui->cBoxMinimizeTrayWhenClosing->isChecked();
    mapSettings[ "MinimizeTrayWhenMinimizing" ] = ui->cBoxMinimizeTrayWhenMinimizing->isChecked();
    mapSettings[ "MinimizeTrayOnStartup" ] = ui->cBoxMinimizeTrayOnStartup->isChecked();

    mapSettings[ "NewNote_Color" ] = ui->cBoxRandomColor->isChecked();
    mapSettings[ "NewNote_RandomPositionOnScreen" ] = ui->cBoxRandomPositionOnScreen->isChecked();
    mapSettings[ "NewNote_Visible" ] = ui->cBoxVisible->isChecked();
    mapSettings[ "NewNote_OnTop" ] = ui->cBoxOnTop->isChecked();
    mapSettings[ "NewNote_Width" ] = ui->sBoxWidth->value();
    mapSettings[ "NewNote_Height" ] = ui->sBoxHeight->value();
    mapSettings[ "NewNote_Left" ] = ui->sBoxLeft->value();
    mapSettings[ "NewNote_Top" ] = ui->sBoxTop->value();
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
}

void Page_Settings::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole( button );
    if ( role == QDialogButtonBox::ApplyRole )
    {
        settingsToMap();
        writeSettings();
        emit acceptChangeSettings();
        emit message( tr( "Settings received and stored" ), 5000 );
    }
}

/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
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


#include "findfadlineedit.h"
#include "utils/execmenu.h"

#include <QDebug>

FindFadLineEdit::FindFadLineEdit( const QString & text, QWidget * parent )
    : FadClearLineEdit( text, parent )
{
    init();
}
FindFadLineEdit::FindFadLineEdit( QWidget * parent )
    : FadClearLineEdit( parent )
{
    init();
}

void FindFadLineEdit::setFindOptions( Shared::FindFlags flags )
{
    findFlags = flags;
}
Shared::FindFlags FindFadLineEdit::findOptions()
{
    return findFlags;
}


void FindFadLineEdit::init()
{
    menuOptions = new QMenu( this );
    actionCaseSensitive = new QAction( this );
    actionWholeWords = new QAction( this );
    actionRegExp = new QAction( this );

    menuOptions->addAction( actionCaseSensitive );
    menuOptions->addAction( actionWholeWords );
    menuOptions->addAction( actionRegExp );

    tButtonOptions.setCursor( Qt::ArrowCursor );
    tButtonOptions.setFocusPolicy( Qt::NoFocus );
    tButtonOptions.setStyleSheet( "QToolButton { border: none; }" );
    tButtonOptions.setToolButtonStyle( Qt::ToolButtonIconOnly );
    tButtonOptions.setIconSize( QSize( 17, 17 ) );
    tButtonOptions.setToolTip( tr( "Options" ) );
    tButtonOptions.setStatusTip( tr( "Options" ) );
    tButtonOptions.setWhatsThis( tr( "Options" ) );
    connect( &tButtonOptions, SIGNAL( clicked() ), SLOT( invokeShowOptions() ) );

    addButton( &tButtonOptions, Left );

    actionCaseSensitive->setText( tr( "Case sensitive" ) );
    actionCaseSensitive->setCheckable( true );
    actionCaseSensitive->setIcon( QIcon( ":/casesensitively" ) );

    actionWholeWords->setText( tr( "Whole words" ) );
    actionWholeWords->setCheckable( true );
    actionWholeWords->setIcon( QIcon( ":/wholewords" ) );

    actionRegExp->setText( tr( "Use regular expression" ) );
    actionRegExp->setCheckable( true );
    actionRegExp->setIcon( QIcon( ":/regexp" ) );

    QActionGroup * findActionGroup = new QActionGroup( this );
    findActionGroup->setExclusive( false );
    findActionGroup->addAction( actionCaseSensitive );
    findActionGroup->addAction( actionWholeWords );
    findActionGroup->addAction( actionRegExp );

    connect( findActionGroup, SIGNAL( triggered(QAction*) ), SLOT( invokeFindFlagsChanged() ) );

    updateIcons();
}


void FindFadLineEdit::invokeShowOptions()
{
    Utils::execMenuAtWidget( menuOptions, &tButtonOptions );
}

void FindFadLineEdit::updateIcons()
{
    tButtonOptions.setIcon( Find::iconForFindFlags( findFlags ) );
}
void FindFadLineEdit::invokeFindFlagsChanged()
{
    findFlags = 0;

    if ( actionCaseSensitive->isChecked() )
        findFlags |= Shared::CaseSensitive;

    if ( actionWholeWords->isChecked() )
        findFlags |= Shared::WholeWords;

    if ( actionRegExp->isChecked() )
        findFlags |= Shared::RegExp;

    updateIcons();
    emit findFlagsChanged();
}

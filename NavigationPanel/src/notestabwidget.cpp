/*
This file is part of qNotesManager.

qNotesManager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

qNotesManager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qNotesManager. If not, see <http://www.gnu.org/licenses/>.
*/

#include "notestabwidget.h"
#include "noteeditwidget.h"
#include "note.h"
#include "utils/func.h"

#include <QBoxLayout>
#include <QApplication>
#include <QToolButton>

NotesTabWidget::NotesTabWidget( QWidget * parent )
    : QTabWidget( parent )
{
    QObject::connect(this, SIGNAL(currentChanged(int)), SLOT(sl_TabWidget_CurrentChanged(int)));
    QObject::connect(this, SIGNAL(tabCloseRequested(int)), SLOT(sl_TabWidget_TabCloseRequested(int)));
    setTabsClosable(true);

    // Контекстное меню
    {
        setContextMenuPolicy( Qt::DefaultContextMenu );

        contextMenu = new QMenu();
        QObject::connect( contextMenu, SIGNAL(aboutToShow()), SLOT( sl_UpdateEnabledContextMenu() ) );

        // TODO: this NULL ICON
//        actionHighlightCurrent = contextMenu->addAction( QIcon( "" ), tr( "Highlight the current tab on the tree" ), this, SLOT( sl_HighlightCurrentTabOnTree() ), QKeySequence() );
        contextMenu->addSeparator();
        actionCloseCurrent = contextMenu->addAction( QIcon( "" ), tr( "Close current tab" ), this, SLOT( sl_CloseCurrentTab() ), QKeySequence() );
        actionCloseAll_Except = contextMenu->addAction( QIcon( "" ), tr( "Close all tabs except the current" ), this, SLOT( sl_CloseAllTabsExceptCurrent() ), QKeySequence() );
        contextMenu->addSeparator();
        actionCloseAll_Left = contextMenu->addAction( QIcon( "" ), tr( "Close all tabs to the left of the current" ), this, SLOT( sl_CloseAllTabsToLeftOfCurrent() ), QKeySequence() );
        actionCloseAll_Right = contextMenu->addAction( QIcon( "" ), tr( "Close all tabs to the right of the current" ), this, SLOT( sl_CloseAllTabsToRightOfCurrent() ), QKeySequence() );
        contextMenu->addSeparator();
        actionCloseAll = contextMenu->addAction( QIcon( "" ), tr( "Close all tabs" ), this, SLOT( sl_CloseAllTabs() ), QKeySequence() );


        QToolButton * contextMenuButton = new QToolButton();
        contextMenuButton->setFocusPolicy( Qt::NoFocus );
        contextMenuButton->setMenu( contextMenu );
        contextMenuButton->setPopupMode( QToolButton::InstantPopup );
        setCornerWidget( contextMenuButton, Qt::TopRightCorner );
    }
}

QList < Note * > NotesTabWidget::displayedNotes()
{
    return hashNoteTabs.keys();
}

Note * NotesTabWidget::note( int index )
{
    if ( !count() )
        return 0;

    QWidget * tab = widget( index );
    NoteEditWidget * noteEdit = dynamic_cast < NoteEditWidget * > ( tab );
    if ( !noteEdit )
    {
        WARNING("Casting error");
        return 0;
    }

    return noteEdit->note();
}
Note * NotesTabWidget::currentNote()
{
    return note( currentIndex() );
}
void NotesTabWidget::setCurrentNote(Note* note)
{
    if (!note)
    {
        WARNING("Null pointer recieved");
        return;
    }

    if (hashNoteTabs.contains(note))
    {
        QWidget * noteEdit = hashNoteTabs.value(note);
        setCurrentWidget(noteEdit);
    }
}
QWidget * NotesTabWidget::getWidgetTab( Note * note )
{
    if (!note)
    {
        WARNING("Null pointer recieved");
        return 0;
    }

    return hashNoteTabs.value( note, 0 );
}

void NotesTabWidget::openNote(Note* note)
{
    if (!note)
    {
		WARNING("Null pointer recieved");
		return;
	}

    if (hashNoteTabs.contains(note))
    {
        QWidget * noteEdit = hashNoteTabs.value(note);
        setCurrentWidget(noteEdit);
		return;
	}

    qApp->setOverrideCursor( Qt::WaitCursor );
    NoteEditWidget * noteEdit = new NoteEditWidget( this );
    noteEdit->setNote( note );
    qApp->restoreOverrideCursor();

    hashNoteTabs.insert(note, noteEdit);
    QObject::connect( note, SIGNAL(sg_VisualPropertiesChanged()), SLOT(sl_Note_PropertiesChanged()) );
    QObject::connect( note, SIGNAL(sg_Removed()), SLOT(sl_AboutRemoveNote()) );

    int index = addTab( noteEdit, note->getIcon(), cropString( note->getName() ) );
    setCurrentIndex( index );
}
void NotesTabWidget::closeNote(Note* n)
{
    if (!n)
    {
		WARNING("Null pointer recieved");
		return;
	}

    if (!hashNoteTabs.contains(n))
    {
		WARNING("Specified note is not open");
		return;
	}

    QWidget * tab = hashNoteTabs.take(n);
    if ( !tab )
    {
        WARNING("Null pointer recieved");
        return;
    }

    int tabIndex = indexOf(tab);
    if (tabIndex == -1)
    {
		WARNING("Could not find associated widget");
		return;
	}

	QObject::disconnect(n, 0, this, 0);
    removeTab(tabIndex);
    delete tab;

    WARNING( qPrintable( QString( "Close tab (%1)" ).arg( (int)n, 0, 16 ) ) );
}
void NotesTabWidget::closeTab(int index)
{
    if (index == -1)
    {
        WARNING("Wrong tab index");
        return;
    }

    QWidget * tab = widget(index);
    if ( !tab )
    {
        WARNING("Can't find widget by index");
        return;
    }

    NoteEditWidget * noteEdit = dynamic_cast < NoteEditWidget * > ( tab );
    Note * note = noteEdit->note();

    closeNote( note );
}
void NotesTabWidget::clear()
{
    while ( hashNoteTabs.count() )
        closeNote( hashNoteTabs.keys().first() );
}

void NotesTabWidget::sl_AboutRemoveNote()
{
    Note * note = qobject_cast < Note * > ( QObject::sender() );
    if ( !note )
    {
        WARNING("Could not find sender note");
        return;
    }
    closeNote( note );
}
void NotesTabWidget::sl_Note_PropertiesChanged()
{
    Note * note = qobject_cast < Note * > ( QObject::sender() );
    if ( !note )
    {
		WARNING("Could not find sender note");
		return;
	}

    if (!hashNoteTabs.contains(note))
    {
		WARNING("Sender note is not registered");
		QObject::disconnect(note, 0, this, 0);
		return;
	}

    QWidget * tab = hashNoteTabs.value(note);
    int tabIndex = indexOf(tab);

    if (tabIndex == -1)
    {
		WARNING("Could not find associated widget");
		return;
	}

    setTabIcon(tabIndex, note->getIcon());
    setTabText(tabIndex, cropString(note->getName()));
}
void NotesTabWidget::sl_TabWidget_CurrentChanged(int index)
{
    if (index == -1)
    {
        emit sg_CurrentNoteChanged(0);
        return;
    }

    QWidget * tab = widget(index);
    NoteEditWidget * noteEdit = dynamic_cast < NoteEditWidget * > ( tab );
    emit sg_CurrentNoteChanged( noteEdit->note() );
}
void NotesTabWidget::sl_TabWidget_TabCloseRequested(int index)
{
	closeTab(index);
}

void NotesTabWidget::sl_CloseAllTabs()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    clear();
    qApp->restoreOverrideCursor();
}
void NotesTabWidget::sl_CloseAllTabsToLeftOfCurrent()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    int index = 0;
    QWidget * tab = widget( index );
    QWidget * currentTab = currentWidget();

    // Пока указатели не совпали и индекс закрываемой вкладки меньше
    // индекса текущей
    while ( tab != currentTab && index < indexOf( currentTab ) )
    {
        closeTab( index );
        tab = widget( index );
    }

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::sl_CloseAllTabsToRightOfCurrent()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    int index = count() - 1;
    QWidget * tab = widget( index );
    QWidget * currentTab = currentWidget();

    // Пока указатели не совпали и индекс закрываемой вкладки меньше
    // индекса текущей
    while ( tab != currentTab && indexOf( currentTab ) < index )
    {
        closeTab( index );
        tab = widget( index );
        index = count() - 1;
    }

    qApp->restoreOverrideCursor();
}
void NotesTabWidget::sl_CloseAllTabsExceptCurrent()
{
    sl_CloseAllTabsToLeftOfCurrent();
    sl_CloseAllTabsToRightOfCurrent();
}
void NotesTabWidget::sl_CloseCurrentTab()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    closeTab( currentIndex() );
    qApp->restoreOverrideCursor();
}
//void NotesTabWidget::sl_HighlightCurrentTabOnTree()
//{

//}

void NotesTabWidget::sl_UpdateEnabledContextMenu()
{
    actionCloseAll_Except->setEnabled( count() != 1 );
    actionCloseAll_Left->setEnabled( currentIndex() > 0 );
    actionCloseAll_Right->setEnabled( currentIndex() < count() - 1 );
    actionCloseAll->setEnabled( count() > 1 );
}

void NotesTabWidget::contextMenuEvent( QContextMenuEvent * event )
{
    // Если вкладок нет, не показываем меню
    if ( count() == 0 )
        return;

    contextMenu->exec( event->globalPos() );
}

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

NotesTabWidget::NotesTabWidget( QWidget * parent )
    : QTabWidget( parent )
{
    QObject::connect(this, SIGNAL(currentChanged(int)), SLOT(sl_TabWidget_CurrentChanged(int)));
    QObject::connect(this, SIGNAL(tabCloseRequested(int)), SLOT(sl_TabWidget_TabCloseRequested(int)));
    setTabsClosable(true);
}

QList<const Note*> NotesTabWidget::displayedNotes() const
{
    return hash.keys();
}

Note * NotesTabWidget::currentNote() const
{
    if ( !count() )
        return 0;

    NoteEditWidget * noteEdit = dynamic_cast < NoteEditWidget * > ( currentWidget() );
    if ( !noteEdit )
    {
        WARNING("Casting error");
        return 0;
    }

    return noteEdit->note();
}
void NotesTabWidget::setCurrentNote(const Note* note)
{
    if (!note)
    {
        WARNING("Null pointer recieved");
        return;
    }

    if (hash.contains(note))
    {
        QWidget * noteEdit = hash.value(note);
        setCurrentWidget(noteEdit);
    }
}

void NotesTabWidget::openNote(Note* note)
{
    if (!note)
    {
		WARNING("Null pointer recieved");
		return;
	}

    if (hash.contains(note))
    {
        QWidget * noteEdit = hash.value(note);
        setCurrentWidget(noteEdit);
		return;
	}

    qApp->setOverrideCursor( Qt::WaitCursor );
    NoteEditWidget * noteEdit = new NoteEditWidget( this );
    noteEdit->setNote( note );
    qApp->restoreOverrideCursor();

    hash.insert(note, noteEdit);
    QObject::connect(note, SIGNAL(sg_VisualPropertiesChanged()), SLOT(sl_Note_PropertiesChanged()));

    int index = addTab( noteEdit, note->GetIcon(), cropString( note->GetName() ) );
    setCurrentIndex( index );
}
void NotesTabWidget::closeNote(const Note* n)
{
    if (!n)
    {
		WARNING("Null pointer recieved");
		return;
	}

    if (!hash.contains(n))
    {
		WARNING("Specified note is not open");
		return;
	}

    QWidget * tab = hash.take(n);
    int tabIndex = indexOf(tab);

    if (tabIndex == -1)
    {
		WARNING("Could not find associated widget");
		return;
	}

	QObject::disconnect(n, 0, this, 0);
    removeTab(tabIndex);
    delete tab;
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
    while ( hash.count() )
        closeNote( hash.keys().first() );
}

void NotesTabWidget::sl_Note_PropertiesChanged()
{
    const Note * note = qobject_cast < Note * > ( QObject::sender() );
    if ( !note )
    {
		WARNING("Could not find sender note");
		return;
	}

    if (!hash.contains(note))
    {
		WARNING("Sender note is not registered");
		QObject::disconnect(note, 0, this, 0);
		return;
	}

    QWidget * tab = hash.value(note);
    int tabIndex = indexOf(tab);

    if (tabIndex == -1)
    {
		WARNING("Could not find associated widget");
		return;
	}

    setTabIcon(tabIndex, note->GetIcon());
    setTabText(tabIndex, cropString(note->GetName()));
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

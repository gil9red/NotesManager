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

#ifndef NOTESTABWIDGET_H
#define NOTESTABWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QHash>
#include <QLayout>
#include <QMenu>

class Note;

class NotesTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit NotesTabWidget( QWidget * parent = 0 );
    QList < const Note * > displayedNotes() const;

    Note * currentNote() const;
    void setCurrentNote( const Note * );

private:
    QHash < const Note *, QWidget * > hash;

public slots:
    void openNote( Note * );
    void closeNote( const Note * );
    void closeTab( int );
    void clear();

private slots:
    void sl_Note_PropertiesChanged();
    void sl_TabWidget_CurrentChanged(int);
    void sl_TabWidget_TabCloseRequested(int);

signals:
    void sg_CurrentNoteChanged(Note*);
};

#endif // NOTESTABWIDGET_H

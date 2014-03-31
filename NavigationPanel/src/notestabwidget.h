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
#include <QToolButton>

class Note;

class NotesTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit NotesTabWidget( QWidget * parent = 0 );
    QList < Note * > displayedNotes();

    Note * note( int );
    Note * currentNote();
    void setCurrentNote(Note *);

private:
    QHash < Note *, QWidget * > hashNoteTabs;
    QMenu * contextMenu;
//    QAction * actionHighlightCurrent;
    QAction * actionCloseCurrent;
    QAction * actionCloseAll_Except;
    QAction * actionCloseAll_Left;
    QAction * actionCloseAll_Right;
    QAction * actionCloseAll;

public slots:
    void openNote( Note * );
    void closeNote(Note * );
    void closeTab( int );
    void clear(); //!< Закрыть все вкладки

private slots:
    void sl_Note_PropertiesChanged();
    void sl_TabWidget_CurrentChanged(int);
    void sl_TabWidget_TabCloseRequested(int);

    void sl_CloseAllTabs(); //!< Закрыть все вкладки
    void sl_CloseAllTabsToLeftOfCurrent(); //!< Закрыть все вкладки слева от текущей
    void sl_CloseAllTabsToRightOfCurrent(); //!< Закрыть все вкладки справа от текущей
    void sl_CloseAllTabsExceptCurrent(); //!< Закрыть все вкладки, кроме текущей
    void sl_CloseCurrentTab(); //!< Закрыть текущую вкладку
//    void sl_HighlightCurrentTabOnTree(); //!< Выделить текущую вкладку на дереве

    void sl_UpdateEnabledContextMenu();

protected:
    void contextMenuEvent( QContextMenuEvent * );

signals:
    void sg_CurrentNoteChanged(Note*);
};

#endif // NOTESTABWIDGET_H

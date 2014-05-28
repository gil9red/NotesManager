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

#ifndef QNM_NOTEMODELITEM_H
#define QNM_NOTEMODELITEM_H

#include "basemodelitem.h"

class Note;

class NoteModelItem : public BaseModelItem
{
    Q_OBJECT

public:
    NoteModelItem( Note * );
    Note * getStoredData() const;

    QVariant data( int role ) const;
    bool setData( const QVariant & value, int role );
    Qt::ItemFlags flags() const;
    bool lessThan( const BaseModelItem * ) const;

private:
    Note * storedData;

private slots:
    void sl_Note_PropertiesChanged();
};

#endif // QNM_NOTEMODELITEM_H
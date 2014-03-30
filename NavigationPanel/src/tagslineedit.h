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

#ifndef TAGSLINEEDIT_H
#define TAGSLINEEDIT_H

#include <QLineEdit>
#include <QList>
#include <QAbstractItemModel>

namespace qNotesManager {
	class TagsLineEdit : public QLineEdit {
	Q_OBJECT

	private:
		QCompleter* completer;
		void editingFinished();

	public:
		explicit TagsLineEdit(QWidget *parent = 0);
		void SetTagsModel(QAbstractItemModel*);

	protected:
		virtual void focusOutEvent (QFocusEvent* event);

	signals:
		void sg_TagsCollectionChanged(QStringList);

	private slots:
		void sl_textEdited (const QString & text);
		void sl_Completer_Activated ( const QString & text );

	};
}

#endif // TAGSLINEEDIT_H

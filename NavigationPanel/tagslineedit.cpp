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

#include "tagslineedit.h"

#include <QCompleter>
#include <QListView>
#include <QRect>
#include <QFocusEvent>
#include <QDebug>

using namespace qNotesManager;

TagsLineEdit::TagsLineEdit (QWidget *parent) : QLineEdit(parent) {
	completer = new QCompleter(this);
    QObject::connect(completer, SIGNAL(activated(QString)), SLOT(sl_Completer_Activated(QString)));
	QListView* listView = new QListView();
	completer->setPopup(listView);
	completer->setWidget(this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);

    QObject::connect(this, SIGNAL(textEdited(QString)), SLOT(sl_textEdited(QString)));
}

void TagsLineEdit::SetTagsModel(QAbstractItemModel* model) {
	completer->setModel(model);
}

void TagsLineEdit::editingFinished() {
	QStringList tags;
	qDebug() << "\nTagsLineEdit::EditingFinished";

	QString text = this->text();
	QString temp = "";
	int start = 0;

	while (start <= (text.length() - 1)) {
		int i = text.indexOf(";", start);
		if (i == -1) {
			if (start <= (text.length() - 1)) {
				temp = text.mid(start, text.length() - start).trimmed();
				if (!temp.isEmpty()) {tags.append(temp);}
			}
			break;
		}
		if (i == start) {
			++start;
			continue;
		}
		temp = text.mid(start, i - start).trimmed();
		if (!temp.isEmpty()) {tags.append(temp);}
		start = i + 1;
	}

	emit sg_TagsCollectionChanged(tags);
}

void TagsLineEdit::sl_Completer_Activated (const QString& selectedText) {
	qDebug() << "\nCompleter activated";

	QString text = this->text();
	int current = cursorPosition();
	int end = current;
	int start = current == 0 ? 0 : current - 1;

	// TODO change to text().indexOf(QRegExp) and text().lastIndexOf(QRegExp)
	while(true) {
		if ((start == 0) || text.mid(start - 1, 1 ) == ";" || text.mid(start - 1, 1) == " ") {break;}
		start--;
	}
	while(true) {
		if (end == (text.length()) || text.mid(end + 1, 1) == " " || text.mid(end + 1, 1) == ";") {break;}
		end++;
	}


	setSelection(start, end - start);
	insert(selectedText + "; ");
}

void TagsLineEdit::sl_textEdited (const QString&) {
	// Do not show completer when editing characters in the middle of a word
	qDebug() << "\nTagsLineEdit::sl_textEdited";

	if (cursorPosition() < (text().length() - 1) && (
			text().mid(cursorPosition(), 1) != " "
			||
			text().mid(cursorPosition(), 1) != ";")) {return;}

	QString text = this->text();
	int current = cursorPosition();
	int end = current;
	int start = current == 0 ? 0 : current - 1;

	// TODO change to text().indexOf(QRegExp) and text().lastIndexOf(QRegExp)
	while(true) {
		if ((start == 0) || text.mid(start - 1, 1) == ";") {break;}
		start--;
	}
	while(true) {
		if (end == (text.length()) || text.mid(end + 1, 1) == ";") {break;}
		end++;
	}

	qDebug() << "Text edited. Start index: " << start << ", end index: " << end;

	if (start == end) {return;}


	QString prefix = text.mid(start, end - start).trimmed();
	if (!prefix.isEmpty()) {
		completer->setCompletionPrefix(prefix);
		qDebug() << "Prefix set: " << prefix;
		completer->complete();
	}

}

void TagsLineEdit::focusOutEvent (QFocusEvent * event) {
	QLineEdit::focusOutEvent(event);

	qDebug() << event->reason();
	qDebug() << event->type();

	editingFinished();
}

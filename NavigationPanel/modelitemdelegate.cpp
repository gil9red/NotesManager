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

#include "modelitemdelegate.h"

#include "basemodelitem.h"
#include "utils/func.h"

#include <QPainter>
#include <QItemEditorFactory>



ModelItemDelegate::ModelItemDelegate(QObject *parent) : QItemDelegate(parent) {
	_isEditing = false;
	QObject::connect(this, SIGNAL(closeEditor(QWidget*)),
					 this, SLOT(sl_Delegate_closeEditor(QWidget*)));
}

void ModelItemDelegate::paint (QPainter* painter, const QStyleOptionViewItem& option,
						const QModelIndex& index) const {
	BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
	if (!item) {
		WARNING("Null pointer recieved");
		return;
	}
	if (item->DataType() != BaseModelItem::Separator) {
		QItemDelegate::paint(painter, option, index);
		return;
	}

	const int lineWidth = 2;
	const int horizontalOffset = 5;
	const int verticalOffset = option.rect.height() / 2 + option.rect.height() % 2;

	QLinearGradient gradient(option.rect.x(), 0, option.rect.width(), 0);
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(0.2, Qt::black);
	gradient.setColorAt(0.8, Qt::black);
	gradient.setColorAt(1, Qt::white);
	QBrush lineBrush(gradient);
	QPen linePen(lineBrush, lineWidth);

	const QLine line(option.rect.x() + horizontalOffset,
			   option.rect.y() + verticalOffset,
			   option.rect.x() + option.rect.width() - horizontalOffset,
			   option.rect.y() + verticalOffset);

	painter->save();
		painter->setPen(linePen);
		painter->drawLine(line);
	painter->restore();
}

void ModelItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
	_isEditing = true;
	QItemDelegate::setEditorData(editor, index);
}

bool ModelItemDelegate::isEditing() const {
	return _isEditing;
}

void ModelItemDelegate::sl_Delegate_closeEditor (QWidget*) {
	_isEditing = false;
}

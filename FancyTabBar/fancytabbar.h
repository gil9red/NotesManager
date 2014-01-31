/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef FANCYTABWIDGET_H
#define FANCYTABWIDGET_H

#include <QIcon>
#include <QWidget>

#include <QTimer>
#include <QPropertyAnimation>

class QPainter;

#include "fancytab.h"

namespace TabBarPosition
{
    enum Position { Above, Below, Left, Right };
}
namespace Corner
{
    enum Corner { OutsideBeginning, OutsideEnd, InsideBeginning, InsideEnd };
}

class FancyTabBar : public QWidget
{
    Q_OBJECT

public:
    FancyTabBar(const TabBarPosition::Position position, QWidget *parent = 0);
    ~FancyTabBar();

    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void paintTab(QPainter *painter, int tabIndex) const;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool validIndex(int index) const { return index >= 0 && index < mAttachedTabs.count(); }

    void setOrientation(const TabBarPosition::Position p) {mPosition = p;}
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void insertTab(int index, const QIcon &icon, const QString &label, bool enabled = true )
    {
        FancyTab *tab = new FancyTab(this);
        tab->icon = icon;
        tab->text = label;
        tab->enabled = enabled;
        mAttachedTabs.insert(index, tab);
    }
    void addTab( const QIcon & icon, const QString & label, bool enabled = true )
    {
        FancyTab * tab = new FancyTab( this );
        tab->icon = icon;
        tab->text = label;
        tab->enabled = enabled;
        mAttachedTabs.append( tab );
    }

    void removeTab(int index)
    {
        FancyTab *tab = mAttachedTabs.takeAt(index);
        delete tab;
    }
    void setCurrentIndex(int index);
    int currentIndex() const { return mCurrentIndex; }

    void setTabToolTip(int index, QString toolTip) { mAttachedTabs[index]->toolTip = toolTip; }
    QString tabToolTip(int index) const { return mAttachedTabs.at(index)->toolTip; }

    QIcon tabIcon(int index) const { return mAttachedTabs.at(index)->icon; }
    QString tabText(int index) const { return mAttachedTabs.at(index)->text; }
    int count() const {return mAttachedTabs.count(); }
    QRect tabRect(int index) const;

signals:
    void currentChanged(int);

public slots:
    void emitCurrentIndex();

private:
    QPoint getCorner(const QRect& rect, const Corner::Corner corner) const;
    QRect adjustRect(const QRect& rect, const qint8 offsetOutside, const qint8 offsetInside, const qint8 offsetStart, const qint8 offsetEnd) const;

    // Same with a point. + means towards Outside/End, - means towards Inside/Beginning
    QPoint adjustPoint(const QPoint& point, const qint8 offsetInsideOutside, const qint8 offsetBeginningEnd) const;

    TabBarPosition::Position mPosition;
    static const int m_rounding;
    static const int m_textPadding;
    QRect mHoverRect;
    int mHoverIndex;
    int mCurrentIndex;
    QList<FancyTab*> mAttachedTabs;
    QTimer mTimerTriggerChangedSignal;
    QSize tabSizeHint(bool minimum = false) const;
};

#endif // FANCYTABWIDGET_H

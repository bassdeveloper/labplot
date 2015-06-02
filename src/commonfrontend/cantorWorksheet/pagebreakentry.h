/*
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301, USA.

    ---
    Copyright (C) 2012 Martin Kuettler <martin.kuettler@gmail.com>
 */

#ifndef PAGEBREAKENTRY_H
#define PAGEBREAKENTRY_H

#include "worksheetentry.h"

class WorksheetTextItem;

class PageBreakEntry : public WorksheetEntry
{
  Q_OBJECT;

  public:
    PageBreakEntry(Worksheet* worksheet);
    ~PageBreakEntry();

    enum {Type = UserType + 3};
    int type() const;

    bool isEmpty();
    bool acceptRichText();
    void setContent(const QString& content);
    void setContent(const QDomElement& content, const KZip& file);
    QDomElement toXml(QDomDocument& doc, KZip* archive);
    QString toPlain(const QString& commandSep, const QString& commentStartingSeq, const QString& commentEndingSeq);

    void interruptEvaluation();

    void layOutForWidth(qreal w, bool force = false);

    //void paint(QPainter* painter, const QStyleOptionGraphicsItem * option,
    //         QWidget * widget = 0);

  public Q_SLOTS:
    bool evaluate(EvaluationOption evalOp = FocusNext);
    void updateEntry();
    void populateMenu(QMenu *menu, const QPointF& pos);

  protected:
    bool wantToEvaluate();
    bool wantFocus();

  private:
    WorksheetTextItem* m_msgItem;
};

#endif /* PAGEBREAKENTRY_H */

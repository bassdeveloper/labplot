/***************************************************************************
    File                 : XYCurvePrivate.h
    Project              : LabPlot/SciDAVis
    Description          : Private members of XYCurvePrivate
    --------------------------------------------------------------------
    Copyright            : (C) 2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2010-2013 Alexander Semke (alexander.semke*web.de)
							  (replace * with @ in the email addresses) 
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef XYCURVEPRIVATE_H
#define XYCURVEPRIVATE_H

#include "backend/worksheet/AbstractCurveSymbol.h"
#include <vector>

class CurveSymbolFactory;

class XYCurvePrivate: public QGraphicsItem {
	public:
		XYCurvePrivate(XYCurve *owner);
		~XYCurvePrivate();

		QString name() const;
		virtual QRectF boundingRect() const;
		QPainterPath shape() const;

		bool m_printing;

		void retransform();
		void updateLines();
		void updateDropLines();
		void updateSymbols();
		void updateValues();
		void updateErrorBars();		
		bool swapVisible(bool on);
		QString swapSymbolsTypeId(const QString &id);
		void recalcShapeAndBoundingRect();

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget = 0);
		
		const AbstractColumn* xColumn;
		const AbstractColumn* yColumn;
		QString xColumnPath;
		QString yColumnPath;
		
		XYCurve::LineType lineType;
		int lineInterpolationPointsCount;
		QPen linePen;
		qreal lineOpacity;
		
		XYCurve::DropLineType dropLineType;
		QPen dropLinePen;
		qreal dropLineOpacity;
		
		QBrush symbolsBrush;
		QPen symbolsPen;
		qreal symbolsOpacity;
		qreal symbolsRotationAngle;
		qreal symbolsSize;
		qreal symbolsAspectRatio;
		QString symbolsTypeId;
	
		XYCurve::ValuesType valuesType;
		const AbstractColumn* valuesColumn;
		QString valuesColumnPath;
		XYCurve::ValuesPosition valuesPosition;
		qreal valuesDistance;
		qreal valuesRotationAngle;
		qreal valuesOpacity;
		QString valuesPrefix;
		QString valuesSuffix;		
		QFont valuesFont;
		QColor valuesColor;
		
		//error bars
		XYCurve::ErrorType xErrorType;
		const AbstractColumn* xErrorPlusColumn;
		QString xErrorPlusColumnPath;
		const AbstractColumn* xErrorMinusColumn;
		QString xErrorMinusColumnPath;
		
		XYCurve::ErrorType yErrorType;
		const AbstractColumn* yErrorPlusColumn;
		QString yErrorPlusColumnPath;
		const AbstractColumn* yErrorMinusColumn;
		QString yErrorMinusColumnPath;

		XYCurve::ErrorBarsType errorBarsType;
		double errorBarsCapSize;
		QPen errorBarsPen;
		qreal errorBarsOpacity;
		
		QPainterPath linePath;
		QPainterPath dropLinePath;
		QPainterPath valuesPath;
		QPainterPath errorBarsPath;
		QPainterPath symbolsPath;
		AbstractCurveSymbol* symbolsPrototype;
		CurveSymbolFactory* symbolsFactory;
		QRectF boundingRectangle;
		QPainterPath curveShape;
		QList<QPointF> symbolPointsLogical;	//points in logical coordinates
		QList<QPointF> symbolPointsScene;	//points in scene coordinates
		std::vector<bool> visiblePoints;	//vector of the size of symbolPointsLogical with true of false for the points currently visible or not in the plot
		QList<QPointF> valuesPoints;
		QList<QString> valuesStrings;

		XYCurve * const q;

	private:
        void contextMenuEvent(QGraphicsSceneContextMenuEvent*);
};

#endif

/***************************************************************************
File		: AbstractDataSource.h
Project		: LabPlot
Description 	: Interface for data sources
--------------------------------------------------------------------
Copyright	: (C) 2009 Alexander Semke (alexander.semke@web.de)
Copyright	: (C) 2015 Stefan Gerlach (stefan.gerlach@uni.kn)
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
#ifndef ABSTRACTDATASOURCE_H
#define ABSTRACTDATASOURCE_H

#include "backend/core/AbstractPart.h"
#include "backend/core/AbstractScriptingEngine.h"
#include "backend/datasources/filters/AbstractFileFilter.h"

class AbstractDataSource : public AbstractPart, public scripted{

	public:
   		AbstractDataSource(AbstractScriptingEngine *engine, const QString& name);
		virtual ~AbstractDataSource() {};
		void clear();
		int resize(AbstractFileFilter::ImportMode mode, QStringList colNameList, int cols);
};

#endif // ifndef ABSTRACTDATASOURCE_H

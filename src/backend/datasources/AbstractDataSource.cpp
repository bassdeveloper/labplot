/***************************************************************************
File			: AbstractDataSource.cpp
Project			: LabPlot
Description		: Abstract interface for data sources
--------------------------------------------------------------------
Copyright		: (C) 2009 Alexander Semke (alexander.semke@web.de)
Copyright		: (C) 2015 Stefan Gerlach (stefan.gerlach@uni.kn)
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
#include "AbstractDataSource.h"
#include "backend/core/column/Column.h"

/*!
\class AbstractDataSource
\brief Interface for the data sources.

\ingroup datasources
*/

AbstractDataSource::AbstractDataSource(AbstractScriptingEngine *engine, const QString& name):
	AbstractPart(name), scripted(engine){

}

void AbstractDataSource::clear() {
	int columns = childCount<Column>();
        for (int i=0; i<columns; i++){
                child<Column>(i)->setUndoAware(false);
                child<Column>(i)->setSuppressDataChangedSignal(true);
                child<Column>(i)->clear();
                child<Column>(i)->setUndoAware(true);
                child<Column>(i)->setSuppressDataChangedSignal(false);
                child<Column>(i)->setChanged();
        }
}

/*!
	resize data source to cols columns
	returns column offset depending on import mode
*/
int AbstractDataSource::resize(AbstractFileFilter::ImportMode mode, QStringList colNameList, int cols) {
	setUndoAware(false);

	// name additional columns
	for (int k=colNameList.size(); k<cols; k++ )
		colNameList.append( "Column " + QString::number(k+1) );

	int columnOffset=0; //indexes the "start column" in the spreadsheet. Starting from this column the data will be imported.

	Column * newColumn;
        if (mode==AbstractFileFilter::Append){
                columnOffset=childCount<Column>();
                for ( int n=0; n<cols; n++ ){
                        newColumn = new Column(colNameList.at(n), AbstractColumn::Numeric);
                        newColumn->setUndoAware(false);
                        addChild(newColumn);
                }
	}else if (mode==AbstractFileFilter::Prepend){
                Column* firstColumn = child<Column>(0);
                for ( int n=0; n<cols; n++ ){
                        newColumn = new Column(colNameList.at(n), AbstractColumn::Numeric);
                        newColumn->setUndoAware(false);
                        insertChildBefore(newColumn, firstColumn);
                }
        }else if (mode==AbstractFileFilter::Replace){
                //replace completely the previous content of the data source with the content to be imported.
                int columns = childCount<Column>();

                if (columns > cols){
                        //there're more columns in the data source then required
                        //-> remove the superfluous columns
                        for(int i=0;i<columns-cols;i++) {
                                removeChild(child<Column>(0));
                        }

                        //rename the columns, that are already available
                        for (int i=0; i<cols; i++){
                                child<Column>(i)->setUndoAware(false);
                                child<Column>(i)->setColumnMode( AbstractColumn::Numeric);
                                child<Column>(i)->setName(colNameList.at(i));
                                child<Column>(i)->setSuppressDataChangedSignal(true);
                        }
                }else{
                        //rename the columns, that are already available
                        for (int i=0; i<columns; i++){
                                child<Column>(i)->setUndoAware(false);
                                child<Column>(i)->setColumnMode( AbstractColumn::Numeric);
                                child<Column>(i)->setName(colNameList.at(i));
                                child<Column>(i)->setSuppressDataChangedSignal(true);
                        }

                        //create additional columns if needed
                        for(int i=columns; i < cols; i++) {
                                newColumn = new Column(colNameList.at(i), AbstractColumn::Numeric);
                                newColumn->setUndoAware(false);
                                addChild(newColumn);
                                child<Column>(i)->setSuppressDataChangedSignal(true);
                        }
                }
	}

	return columnOffset;
}

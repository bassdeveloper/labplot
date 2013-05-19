/***************************************************************************
    File                 : Project.cpp
    Project              : SciDAVis
    Description          : Represents a SciDAVis project.
    --------------------------------------------------------------------
    Copyright            : (C) 2011-2013 Alexander Semke (alexander.semke*web.de)
    Copyright            : (C) 2007-2008 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2007 Knut Franke (knut.franke*gmx.de)
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
#include "backend/core/Project.h"
#include "backend/lib/XmlStreamReader.h"
#include "backend/spreadsheet/Spreadsheet.h"
#include "backend/worksheet/plots/cartesian/XYCurve.h"

#include <QUndoStack>
#include <QMenu>
#include <QDateTime>

/**
 * \class Project
 * \brief Represents a SciDAVis project.
 *  \ingroup core
 * Project manages an undo stack and is responsible for creating ProjectWindow instances
 * as views on itself.
 */

/**
 * \enum Project::MdiWindowVisibility
 * \brief MDI subwindow visibility setting
 */
/**
 * \var Project::folderOnly
 * \brief only show MDI windows corresponding to Parts in the current folder
 */
/**
 * \var Project::foldAndSubfolders
 * \brief show MDI windows corresponding to Parts in the current folder and its subfolders
 */
/**
 * \var Project::allMdiWindows
 * \brief show MDI windows for all Parts in the project simultaneously
 */

class Project::Private
{
	public:
		Private() :
			mdi_window_visibility(Project::folderOnly),
			primary_view(0),
			scripting_engine(0),
		 	file_name(QString()),

#ifdef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
			version(SciDAVis::version()),
#else
			version(0),
			labPlot(QString(LVERSION)),
#endif
			author(QString(getenv("USER"))),
			modification_time(QDateTime::currentDateTime()),
			changed(false)
			{}

		QUndoStack undo_stack;
		MdiWindowVisibility mdi_window_visibility;
		QWidget * primary_view;
		AbstractScriptingEngine * scripting_engine;
		QString file_name;
		int version;
#ifndef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
		QString labPlot;
#endif
		QString  author;
		QDateTime modification_time;
		bool changed;
};

Project::Project()
	: Folder(tr("Project")), d(new Private())
{
#ifndef SUPPRESS_SCRIPTING_INIT
	// TODO: intelligent engine choosing
	Q_ASSERT(ScriptingEngineManager::instance()->engineNames().size() > 0);
	QString engine_name = ScriptingEngineManager::instance()->engineNames()[0];
	d->scripting_engine = ScriptingEngineManager::instance()->engine(engine_name);
#endif
}

Project::~Project()
{
	delete d;
}

QUndoStack *Project::undoStack() const
{
	return &d->undo_stack;
}

//TODO does Project really need a view?
QWidget *Project::view()
{
	return d->primary_view;
}

QMenu *Project::createContextMenu()
{
	QMenu * menu = new QMenu(); // no remove action from AbstractAspect in the project context menu
	emit requestProjectContextMenu(menu);
	return menu;
}

QMenu *Project::createFolderContextMenu(const Folder * folder)
{
	QMenu * menu = const_cast<Folder *>(folder)->AbstractAspect::createContextMenu();
	Q_ASSERT(menu);
	emit requestFolderContextMenu(folder, menu);
	return menu;
}

void Project::setMdiWindowVisibility(MdiWindowVisibility visibility)
{
	d->mdi_window_visibility = visibility;
	emit mdiWindowVisibilityChanged();
}

Project::MdiWindowVisibility Project::mdiWindowVisibility() const
{
	return d->mdi_window_visibility;
}

AbstractScriptingEngine * Project::scriptingEngine() const
{
	return d->scripting_engine;
}

/* ================== static methods ======================= */
#ifdef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
ConfigPageWidget * Project::makeConfigPage()
{
	 return new ProjectConfigPage();
}

QString Project::configPageLabel()
{
	return QObject::tr("General");
}
#endif

CLASS_D_ACCESSOR_IMPL(Project, QString, fileName, FileName, file_name)
BASIC_D_ACCESSOR_IMPL(Project, int, version, Version, version)
#ifndef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
CLASS_D_ACCESSOR_IMPL(Project, QString, labPlot, LabPlot, labPlot)
#endif
// TODO: add support for these in the SciDAVis UI
CLASS_D_ACCESSOR_IMPL(Project, QString, author, Author, author)
CLASS_D_ACCESSOR_IMPL(Project, QDateTime, modificationTime, ModificationTime, modification_time)

void Project ::setChanged(const bool value){
	if ( value && !d->changed )
		emit changed();
	
	d->changed = value;
}

bool Project ::hasChanged() const{
	return d->changed ;
} 

////////////////////////////////////////////////////////////////////////////////
//! \name serialize/deserialize
//@{
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Save as XML
 */
void Project::save(QXmlStreamWriter * writer) const
{
    writer->setAutoFormatting(true);
	writer->writeStartDocument();
#ifdef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
	writer->writeDTD("<!DOCTYPE SciDAVisProject>");
#else
	writer->writeDTD("<!DOCTYPE LabPlotXML>");
#endif

	writer->writeStartElement("project");
	writer->writeAttribute("version", QString::number(version()));
	writer->writeAttribute("file_name", fileName());
	writer->writeAttribute("modification_time" , modificationTime().toString("yyyy-dd-MM hh:mm:ss:zzz"));
	writer->writeAttribute("author", author());
#ifndef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
	writer->writeAttribute("LabPlot" , labPlot());
#endif
	writeBasicAttributes(writer);

	writeCommentElement(writer);

	foreach(AbstractAspect * child, children<AbstractAspect>(IncludeHidden)) {
		writer->writeStartElement("child_aspect");
		child->save(writer);
		writer->writeEndElement();
	}

	writer->writeEndElement();
	writer->writeEndDocument();
}

/**
 * \brief Load from XML
 */
bool Project::load(XmlStreamReader * reader)
{
	while (!(reader->isStartDocument() || reader->atEnd()))
		reader->readNext();
	if(!(reader->atEnd()))
	{
		if (!reader->skipToNextTag()) return false;

		if (reader->name() == "project") {
			setComment("");
			removeAllChildren();

			bool ok;
			int version = reader->readAttributeInt("version", &ok);
			if(!ok)
			{
				reader->raiseError(tr("invalid or missing project version"));
				return false;
			}
			setVersion(version);
			// version dependent staff goes here

			if (!readBasicAttributes(reader)) return false;
			if (!readProjectAttributes(reader)) return false;

			while (!reader->atEnd())
			{
				reader->readNext();

				if (reader->isEndElement()) break;

				if (reader->isStartElement())
				{
					if (reader->name() == "comment")
					{
						if (!readCommentElement(reader))
							return false;
					}
					else if(reader->name() == "child_aspect")
					{
						if (!readChildAspectElement(reader))
							return false;
					}
					else // unknown element
					{
						reader->raiseWarning(tr("unknown element '%1'").arg(reader->name().toString()));
						if (!reader->skipToEndElement()) return false;
					}
				}
			}

			//everything is read now.
			//restore the pointer to the data sets (columns) in xy-curves etc.
			QList<AbstractAspect*> curves = children("XYCurve", AbstractAspect::Recursive);
			if (curves.size()!=0) {
				QList<AbstractAspect*> spreadsheets = children("Spreadsheet", AbstractAspect::Recursive);
				XYCurve* curve;
				Spreadsheet* sheet;
				QString name;
				foreach (AbstractAspect* aspect, curves) {
					curve = dynamic_cast<XYCurve*>(aspect);
					if (!curve) continue;

					RESTORE_COLUMN_POINTER(xColumn, XColumn);
					RESTORE_COLUMN_POINTER(yColumn, YColumn);
					RESTORE_COLUMN_POINTER(valuesColumn, ValuesColumn);
					RESTORE_COLUMN_POINTER(xErrorPlusColumn, XErrorPlusColumn);
					RESTORE_COLUMN_POINTER(xErrorMinusColumn, XErrorMinusColumn);
					RESTORE_COLUMN_POINTER(yErrorPlusColumn, YErrorPlusColumn);
					RESTORE_COLUMN_POINTER(yErrorMinusColumn, YErrorMinusColumn);					
				}
			}
		} else {// no project element
			reader->raiseError(tr("no project element found"));
		}
	} else {// no start document
		reader->raiseError(tr("no valid XML document found"));
	}

	return !reader->hasError();
}

bool Project::readProjectAttributes(XmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	QXmlStreamAttributes attribs = reader->attributes();
	QString str;

	str = attribs.value(reader->namespaceUri().toString(), "file_name").toString();
	if(str.isEmpty())
	{
		reader->raiseError(prefix+tr("project file name missing")+postfix);
		return false;
	}
	setFileName(str);

	str = attribs.value(reader->namespaceUri().toString(), "modification_time").toString();
	QDateTime modification_time = QDateTime::fromString(str, "yyyy-dd-MM hh:mm:ss:zzz");
	if(str.isEmpty() || !modification_time.isValid())
	{
		reader->raiseWarning(tr("Invalid project modification time. Using current time."));
		setModificationTime(QDateTime::currentDateTime());
	}
	else
		setModificationTime(modification_time);

	str = attribs.value(reader->namespaceUri().toString(), "author").toString();
	setAuthor(str);

#ifndef ACTIVATE_SCIDAVIS_SPECIFIC_CODE
	str = attribs.value(reader->namespaceUri().toString(), "LabPlot").toString();
	if(str.isEmpty())
	{
		reader->raiseError(prefix+tr("LabPlot attribute missing")+postfix);
		return false;
	}
	setLabPlot(str);
#endif

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//@}
////////////////////////////////////////////////////////////////////////////////

// void Project::staticInit()
// {
	// defaults for global settings
// 	Project::setGlobalDefault("default_mdi_window_visibility", Project::folderOnly);
// 	Project::setGlobalDefault("auto_save", true);
// 	Project::setGlobalDefault("auto_save_interval", 15);
// 	Project::setGlobalDefault("default_scripting_language", QString("muParser"));
// 	// TODO: not really Project-specific; maybe put these somewhere else:
// 	Project::setGlobalDefault("language", QString("en"));
// 	Project::setGlobalDefault("auto_search_updates", false);
// 	Project::setGlobalDefault("locale_use_group_separator", false);
// }

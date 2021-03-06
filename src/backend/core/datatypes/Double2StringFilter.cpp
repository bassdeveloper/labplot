/***************************************************************************
    File                 : Double2StringFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Benkert
    Email (use @ for *)  : knut.franke*gmx.de, thzs@gmx.net
    Description          : Locale-aware conversion filter double -> QString.
                           
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

#include "Double2StringFilter.h"
#include "backend/lib/XmlStreamReader.h"
#include <QXmlStreamWriter>
#include <QUndoCommand>

#include <KLocale>

class Double2StringFilterSetFormatCmd : public QUndoCommand
{
	public:
		Double2StringFilterSetFormatCmd(Double2StringFilter* target, char new_format);

		virtual void redo();
		virtual void undo();

	private:
		Double2StringFilter* m_target;
		char m_other_format;
};

class Double2StringFilterSetDigitsCmd : public QUndoCommand
{
	public:
		Double2StringFilterSetDigitsCmd(Double2StringFilter* target, int new_digits);

		virtual void redo();
		virtual void undo();

	private:
		Double2StringFilter* m_target;
		int m_other_digits;
};

void Double2StringFilter::writeExtraAttributes(QXmlStreamWriter * writer) const
{
	writer->writeAttribute("format", QString(QChar(numericFormat())));
	writer->writeAttribute("digits", QString::number(numDigits()));
}

bool Double2StringFilter::load(XmlStreamReader * reader)
{
	QXmlStreamAttributes attribs = reader->attributes();
	QString format_str = attribs.value(reader->namespaceUri().toString(), "format").toString();
	QString digits_str = attribs.value(reader->namespaceUri().toString(), "digits").toString();

	if (AbstractSimpleFilter::load(reader))
	{
		bool ok;
		int digits = digits_str.toInt(&ok);
		if( (format_str.size() != 1) || !ok )
			reader->raiseError(i18n("missing or invalid format attribute"));
		else
		{
			setNumericFormat( format_str.at(0).toAscii() );
			setNumDigits( digits );
		}
	}
	else
		return false;

	return !reader->hasError();
}

void Double2StringFilter::setNumericFormat(char format) 
{ 
	exec(new Double2StringFilterSetFormatCmd(this, format));
}

void Double2StringFilter::setNumDigits(int digits) 
{ 
	exec(new Double2StringFilterSetDigitsCmd(this, digits));
}

Double2StringFilterSetFormatCmd::Double2StringFilterSetFormatCmd(Double2StringFilter* target, char new_format)
	: m_target(target), m_other_format(new_format) 
{
	if(m_target->parentAspect())
		setText(i18n("%1: set numeric format to '%2'", m_target->parentAspect()->name(), new_format));
	else
		setText(i18n("set numeric format to '%1'", new_format));
}

void Double2StringFilterSetFormatCmd::redo() 
{
	char tmp = m_target->m_format;
	m_target->m_format = m_other_format;
	m_other_format = tmp;
	emit m_target->formatChanged();
}

void Double2StringFilterSetFormatCmd::undo() 
{ 
	redo(); 
}

Double2StringFilterSetDigitsCmd::Double2StringFilterSetDigitsCmd(Double2StringFilter* target, int new_digits)
	: m_target(target), m_other_digits(new_digits) 
{
	if(m_target->parentAspect())
		setText(i18n("%1: set decimal digits to %2", m_target->parentAspect()->name(), new_digits));
	else
		setText(i18n("set decimal digits to %1", new_digits));
}

void Double2StringFilterSetDigitsCmd::redo() 
{
	int tmp = m_target->m_digits;
	m_target->m_digits = m_other_digits;
	m_other_digits = tmp;
	emit m_target->digitsChanged();
}

void Double2StringFilterSetDigitsCmd::undo() 
{ 
	redo(); 
}




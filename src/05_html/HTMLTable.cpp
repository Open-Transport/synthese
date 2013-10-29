
/** HTMLTable class implementation.
	@file HTMLTable.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <sstream>
#include <assert.h>

#include "HTMLTable.h"

using namespace std;

namespace synthese
{
	namespace html
	{
		HTMLTable::HTMLTable(
			const HTMLTable::ColsVector& header,
			std::string className,
			std::string id
		):	_id(id),
			_cols(header.size()),
			_section(UNKNOWN_SECTION),
			_curCol(-1),
			_curRow(-1),
			_className(className),
			_rowOpen(false)
			
		{
			if (header.empty())
			{
				return;
			}

			string lastHeader;
			stringstream s;
			int colspan = 1;
			for (ColsVector::const_iterator it = header.begin(); it != header.end(); ++it)
			{
				if(it != header.begin())
				{
					if(*it == lastHeader)
					{
						++colspan;
					}
					else
					{
						s << "<th";
						if (colspan > 1)
						{
							s << " colspan=\"" << colspan << "\"";
						}
						s << ">" << lastHeader << "</th>";
						colspan=1;
					}
				}
				lastHeader = *it;
			}
			s << "<th";
			if (colspan > 1)
			{
				s << " colspan=\"" << colspan << "\"";
			}
			s << ">" << lastHeader << "</th>";
			_headers = s.str();
		}



		HTMLTable::HTMLTable(
			int cols/*=0*/,
			std::string className,
			std::string id
		):	_id(id),
			_cols(cols),
			_section(UNKNOWN_SECTION),
			_curCol(-1),
			_curRow(-1),
			_className(className),
			_rowOpen(false)
		{}



		std::string HTMLTable::open()
		{
			stringstream s;
			s << "<table";
			if(!_className.empty())
			{
				s << " class=\"" << _className << "\"";
			}
			if(!_id.empty())
			{
				s << " id=\"" << _id << "\"";
			}
			s << ">";
			if(!_headers.empty())
			{
				head(s);
				s << "<tr>" << _headers << "</tr>";
			}
			return s.str();
		}



		std::string HTMLTable::close()
		{
			stringstream s;
			_closeSection(s);
			s << "</table>";
			return s.str();
		}



		void HTMLTable::_closeRow(std::ostream& stream)
		{
			if (_curCol == -1 || !_rowOpen)
			{
				return;
			}
			for (; _curCol < _cols-1; ++_curCol)
			{
				stream << col() << "&nbsp;";
			}
			stream << (_lastColWasH ? "</th>" : "</td>");
			stream << "</tr>";
			_rowOpen = false;
		}



		std::string HTMLTable::col(
			size_t colSpan/*=1*/,
			string className,
			bool isHeader,
			string style,
			size_t rowSpan
		){
			stringstream s;
			if((_cols && (_curCol > _cols)) || _curRow == -1 || !_rowOpen)
			{
				s << row();
			}
			else if (_curCol != -1)
			{
				s << (_lastColWasH ? "</th>" : "</td>");
			}
			s << (isHeader ? "<th" : "<td");
			if(colSpan > 1)
			{
				s << " colspan=\"" << colSpan << "\"";
			}
			if(rowSpan > 1)
			{
				s << " rowspan=\"" << rowSpan << "\"";
			}
			if(!className.empty())
			{
				s << " class=\"" << className << "\"";
			}
			if(!style.empty())
			{
				s << " style=\"" << style << "\"";
			}
			s << ">";
			_curCol += colSpan;
			_lastColWasH = isHeader;
			return s.str();
		}



		std::string HTMLTable::row(string className, std::string idName)
		{
			stringstream s;
			_closeRow(s);
			s << "<tr class=\"";
			if (!className.empty())
			{
				s << className;
			}
			else
			{
				s << "r" << ((_curRow % 2) ? "1" : "2");
			}
			if (!idName.empty())
			{
				s << "\" id=\"" << idName;
			}
			s << "\">";
			++_curRow;
			_curCol = -1;
			_rowOpen = true;
			return s.str();
		}



		std::string HTMLTable::goCol( int colNumber, int colSpan /*=1*/, std::string className/*=""*/ )
		{
			stringstream s;
			if (_curCol >= colNumber)
			{
				s << row();
			}
			for (; _curCol+1 < colNumber;)
			{
				s << col();
			}
			s << col(colSpan, className);
			return s.str();
		}



		int HTMLTable::_getColsNumber() const
		{
			return _cols;
		}



		HTMLTable::~HTMLTable(
		){
		}



		void HTMLTable::head(
			ostream& stream,
			std::string className
		){
			if(_section == UNKNOWN_SECTION)
			{
				_closeRow(stream);
				stream  << "<thead";
				if(!className.empty())
				{
					stream << " class=\"" << className << "\"";
				}
				stream << ">";
				_section = HEAD;
			}
			assert(_section == HEAD);
		}



		void HTMLTable::body(
			ostream& stream,
			std::string className
		){
			if(	_section == UNKNOWN_SECTION ||
				_section == HEAD
			){
				_closeSection(stream);
				stream << "<tbody";
				if(!className.empty())
				{
					stream << " class=\"" << className << "\"";
				}
				stream << ">";
				_section = BODY;
			}
			assert(_section == BODY);
		}



		void HTMLTable::foot(
			ostream& stream,
			std::string className
		){
			if(	_section == UNKNOWN_SECTION ||
				_section == HEAD ||
				_section == BODY
			){
				_closeSection(stream);
				stream << "<tfoot";
				if(!className.empty())
				{
					stream << " class=\"" << className << "\"";
				}
				stream << ">";
				_section = FOOT;
			}
			assert(_section == FOOT);
		}



		void HTMLTable::_closeSection( std::ostream& stream )
		{
			_closeRow(stream);
			if(_section == HEAD)
			{
				stream << "</thead>";
			}
			else if(_section == BODY)
			{
				stream << "</tbody>";
			}
			else if(_section == FOOT)
			{
				stream << "</tfoot>";
			}
		}
}	}

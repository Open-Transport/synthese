
/** HTMLTable class implementation.
	@file HTMLTable.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "05_html/HTMLTable.h"

using namespace std;

namespace synthese
{
	namespace html
	{


		HTMLTable::HTMLTable(const HTMLTable::ColsVector& header, std::string className /*= ""*/ )
			: _cols(0)
			, _className(className)
			, _curCol(-1)
			, _curRow(-1)
		{
			if (header.empty())
				return;

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
							s << " colspan=\"" << colspan << "\"";
						s << ">" << lastHeader << "</th>";
						colspan=1;
						++_cols;
					}
				}
				lastHeader = *it;
			}
			++_cols;
			s << "<th";
			if (colspan > 1)
				s << " colspan=\"" << colspan << "\"";
			s << ">" << lastHeader << "</th>";
			_headers = s.str();			
		}

		HTMLTable::HTMLTable(int cols/*=0*/, std::string className /*= ""*/ )
			: _cols(cols)
			, _className(className)
			, _curCol(-1)
			, _curRow(-1)
		{

		}

		std::string HTMLTable::open()
		{
			stringstream s;
			s << "<table";
			if (!_className.empty())
				s << " class=\"" << _className << "\"";
			s << ">";
			if (!_headers.empty())
				s << "<tr>" << _headers << "</tr>";
			return s.str();
		}

		std::string HTMLTable::close()
		{
			stringstream s;
			if (_curRow > -1)
				s << _closeRow();
			s << "</table>";
			return s.str();
		}

		std::string HTMLTable::_closeRow()
		{
			if (_curCol == -1)
				return "";

			stringstream s;
			for (; _curCol < _cols-1; ++_curCol)
				s << col() << "&nbsp;";
			s << (_lastColWasH ? "</th>" : "</td>");
			s << "</tr>";
			return s.str();
		}



		std::string HTMLTable::col(
			size_t colSpan/*=1*/,
			string className,
			bool isHeader,
			string style
		){
			stringstream s;
			if (_cols && (_curCol > _cols) || _curRow == -1)
				s << row();
			else if (_curCol != -1)
				s << (_lastColWasH ? "</th>" : "</td>");
			s << (isHeader ? "<th" : "<td");
			if (colSpan > 1)
				s << " colspan=\"" << colSpan << "\"";
			if (!className.empty())
				s << " class=\"" << className << "\"";
			if(!style.empty())
			{
				s << " style=\"" << style << "\"";
			}
			s << ">";
			_curCol += colSpan;
			_lastColWasH = isHeader;
			return s.str();
		}

		std::string HTMLTable::row(string className)
		{
			stringstream s;
			if (_curRow > -1)
				s << _closeRow();
			s << "<tr class=\"";
			if (!className.empty())
				s << className;
			else
				s << "r" << ((_curRow % 2) ? "1" : "2");
			s << "\">";
			++_curRow;
			_curCol = -1;
			return s.str();
		}

		std::string HTMLTable::goCol( int colNumber, int colSpan /*=1*/, std::string className/*=""*/ )
		{
			stringstream s;
			if (_curCol >= colNumber)
				s << row();
			for (; _curCol+1 < colNumber;)
				s << col();
			s << col(colSpan, className);
			return s.str();
		}

		int HTMLTable::_getColsNumber() const
		{
			return _cols;
		}



		HTMLTable::~HTMLTable(
		) {

		}
	}
}

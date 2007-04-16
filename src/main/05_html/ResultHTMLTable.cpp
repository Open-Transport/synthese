
/** ResultHTMLTable class implementation.
	@file ResultHTMLTable.cpp

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

#include "01_util/Conversion.h"

#include "05_html/ResultHTMLTable.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace html
	{

		const std::string ResultHTMLTable::_PARAMETER_FIRST = "rhtf";
		const std::string ResultHTMLTable::_PARAMETER_ORDER_FIELD = "rhto";
		const std::string ResultHTMLTable::_PARAMETER_RAISING_ORDER = "rhtr";
		const std::string ResultHTMLTable::_PARAMETER_MAX_SIZE = "rhts";

		ResultHTMLTable::ResultHTMLTable(
			const ResultHTMLTable::HeaderVector& header
			, const HTMLForm& searchForm
			, const RequestParameters& requestParameters
			, const ResultParameters& resultParameters
			, std::string iconPath )
			: HTMLTable(header.size(), "adminresults")
			, _searchForm(searchForm)
			, _maxSize(requestParameters.maxSize)
			, _first(requestParameters.first)
			, _orderField(requestParameters.orderField)
			, _raisingOrder(requestParameters.raisingOrder)
			, _next(resultParameters.next)
			, _size(resultParameters.size)
		{
			stringstream s;
			for (HeaderVector::const_iterator it = header.begin(); it != header.end(); ++it)
			{
				s << "<th>";
				
				// This column is sortable
				if (!it->first.empty())
				{
					HTMLForm::HiddenFieldsMap h;
					h.insert(make_pair(_PARAMETER_ORDER_FIELD, it->first));
					h.insert(make_pair(_PARAMETER_RAISING_ORDER, Conversion::ToString((_orderField == it->first) ? !_raisingOrder : true)));
					s << HTMLModule::getHTMLLink(_searchForm.getURL(h), it->second);

					if (it->first == _orderField)
					{
						s << "&nbsp;";
						if (!iconPath.empty())
							s << "<img src=\"" << iconPath << (_raisingOrder ? "down" : "up") << ".png\" alt=\"" << (_raisingOrder ? "V" : "^") << "\" />";
						else
							s << (_raisingOrder ? "V" : "^");
					}
				}
				else
					s << it->second;
				s << "</th>";
			}
			_headers = s.str();

			_searchForm.addHiddenField(_PARAMETER_FIRST, Conversion::ToString(_first));
			_searchForm.addHiddenField(_PARAMETER_ORDER_FIELD, Conversion::ToString(_orderField));
			_searchForm.addHiddenField(_PARAMETER_RAISING_ORDER, Conversion::ToString(_raisingOrder));
			_searchForm.addHiddenField(_PARAMETER_MAX_SIZE, Conversion::ToString(_maxSize));
		}

		ResultHTMLTable::RequestParameters ResultHTMLTable::getParameters(const map<string, string>& m, const string& defaultOrderField, int defaultMaxSize )
		{
			map<string, string>::const_iterator it;
			RequestParameters p;

			it = m.find(_PARAMETER_FIRST);
			p.first = (it == m.end()) ? 0 : Conversion::ToInt(it->second);

			it = m.find(_PARAMETER_ORDER_FIELD);
			p.orderField = (it == m.end()) ? defaultOrderField : it->second;

			it = m.find(_PARAMETER_RAISING_ORDER);
			p.raisingOrder = (it == m.end()) ? true : Conversion::ToBool(it->second);

			it = m.find(_PARAMETER_MAX_SIZE);
			p.maxSize = (it == m.end()) ? defaultMaxSize : Conversion::ToInt(it->second);

			return p;
		}

		std::string ResultHTMLTable::close()
		{
			stringstream s;

			s << row();
			s << "<th colspan=\"" << _getColsNumber() << "\">";
			if (_first)
			{
				HTMLForm::HiddenFieldsMap f;
				f.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString((_first > _maxSize) ? _first - _maxSize : 0)));
				s << HTMLModule::getHTMLLink(_searchForm.getURL(f), "<<") << "&nbsp;|&nbsp;";
			}
			s << _first << "&nbsp;->&nbsp;" << (_first + _size - 1);
			if (_next)
			{
				HTMLForm::HiddenFieldsMap f;
				f.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString(_first + _size)));
				s << "&nbsp;|&nbsp;" << HTMLModule::getHTMLLink(_searchForm.getURL(f), ">>");
			}
			s << HTMLTable::close();
			return s.str();
		}

		std::map<std::string, std::string> ResultHTMLTable::getParametersMap( const RequestParameters& requestParameters)
		{
			std::map<std::string, std::string> map;
			map.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString(requestParameters.first)));
			map.insert(make_pair(_PARAMETER_MAX_SIZE, Conversion::ToString(requestParameters.maxSize)));
			map.insert(make_pair(_PARAMETER_ORDER_FIELD, requestParameters.orderField));
			map.insert(make_pair(_PARAMETER_RAISING_ORDER, Conversion::ToString(requestParameters.raisingOrder)));
			return map;
		}
	}
}

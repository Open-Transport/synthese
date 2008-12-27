////////////////////////////////////////////////////////////////////////////////
/// ResultHTMLTable class implementation.
///	@file ResultHTMLTable.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "Conversion.h"

#include "ResultHTMLTable.h"

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
		const int ResultHTMLTable::UNLIMITED_SIZE = -1;
		const string ResultHTMLTable::CSS_CLASS("adminresults");

		ResultHTMLTable::ResultHTMLTable(
			const ResultHTMLTable::HeaderVector& header
			, const HTMLForm& searchForm
			, const RequestParameters& requestParameters
			, const ResultParameters& resultParameters
			, std::string iconPath )
			: HTMLTable(header.size(), CSS_CLASS)
			, _searchForm(searchForm)
			, _maxSize(requestParameters.maxSize)
			, _first(requestParameters.first)
			, _orderField(requestParameters.orderField)
			, _raisingOrder(requestParameters.raisingOrder)
			, _next(resultParameters.next)
			, _size(resultParameters.size)
		{
			stringstream s;
			int colspan(1);
			for (HeaderVector::const_iterator it = header.begin(); it != header.end(); ++it)
			{
				if ((it+1) != header.end() && *(it+1) == *it)
					++colspan;
				else
				{
					s << "<th";
					if (colspan > 1)
						s << " colspan=\"" << colspan << "\"";
					s << ">";

					// This column is sortable
					if (!it->first.empty())
					{
						HTMLForm::HiddenFieldsMap h;
						h.insert(make_pair(_PARAMETER_ORDER_FIELD, it->first));
						h.insert(make_pair(_PARAMETER_RAISING_ORDER, Conversion::ToString((_orderField == it->first) ? !_raisingOrder : true)));
						s << HTMLModule::getHTMLLink(_searchForm.getURL(h), it->second);

						if (it->first == _orderField)
						{
							s << "&nbsp;" << HTMLModule::getHTMLImage(iconPath + (_raisingOrder ? "up" : "down") + ".png", _raisingOrder ? "V" : "^");
						}						
					}
					else
						s << it->second;
					s << "</th>";
					colspan = 1;
				}
			}
			_headers = s.str();

			_searchForm.addHiddenField(_PARAMETER_FIRST, Conversion::ToString(_first));
			_searchForm.addHiddenField(_PARAMETER_ORDER_FIELD, Conversion::ToString(_orderField));
			_searchForm.addHiddenField(_PARAMETER_RAISING_ORDER, Conversion::ToString(_raisingOrder));
			_searchForm.addHiddenField(_PARAMETER_MAX_SIZE, Conversion::ToString(_maxSize));
		}

		void ResultHTMLTable::RequestParameters::setFromParametersMap(const map<string, string>& m, const string defaultOrderField, int defaultMaxSize, bool defaultRaisingOrder)
		{
			map<string, string>::const_iterator it;

			it = m.find(_PARAMETER_FIRST);
			first = (it == m.end()) ? 0 : Conversion::ToInt(it->second);

			it = m.find(_PARAMETER_ORDER_FIELD);
			orderField = (it == m.end()) ? defaultOrderField : it->second;

			it = m.find(_PARAMETER_RAISING_ORDER);
			raisingOrder = (it == m.end()) ? defaultRaisingOrder : Conversion::ToBool(it->second);

			it = m.find(_PARAMETER_MAX_SIZE);
			maxSize = (it == m.end()) ? defaultMaxSize : Conversion::ToInt(it->second);
		}

		std::string ResultHTMLTable::close()
		{
			stringstream s;

			if (_maxSize != UNLIMITED_SIZE && _size != UNLIMITED_SIZE)
			{
				s << row();
				s << col(_getColsNumber(), string(), true);
				if (_first)
				{
					HTMLForm::HiddenFieldsMap f;
					f.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString((_first > _maxSize) ? _first - _maxSize - 1: 0)));
					s << HTMLModule::getHTMLLink(_searchForm.getURL(f), HTMLModule::getHTMLImage("resultset_previous.png", "<<")) << "&nbsp;|&nbsp;";
				}
				if (_size > 0)
					s << "Résultats&nbsp;" << (_first + 1) << "&nbspà&nbsp;" << (_first + _size);
				else
					s << "Aucun résultat";
				if (_next)
				{
					HTMLForm::HiddenFieldsMap f;
					f.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString(_first + _size)));
					s << "&nbsp;|&nbsp;" << HTMLModule::getHTMLLink(_searchForm.getURL(f), HTMLModule::getHTMLImage("resultset_next.png", ">>"));
				}
			}
			s << HTMLTable::close();
			return s.str();
		}

		std::map<std::string, std::string> ResultHTMLTable::RequestParameters::getParametersMap() const
		{
			std::map<std::string, std::string> map;
			map.insert(make_pair(_PARAMETER_FIRST, Conversion::ToString(first)));
			map.insert(make_pair(_PARAMETER_MAX_SIZE, Conversion::ToString(maxSize)));
			map.insert(make_pair(_PARAMETER_ORDER_FIELD, orderField));
			map.insert(make_pair(_PARAMETER_RAISING_ORDER, Conversion::ToString(raisingOrder)));
			return map;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/// ResultHTMLTable class implementation.
///	@file ResultHTMLTable.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "ParametersMap.h"
#include "ResultHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace html
	{

		const std::string ResultHTMLTable::_PARAMETER_FIRST = "rhtf";
		const std::string ResultHTMLTable::_PARAMETER_ORDER_FIELD = "rhto";
		const std::string ResultHTMLTable::_PARAMETER_RAISING_ORDER = "rhtr";
		const std::string ResultHTMLTable::_PARAMETER_MAX_SIZE = "rhts";
		const string ResultHTMLTable::CSS_CLASS("adminresults");


		void ResultHTMLTable::RequestParameters::setFromParametersMap(
			const util::ParametersMap& m,
			const string defaultOrderField,
			optional<size_t> defaultMaxSize,
			bool defaultRaisingOrder,
			const string& prefix
		){
			map<string, string>::const_iterator it;

			_prefix = prefix;

			try
			{
				first = lexical_cast<size_t>(m.getValue(_prefix + _PARAMETER_FIRST));
			}catch(...)
			{
				try
				{
					first = lexical_cast<size_t>(m.getValue(_PARAMETER_FIRST));
				}
				catch(...)
				{
					first = 0;
				}
			}

			try
			{
				orderField = m.getValue(_prefix + _PARAMETER_ORDER_FIELD);
			}
			catch(...)
			{
				try
				{
					orderField = m.getValue(_PARAMETER_ORDER_FIELD);
				}
				catch(...)
				{
					orderField = defaultOrderField;
				}
			}

			try
			{
				raisingOrder = lexical_cast<bool>(m.getValue(_prefix + _PARAMETER_RAISING_ORDER));
			}
			catch(...)
			{
				try
				{
					raisingOrder = lexical_cast<bool>(m.getValue(_PARAMETER_RAISING_ORDER));
				}
				catch(...)
				{
					raisingOrder = defaultRaisingOrder;
				}
			}

			try
			{
				maxSize = lexical_cast<size_t>(m.getValue(_prefix + _PARAMETER_MAX_SIZE));
			}
			catch(...)
			{
				try
				{
					maxSize = lexical_cast<size_t>(m.getValue(_PARAMETER_MAX_SIZE));
				}
				catch(...)
				{
					maxSize = defaultMaxSize;
				}
			}
		}

		std::string ResultHTMLTable::close()
		{
			stringstream s;

			if (_requestParameters.maxSize)
			{
				s << row();
				s << col(_getColsNumber(), string(), true);
				if (_requestParameters.first)
				{
					HTMLForm::HiddenFieldsMap f;
					f.insert(
						make_pair(
							_requestParameters._getParameterCode(_PARAMETER_FIRST),
							lexical_cast<string>(
								_requestParameters.first > *_requestParameters.maxSize ?
								_requestParameters.first - *_requestParameters.maxSize - 1:
								0
					)	)	);
					s << HTMLModule::getHTMLLink(
							_searchForm.getURL(f),
							HTMLModule::getHTMLImage("/admin/img/resultset_previous.png", "<<")
						) <<
						"&nbsp;|&nbsp;"
					;
				}
				if (size > 0)
					s <<
						"Résultats&nbsp;" << (_requestParameters.first + 1) <<
						"&nbspà&nbsp;" << (_requestParameters.first + size)
					;
				else
					s << "Aucun résultat";
				if (next)
				{
					HTMLForm::HiddenFieldsMap f;
					f.insert(make_pair(
							_requestParameters._getParameterCode(_PARAMETER_FIRST),
							lexical_cast<string>(_requestParameters.first + size)
					)	);
					s <<
						"&nbsp;|&nbsp;" <<
						HTMLModule::getHTMLLink(
							_searchForm.getURL(f),
							HTMLModule::getHTMLImage("/admin/img/resultset_next.png", ">>")
						)
					;
				}
			}
			s << HTMLTable::close();
			return s.str();
		}

		std::map<std::string, std::string> ResultHTMLTable::RequestParameters::getParametersMap(const string& prefix) const
		{
			std::map<std::string, std::string> map;
			map.insert(make_pair(_getParameterCode(prefix + _PARAMETER_FIRST), lexical_cast<string>(first)));
			if(maxSize)
				map.insert(make_pair(_getParameterCode(prefix + _PARAMETER_MAX_SIZE), lexical_cast<string>(*maxSize)));
			map.insert(make_pair(_getParameterCode(prefix + _PARAMETER_ORDER_FIELD), orderField));
			map.insert(make_pair(
				_getParameterCode(prefix + _PARAMETER_RAISING_ORDER),
				lexical_cast<string>(raisingOrder)
			)	);
			return map;
		}



		std::string ResultHTMLTable::RequestParameters::_getParameterCode(
			const std::string& parameter
		) const {
			return _prefix + parameter;
		}
	}
}

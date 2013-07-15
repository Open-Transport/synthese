
/** ResultHTMLTable class header.
	@file ResultHTMLTable.h

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

#ifndef SYNTHESE_ResultHTMLTable_h__
#define SYNTHESE_ResultHTMLTable_h__

#include <utility>
#include <vector>
#include <sstream>

#include "HTMLTable.h"
#include "HTMLForm.h"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace html
	{
		/** ResultHTMLTable class.
			@ingroup m05
		*/
		class ResultHTMLTable : public HTMLTable
		{
		public:
			typedef std::vector<std::pair<std::string, std::string> > HeaderVector;
			static const std::string	CSS_CLASS;

			struct RequestParameters
			{
			public:

				////////////////////////////////////////////////////////////////////
				///	getParameterMap.
				///	@param parameter
				///	@return std::string
				///	@author Hugues Romain
				///	@date 2009
				std::string _getParameterCode(
					const std::string& parameter
				) const;

				boost::optional<size_t> maxSize;
				size_t					first;
				std::string			orderField;
				bool				raisingOrder;
				std::string			_prefix;

				RequestParameters()
					: maxSize(200),
					first(0),
					raisingOrder(true)
				{}


				/** Reads the parameters of the request from the map representing the query.
					@param map the map
					@param defaultOrderField column on which sort the table, if none is specified in the map
					@param defaultMaxSiwe maximum size of the table if not specified in the map
					@param defaultRaisingOrder default direction of sorting if not specified in the map
					@param prefix prefix of the field codes in the map (default = none)
				*/
				void setFromParametersMap(
					const util::ParametersMap& map,
					const std::string defaultOrderField = std::string(),
					boost::optional<size_t> defaultMaxSize = boost::optional<size_t>(),
					bool defaultRaisingOrder = true,
					const std::string& prefix = std::string()
				);

				std::map<std::string, std::string> getParametersMap(
					const std::string& prefix = std::string()
				) const;
			};

		private:
			static const std::string _PARAMETER_FIRST;
			static const std::string _PARAMETER_ORDER_FIELD;
			static const std::string _PARAMETER_RAISING_ORDER;
			static const std::string _PARAMETER_MAX_SIZE;

			RequestParameters	_requestParameters;
			bool				next;
			size_t				size;

		protected:
			HTMLForm		_searchForm;

		public:
			/** Constructor.
				@param header : vector of pairs field code / col caption. If field code is non empty then the searchRequest can be reloaded with an ordering by the column.
				@param searchRequest The request to use to fill a similar table with other parameters
				@param actionRequest A request to launch by the table content (NULL = no action request)
				@param selectFieldName A first col with radio buttons will be drawn, named by the parameter. If empty then no radio button.
			*/
			template<class T>
			ResultHTMLTable(
				const HeaderVector& header,
				const HTMLForm& searchForm,
				const RequestParameters& requestParameters,
				T& v,
				std::string iconPath = "/admin/img/"
			):	HTMLTable(header.size(), CSS_CLASS),
				_requestParameters(requestParameters),
				_searchForm(searchForm)
			{
				if (requestParameters.maxSize)
				{
					next = v.size() == *requestParameters.maxSize + 1;
					if (next && !v.empty())
						v.pop_back();
					size = v.size();
				}

				std::stringstream s;
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
							h.insert(make_pair(
									_requestParameters._getParameterCode(_PARAMETER_ORDER_FIELD),
									it->first
							)	);
							h.insert(make_pair(
									_requestParameters._getParameterCode(_PARAMETER_RAISING_ORDER),
									boost::lexical_cast<std::string>(
										_requestParameters.orderField == it->first ?
										!_requestParameters.raisingOrder :
										true
							)	)	);
							s << HTMLModule::getHTMLLink(_searchForm.getURL(h), it->second);

							if (it->first == _requestParameters.orderField)
							{
								s << "&nbsp;" << HTMLModule::getHTMLImage(
									iconPath + (_requestParameters.raisingOrder ? "up" : "down") + ".png",
									_requestParameters.raisingOrder ? "V" : "^"
								);
							}
						}
						else
							s << it->second;
						s << "</th>";
						colspan = 1;
					}
				}
				_headers = s.str();

				_searchForm.addHiddenField(
					_requestParameters._getParameterCode(_PARAMETER_FIRST),
					boost::lexical_cast<std::string>(_requestParameters.first)
				);
				_searchForm.addHiddenField(
					_requestParameters._getParameterCode(_PARAMETER_ORDER_FIELD),
					boost::lexical_cast<std::string>(_requestParameters.orderField)
				);
				_searchForm.addHiddenField(
					_requestParameters._getParameterCode(_PARAMETER_RAISING_ORDER),
					boost::lexical_cast<std::string>(_requestParameters.raisingOrder)
				);
				if(_requestParameters.maxSize)
				{
					_searchForm.addHiddenField(
						_requestParameters._getParameterCode(_PARAMETER_MAX_SIZE),
						boost::lexical_cast<std::string>(*_requestParameters.maxSize)
					);
				}
			}



			virtual std::string close();
		};
	}
}

#endif // SYNTHESE_ResultHTMLTable_h__

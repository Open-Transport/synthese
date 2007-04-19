
/** ResultHTMLTable class header.
	@file ResultHTMLTable.h

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

#ifndef SYNTHESE_ResultHTMLTable_h__
#define SYNTHESE_ResultHTMLTable_h__

#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

namespace synthese
{
	namespace html
	{
		/** ResultHTMLTable class.
			@ingroup m05
		*/
		class ResultHTMLTable : public HTMLTable
		{
		private:
			static const std::string _PARAMETER_FIRST;
			static const std::string _PARAMETER_ORDER_FIELD;
			static const std::string _PARAMETER_RAISING_ORDER;
			static const std::string _PARAMETER_MAX_SIZE;

			const int			_maxSize;
			const int			_first;
			const std::string	_orderField;
			const bool			_raisingOrder;
			const bool			_next;
			const int			_size;
			
		public:
			typedef std::vector<std::pair<std::string, std::string> > HeaderVector;
			static const int UNLIMITED_SIZE;

			struct RequestParameters
			{
				int					maxSize;
				int					first;
				std::string			orderField;
				bool				raisingOrder;
				RequestParameters() 
					: first(0)
					, maxSize(30)
					, raisingOrder(true)
				{}
			};

			struct ResultParameters
			{
				bool				next;
				int					size;
				ResultParameters()
					: next(false)
					, size(UNLIMITED_SIZE)
				{}
			};

		protected:
			HTMLForm		_searchForm;
			
		public:
			/** Constructor.
				@param header : vector of pairs field code / col caption. If field code is non empty then the searchRequest can be reloaded with an ordering by the column.
				@param searchRequest The request to use to fill a similar table with other parameters
				@param actionRequest A request to launch by the table content (NULL = no action request)
				@param selectFieldName A first col with radio buttons will be drawn, named by the parameter. If empty then no radio button.
			*/
			ResultHTMLTable(const HeaderVector& header
				, const HTMLForm& searchForm
				, const RequestParameters& requestParameters
				, const ResultParameters& resultParameters
				, std::string iconPath = std::string()
				);

			virtual std::string close();

			static RequestParameters getParameters(
				const std::map<std::string, std::string>& map
				, const std::string defaultOrderField = std::string()
				, int defaultMaxSize = UNLIMITED_SIZE
				);
			
			template <class T>
			static ResultParameters getParameters(
				const RequestParameters& requestParameters
				, std::vector<boost::shared_ptr<T> >& result);

			static std::map<std::string, std::string> getParametersMap(
				const RequestParameters& requestParameters);
			
		};

		template <class T>
		ResultHTMLTable::ResultParameters ResultHTMLTable::getParameters(
			const RequestParameters& requestParameters
			, std::vector<boost::shared_ptr<T> >& result
		){
			ResultParameters p;
			p.next = (requestParameters.maxSize != UNLIMITED_SIZE) && (result.size() > requestParameters.maxSize);
			if (p.next)
				result.pop_back();
			p.size = result.size();
			return p;
		}
	}
}

#endif // SYNTHESE_ResultHTMLTable_h__

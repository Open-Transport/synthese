
/** ForEachExpression class header.
	@file ForEachExpression.hpp

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

#ifndef SYNTHESE_cms_ForEachExpression_hpp__
#define SYNTHESE_cms_ForEachExpression_hpp__

#include "Expression.hpp"

#include "WebpageContent.hpp"

namespace synthese
{
	namespace cms
	{
		/** ForEachExpression class.
			@ingroup m36
		*/
		class ForEachExpression:
			public Expression
		{
		private:
			static const std::string DATA_ITEMS_COUNT;
			static const std::string DATA_RANK;
			static const std::string PARAMETER_EMPTY;
			static const std::string PARAMETER_SORT_DOWN;
			static const std::string PARAMETER_SORT_UP;
			static const std::string PARAMETER_TEMPLATE;


			std::string _arrayCode;
			WebpageContent _pageCode;
			WebpageContent _inlineTemplate;
			WebpageContent _emptyTemplate;
			WebpageContent _sortUpTemplate;
			WebpageContent _sortDownTemplate;
			typedef std::vector<std::pair<std::string, WebpageContent> > Parameters;
			Parameters _parameters;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor by partial string parsing
			ForEachExpression(
				std::string::const_iterator& it,
				std::string::const_iterator end
			);



			virtual void display(
				std::ostream& stream,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;



			void _displayItem(
				std::ostream& stream,
				const server::Request& request,
				const Webpage& page,
				const util::ParametersMap& baseParametersMap,
				const util::ParametersMap& item,
				util::ParametersMap& variables,
				const Webpage* templatePage,
				size_t& rank,
				size_t itemsCount
			) const;


			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;
		};
}	}

#endif // SYNTHESE_cms_ForEachExpression_hpp__


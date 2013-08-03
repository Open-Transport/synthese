
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

#include "CMSScript.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace server
	{
		class FunctionAPI;
	}

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
			static const std::string DATA_DEPTH;
			static const std::string DATA_RECURSIVE_CONTENT;
			static const std::string PARAMETER_EMPTY;
			static const std::string PARAMETER_SORT_DOWN;
			static const std::string PARAMETER_SORT_UP;
			static const std::string PARAMETER_SORT_ALGO;
			static const std::string PARAMETER_MAX_PER_SORT_KEY;
			static const std::string PARAMETER_RESULTS_IN_A_SUBMAP;
			static const std::string PARAMETER_TEMPLATE;
			static const std::string PARAMETER_RECURSIVE;

			struct Item
			{
				std::string key;
				boost::shared_ptr<Expression> index;
			};
			typedef std::vector<Item> Items;
			Items _variable;

			CMSScript _pageCode;
			CMSScript _inlineTemplate;
			CMSScript _emptyTemplate;
			CMSScript _sortUpTemplate;
			CMSScript _sortDownTemplate;
			CMSScript _sortAlgoNode;
			CMSScript _maxPerSortKey;
			bool _resultsInASubmap;
			typedef std::vector<std::pair<std::string, CMSScript> > Parameters;

			Parameters _parameters;
			bool _recursive;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor by partial string parsing
			ForEachExpression(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				bool ignoreWhiteChars
			);



			virtual void display(
				std::ostream& stream,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables,
				size_t depth
			) const;



			void _displayItem(
				std::ostream& stream,
				const server::Request& request,
				const Webpage& page,
				const util::ParametersMap& baseParametersMap,
				const util::ParametersMap& item,
				util::ParametersMap& variables,
				boost::optional<const CMSScript&> templateContent,
				const Webpage* templatePage,
				size_t& rank,
				size_t itemsCount,
				const std::string& recursiveContent,
				size_t depth
			) const;


			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;

			virtual server::FunctionAPI getAPI() const;
		};
}	}

#endif // SYNTHESE_cms_ForEachExpression_hpp__


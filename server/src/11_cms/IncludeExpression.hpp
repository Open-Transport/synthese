
/** IncludeExpression class header.
	@file IncludeExpression.hpp

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

#ifndef SYNTHESE_cms_IncludeExpression_hpp__
#define SYNTHESE_cms_IncludeExpression_hpp__

#include "Expression.hpp"

#include "CMSScript.hpp"

namespace synthese
{
	namespace cms
	{
		/** IncludeExpression class.
			@ingroup m36
		*/
		class IncludeExpression:
			public Expression
		{
		private:
			std::string _siteURL;
			CMSScript _pageName;
			typedef std::vector<std::pair<std::string, CMSScript> > Parameters;
			Parameters _parameters;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor by partial string parsing
			IncludeExpression(
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


			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;
		};
}	}

#endif // SYNTHESE_cms_IncludeExpression_hpp__


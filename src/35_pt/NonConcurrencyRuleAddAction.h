
/** NonConcurrencyRuleAddAction class header.
	@file NonConcurrencyRuleAddAction.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_NonConcurrencyRuleAddAction_H__
#define SYNTHESE_NonConcurrencyRuleAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace pt
	{
		/** NonConcurrencyRuleAddAction action class.
			@ingroup m35Actions refActions
		*/
		class NonConcurrencyRuleAddAction:
			public util::FactorableTemplate<server::Action, NonConcurrencyRuleAddAction>
		{
		public:
			static const std::string PARAMETER_HIDDEN_LINE_ID;
			static const std::string PARAMETER_PRIORITY_LINE_ID;
			static const std::string PARAMETER_DURATION;

		private:
			boost::shared_ptr<pt::CommercialLine> _hiddenLine;
			boost::shared_ptr<pt::CommercialLine> _priorityLine;
			boost::posix_time::time_duration	_duration;


		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setPriorityLine(boost::shared_ptr<const pt::CommercialLine> value);
			void setHiddenLine(boost::shared_ptr<const pt::CommercialLine> value);
		};
	}
}

#endif // SYNTHESE_NonConcurrencyRuleAddAction_H__

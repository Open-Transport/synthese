
/** RealTimePTDataInterSYNTHESE class header.
	@file RealTimePTDataInterSYNTHESE.hpp

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

#ifndef SYNTHESE_pt_RealTimePTDataInterSYNTHESE_hpp__
#define SYNTHESE_pt_RealTimePTDataInterSYNTHESE_hpp__

#include "FactorableTemplate.h"
#include "InterSYNTHESEContent.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;

		//////////////////////////////////////////////////////////////////////////
		//// RealTimePTDataInterSYNTHESE class.
		///	@ingroup m35
		///
		///	Messages schema :
		///		- service id
		///		- loop of :
		///			- stop rank
		///			- arrival time
		///			- departure time
		///			- stop id (0 if NULL)
		/// the data fields are separated by , 
		class RealTimePTDataInterSYNTHESE:
			public util::FactorableTemplate<inter_synthese::InterSYNTHESESyncTypeFactory, RealTimePTDataInterSYNTHESE>
		{
			static const std::string FIELD_SEPARATOR;

		public:
			class Content:
				public inter_synthese::InterSYNTHESEContent
			{
			public:
				typedef std::vector<bool> RanksToSync;

			private:
				const SchedulesBasedService& _service;
				boost::optional<const RanksToSync&> _ranksToSync;

			public:
				Content(
					const SchedulesBasedService& service,
					boost::optional<const RanksToSync&> ranksToSync = boost::optional<const RanksToSync&>()
				);

				virtual std::string getPerimeter() const;
				virtual std::string getContent() const;
			};

			RealTimePTDataInterSYNTHESE();

			virtual bool mustBeEnqueued(
				const std::string& configPerimeter,
				const std::string& messagePerimeter
			) const;

			virtual void initSync(
			) const;

			virtual bool sync(
				const std::string& parameter,
				const inter_synthese::InterSYNTHESEIdFilter* idFilter
			) const;

			virtual void closeSync(
			) const;

			virtual void initQueue(
				const inter_synthese::InterSYNTHESESlave& slave,
				const std::string& perimeter
			) const;
		};
	}
}

#endif // SYNTHESE_pt_RealTimePTDataInterSYNTHESE_hpp__


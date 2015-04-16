
/** DeactivationPTDataInterSYNTHESE class header.
	@file DeactivationPTDataInterSYNTHESE.hpp
	
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

#ifndef SYNTHESE_pt_DeactivationPTDataInterSYNTHESE_hpp__
#define SYNTHESE_pt_DeactivationPTDataInterSYNTHESE_hpp__

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
		//// DeactivationPTDataInterSYNTHESE class.
		///	@ingroup m35
		///
		///	Messages schema :
		///		- service id
		class DeactivationPTDataInterSYNTHESE:
			public util::FactorableTemplate<inter_synthese::InterSYNTHESESyncTypeFactory, DeactivationPTDataInterSYNTHESE>
		{
			static const std::string FIELD_SEPARATOR;

		public:
			class Content:
				public inter_synthese::InterSYNTHESEContent
			{

			private:
				const SchedulesBasedService& _service;

			public:
				Content(
					const SchedulesBasedService& service
				);

				virtual std::string getPerimeter() const;
				virtual std::string getContent() const;
				virtual boost::posix_time::ptime getExpirationTime() const;
			};

			DeactivationPTDataInterSYNTHESE();

			virtual bool mustBeEnqueued(
				const std::string& configPerimeter,
				const std::string& messagePerimeter
			) const;

			virtual void initSync(
			) const;

			virtual bool sync(
				const std::string& parameter
			) const;

			virtual void closeSync(
			) const;

			virtual void initQueue(
				const inter_synthese::InterSYNTHESESlave& slave,
				const std::string& perimeter
			) const;

			virtual SortedItems sort(const RandomItems& randItems) const;
		};
}	}

#endif // SYNTHESE_pt_DeactivationPTDataInterSYNTHESE_hpp__

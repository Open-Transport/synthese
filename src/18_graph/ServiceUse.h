
/** ServiceUse class header.
	@file ServiceUse.h

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

#ifndef SYNTHESE_env_ServiceUse_h__
#define SYNTHESE_env_ServiceUse_h__

#include "ServicePointer.h"

namespace synthese
{
	namespace graph
	{
		class Edge;
		class Vertex;

		/** ServiceUse class.
			@ingroup m18
		*/
		class ServiceUse : public ServicePointer
		{
		private:
			//! @name Second step data
			//@{
				const Edge*				_secondEdge;
				boost::posix_time::ptime			_secondActualDateTime;
				const Vertex*			_secondRTVertex;
			//@}

		public:
			ServiceUse(const ServicePointer& servicePointer, const Edge* edge);
			
			ServiceUse();
			

			//! @name Getters
			//@{
				const Edge*				getSecondEdge()				const;
				const boost::posix_time::ptime&	getSecondActualDateTime()	const;
				const Vertex*			getSecondVertex()			const;
			//@}

			//! @name Queries
			//@{
				const Edge*				getDepartureEdge()			const;
				const Edge*				getArrivalEdge()			const;
				const Vertex*			getDepartureRTVertex()			const;
				const Vertex*			getArrivalRTVertex()			const;
				const boost::posix_time::ptime&	getDepartureDateTime()		const;
				const boost::posix_time::ptime&	getArrivalDateTime()		const;
				boost::posix_time::time_duration	getDuration()				const;
				double					getDistance()				const;
				UseRule::RunPossibilityType		isUseRuleCompliant(
				)	const;
				boost::posix_time::ptime			getReservationDeadLine()	const;
			//@}

			//! @name Update
			//@{
				void shift(boost::posix_time::time_duration duration);
				void reverse();
			//@}
		};
	}
}

#endif // SYNTHESE_env_ServiceUse_h__

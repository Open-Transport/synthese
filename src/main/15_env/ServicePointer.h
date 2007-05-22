
/** ServicePointer class header.
	@file ServicePointer.h

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

#ifndef SYNTHESE_env_ServicePointer_h__
#define SYNTHESE_env_ServicePointer_h__

#include "04_time/DateTime.h"

namespace synthese
{
	namespace env
	{
		class Service;
		class Edge;

		/** Service Pointer class.
			@ingroup m15

			A service pointer determinates :
				- a service
				- a calendar date
				- an edge
				- a direction (departure from the edge or arrival at the edge)
		*/
		class ServicePointer
		{
		public:
			/** Service determination method.
				- DEPARTURE_TO_ARRIVAL = the service is chosen from a presence time before a departure. The arrival will be chosen in the following edges.
				- ARRIVAL_TO_DEPARTURE = the service is chosen from a presence time after an arrival. The departure will be chosen in the preceding edges.
			*/
			typedef enum
			{
				DEPARTURE_TO_ARRIVAL
				, ARRIVAL_TO_DEPARTURE
				, NULL_POINTER
			} DeterminationMethod;

		private:
			//! @name Initial parameters
			//@{
				const DeterminationMethod	_determinationMethod;
				const env::Edge*			_edge;
			//@}


			//! @name Result elements
			//@{
				const Service*				_service;
				time::DateTime				_originDateTime;
				time::DateTime				_actualTime;
				int							_serviceIndex;
			//@}
			
		public:
			ServicePointer();
			ServicePointer(DeterminationMethod method, const env::Edge* edge);

			//! @name Setters
			//@{
				void	setActualTime(const time::DateTime& dateTime);
				void	setService(const Service* service);
				void	setOriginDateTime(const time::DateTime& dateTime);
				void	setServiceIndex(int index);
			//@}

			//! @name Getters
			//@{
				const Service*			getService()		const;
				const time::DateTime&	getActualDateTime()	const;
				const time::DateTime&	getOriginDateTime()	const;
				DeterminationMethod		getMethod()			const;
				int						getServiceIndex()	const;
			//@}

			//! @name Queries
			//@{
				bool				isReservationRuleCompliant(const time::DateTime& computingDateTime)	const;
				const env::Edge*	getEdge()															const;
			//@}


		};
	}
}

#endif // SYNTHESE_env_ServicePointer_h__

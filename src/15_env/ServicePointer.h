
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

#include "15_env/Types.h"

namespace synthese
{
	namespace env
	{
		class Service;
		class Edge;

		/** Service Pointer class.
			@ingroup m35

			A service pointer determinates :
				- a service
				- a calendar date
				- an edge
				- a direction (departure from the edge or arrival at the edge)
		*/
		class ServicePointer
		{
		protected:
			//! @name Initial parameters
			//@{
				AccessDirection				_determinationMethod;
				const env::Edge*			_edge;
			//@}

			//! @name Result elements
			//@{
				const Service*				_service;
				time::DateTime				_originDateTime;
				time::DateTime				_actualTime;
				int							_serviceIndex;
			//@}

				int							_range;

		public:
			ServicePointer(AccessDirection method, const env::Edge* edge = NULL);

			//! @name Setters
			//@{
				void	setActualTime(const time::DateTime& dateTime);
				void	setService(const Service* service);
				void	setOriginDateTime(const time::DateTime& dateTime);
				void	setServiceIndex(int index);
				void	setServiceRange(int duration);
			//@}

			//! @name Getters
			//@{
				const Service*			getService()			const;
				const time::DateTime&	getActualDateTime()		const;
				const time::DateTime&	getOriginDateTime()		const;
				AccessDirection			getMethod()				const;
				int						getServiceIndex()		const;
				int						getServiceRange()		const;
			//@}

			//! @name Queries
			//@{
				/** Test the respect of the reservation rules.
					@param computingDateTime date of the request
					@return bool true if the service can be used
					@warning If the service is determinated by ARRIVAL_TO_DEPARTURE, then this method always aswers true, because the reservation
						deadline depends only on the departure time, which is not known at this stage. Use ServiceUse::isReservationRuleCompliant method to validate the reservation
						deadline respect.
					@author Hugues Romain
					@date 2007
				
					@todo Handle mixed reservation type
				*/
				virtual bool isReservationRuleCompliant(
					const time::DateTime& computingDateTime
				)	const;
				const env::Edge*	getEdge()															const;
			//@}


		};
	}
}

#endif // SYNTHESE_env_ServicePointer_h__


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

#include "GraphTypes.h"
#include "UseRule.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace graph
	{
		class Service;
		class Edge;
		class Vertex;

		/** Service Pointer class.
			@ingroup m18
			@author Hugues Romain

			A service pointer determines :
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
				AccessDirection		_determinationMethod;
				const Edge*			_edge;
				const Vertex*		_RTVertex;
				std::size_t			_userClassRank;
				bool				_RTData;
			//@}

			//! @name Result elements
			//@{
				const Service*		_service;
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_actualTime;
				boost::posix_time::ptime		_theoreticalTime;
				boost::posix_time::time_duration	_range;
			//@}
			
		public:
			ServicePointer(
				bool RTData,
				AccessDirection method,
				std::size_t userClassRank,
				const Edge* edge = NULL
			);
			ServicePointer();

			//! @name Setters
			//@{
				void	setActualTime(const boost::posix_time::ptime& dateTime) { _actualTime = dateTime; }
				void	setTheoreticalTime(const boost::posix_time::ptime& dateTime) { _theoreticalTime = dateTime; }
				void	setService(const Service* service) { _service = service; }
				void	setOriginDateTime(const boost::posix_time::ptime& dateTime){ _originDateTime = dateTime; }
				void	setServiceRange(boost::posix_time::time_duration duration){ _range = duration; }
				void	setRealTimeVertex(const Vertex* value){ _RTVertex = value; }
			//@}

			//! @name Getters
			//@{
				const Edge*				getEdge()	const { return _edge; }
				const Vertex*			getRealTimeVertex()			const { return _RTVertex; }
				const Service*			getService()				const { return _service; }
				const boost::posix_time::ptime&	getActualDateTime()			const { return _actualTime; }
				const boost::posix_time::ptime&	getTheoreticalDateTime()	const { return _theoreticalTime; }
				const boost::posix_time::ptime&	getOriginDateTime()			const { return _originDateTime; }
				AccessDirection			getMethod()					const { return _determinationMethod; }
				boost::posix_time::time_duration	getServiceRange() const { return _range; }
				std::size_t				getUserClassRank() const { return _userClassRank; }
				bool					getRTData() const { return _RTData; }
			//@}

			//! @name Queries
			//@{
				/** Test the respect of the reservation rules.
					@return bool true if the service can be used
					@warning If the service is determined by ARRIVAL_TO_DEPARTURE, then this method always aswers true, because the reservation
						deadline depends only on the departure time, which is not known at this stage. Use ServiceUse::isReservationRuleCompliant method to validate the reservation
						deadline respect.
					@author Hugues Romain
					@date 2007
				*/
				virtual UseRule::RunPossibilityType isUseRuleCompliant(
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the use rule applicable to the user registered in the service use.
				/// @return the use rule
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				const UseRule& getUseRule() const;
			//@}
		};
	}
}

#endif // SYNTHESE_env_ServicePointer_h__

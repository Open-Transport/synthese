
/** ServiceComposition class header.
	@file ServiceComposition.hpp

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

#ifndef SYNTHESE_pt_operations_ServiceComposition_hpp__
#define SYNTHESE_pt_operations_ServiceComposition_hpp__

#include "Composition.hpp"
#include "SchedulesBasedService.h"

namespace synthese
{
	namespace pt
	{
		class ScheduledService;
	}

	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		/// Composition class.
		/// Allocation of one or more vehicles (train) to one service
		/// (ScheduledService), valid on one or more days (Calendar)
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m37
		/// @author Hugues Romain
		class ServiceComposition:
			public Composition
		{
		private:
			const pt::ScheduledService* _service;
			pt::SchedulesBasedService::ServedVertices _servedVertices;

		public:
			ServiceComposition(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
				void setService(const pt::ScheduledService* value){ _service = value; }
				void setServedVertices(const pt::SchedulesBasedService::ServedVertices& value){ _servedVertices = value; }
			//@}

			//! @name Getters
			//@{
				const pt::ScheduledService* getService() const { return _service; }
				const pt::SchedulesBasedService::ServedVertices& getServedVertices() const { return _servedVertices; }
			//@}
		};
	}
}

#endif

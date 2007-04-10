
/** BroadcastPoint class header.
	@file BroadcastPoint.h

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

#ifndef SYNTHESE_BROADCAST_POINT_H
#define SYNTHESE_BROADCAST_POINT_H

#include <string>
#include <vector>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace	;
		class PhysicalStop;
	}

	namespace departurestable
	{
		class BroadcastPointAlarmBroadcast;

		/** Point de diffusion d'informations.
			@ingroup m15

			Un point de diffusion est un lieu où peuvent être disposés des écrans de diffusion d'information (DisplayScreen).

			Le point de diffusion appartient nécessairement à un lieu (Place) et chaque terminal d'affichage appartient à un point de diffusion.

			Par héritage, un @ref PhysicalStop "arrêt physique" est automatiquement un point de diffusion 
		*/
		class BroadcastPoint : public util::Registrable<uid,BroadcastPoint>
		{
		private:
			std::string	_name;
			boost::shared_ptr<const env::ConnectionPlace>	_place;
			boost::shared_ptr<const env::PhysicalStop>	_physicalStop;	//!< Indicates that the broadcastpoint corresponds to a real physical stop

			std::vector<boost::shared_ptr<BroadcastPointAlarmBroadcast> >	_alarms;

		public:
			BroadcastPoint();

			void setName(const std::string& name);
			void setConnectionPlace(boost::shared_ptr<const env::ConnectionPlace> place);
			void setPhysicalStop(boost::shared_ptr<const env::PhysicalStop> physicalStop);

			const std::string getName() const;
			boost::shared_ptr<const env::ConnectionPlace> getConnectionPlace() const;
			boost::shared_ptr<const env::PhysicalStop> getPhysicalStop() const;

			std::string	getFullName()	const;
		};
	}
}

#endif


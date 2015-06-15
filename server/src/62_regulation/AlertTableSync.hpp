//////////////////////////////////////////////////////////////////////////
///	AlertTableSync class header.
///	@file AlertTableSync.hpp
///	@author Camille Hue
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_regulation_AlertTableSync_hpp__
#define SYNTHESE_regulation_AlertTableSync_hpp__

#include "Alert.hpp"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace regulation
	{

		class AlertTableSync:
            // TODO : ne pas tout monter en RAM : ConditionalTableSync 
			public db::DBDirectTableSyncTemplate<
				AlertTableSync,
				Alert
			>
		{
		public:

			//! @name Services
			//@{
				static SearchResult Search(
					util::Env& env,
					boost::optional<AlertType> alertType
				);

			//@}
		};
	}
}

#endif // SYNTHESE_AlertTableSync_hpp__

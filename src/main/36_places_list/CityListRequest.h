
/** CityListRequest class header.
	@file CityListRequest.h

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

#ifndef SYNTHESE_CityListRequest_H__
#define SYNTHESE_CityListRequest_H__

#include "30_server/Function.h"

namespace synthese
{
	namespace placeslist
	{

		/** City list request.
			@ingroup m36

		*/
		class CityListRequest : public server::Request
		{
		private:
			std::string _input;
			size_t _n;

		protected:
			ParametersMap _getParametersMap() const;
			void _setFromParametersMap(const ParametersMap& map);

		public:
			void _run(std::ostream& stream) const;

		};
	}
}

#endif // SYNTHESE_CityListRequest_H__


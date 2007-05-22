
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

#include "15_env/ServicePointer.h"
#include "15_env/SquareDistance.h"

namespace synthese
{
	namespace env
	{
		class Edge;

		/** ServiceUse class.
			@ingroup m15
		*/
		class ServiceUse
		{
		private:
			//! @name First step data
			//@{
				const ServicePointer	_servicePointer;
			//@}

			//! @name Second step data
			//@{
				const Edge*				_edge;
				time::DateTime			_actualDateTime;
			//@}

			int _continuousServiceRange;
			SquareDistance _squareDistance;

		public:
			ServiceUse(const ServicePointer& servicePointer, const Edge* edge);

			//! @name Getters
			//@{
				const Edge*				getEdge()				const;
				const ServicePointer&	getServicePointer()		const;
				const time::DateTime&	getActualDateTime()		const;
			//@}

			//! @name Queries
			//@{
				const Edge*				getDepartureEdge()		const;
				const Edge*				getArrivalEdge()		const;
				const time::DateTime&	getDepartureDateTime()	const;
				const time::DateTime&	getArrivalDateTime()	const;
			//@}
		};
	}
}

#endif // SYNTHESE_env_ServiceUse_h__

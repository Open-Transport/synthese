
/** Compliance class header.
	@file Compliance.h

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

#ifndef SYNTHESE_COMPLIANCE_H
#define SYNTHESE_COMPLIANCE_H

#include "Registrable.h"

#include <boost/logic/tribool.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace env
	{
		class ReservationRule;

		/** Generic compliance value class.
			@ingroup m35
		*/
		class Compliance
		:	public virtual util::Registrable
		{
		private:
			boost::logic::tribool						_compliant;			//!< Value (see the derivated class for the signification of the 3 availables status)
			int											_capacity;			//!< Maximal person number which can be served (0 = unlimited)
			boost::shared_ptr<const ReservationRule>	_reservationRule;	//!< Link to the reservation rule to respect to use the compliance (NULL = no needed reservation, reservation service unavailable)

		protected:

			Compliance(
				boost::logic::tribool compliant
				, int capacity
				, boost::shared_ptr<const ReservationRule> reservationRule = boost::shared_ptr<ReservationRule>()
			);
			
		public:

			~Compliance();

			//! @name Getters/Setters
			//@{
				int getCapacity () const;
				const boost::logic::tribool& isCompliant () const;
				boost::shared_ptr<const ReservationRule> getReservationRule() const;
			//@}

			//! @name Setters
			//@{
				void setCapacity (int capacity);
				void setCompliant (const boost::logic::tribool& status);
				void setReservationRule(boost::shared_ptr<const ReservationRule> value);
			//@}

			//! @name Queries
			//@{
				bool isCompatibleWith(const Compliance& compliance) const;
				bool isCompatibleWith(const boost::logic::tribool& value) const;
			//@}

		};
	}
}

#endif

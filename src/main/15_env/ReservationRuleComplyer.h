
/** ReservationRuleComplyer class header.
	@file ReservationRuleComplyer.h

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

#ifndef SYNTHESE_ENV_RESERVATIONRULECOMPLYER_H
#define SYNTHESE_ENV_RESERVATIONRULECOMPLYER_H


#include <vector>


namespace synthese
{


	namespace env
	{

		class ReservationRule;


		/** 
		@ingroup m15
		*/
		class ReservationRuleComplyer
		{

		private:

			const ReservationRuleComplyer* _parent;

			ReservationRule* _reservationRule;

		protected:

			ReservationRuleComplyer (const ReservationRuleComplyer* parent = NULL, 
						ReservationRule* reservationRule = 0);

		public:

			~ReservationRuleComplyer ();


			//! @name Getters/Setters
			//@{
			const ReservationRule* getReservationRule () const;
			
			void	setReservationRule (ReservationRule* reservationRule);
			void	setParent(const ReservationRuleComplyer* parent);
			//@}

			//! @name Query methods.
			//@{
			//@}
		    
			//! @name Update methods.
			//@{
			//@}
		    
		    
		};
	}
}

#endif 	    

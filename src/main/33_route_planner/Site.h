
/** Site class header.
	@file Site.h

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

#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

#include "04_time/Date.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <string>
#include <set>

namespace synthese
{
	namespace env
	{
		class CommercialLine;
	}

	namespace interfaces
	{
		class Interface;
	}

	namespace routeplanner
	{

		/** Route planning workspace.
			@ingroup m33
			@author Hugues Romain
			@date 2005-2006
		*/
		class Site : public synthese::util::Registrable<uid, Site>
		{
			//! \name Properties
			//@{
				const interfaces::Interface*	_interface;
				std::string						_name;  //!< Name of the site
				synthese::time::Date			_startValidityDate;   
				synthese::time::Date			_endValidityDate;   
			//@}

			//! \name Environment
			//@{
				std::set<env::CommercialLine*>	_lines;
			//@}

			//! \name Filters
			//@{
				bool _onlineBookingAllowed;
				bool _pastSolutionsDisplayed;
			//@}

		public:
			//! \name Constructeur
			//@{
			Site( const uid& uid);
			//@}

			//! \name Modificateurs
			//@{
				void setInterface ( const synthese::interfaces::Interface* interf);
				void setStartDate ( const synthese::time::Date& dateDebut );
				void setEndDate ( const synthese::time::Date& dateFin );
				void setOnlineBookingAllowed ( const bool valeur );
				void setPastSolutionsDisplayed ( bool );
				void setName(const std::string& name);
			//@}

			//! \name Accesseurs
			//@{
//			const std::string& getIdentifiant() const;
			const interfaces::Interface* getInterface() const;
//			const synthese::time::Date& getDateDebut() const;
//			const synthese::time::Date& getDateFin() const;
			bool onlineBookingAllowed() const;
//			bool getSolutionsPassees() const;
			//@}

			//! \name Calculateurs
			//@{
			bool dateControl() const;
			//@}


		};
	}
}

#endif


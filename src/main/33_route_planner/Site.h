
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
#include "01_util/Constants.h"

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
				boost::shared_ptr<const interfaces::Interface>	_interface;
				std::string										_name;  //!< Name of the site
				time::Date										_startValidityDate;
				time::Date										_endValidityDate;
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

			//! \name Cached used days
			//@{
				time::Date _minDateInUse;
				time::Date _maxDateInUse;
			//@}

		public:
			static const std::string TEMPS_MIN_CIRCULATIONS;
			static const std::string TEMPS_MAX_CIRCULATIONS;


			//! \name Constructeur
			//@{
				Site(uid uid = UNKNOWN_VALUE);
			//@}

			//! \name Setters
			//@{
				void setInterface (boost::shared_ptr<const interfaces::Interface> interf);
				void setStartDate ( const synthese::time::Date& dateDebut );
				void setEndDate ( const synthese::time::Date& dateFin );
				void setOnlineBookingAllowed ( const bool valeur );
				void setPastSolutionsDisplayed ( bool );
				void setName(const std::string& name);
			//@}

			//! \name Getters
			//@{
				boost::shared_ptr<const interfaces::Interface> getInterface() const;
				bool getOnlineBookingAllowed() const;
				bool getPastSolutionsDisplayed() const;
				const time::Date& getStartDate() const;
				const time::Date& getEndDate() const;
				const time::Date& getMinDateInUse () const;
				const time::Date& getMaxDateInUse () const;
			//@}

			// \name Modifiers
			//@{
				void updateMinMaxDatesInUse (const time::Date& newDate, bool marked);
			//@}

			//! \name Queries
			//@{
				bool dateControl() const;

				/** Interprets date from text and environment data.
					@param text Text to interpret
					@return Interpreted date
					@author Hugues Romain
					@date 2005-2006
					@warning The parameters are not verified

					The returned date depends on the text :
						- date au format texte interne : date transcrite (no control) (ex : 20070201 => 1/2/2007)
						- commande de date classique (synthese::time::TIME_MIN ('m'), synthese::time::TIME_MAX ('M'), synthese::time::TIME_CURRENT ('A'), synthese::time::TIME_UNKNOWN ('?')) : la date correspondante (voir synthese::time::Date::setDate())
						- texte vide : identical to synthese::time::TIME_CURRENT
						- synthese::time::TIME_MIN_CIRCULATIONS ('r') : First date where at least one service runs (see Environment::getMinDateInUse())
						- TEMPS_MAX_CIRCULATIONS ('R') : Last date where at least one service runs (see Environment::getMaxDateInUse())

					The following assertion is always assumed : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
				*/	
				time::Date interpretDate( const std::string& text ) const;
			//@}

		};
	}
}

#endif

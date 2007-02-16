
/** DisplayScreen class header.
	@file DisplayScreen.h

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

#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

#include <set>
#include <string>
#include <ostream>
#include <map>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"

#include "15_env/Types.h"

#include "34_departures_table/Types.h"
#include "34_departures_table/ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "34_departures_table/StandardArrivalDepartureTableGenerator.h"


namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class Line;
	}

	namespace departurestable
	{
		class BroadcastPoint;
		class DisplayType;

		/** Terminal d'affichage.
			@ingroup m34

			Un terminal d'affichage est un dispositif mat�riel d�di� � l'affichage non interactif d'informations provenant de SYNTHESE (exemple: tableau de d�parts).
			Un terminal d'affichage est reli� � un emplacement (exemple : arr�t logique).
			On consid�re par hypoth�se que l'arr�t logique li� correspond � la fois � l'emplacement du panneau et � la d�finition premi�re de ce qui y est affich�.
			L'emplacement est pr�cis� par un champ texte ("tableau de d�part situ� au-dessus de l'escalier nord") ainsi que des coordonn�es g�ographiques permettant de le situer sur une carte (ces donn�es sont facultatives et ne servent qu'� d�signer le terminal dans le cadre d'un composant d'administration par exemple).
			A un terminal d'affichage sont reli�s des clauses visant � d�finir le contenu qui y est diffus� ainsi que le format du contenu (protocole, charte graphique, etc...) :
				- Nature de l'affichage (fonction SYNTHESE) :
						- Tableau de d�part chronologique
						- Tableau de d�part � destination privil�gi�e
						- Tableau d'arriv�e chronologique
						- Tableau d'arriv�e � provenance privil�gi�e
				- Format de sortie (d�fini par l'interface utilis�e)
				- Param�trage du contenu g�n�r� par la fonction : 
						- Titre
						- Nombre de d�parts ou d'arriv�es
						- Filtre de terminus (sert � n'afficher que les lignes ayant pour origine le lieu affich�)
						- Filtre d'arr�t(s) physique(s)
						- Liste des points de passage � afficher
						- Autres param�tres (liste compl�te : voir IHM)

		*/
		class DisplayScreen : public util::Registrable<uid,DisplayScreen>
		{
			public:
			typedef enum { STANDARD_METHOD = 0, WITH_FORCED_DESTINATIONS_METHOD = 1 } GenerationMethod;

		protected:
			//! \name Localization
			//@{
				const BroadcastPoint*	_localization;		//!< Localization of the display screen (belongs to a place)
				std::string				_localizationComment;
			//@}

			//! \name Technical data
			//@{
				const DisplayType*	_displayType;
				int					_wiringCode;	// Display ID in a bus
			//@}

			//! \name Appearance
			//@{
				std::string			_title;				//!< Titre pour affichage
				int					_blinkingDelay;
				bool				_trackNumberDisplay;
				bool				_serviceNumberDisplay;
			//@}

			//! \name Content
			//@{
				env::PhysicalStopsSet		_physicalStops;				//!< Quai(s) affich�s
				bool						_allPhysicalStopsDisplayed;
				ForbiddenPlacesList			_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
				LineFilter					_forbiddenLines;
				DeparturesTableDirection	_direction;
				EndFilter					_originsOnly;
				DisplayedPlacesList			_displayedPlaces;
				int							_maxDelay;			//!< Max time length for the table
				int							_clearingDelay;
				int							_firstRow;
			//@}

			//!	\name Preselection
			//@{
				GenerationMethod			_generationMethod;
				DisplayedPlacesList			_forcedDestinations;	//!< Destinations � afficher absolument
				int							_destinationForceDelay;	//!< Dur�e pendant laquelle une destination est forc�e
			
			//@}

			//! \name Maintenance
			//@{
				int							_maintenanceChecksPerDay;
				bool						_maintenanceIsOnline;
				std::string					_maintenanceMessage;
			//@}


			//!	\name M�thodes prot�g�es
			//@{
				time::DateTime	_MomentFin(const time::DateTime& __MomentDebut)			const;
			//@}

		public:
			//!	\name Constructors/Destructors
			//@{
				DisplayScreen();
				~DisplayScreen(void);
			//@}

			//!	\name Setters
			//@{
				void	setLocalization(const BroadcastPoint*);
				void	setLocalizationComment(const std::string&);
				void	setType(const DisplayType*);
				void	setWiringCode(int);
				void	setTitle(const std::string&);
				void	setBlinkingDelay(int);
				void	setTrackNumberDisplay(bool value);
				void	setServiceNumberDisplay(bool value);
				void	clearPhysicalStops();
				void	addPhysicalStop(const env::PhysicalStop*);
				void	setAllPhysicalStopsDisplayed(bool value);
				void	clearForbiddenPlaces();
				void	addForbiddenPlace(const env::ConnectionPlace*);
				void	setDirection(DeparturesTableDirection direction);
				void	setOriginsOnly(EndFilter);
				void	clearDisplayedPlaces();
				void	addDisplayedPlace(const env::ConnectionPlace*);
				void	setMaxDelay(int);
				void	setClearingDelay(int delay);
				void	setFirstRow(int row);
				void	setGenerationMethod(GenerationMethod method);
				void	clearForcedDestinations();
				void	addForcedDestination(const env::ConnectionPlace*);
				void	removeForcedDestination(const env::ConnectionPlace*);
				void	setDestinationForceDelay(int);
				void	setMaintenanceChecksPerDay(int number);
				void	setMaintenanceIsOnline(bool value);
				void	setMaintenanceMessage(const std::string& message);
			//@}

			//!	\name Getters
			//@{
				const BroadcastPoint*			getLocalization()				const;
				const std::string&				getLocalizationComment()		const;
				const DisplayType*				getType()						const;				
				int								getWiringCode()					const;
				const std::string&				getTitle()						const;
				int								getBlinkingDelay()				const;
				bool							getTrackNumberDisplay()			const;
				bool							getServiceNumberDisplay()		const;
				bool							getAllPhysicalStopsDisplayed()	const;
				const ForbiddenPlacesList&		getForbiddenPlaces()			const;
				const LineFilter&				getForbiddenLines()				const;
				DeparturesTableDirection		getDirection()					const;
				EndFilter						getEndFilter()					const;
				const DisplayedPlacesList&		getDisplayedPlaces()			const;
				int								getMaxDelay()					const;
				int								getClearingDelay()				const;
				int								getFirstRow()					const;
				GenerationMethod				getGenerationMethod()			const;
				const DisplayedPlacesList&		getForcedDestinations()			const;
				int								getForceDestinationDelay()		const;
				int								getMaintenananceChecksPerDay()	const;
				bool							getIsOnline()					const;
				const std::string&				getMaintenanceMessage()			const;
				
			//@}

			//! \name Queries
			//@{
				ArrivalDepartureTableGenerator*	getGenerator(const time::DateTime& startTime) const;
				void							display(std::ostream& stream, const time::DateTime& date) const;
				void							recordSupervision(const std::string& supervisionValue) const;
				std::string						getFullName()	const;
				const env::PhysicalStopsSet&	getPhysicalStops(bool result=true)				const;

				std::map<std::string, std::pair<uid, std::string> > 
					getSortedAvaliableDestinationsLabels(const std::set<const env::ConnectionPlace*>& placesToAvoid) const;

			//@}

		};

	}
}

#endif

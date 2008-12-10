
/// DisplayScreen class header.
///	@file DisplayScreen.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

#include <set>
#include <string>
#include <ostream>
#include <map>

#include "Registrable.h"
#include "Registry.h"
#include "DateTime.h"
#include "DBLog.h"
#include "15_env/Types.h"
#include "34_departures_table/Types.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class Line;
	}

	namespace departurestable
	{
		class DisplayType;

		/** Terminal d'affichage.
			@ingroup m54

			Un terminal d'affichage est un dispositif matériel dédié à l'affichage non interactif d'informations provenant de SYNTHESE (exemple: tableau de départs).
			Un terminal d'affichage est relié à un emplacement (exemple : arrêt logique).
			On considère par hypothèse que l'arrêt logique lié correspond à la fois à l'emplacement du panneau et à la définition première de ce qui y est affiché.
			L'emplacement est précisé par un champ texte ("tableau de départ situé au-dessus de l'escalier nord") ainsi que des coordonnées géographiques permettant de le situer sur une carte (ces données sont facultatives et ne servent qu'à désigner le terminal dans le cadre d'un composant d'administration par exemple).
			A un terminal d'affichage sont reliés des clauses visant à définir le contenu qui y est diffusé ainsi que le format du contenu (protocole, charte graphique, etc...) :
				- Nature de l'affichage (fonction SYNTHESE) :
						- Tableau de départ chronologique
						- Tableau de départ à destination privilégiée
						- Tableau d'arrivée chronologique
						- Tableau d'arrivée à provenance privilégiée
				- Format de sortie (défini par l'interface utilisée)
				- Paramétrage du contenu généré par la fonction : 
						- Titre
						- Nombre de départs ou d'arrivées
						- Filtre de terminus (sert à n'afficher que les lignes ayant pour origine le lieu affiché)
						- Filtre d'arrêt(s) physique(s)
						- Liste des points de passage à afficher
						- Autres paramètres (liste complète : voir IHM)

		*/
		class DisplayScreen
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<DisplayScreen>	Registry;

			typedef enum {
				STANDARD_METHOD = 0
				, WITH_FORCED_DESTINATIONS_METHOD = 1
			} GenerationMethod;

			struct Complements
			{
				DisplayStatus				status;
				std::string					statusText;
				time::DateTime				lastOKStatus;
				time::DateTime				lastControl;
				DisplayDataControlResult	dataControl;
				std::string					dataControlText;
				time::DateTime				lastOKDataControl;
				Complements()
					: status(DISPLAY_STATUS_UNKNOWN), lastOKStatus(time::TIME_UNKNOWN)
					, lastControl(time::TIME_UNKNOWN), dataControl(DISPLAY_DATA_UNKNOWN)
					, lastOKDataControl(time::TIME_UNKNOWN) {}
			};

		protected:
			//! \name Localization
			//@{
				const env::PublicTransportStopZoneConnectionPlace*	_localization;		//!< Localization of the display screen (belongs to a place)
				std::string											_localizationComment;
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
				bool				_displayTeam;
			//@}

			//! \name Content
			//@{
				env::PhysicalStops			_physicalStops;				//!< Quai(s) affichés
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
				DisplayedPlacesList			_forcedDestinations;	//!< Destinations à afficher absolument
				int							_destinationForceDelay;	//!< Durée pendant laquelle une destination est forcée
			
			//@}

			//! \name Maintenance
			//@{
				int							_maintenanceChecksPerDay;
				bool						_maintenanceIsOnline;
				std::string					_maintenanceMessage;
				Complements					_complements;
			//@}


			//!	\name Méthodes protégées
			//@{
				time::DateTime	_MomentFin(const time::DateTime& __MomentDebut)			const;
			//@}

		public:
			//!	\name Constructors/Destructors
			//@{
				DisplayScreen(util::RegistryKeyType key = UNKNOWN_VALUE);
			//@}

			//!	\name Setters
			//@{
				void	setAllPhysicalStopsDisplayed(bool value);
				void	setBlinkingDelay(int);
				void	setClearingDelay(int delay);
				void	setDestinationForceDelay(int);
				void	setDirection(DeparturesTableDirection direction);
				void	setFirstRow(int row);
				void	setGenerationMethod(GenerationMethod method);
				void	setLocalization(const env::PublicTransportStopZoneConnectionPlace*);
				void	setLocalizationComment(const std::string&);
				void	setMaintenanceChecksPerDay(int number);
				void	setMaintenanceIsOnline(bool value);
				void	setMaintenanceMessage(const std::string& message);
				void	setMaxDelay(int);
				void	setOriginsOnly(EndFilter);
				void	setServiceNumberDisplay(bool value);
				void	setTitle(const std::string&);
				void	setTrackNumberDisplay(bool value);
				void	setType(const DisplayType*);
				void	setWiringCode(int);				
				void	setComplements(const Complements& complements);
				void	setDisplayTeam(bool value);
			//@}

			//! \name Modifiers
			//@{
				void	addDisplayedPlace(const env::PublicTransportStopZoneConnectionPlace*);
				void	addForbiddenPlace(const env::PublicTransportStopZoneConnectionPlace*);
				void	addForcedDestination(const env::PublicTransportStopZoneConnectionPlace*);
				void	addPhysicalStop(const env::PhysicalStop*);
				void	clearDisplayedPlaces();
				void	clearForbiddenPlaces();
				void	clearForcedDestinations();
				void	clearPhysicalStops();
				void	copy(const DisplayScreen*);
				void	removeDisplayedPlace(const env::PublicTransportStopZoneConnectionPlace*);
				void	removeForbiddenPlace(const env::PublicTransportStopZoneConnectionPlace*);
				void	removeForcedDestination(const env::PublicTransportStopZoneConnectionPlace*);
				void	removePhysicalStop(const env::PhysicalStop*);
			//@}

			//!	\name Getters
			//@{
				const env::PublicTransportStopZoneConnectionPlace*	getLocalization()	const;
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
				int								getMaintenanceChecksPerDay()	const;
				bool							getIsOnline()					const;
				const std::string&				getMaintenanceMessage()			const;
				const Complements&				getComplements()				const;
				bool							getDisplayTeam()				const;
			//@}

			//! \name Queries
			//@{
				boost::shared_ptr<ArrivalDepartureTableGenerator>	getGenerator(const time::DateTime& startTime)		const;
				void												display(std::ostream& stream, const time::DateTime& date)				const;
				void												recordSupervision(const std::string&)	const;
				std::string											getFullName()															const;
				const env::PhysicalStops&							getPhysicalStops(bool result=true)										const;

				std::vector<std::pair<uid, std::string> > 
					getSortedAvaliableDestinationsLabels(const DisplayedPlacesList& placesToAvoid)	const;
			//@}

		};

	}
}

#endif

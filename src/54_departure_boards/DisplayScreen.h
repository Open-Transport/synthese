#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////

#include <set>
#include <string>
#include <ostream>
#include <map>

#include "Registrable.h"
#include "Registry.h"
#include "DBLog.h"
#include "DeparturesTableTypes.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "TreeNode.hpp"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeMultiClassRootPolicy.hpp"
#include "Named.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace geography
	{
		class NamedPlace;
	}

	namespace pt
	{
		class StopArea;
		class JourneyPattern;
	}

	namespace departure_boards
	{
		class DisplayType;
		class DisplayScreenCPU;
		class DisplayMonitoringStatus;

		/** Terminal d'affichage.
			@ingroup m54

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
		class DisplayScreen:
			public virtual util::Registrable,
			public tree::TreeNode<
				DisplayScreen,
				tree::TreeAlphabeticalOrderingPolicy,
				tree::TreeMultiClassRootPolicy<geography::NamedPlace, DisplayScreenCPU>
			>,
			public util::Named
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<DisplayScreen>	Registry;

			typedef enum {
				STANDARD_METHOD = 0,
				WITH_FORCED_DESTINATIONS_METHOD = 1,
				ROUTE_PLANNING = 2,
				DISPLAY_CHILDREN_ONLY = 3
			} GenerationMethod;


		protected:
			//! \name Technical data
			//@{
				const DisplayType*									_displayType;
				int													_wiringCode;	// Display ID in a bus
				int													_comPort;
				std::string											_macAddress;
			//@}

			//! \name Appearance
			//@{
				std::string			_title;				//!< Titre pour affichage
				int					_blinkingDelay;
				bool				_trackNumberDisplay;
				bool				_serviceNumberDisplay;
				bool				_displayTeam;
				bool				_displayClock;
				DisplayedPlacesList		_displayedPlaces;
				TransferDestinationsList	_transfers;
			//@}

			//! \name Content
			//@{
				const pt::StopArea*			_displayedPlace;		//!< Place where the services must depart or arrive
				ArrivalDepartureTableGenerator::PhysicalStops	_physicalStops;				//!< Quai(s) affich�s
				bool						_allPhysicalStopsDisplayed;
				ForbiddenPlacesList			_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
				LineFilter					_forbiddenLines;
				DeparturesTableDirection	_direction;
				EndFilter					_originsOnly;
				int							_maxDelay;			//!< Max time length for the table
				int							_clearingDelay;
				int							_firstRow;
				bool						_routePlanningWithTransfer;
			//@}

			//!	\name Preselection
			//@{
				GenerationMethod			_generationMethod;
				DisplayedPlacesList			_forcedDestinations;	//!< Destinations � afficher absolument
				int							_destinationForceDelay;	//!< Dur�e pendant laquelle une destination est forc�e
			
			//@}

			//! \name Maintenance
			//@{
				bool						_maintenanceIsOnline;
				std::string					_maintenanceMessage;
			//@}


			//!	\name Protected
			//@{
				boost::posix_time::ptime	_MomentFin(const boost::posix_time::ptime& __MomentDebut)			const;
			//@}

		public:
			//!	\name Constructor/Destructor
			//@{
				DisplayScreen(util::RegistryKeyType key = 0);
				~DisplayScreen();
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
				void	setDisplayedPlace(const pt::StopArea* value){ _displayedPlace = value; }
				void	setMaintenanceIsOnline(bool value);
				void	setMaintenanceMessage(const std::string& message);
				void	setMaxDelay(int);
				void	setOriginsOnly(EndFilter);
				void	setServiceNumberDisplay(bool value);
				void	setTitle(const std::string&);
				void	setTrackNumberDisplay(bool value);
				void	setType(const DisplayType*);
				void	setWiringCode(int);				
				void	setDisplayTeam(bool value);
				void	setDisplayClock(bool value);
				void	setComPort(int value);
				void	setMacAddress(const std::string& value);
				void	setRoutePlanningWithTransfer(bool value);
			//@}

			//! \name Modifiers
			//@{
				void	addDisplayedPlace(const pt::StopArea*);
				void	addForbiddenPlace(const pt::StopArea*);
				void	addForcedDestination(const pt::StopArea*);
				void	addPhysicalStop(const pt::StopPoint*);
				void	clearDisplayedPlaces();
				void	clearForbiddenPlaces();
				void	clearForcedDestinations();
				void	clearPhysicalStops();
				void	copy(const DisplayScreen&);
				void	removeDisplayedPlace(const pt::StopArea*);
				void	removeForbiddenPlace(const pt::StopArea*);
				void	removeForcedDestination(const pt::StopArea*);
				void	removePhysicalStop(const pt::StopPoint*);
				
				//////////////////////////////////////////////////////////////////////////
				/// Adds a destination by transfer to display if necessary.
				/// @param transferPlace place to the transfer must be
				/// @param destinationPlace destination place 
				void addTransferDestination(
					TransferDestinationsList::key_type transferPlace,
					TransferDestinationsList::mapped_type::value_type destinationPlace
				);

				//////////////////////////////////////////////////////////////////////////
				/// Remove a destination by transfer to display if necessary.
				/// @param transferPlace place to the transfer must be
				/// @param destinationPlace destination place 
				void removeTransferDestination(
					TransferDestinationsList::key_type transferPlace,
					TransferDestinationsList::mapped_type::value_type destinationPlace
				);



				//////////////////////////////////////////////////////////////////////////
				/// Clears the destination by transfer list.
				void clearTransferDestinations();
			//@}

			//!	\name Getters
			//@{
				const pt::StopArea*				getDisplayedPlace()				const { return _displayedPlace; }
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
				bool							getIsOnline()					const;
				const std::string&				getMaintenanceMessage()			const;
				bool							getDisplayTeam()				const;
				int								getComPort()					const;
				bool							getDisplayClock()				const;
				std::string						getMacAddress()					const;
				bool							getRoutePlanningWithTransfer()	const;
				const TransferDestinationsList&	getTransferdestinations()		const;
			//@}

			//! \name Services
			//@{
				boost::shared_ptr<ArrivalDepartureTableGenerator>	getGenerator(const boost::posix_time::ptime& startTime)		const;

				const geography::NamedPlace* getLocation() const;
				
				//////////////////////////////////////////////////////////////////////////
				/// Display the content generated for the screen
				void display(
					std::ostream& stream,
					const boost::posix_time::ptime& date,
					const server::Request* request
				) const;

				std::string											getFullName()															const;
				const ArrivalDepartureTableGenerator::PhysicalStops&	getPhysicalStops(bool result=true)										const;

				typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

				Labels getSortedAvaliableDestinationsLabels(const DisplayedPlacesList& placesToAvoid) const;

				/** Analyzes a monitoring status to determinate if the display is down or up.
					@param status Status to read
					@return true if the status is too old for the display screen
					If the status is too old, a log entry is writen.
					If the screen is not monitored for any reason, then the method always returns false.
					@warning This method checks only if the status is too old. It does not read the status itself.
				*/
				bool isDown(const DisplayMonitoringStatus& status) const;


				/** Returns if the screen should sens monitoring status.
					@return true if :
						- the screen has a display type
						- the screen is online
						- the type has a monitoring interface
						- the type defines a positive time between monitoring checks
				*/
				bool isMonitored() const;
			//@}

		};

	}
}

#endif

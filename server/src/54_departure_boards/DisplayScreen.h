////////////////////////////////////////////////////////////////////
/// DisplayScreen class header.
///	@file DisplayScreen.h
///	@author Hugues Romain
///	@date 2008
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
////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

#include "Object.hpp"

#include "BroadcastPoint.hpp"
#include "DataSourceLinksField.hpp"
#include "DisplayScreenCPU.h"
#include "DisplayType.h"
#include "ImportableTemplate.hpp"
#include "PlaceWithDisplayBoards.hpp"
#include "Registrable.h"
#include "TreeNode.hpp"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeMultiClassRootPolicy.hpp"
#include "WithGeometry.hpp"

#include "Registry.h"
#include "DBLog.h"
#include "DeparturesTableTypes.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"

#include <set>
#include <string>
#include <ostream>
#include <map>

namespace synthese
{
	namespace departure_boards
	{
		FIELD_POINTER(BroadCastPoint, pt::StopArea)
		FIELD_STRING(BroadCastPointComment)
		FIELD_POINTER(DisplayTypePtr, departure_boards::DisplayType)
		FIELD_INT(WiringCode)
		FIELD_INT(BlinkingDelay)
		FIELD_BOOL(TrackNumberDisplay)
		FIELD_BOOL(ServiceNumberDisplay)
		FIELD_BOOL(DisplayTeam)
		FIELD_STRING(PhysicalStops)
		FIELD_BOOL(AllPhysicalDisplayed)
		FIELD_STRING(ForbiddenArrivalPlaces)
		FIELD_STRING(AllowedLines)
		FIELD_INT(Direction)
		FIELD_INT(OriginsOnly)
		FIELD_STRING(DisplayedPlaces)
		FIELD_INT(MaxDelay)
		FIELD_INT(ClearingDelay)
		FIELD_INT(FirstRow)
		FIELD_INT(GenerationMethodCode)
		FIELD_STRING(ForcedDestinations)
		FIELD_INT(DestinationForceDelay)
		FIELD_INT(MaintenanceChecksPerDay)
		FIELD_BOOL(MaintenanceIsOnline)
		FIELD_BOOL(DisplayClock)
		FIELD_POINTER(CpuHost, departure_boards::DisplayScreenCPU)
		FIELD_INT(ComPort)
		FIELD_BOOL(RoutePlanningWithTransfer)
		FIELD_STRING(TransferDestinations)
		FIELD_POINTER(Up, departure_boards::DisplayScreen)
		FIELD_INT(SubScreenTypeCode)
		FIELD_BOOL(AllowCanceled)
		FIELD_POINTER(StopPointLocation, pt::StopPoint)
		FIELD_INT(MaxTransferDuration)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(BroadCastPoint),//!< Place where the services must depart or arrive
			FIELD(BroadCastPointComment),
			FIELD(DisplayTypePtr),
			FIELD(WiringCode),// Display ID in a bus
			FIELD(Title),//!< Titre pour affichage
			FIELD(BlinkingDelay),
			FIELD(TrackNumberDisplay),
			FIELD(ServiceNumberDisplay),
			FIELD(DisplayTeam),
			FIELD(PhysicalStops),
			FIELD(AllPhysicalDisplayed),
			FIELD(ForbiddenArrivalPlaces),
			FIELD(AllowedLines),
			FIELD(Direction),
			FIELD(OriginsOnly),
			FIELD(DisplayedPlaces),
			FIELD(MaxDelay),//!< Max time length for the table
			FIELD(ClearingDelay),
			FIELD(FirstRow),
			FIELD(GenerationMethodCode),
			FIELD(ForcedDestinations),
			FIELD(DestinationForceDelay),//!< Durée pendant laquelle une destination est forcée
			FIELD(MaintenanceChecksPerDay),
			FIELD(MaintenanceIsOnline),
			FIELD(MaintenanceMessage),
			FIELD(DisplayClock),
			FIELD(ComPort),
			FIELD(CpuHost),
			FIELD(MacAddress),
			FIELD(RoutePlanningWithTransfer),
			FIELD(TransferDestinations),
			FIELD(Up),
			FIELD(SubScreenTypeCode),
			FIELD(impex::DataSourceLinks),
			FIELD(AllowCanceled),
			FIELD(StopPointLocation),
			FIELD(MaxTransferDuration)
		> DisplayScreenSchema;
	}

	namespace server
	{
		class Request;
	}

	namespace graph
	{
		class Journey;
	}

	namespace pt
	{
		class CommercialLine;
		class StopArea;
		class JourneyPattern;
		class TransportNetwork;
	}

	namespace departure_boards
	{
		class DisplayType;
		class DisplayMonitoringStatus;

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
		class DisplayScreen:
			public virtual Object<DisplayScreen, DisplayScreenSchema>,
			public virtual util::Registrable,
			public tree::TreeNode<
				DisplayScreen,
				tree::TreeAlphabeticalOrderingPolicy,
				tree::TreeMultiClassRootPolicy<
					PlaceWithDisplayBoards,
					DisplayScreenCPU
			>	>,
			public WithGeometry<geos::geom::Point>,
			public impex::ImportableTemplate<DisplayScreen>,
			public util::FactorableTemplate<messages::BroadcastPoint, DisplayScreen>
		{
		public:
			static const std::string VALUE_DISPLAY_SCREEN;

			/// Chosen registry class.
			typedef util::Registry<DisplayScreen>	Registry;

			typedef enum {
				STANDARD_METHOD = 0,
				WITH_FORCED_DESTINATIONS_METHOD = 1,
				ROUTE_PLANNING = 2,
				DISPLAY_CHILDREN_ONLY = 3
			} GenerationMethod;

			typedef enum {
				SUB_CONTENT = 0,
				CONTINUATION_TRANSFER = 1
			} SubScreenType;


		protected:
			static const std::string DATA_SCREEN_ID;
			static const std::string DATA_MAC_ADDRESS;
			static const std::string DATA_TITLE;
			static const std::string DATA_IS_ONLINE;
			static const std::string DATA_MAINTENANCE_MESSAGE;
			static const std::string DATA_NAME;
			static const std::string DATA_TYPE_ID;
			static const std::string DATA_LOCATION_ID;
			static const std::string DATA_CPU_ID;
			static const std::string DATA_X;
			static const std::string DATA_Y;

			//! \name Appearance
			//@{
				DisplayedPlacesList		_displayedPlaces;
				TransferDestinationsList	_transfers;
			//@}

			//! \name Content
			//@{
				SubScreenType				_subScreenType;
				ArrivalDepartureTableGenerator::PhysicalStops	_physicalStops;				//!< Filter on departure stop point
				ForbiddenPlacesList			_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
				LineFilter					_allowedLines;
				DeparturesTableDirection	_direction;
				EndFilter					_originsOnly;
			//@}

			//!	\name Preselection
			//@{
				GenerationMethod			_generationMethod;
				DisplayedPlacesList			_forcedDestinations;	//!< Destinations to display absolutely
			//@}


			//!	\name Protected
			//@{
				boost::posix_time::ptime	_MomentFin(const boost::posix_time::ptime& __MomentDebut)			const;

				bool _locationOnLine(
					const pt::CommercialLine& line,
					boost::optional<bool> direction
				) const;

				bool _locationOnNetwork(
					const pt::TransportNetwork& network
				) const;

		public:
				//////////////////////////////////////////////////////////////////////////
				/// Generation of the content of the screen for standard screens only (and derivated types).
				/// @param startTime start time
				/// @param endTime end time
				/// @param rootCall true if the call of this method is not recursive
				/// @param scom enable the use of SCOM for the results, false by default
				/// @result the result of generation
				/// @author Hugues Romain
				/// @since 3.2.0
				ArrivalDepartureList generateStandardScreen(
					const boost::posix_time::ptime& startTime,
					const boost::posix_time::ptime& endTime,
					bool rootCall = true,
					bool scom = false
				) const;


				IntermediateStop::TransferDestinations _generateTransferDestinations(
					const graph::Journey& approachJourney,
					const pt::StopArea& stopArea,
					const boost::posix_time::ptime& startTime,
					const boost::posix_time::ptime& endTime
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Search a continuation transfer at a place in the sub screens.
				/// @param stop departure stop to search
				/// @result pointer to such a sub screen, NULL if no screen has been found
				/// @author Hugues Romain
				/// @since 3.2.0
				DisplayScreen*	_getContinuationTransferScreen(
					const pt::StopArea& stop
				) const;
			//@}

		public:
			//!	\name Constructor/Destructor
			//@{
				DisplayScreen(util::RegistryKeyType key = 0);
				~DisplayScreen();
			//@}

			//!	\name Setters
			//@{
				void	setDirection(DeparturesTableDirection direction);
				void	setGenerationMethod(GenerationMethod method);
				void	setOriginsOnly(EndFilter);
				void	setSubScreenType(SubScreenType value){ _subScreenType = value; }
				void	setAllowedLines(const LineFilter& value){ _allowedLines = value; }
				void	setStops(const ArrivalDepartureTableGenerator::PhysicalStops& value);
			//@}

			//! \name Modifiers
			//@{
				void	addDisplayedPlace(const pt::StopArea*);
				void	addForbiddenPlace(const pt::StopArea*);
				void	addForcedDestination(const pt::StopArea*);
				void	clearDisplayedPlaces();
				void	clearForbiddenPlaces();
				void	clearForcedDestinations();
				void	copy(const DisplayScreen&);
				void	removeDisplayedPlace(const pt::StopArea*);
				void	removeForbiddenPlace(const pt::StopArea*);
				void	removeForcedDestination(const pt::StopArea*);
				void	computeStrAndSetPhysicalStops();

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
				const ForbiddenPlacesList&		getForbiddenPlaces()			const;
				const LineFilter&				getForbiddenLines()				const;
				DeparturesTableDirection		getDirection()					const;
				EndFilter						getEndFilter()					const;
				const DisplayedPlacesList&		getDisplayedPlaces()			const;
				GenerationMethod				getGenerationMethod()			const;
				const DisplayedPlacesList&		getForcedDestinations()			const;
				const TransferDestinationsList&	getTransferdestinations()		const { return _transfers; }
				SubScreenType					getSubScreenType()				const { return _subScreenType; }
				const LineFilter&				getAllowedLines()				const { return _allowedLines; }
			//@}

			//! \name Services
			//@{
				boost::shared_ptr<ArrivalDepartureTableGenerator>	getGenerator(const boost::posix_time::ptime& startTime)		const;

				const geography::NamedPlace* getLocation() const;



				std::string getFullName() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the list of the stops to display.
				/// @param result if true, the result takes into acount of the value of _allPhysicalStops; if false the method retrieves only the attribute
				/// @return the stops to display
				/// @author Hugues Romain
				const ArrivalDepartureTableGenerator::PhysicalStops& getPhysicalStops(
					bool result = true
				) const;

				typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

				Labels getSortedAvailableDestinationsLabels(const DisplayedPlacesList& placesToAvoid) const;

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

			//! @name Static output methods
			//@{
				static const std::string GetSubScreenTypeLabel(SubScreenType value);
			//@}


			//! @name BroadcastPoint virtual methods
			//@{
				virtual std::set<messages::MessageType*> getMessageTypes() const;

				virtual bool displaysMessage(
					const messages::Alarm::LinkedObjects& linkedObjects,
					const util::ParametersMap& parameters
				) const;

				virtual void getBroadcastPoints(BroadcastPoints& result) const;
			//@}
			
			//! @name Modifiers
			//@{
				virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix) const;
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif

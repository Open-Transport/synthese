
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

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"

#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "34_departures_table/StandardArrivalDepartureTableGenerator.h"


namespace synthese
{
	namespace env
	{
		class PhysicalStop;
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
			typedef enum { STANDARD_METHOD, WITH_FORCED_DESTINATIONS_METHOD } GenerationMethod;

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
				DeparturesTableModule::PhysicalStopsList	_physicalStops;				//!< Quai(s) affich�s
				DeparturesTableModule::ForbiddenPlacesList	_forbiddenArrivalPlaces;	//!< Places not to serve. If so, then the line is not selected
				DeparturesTableModule::LineFilter			_forbiddenLines;
				DeparturesTableModule::Direction			_direction;
				DeparturesTableModule::EndFilter			_originsOnly;
				DeparturesTableModule::DisplayedPlacesList	_displayedPlaces;
				int											_maxDelay;			//!< Max time length for the table
				int											_clearingDelay;
				int											_firstRow;
			//@}

			//!	\name Preselection
			//@{
				const GenerationMethod					_generationMethod;
				DeparturesTableModule::DisplayedPlacesList	_forcedDestinations;	//!< Destinations � afficher absolument
				int										_destinationForceDelay;	//!< Dur�e pendant laquelle une destination est forc�e
			
			//@}


			//!	\name M�thodes prot�g�es
			//@{
				time::DateTime	_MomentFin(const time::DateTime& __MomentDebut)			const;
			//@}

		public:
			//!	\name Constructeur et destructeur
			//@{
				DisplayScreen(const uid&, GenerationMethod);
				~DisplayScreen(void);
			//@}

			//!	\name Modificateurs
			//@{
				void			setLocalization(const BroadcastPoint*);
				void			AddQuaiAutorise(const env::PhysicalStop*);
				void			SetTitre(const std::string&);
				void			SetOriginesSeulement(const DeparturesTableModule::EndFilter& __Valeur);
				void			AddDestinationAffichee(const env::ConnectionPlace* __PointArret);
				void			SetNumeroPanneau(int);
				void			addForcedDestination(const env::ConnectionPlace*);
				void			setDestinationForceDelay(int);
				void			setMaxDelay(int);
				void			addForbiddenPlace(const env::ConnectionPlace*);
			//@}

			//!	\name Accesseurs and computers
			//@{
				const std::string&				getTitre()			const;
				int								getNumeroPanneau()	const;
				ArrivalDepartureTableGenerator*	getGenerator(const time::DateTime& startTime) const;
			//@}

		};

	}
}

#endif

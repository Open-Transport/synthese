
/** Line class header.
	@file Line.h

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

#ifndef SYNTHESE_ENV_LINE_H
#define SYNTHESE_ENV_LINE_H

#include <vector>
#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "15_env/Path.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
		class Axis;
		class LineStop;
		class RollingStock;
		class Service;
		class LineAlarmBroadcast;
		class CommercialLine;
		class TransportNetwork;


		/** Technical line.
			TRIDENT JourneyPattern = Mission
			 inherits from Route
			@ingroup m15

			Une ligne technique est un regroupement de services, constituÃ© dans un but d'amÃ©lioration des performances de la recherche d'itinÃ©raires. En effet, plusieurs services groupÃ©s dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son intÃ©rÃÂªt au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement ÃÂ  l'intÃ©rieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant ÃÂªtre groupÃ©s selon les critÃÅ¡res ci-dessous, mais n'appartenant pas ÃÂ  la mÃÂªme ligne commerciale, ne seront pas groupÃ©s dans une ligne technique. De ce fait, les lignes techniques sont considÃ©rÃ©s comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la thÃ©orie des lignes&nbsp;:
				- Les services d'une mÃÂªme ligne doivent desservir exactement les mÃÂªmes points d'arrÃÂªt, dans le mÃÂªme ordre
				- Les conditions de desserte de chaque point d'arrÃÂªt (dÃ©part/passage/arrivÃ©e et horaires saisis/non saisis) sont les mÃÂªmes pour tous les services d'une ligne
				- Les services d'une mÃÂªme ligne sont assurÃ©s par le mÃÂªme matÃ©riel roulant, observent les mÃÂªmes conditions de rÃ©servation, appartiennent au mÃÂªme rÃ©seau de transport, et suivent la mÃÂªme tarification
				- Un service d'une ligne ne doit pas desservir un point d'arrÃÂªt ÃÂ  la mÃÂªme heure qu'un autre service de la mÃÂªme ligne
				- Un service d'une ligne desservant un point d'arrÃÂªt avant un autre de la mÃÂªme ligne ne doit pas desservir un autre point d'arrÃÂªt aprÃÅ¡s ce dernier&nbsp;: deux services ne doivent pas se ÃÂ«&nbsp;doubler&nbsp;ÃÂ»

			NB : la correspondance entre deux services d'une mÃÂªme ligne est interdite, sauf dans les axes libres.

		*/
		class Line : 
			public synthese::util::Registrable<uid,Line>,
			public Path
		{
		private:

			const Axis*				_axis;
			const CommercialLine*	_commercialLine;
		    
			const RollingStock* _rollingStock;

			std::string _name;			//!< Name (code)
			std::string _timetableName; //!< Name for timetable
			std::string _direction;		//!< Direction (shown on vehicles)

			bool _isWalkingLine;
		    
			bool _useInDepartureBoards; 
			bool _useInTimetables; 
			bool _useInRoutePlanning; 

		public:

			Line (const uid& id,
			const std::string& name, 
			const Axis* axis);

			Line();
			virtual ~Line();



			//! @name Getters
			//@{
				virtual uid				getId ()					const;
				const std::string&		getName ()					const;
				bool					getUseInDepartureBoards ()	const;
				bool					getUseInTimetables ()		const;
				bool					getUseInRoutePlanning ()	const;
				const Axis*				getAxis ()					const;
				const std::string&		getDirection ()				const;
				const std::string&		getTimetableName ()			const;
				const TransportNetwork* getNetwork ()				const;
				const RollingStock*		getRollingStock()			const;
				bool					getWalkingLine ()			const;
				const CommercialLine*	getCommercialLine()			const;
			//@}


			//! @name Setters
			//@{
				void setUseInDepartureBoards (bool useInDepartureBoards);
				void setName (const std::string& name);
				void setWalkingLine (bool isWalkingLine);
				void setRollingStock(const RollingStock*);
				void setTimetableName (const std::string& timetableName);
				void setDirection (const std::string& direction);
				void setAxis(const Axis* axis);
				void setUseInRoutePlanning (bool useInRoutePlanning);
				void setUseInTimetables (bool useInTimetables);
				void setCommercialLine(const CommercialLine* commercialLine);
			//@}



			//! @name Update methods
			//@{
			//@}
		    
			//! @name Query methods
			//@{
				bool isRoad () const;
				bool isLine () const;
				bool isPedestrianMode() const;

				bool isReservable () const;

				const PhysicalStop* getDestination() const;
			//@}
		    
		};
	}
}

#endif


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

			Une ligne technique est un regroupement de services, constitué dans un but d'amélioration des performances de la recherche d'itinéraires. En effet, plusieurs services groupés dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son intérÃªt au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement Ã  l'intérieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant Ãªtre groupés selon les critÃšres ci-dessous, mais n'appartenant pas Ã  la mÃªme ligne commerciale, ne seront pas groupés dans une ligne technique. De ce fait, les lignes techniques sont considérés comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la théorie des lignes&nbsp;:
				- Les services d'une mÃªme ligne doivent desservir exactement les mÃªmes points d'arrÃªt, dans le mÃªme ordre
				- Les conditions de desserte de chaque point d'arrÃªt (départ/passage/arrivée et horaires saisis/non saisis) sont les mÃªmes pour tous les services d'une ligne
				- Les services d'une mÃªme ligne sont assurés par le mÃªme matériel roulant, observent les mÃªmes conditions de réservation, appartiennent au mÃªme réseau de transport, et suivent la mÃªme tarification
				- Un service d'une ligne ne doit pas desservir un point d'arrÃªt Ã  la mÃªme heure qu'un autre service de la mÃªme ligne
				- Un service d'une ligne desservant un point d'arrÃªt avant un autre de la mÃªme ligne ne doit pas desservir un autre point d'arrÃªt aprÃšs ce dernier&nbsp;: deux services ne doivent pas se Â«&nbsp;doubler&nbsp;Â»

			NB : la correspondance entre deux services d'une mÃªme ligne est interdite, sauf dans les axes libres.

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

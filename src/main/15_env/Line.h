
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

			Une ligne technique est un regroupement de services, constitu� dans un but d�am�lioration des performances de la recherche d'itin�raires. En effet, plusieurs services group�s dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son int�r�t au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement � l'int�rieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant �tre group�s selon les crit�res ci-dessous, mais n'appartenant pas � la m�me ligne commerciale, ne seront pas group�s dans une ligne technique. De ce fait, les lignes techniques sont consid�r�s comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la th�orie des lignes&nbsp;:
				- Les services d�une m�me ligne doivent desservir exactement les m�mes points d�arr�t, dans le m�me ordre
				- Les conditions de desserte de chaque point d�arr�t (d�part/passage/arriv�e et horaires saisis/non saisis) sont les m�mes pour tous les services d�une ligne
				- Les services d�une m�me ligne sont assur�s par le m�me mat�riel roulant, observent les m�mes conditions de r�servation, appartiennent au m�me r�seau de transport, et suivent la m�me tarification
				- Un service d�une ligne ne doit pas desservir un point d�arr�t � la m�me heure qu�un autre service de la m�me ligne
				- Un service d�une ligne desservant un point d�arr�t avant un autre de la m�me ligne ne doit pas desservir un autre point d�arr�t apr�s ce dernier&nbsp;: deux services ne doivent pas se �&nbsp;doubler&nbsp;�

			NB : la correspondance entre deux services d'une m�me ligne est interdite, sauf dans les axes libres.

		*/
		class Line : 
			public synthese::util::Registrable<uid,Line>,
			public Path
		{
		private:

			const Axis*				_axis;
			const CommercialLine*	_commercialLine;
		    
			uid _rollingStockId;

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
				const uid&				getId ()					const;
				const std::string&		getName ()					const;
				bool					getUseInDepartureBoards ()	const;
				bool					getUseInTimetables ()		const;
				bool					getUseInRoutePlanning ()	const;
				const Axis*				getAxis ()					const;
				const std::string&		getDirection ()				const;
				const std::string&		getTimetableName ()			const;
				const TransportNetwork* getNetwork ()				const;
				const uid&				getRollingStockId ()		const;
				bool					getWalkingLine ()			const;
				const CommercialLine*	getCommercialLine()			const;
			//@}


			//! @name Setters
			//@{
				void setUseInDepartureBoards (bool useInDepartureBoards);
				void setName (const std::string& name);
				void setWalkingLine (bool isWalkingLine);
				void setRollingStockId (const uid& id);
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

				bool isReservable () const;

				PhysicalStop* getDestination() const;
			//@}
		    
		};
	}
}

#endif

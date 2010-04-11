
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

#include <boost/logic/tribool.hpp>

#include "Path.h"
#include "Importable.h"
#include "Registry.h"
#include "Named.h"

namespace synthese
{
	namespace graph
	{
		class Service;
	}
	
	namespace pt
	{
		class TransportNetwork;
		class PhysicalStop;
		class RollingStock;
		class SubLine;
	}
	
	namespace pt
	{
		class LineStop;
		class LineAlarmBroadcast;
		class CommercialLine;


		/** Technical line.
			TRIDENT JourneyPattern = Mission
			 inherits from Route
			@ingroup m35

			Une ligne technique est un regroupement de services, constitué dans un but d'amélioration des performances de la recherche d'itinéraires. En effet, plusieurs services groupés dans une ligne ne comptent que pour un dans leur participation au temps de calcul, ce qui rend tout son intér�ªt au regroupement des services en lignes.
			Le regroupement en lignes techniques s'effectue cependant, par convention, uniquement �  l'intérieur des \ref defLigneCommerciale "lignes commerciales". Ainsi deux services pouvant �ªtre groupés selon les crit�šres ci-dessous, mais n'appartenant pas �  la m�ªme ligne commerciale, ne seront pas groupés dans une ligne technique. De ce fait, les lignes techniques sont considérés comme des subdivisions de lignes commerciales.

			Cependant, pour assurer la justesse des calculs, les regroupements en ligne doivent respecter des conditions strictes, constituant ainsi la théorie des lignes&nbsp;:
				- Les services d'une m�ªme ligne doivent desservir exactement les m�ªmes points d'arr�ªt, dans le m�ªme ordre
				- Les conditions de desserte de chaque point d'arr�ªt (départ/passage/arrivée et horaires saisis/non saisis) sont les m�ªmes pour tous les services d'une ligne
				- Les services d'une m�ªme ligne sont assurés par le m�ªme matériel roulant, observent les m�ªmes conditions de réservation, appartiennent au m�ªme réseau de transport, et suivent la m�ªme tarification
				- Un service d'une ligne ne doit pas desservir un point d'arr�ªt �  la m�ªme heure qu'un autre service de la m�ªme ligne
				- Un service d'une ligne desservant un point d'arr�ªt avant un autre de la m�ªme ligne ne doit pas desservir un autre point d'arr�ªt apr�šs ce dernier&nbsp;: deux services ne doivent pas se �«&nbsp;doubler&nbsp;�»

			NB : la correspondance entre deux services d'une m�ªme ligne est interdite, sauf dans les axes libres.

			If a service is responsible of a break of the preceding rules, then the line is copied as a SubLine, and the service is linked to the new line. The _sublines container keeps a pointer on each SubLine.
		*/
		class Line:
			public graph::Path,
			public impex::Importable,
			public util::Named
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Line>	Registry;

			typedef std::vector<pt::SubLine*> SubLines;

		private:
			std::string _timetableName; //!< Name for timetable
			std::string _direction;		//!< Direction (shown on vehicles)

			bool _isWalkingLine;
		    
			bool _useInDepartureBoards; 
			bool _useInTimetables; 
			bool _useInRoutePlanning;

			SubLines	_subLines;	//!< Copied lines handling services which not serve the line theory

			bool _wayBack;	//!< true if back route, false else (forward route or unknown)

		public:

			Line(
				util::RegistryKeyType id = 0,
				std::string name = std::string()
			);

			virtual ~Line();



			//! @name Getters
			//@{
				bool				getUseInDepartureBoards ()	const;
				bool				getUseInTimetables ()		const;
				bool				getUseInRoutePlanning ()	const;
				const std::string&	getDirection ()				const;
				const std::string&	getTimetableName ()			const;
				pt::RollingStock*	getRollingStock()			const;
				bool				getWalkingLine ()			const;
				CommercialLine*		getCommercialLine()			const;
				const SubLines		getSubLines()				const;
				bool				getWayBack()				const { return _wayBack; }
			//@}


			//! @name Setters
			//@{
				void setUseInDepartureBoards (bool useInDepartureBoards);
				void setWalkingLine (bool isWalkingLine);
				void setRollingStock(pt::RollingStock*);
				void setTimetableName (const std::string& timetableName);
				void setDirection (const std::string& direction);
				void setUseInRoutePlanning (bool useInRoutePlanning);
				void setUseInTimetables (bool useInTimetables);
				void setCommercialLine(CommercialLine* value);
				void setWayBack(bool value) { _wayBack = value; }
			//@}



			//! @name Update methods
			//@{

				/** Adds a sub-line to the line.
					@param line sub-line to add
					@return int rank of the sub-line in the array of sub-lines
					@author Hugues Romain
					@date 2008
				*/
				int addSubLine(pt::SubLine* line);


				/** Adds a service to a line.
					@param service Service to add
					@param ensureLineTheory If true, the method verifies if the service is compatible 
							with the other ones, by the way of the lines theory. If not, then it attempts 
							to register the service in an existing SubLine, or creates one if necessary.
							Note : in this case, the service is NOT added to the current line.

					@author Hugues Romain
					@date 2007
				*/
				virtual void addService(
					graph::Service* service,
					bool ensureLineTheory
				);
			//@}
		    
			//! @name Services
			//@{
				virtual std::string getRuleUserName() const { return "Mission " + getName(); }

				bool isPedestrianMode() const;

				bool isReservable () const;

				const pt::PhysicalStop* getDestination () const;
				const pt::PhysicalStop* getOrigin () const;
				const LineStop* getLineStop(std::size_t rank) const;

				/** Tests if the line theory would be respected if the service were inserted into the line.
					@param service service to test
					@return bool true if the line theory would be respected
					@author Hugues Romain
					@date 2008					
				*/
				bool respectsLineTheory(
					bool RTData,
					const graph::Service& service
				) const;

				/** Content comparison operator.
					@param stops Array of physical stops
					@return true if the line serves exactly the stops in the same order.
					@warning There is no test on the departure/arrival attributes
				*/
				bool operator==(const std::vector<pt::PhysicalStop*> stops) const;
			//@}
		    
		};
	}
}

#endif

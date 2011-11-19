
/** JourneyPatternCopy class header.
	@file JourneyPatternCopy.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_env_SubLine_h__
#define SYNTHESE_env_SubLine_h__

#include "JourneyPattern.hpp"

namespace synthese
{
	namespace pt
	{
		/** Sub-JourneyPattern class.
			@ingroup m35

			A sub-line is a copy of a line, which handles services which would break the line theory if they have been inserted in the original line.

			It happens in several cases :
				- a service start before another, which arrive at destination before the service.
				- a service starts in a continuous service range

			In theory, a stop could be added in the main line without doing the update in the subline.
			This is avoided by the rule saying that no stop can be added once a service is inserted in the line (a subline is created only if at least a service is registered)
		*/
		class JourneyPatternCopy:
			public JourneyPattern
		{
			JourneyPattern*	_mainLine;

		public:
			/** Constructor.
				@param line JourneyPattern to copy
				@author Hugues Romain
				@date 2008

				The subline constructor :
					- copy all data (including LineStop) of the original line
					- insert the subline in the sublines registry of the line.
			*/
			JourneyPatternCopy(JourneyPattern& line);


			/** Destructor.
				@author Hugues Romain
				@date 2008

				Deletes all the contained linestops;
			*/
			~JourneyPatternCopy();


			/** Adds the service only if it is compatible according to the line theory.
				@param service service to add
				@return bool true if the service has been added
				@author Hugues Romain
				@date 2008
			*/
			bool addServiceIfCompatible(graph::Service& service);


			/** Main line getter.
				@return JourneyPattern* The line from which the sub-line was extracted
				@author Hugues Romain
				@date 2008
			*/
			JourneyPattern*	getMainLine() const { return _mainLine; }
		};
	}
}

#endif // SYNTHESE_env_SubLine_h__

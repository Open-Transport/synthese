
/** EnvModule class header.
	@file EnvModule.h

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

#ifndef SYNTHESE_EnvModule_H__
#define SYNTHESE_EnvModule_H__

#include <vector>
#include <set>
#include <utility>

#include "01_util/ModuleClass.h"
#include "01_util/UId.h"
#include "01_util/FactorableTemplate.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "15_env/Types.h"

#include "12_security/Types.h"

namespace synthese
{
	/**	@defgroup m35Actions 35 Actions
		@ingroup m35

		@defgroup m35Pages 35 Pages
		@ingroup m35

		@defgroup m35Library 35 Interface library
		@ingroup m35

		@defgroup m35Functions 35 Functions
		@ingroup m35

		@defgroup m35LS 35 Table synchronizers
		@ingroup m35

		@defgroup m35Admin 35 Administration pages
		@ingroup m35

		@defgroup m35Rights 35 Rights
		@ingroup m35

		@defgroup m35Logs 35 DB Logs
		@ingroup m35

		@defgroup m35 35 Transport
		@ingroup m3
	@{
	*/

	/** 35 Transport network module namespace.
		The env name is due to historical reasons.
	*/
	namespace env
	{
		class Path;
		class AddressablePlace;
		class IncludingPlace;
		class Place;
		class Vertex;
		class NonPermanentService;
		

		/** 35 Transport network module class.
		*/
		class EnvModule : public util::FactorableTemplate<util::ModuleClass, EnvModule>
		{
		private:

			static lexmatcher::LexicalMatcher<uid> _citiesMatcher; //!< @todo To be moved in 36_transport_website
			static lexmatcher::LexicalMatcher<uid> _citiesT9Matcher;


		public:
		
			void initialize();
			
			

			/** Fetches a addressable place given its id.
				All the containers storong objects typed (or subtyped) as AddressablePlace
				are inspected.
			*/
			static boost::shared_ptr<const AddressablePlace> fetchAddressablePlace (const uid& id);
			static boost::shared_ptr<AddressablePlace> fetchUpdateableAddressablePlace (const uid& id);

			static boost::shared_ptr<const IncludingPlace> fetchIncludingPlace (const uid& id);

			static boost::shared_ptr<const Place> fetchPlace (const uid& id);
			static const Place* FetchPlace(const std::string& city, const std::string& place);


 		    static boost::shared_ptr<const Vertex> fetchVertex (const uid& id);

			/** Retrieves all lines associated withb a given commercial line id.
			 */
			static LineSet fetchLines (const uid& commercialLineId);

			/** Find the best matches in the city list comparing to a text entry.
				@param fuzzyName The text entry to compare
				@param nbMatches The number of matches to return
				@param t9 true indicates that the text entry follows the t9 format (optional : default = false)
				@return synthese::env::CityList The list of results
				@author Hugues Romain
				@date 2008
				
			*/
			static CityList guessCity(
				const std::string& fuzzyName
				, int nbMatches = 10
				, bool t9 = false
			);
			
			static void AddToCitiesMatchers(City* city);
			static void RemoveFromCitiesMatchers(City* city);


			static boost::shared_ptr<Path> fetchPath (const uid& id);
			static boost::shared_ptr<NonPermanentService> fetchService (const uid& id);

			static std::vector<std::pair<uid, std::string> >	getCommercialLineLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel 
				, bool withAll=false
				);

			static void getNetworkLinePlaceRightParameterList(security::ParameterLabelsVector& m);

			virtual std::string getName() const;
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__

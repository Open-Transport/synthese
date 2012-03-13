
//////////////////////////////////////////////////////////////////////////////////////////
///	PlacesListService class header.
///	@file PlacesListService.hpp
///	@author Hugues Romain
///	@date 2012
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

#ifndef SYNTHESE_PlacesList2Service_H__
#define SYNTHESE_PlacesList2Service_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "City.h"
#include "LexicalMatcher.h"
#include "StopArea.hpp"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace road
	{
		class House;
		class PublicPlace;
		class RoadPlace;
	}

	namespace pt_website
	{
		class PTServiceConfig;

		//////////////////////////////////////////////////////////////////////////
		///	56.15 Function : PlacesListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Places_list_(version_2)
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.3.0
		class PlacesListService:
			public util::FactorableTemplate<server::Function,PlacesListService>
		{
		public:
			static const std::string PARAMETER_CONFIG_ID;
			static const std::string PARAMETER_TEXT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_CITIES_WITH_AT_LEAST_A_STOP;
			static const std::string PARAMETER_SORTED;
			static const std::string PARAMETER_ITEM_PAGE_ID;
			static const std::string PARAMETER_CLASS_PAGE_ID;
			static const std::string PARAMETER_MIN_SCORE;
			static const std::string PARAMETER_CLASS_FILTER;
			static const std::string PARAMETER_SRID;

			static const std::string DATA_BEST_PLACE;
			static const std::string DATA_CLASS;
			static const std::string DATA_PLACES;
			static const std::string DATA_STOPS;
			static const std::string DATA_STOP;
			static const std::string DATA_CITIES;
			static const std::string DATA_CITY;
			static const std::string DATA_ROADS;
			static const std::string DATA_ROAD;
			static const std::string DATA_PUBLIC_PLACES;
			static const std::string DATA_PUBLIC_PLACE;
			static const std::string DATA_ADDRESSES;
			static const std::string DATA_ADDRESS;
			static const std::string DATA_RANK;
			static const std::string DATA_CONTENT;
			static const std::string DATA_KEY;
			static const std::string DATA_LEVENSHTEIN;
			static const std::string DATA_PHONETIC_SCORE;
			static const std::string DATA_PHONETIC_STRING;

		protected:
			//! \name Page parameters
			//@{
				std::string _text;
				const PTServiceConfig* _config;
				boost::shared_ptr<const geography::City> _city;
				bool _sorted;
				bool _citiesWithAtLeastAStop;
				boost::shared_ptr<const cms::Webpage> _itemPage;
				boost::shared_ptr<const cms::Webpage> _classPage;
				boost::optional<std::size_t> _number;
				double _minScore;
				std::string _classFilter;
				const CoordinatesSystem* _coordinatesSystem;
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Places_list_(version_2)#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.3.0
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Places_list_(version_2)#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.3.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


		private:
			void _displayClass(
				std::ostream& stream,
				const util::ParametersMap & result,
				const server::Request& request,
				boost::shared_ptr<util::ParametersMap> bestPlace,
				const std::string& bestPlaceClassName
			) const;

			void _displayItems(
				std::ostream& stream,
				const std::string& className,
				const std::vector<boost::shared_ptr<util::ParametersMap> >& map,
				const server::Request& request
			) const;

			void _displayItems(
				std::ostream& stream,
				const std::string& className,
				const std::vector<boost::shared_ptr<util::ParametersMap> >& map,
				const server::Request& request,
				std::size_t& rank
			) const;


		public:
			PlacesListService();



			/// @name Getters
			//@{
				const std::string& getText() const { return _text; }
				bool getCitiesWithAtLeastAStop() const { return _citiesWithAtLeastAStop; }
			//@}

			/// @name Setters
			//@{
				void setClassFilter(const std::string& value){ _classFilter = value; }
				void setOutputFormat(const std::string& value){ _outputFormat = value; }
				void setSorted(bool value){ _sorted = value; }
				void setNumber(boost::optional<std::size_t> value){ _number = value; }
				void setText(const std::string& value){ _text = value; }
				void setCoordinatesSystem(const CoordinatesSystem* coordinatesSystem ){ _coordinatesSystem = coordinatesSystem; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2012
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2012
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2012
			virtual std::string getOutputMimeType() const;



			typedef lexical_matcher::LexicalMatcher<
				boost::shared_ptr<geography::Place>
			>::MatchResult::value_type PlaceResult;

			//////////////////////////////////////////////////////////////////////////
			/// Reads the best result and load the corresponding object from the
			/// main environment.
			//////////////////////////////////////////////////////////////////////////
			/// @return a named place or a city
			/// @author Hugues Romain
			/// @date 2012
			PlaceResult getPlaceFromBestResult(
				const util::ParametersMap& result
			) const;

		private:
			template<class T>
			void _registerItems(
				util::ParametersMap& map,
				const typename lexical_matcher::LexicalMatcher<boost::shared_ptr<T> >::MatchResult& items
			) const {

				// Items count
				size_t itemsNumber(0);

				BOOST_FOREACH(const typename lexical_matcher::LexicalMatcher<boost::shared_ptr<T> >::MatchHit& item, items)
				{
					// Avoid empty cities
					if(_citiesWithAtLeastAStop &&
						dynamic_cast<geography::City*>(item.value.get()) &&
						dynamic_cast<geography::City&>(*item.value).getLexicalMatcher(pt::StopArea::FACTORY_KEY).size() == 0
					){
						continue;
					}

					// Allocation
					boost::shared_ptr<util::ParametersMap> pm(new util::ParametersMap);

					// Attributes
					pm->insert(DATA_KEY, item.key.getSource());
					pm->insert(DATA_PHONETIC_STRING, item.key.getPhoneticString());
					pm->insert(DATA_LEVENSHTEIN, item.score.levenshtein);
					pm->insert(DATA_PHONETIC_SCORE, item.score.phoneticScore);

					// Business object export and registration
					_attemptToRead<T, geography::City>(*item.value, pm, map, DATA_CITY);
					_attemptToRead<T, pt::StopArea>(*item.value, pm, map, DATA_STOP);
					_attemptToRead<T, road::RoadPlace>(*item.value, pm, map, DATA_ROAD);
					_attemptToRead<T, road::House>(*item.value, pm, map, DATA_ADDRESS);
					_attemptToRead<T, road::PublicPlace>(*item.value, pm, map, DATA_PUBLIC_PLACE);

					if(_number)
					{
						++itemsNumber;
						if(itemsNumber > *_number)
						{
							break;
						}
					}
				}
			}


			template<class BaseClass, class ObjectClass>
			void _attemptToRead(
				const BaseClass& object,
				boost::shared_ptr<util::ParametersMap> itemMap,
				util::ParametersMap& mainMap,
				const std::string& className
			) const {
				const ObjectClass* obj(
					dynamic_cast<const ObjectClass*>(&object)
				);
				if(obj)
				{
					obj->toParametersMap(*itemMap,_coordinatesSystem);
					mainMap.insert(className, itemMap);
				}
			}
		};
}	}

#endif // SYNTHESE_PlacesList2Service_H__

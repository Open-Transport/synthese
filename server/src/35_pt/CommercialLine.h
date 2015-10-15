
/** CommercialLine class header.
	@file CommercialLine.h

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

#ifndef SYNTHESE_CommercialLine_h__
#define SYNTHESE_CommercialLine_h__

#include <string>
#include <set>

#include "Object.hpp"

#include "TreeFolderDownNode.hpp"

#include "Calendar.h"
#include "DataSourceLinksField.hpp"
#include "GraphTypes.h"
#include "ImportableTemplate.hpp"
#include "PathGroup.h"
#include "RGBColor.h"
#include "StringField.hpp"
#include "TimeField.hpp"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace geography
	{
		class City;
	}

	namespace graph
	{
		class Service;
	}

	namespace tree
	{
		class TreeFolderUpNode;
	}

	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt
	{
		class TransportNetwork;
		class NonConcurrencyRule;
		class ReservationContact;
		class StopArea;

		FIELD_POINTER(Network, tree::TreeFolderUpNode)
		FIELD_STRING(ShortName)
		FIELD_STRING(LongName)
		FIELD_STRING(Color)
		FIELD_STRING(ForegroundColor)
		FIELD_STRING(Style)
		FIELD_STRING(LineImage)
		FIELD_STRING(OptionalReservationPlaces)
		FIELD_DATASOURCE_LINKS(CreatorId)
		FIELD_ID(BikeComplianceId)
		FIELD_ID(HandicappedComplianceId)
		FIELD_ID(PedestrianComplianceId)
		FIELD_POINTER(LineReservationContact, ReservationContact)
		FIELD_POINTER(LineCalendarTemplate, calendar::CalendarTemplate) //!< List of days when the line is supposed to run
		FIELD_STRING(MapUrl)
		FIELD_STRING(DocUrl)
		FIELD_ID(TimetableId)
		FIELD_TIME(DisplayDurationBeforeFirstDeparture)
		FIELD_INT(WeightForSorting)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Network),
			FIELD(Name),
			FIELD(ShortName),
			FIELD(LongName),
			FIELD(Color),
			FIELD(ForegroundColor),
			FIELD(Style),
			FIELD(LineImage),
			FIELD(OptionalReservationPlaces),
			FIELD(CreatorId),
			FIELD(BikeComplianceId),
			FIELD(HandicappedComplianceId),
			FIELD(PedestrianComplianceId),
			FIELD(LineReservationContact),
			FIELD(LineCalendarTemplate),
			FIELD(MapUrl),
			FIELD(DocUrl),
			FIELD(TimetableId),
			FIELD(DisplayDurationBeforeFirstDeparture),
			FIELD(WeightForSorting)
		> CommercialLineSchema;

		/** Commercial line.
			@ingroup m35

			TRIDENT = Line

			A commercial line is a group of @ref JourneyPattern "routes" known as a same offer unit by the customers and by the network organization. It belongs to a @ref TransportNetwork "transport network".

			The commercial line handles the denomination of the group of routes.
		*/
		class CommercialLine:
			public graph::PathGroup,
			public impex::ImportableTemplate<CommercialLine>,
			public tree::TreeFolderDownNode<TransportNetwork>,
			public virtual Object<CommercialLine, CommercialLineSchema>
		{
		public:
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_LONG_NAME;
			static const std::string DATA_LINE_NAME;
			static const std::string DATA_LINE_COLOR;
			static const std::string DATA_LINE_FOREGROUND_COLOR;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_LINE_ID;
			static const std::string DATA_LINE_CREATOR_ID;
			static const std::string DATA_LINE_MAP_URL;
			static const std::string DATA_LINE_DOC_URL;
			static const std::string DATA_LINE_TIMETABLE_ID;
			static const std::string DATA_MAX_DISPLAY_DELAY;

			/// Chosen registry class.
			typedef util::Registry<CommercialLine>	Registry;

			typedef std::set<const pt::StopArea*> PlacesSet;

			typedef std::set<const pt::NonConcurrencyRule*> NonConcurrencyRules;

			typedef std::multimap<std::string, graph::Service*> ServicesByNumber;

		private:
			/// @name data
			//@{
				boost::optional<util::RGBColor>		_color;		//!< CommercialLine color
				boost::optional<util::RGBColor>		_fgColor;		//!< CommercialLine foreground color

				PlacesSet	_optionalReservationPlaces;

				NonConcurrencyRules _nonConcurrencyRules;
			//@}

			/// @name Mutexes
			//@{
				mutable boost::recursive_mutex _nonConcurrencyRulesMutex;
			//@}

			/// @name Indices
			//@{
				mutable ServicesByNumber _servicesByNumber;
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@param key id of the object (optional)
			/// Initializes the following default use rules :
			///  - USER_PEDESTRIAN : allowed
			CommercialLine(util::RegistryKeyType key = 0);
			~CommercialLine();

			//! @name Getters
			//@{
				const std::string& getStyle () const { return get<Style>(); }
				TransportNetwork* getNetwork () const;
				const std::string& getShortName () const { return get<ShortName>(); }
				const std::string& getLongName () const { return get<LongName>(); }
				const std::string& getImage () const { return get<LineImage>(); }
				const boost::optional<util::RGBColor>& getColor () const { return _color; }
				const boost::optional<util::RGBColor>& getFgColor () const { return _fgColor; }
				const pt::ReservationContact* getReservationContact() const;
				const PlacesSet& getOptionalReservationPlaces() const { return _optionalReservationPlaces; }
				const NonConcurrencyRules& getNonConcurrencyRules() const { return _nonConcurrencyRules; }
				calendar::CalendarTemplate* getCalendarTemplate() const;
				boost::recursive_mutex& getNonConcurrencyRulesMutex() const { return _nonConcurrencyRulesMutex; }
				const std::string& getMapURL() const { return get<MapUrl>(); }
				const std::string& getDocURL() const { return get<DocUrl>(); }
				util::RegistryKeyType getTimetableId() const { return get<TimetableId>(); }
				const boost::posix_time::time_duration& getDisplayDurationBeforeFirstDeparture() const { return get<DisplayDurationBeforeFirstDeparture>(); }
				int getWeightForSorting() const { return get<WeightForSorting>(); }
				virtual std::string getName() const { return get<Name>(); }
			//@}

			//! @name Setters
			//@{
				void setStyle (const std::string& value) { set<Style>(value); }
				void setShortName (const std::string& shortName) { set<ShortName>(shortName); }
				void setLongName (const std::string& longName) { set<LongName>(longName); }
				void setImage (const std::string& image) { set<LineImage>(image); }
				void setColor (const boost::optional<util::RGBColor>& color);
				void setFgColor (const boost::optional<util::RGBColor>& color);
				void setReservationContact(pt::ReservationContact* value);
				void setCalendarTemplate(calendar::CalendarTemplate* value);
				void setNonConcurrencyRules(const NonConcurrencyRules& value) { _nonConcurrencyRules = value; }
				void setOptionalReservationPlaces(const PlacesSet& value);
				void setMapURL(const std::string& value){ set<MapUrl>(value); }
				void setDocURL(const std::string& value){ set<DocUrl>(value); }
				void setTimetableId(util::RegistryKeyType value){ set<TimetableId>(value); }
				void setDisplayDurationBeforeFirstDeparture(const boost::posix_time::time_duration& value){ set<DisplayDurationBeforeFirstDeparture>(value); }
				void setWeightForSorting(int weightForSorting) { set<WeightForSorting>(weightForSorting); }
				void setName(const std::string& value){ set<Name>(value); }
				virtual void setRules(const Rules& value);
			//@}

			/// @name Indices maintenance
			//@{
				void registerService(const graph::Service& service) const;
				void unregisterService(const graph::Service& service) const;
			//@}

			//! @name Services
			//@{
				typedef std::vector<graph::Service*> ServicesVector;
				ServicesVector getServices(const std::string& number) const;
				ServicesVector getServices() const;


				virtual std::string getRuleUserName() const { return "Ligne " + getName(); }

				//////////////////////////////////////////////////////////////////////////
				/// Tests if a place belongs to the list of optional reservation places of the line.
				/// @param place place to search
				/// @return true if the place belongs to the list of optional reservation places of the line.
				/// @author Hugues Romain
				bool isOptionalReservationPlace(
					const pt::StopArea* place
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Controls if the line runs every day of its calendar template.
				/// @param duration period to analyze (from now)
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				bool respectsCalendarTemplate(boost::gregorian::date_duration duration) const;



				//////////////////////////////////////////////////////////////////////////
				/// Get the days when the line runs.
				/// @param mask Calendar defining which days the check must be done
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				calendar::Calendar getRunDays(const calendar::Calendar& mask) const;


				//////////////////////////////////////////////////////////////////////////
				/// Line CMS exporter.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Lines_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the line has route using a specific transport mode.
				//////////////////////////////////////////////////////////////////////////
				/// @param transportMode the transport mode
				/// @return true if the line has at least a route using the specified
				/// transport mode
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				bool usesTransportMode(
					const vehicle::RollingStock& transportMode
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Checks if a journey pattern calls at the specified city
				bool callsAtCity(const geography::City& city) const;



				//////////////////////////////////////////////////////////////////////////
				/// Checks if at least one service runs at the specified date.
				bool runsAtDate(const boost::gregorian::date& day) const;

				
				
				//////////////////////////////////////////////////////////////////////////
				/// Checks if at least one service runs at least at one of the active dates
				/// of the specified calendar.
				bool runsOnCalendar(const calendar::Calendar& cal) const;



				//////////////////////////////////////////////////////////////////////////
				/// Checks if at least one service runs in the next minutes.
				/// @param when duration to check
				bool runsSoon(const boost::posix_time::time_duration& when) const;
			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds a non concurrency rule.
				/// @param rule the rule to add
				/// This method cleans non concurrency cache of all the services of the line.
				void addConcurrencyRule( const pt::NonConcurrencyRule* rule );


				//////////////////////////////////////////////////////////////////////////
				/// Removes a non concurrency rule.
				/// @param rule the rule to remove
				/// This method cleans non concurrency cache of all the services of the line.
				void removeConcurrencyRule( const pt::NonConcurrencyRule* rule );



				//////////////////////////////////////////////////////////////////////////
				/// Comparator overload.
				/// @param cl the other commercial line to compare with
				/// Return true if current line is inferior (according to its weight and sorting on short name).
				bool operator<(const CommercialLine& cl) const;

				

				struct PointerComparator {
					bool operator()(boost::shared_ptr<const CommercialLine> cl1, boost::shared_ptr<const CommercialLine> cl2) const
					{
						return *cl1 < *cl2;
					}

					bool operator()(const CommercialLine* cl1, const CommercialLine* cl2) const
					{
						return *cl1 < *cl2;
					}
				};

				virtual SubObjects getSubObjects() const;

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_CommercialLine_h__

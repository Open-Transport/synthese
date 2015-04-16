//////////////////////////////////////////////////////////////////////////
/// IsochronAdmin class header.
///	@file IsochronAdmin.hpp
///	@author Gael Sauvanet
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

#ifndef SYNTHESE_IsochronAdmin_H__
#define SYNTHESE_IsochronAdmin_H__

#include "AdminInterfaceElementTemplate.h"

#include "JourneysResult.h"
#include "PTRoutePlannerResult.h"
#include "ResultHTMLTable.h"
#include "StopArea.hpp"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/shared_ptr.hpp>
#include <list>

namespace synthese
{
	namespace analysis
	{
		//////////////////////////////////////////////////////////////////////////
		/// IsochronAdmin Admin compound class.
		///	@ingroup m60Admin refAdmin
		///	@author Gael Sauvanet
		///	@date 2012
		class IsochronAdmin:
			public admin::AdminInterfaceElementTemplate<IsochronAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAMETER_START_PLACE;
				static const std::string PARAMETER_MAX_DISTANCE;

				static const std::string PARAMETER_DATE;
				static const std::string PARAMETER_BEGIN_TIME_SLOT;
				static const std::string PARAMETER_END_TIME_SLOT;
				static const std::string PARAMETER_MAX_CONNECTIONS;

				static const std::string PARAMETER_CURVES_STEP;
				static const std::string PARAMETER_MAX_DURATION;
				static const std::string PARAMETER_DURATION_TYPE;
				static const std::string PARAMETER_FREQUENCY_TYPE;
				static const std::string PARAMETER_SPEED;
			//@}

		private:

			/// @name Search parameters
			//@{
			boost::shared_ptr<const pt::StopArea> _startPlace;
			int _maxDistance;

			boost::gregorian::date _date;
			int _beginTimeSlot;
			int _endTimeSlot;
			int _maxConnections;

			int _curvesStep;
			int _maxDuration;
			int _durationType;
			int _frequencyType;
			int _speed;
			//@}

			typedef enum {
				DURATION_TYPE_FIXED_DATETIME = 0,
				DURATION_TYPE_BEST = 1,
				DURATION_TYPE_AVERAGE = 2,
				DURATION_TYPE_MEDIAN = 3,
				DURATION_TYPE_WORST = 4
			} DurationType;

			typedef enum {
				FREQUENCY_TYPE_NO = 0,
				FREQUENCY_TYPE_HALF_FREQUENCY_AVERAGE = 1,
				FREQUENCY_TYPE_HALF_FREQUENCY_MEDIAN = 2,
				FREQUENCY_TYPE_HALF_FREQUENCY_WORST = 3,
				FREQUENCY_TYPE_AVERAGE = 4,
				FREQUENCY_TYPE_MEDIAN = 5,
				FREQUENCY_TYPE_WORST = 6
			} FrequencyType;

			typedef struct {
				const pt::StopArea* stop;
				int nbSolutions;
				int duration;
				int distance;
				boost::posix_time::ptime lastDepartureTime;
				std::list<boost::posix_time::ptime> timeDepartureList;
			} StopStruct;

			// Best result for each StopArea and on one IntegralSearcher iteration
			typedef std::map<util::RegistryKeyType, algorithm::JourneysResult::ResultSet::const_iterator> BestResultsMap;

			// Results by duration
			typedef std::multimap<int, StopStruct> ResultsMap;

			// ResultsMap access by StopArea
			typedef std::map<util::RegistryKeyType, ResultsMap::iterator> ResultsMapAccess;

		protected:

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Gaël Sauvanet
			///	@date 2012
			IsochronAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Gaël Sauvanet
			///	@date 2012
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Gaël Sauvanet
			///	@date 2012
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Gaël Sauvanet
			///	@date 2012
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2011
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages of the current class to put directly under
			/// a module admin page in the pages tree.
			///	@param module The module
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the module page in the page
			///	@author Hugues Romain
			///	@date 2011
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
}	}

#endif // SYNTHESE_IsochronAdmin_H__


/** GPSdFileFormat class header.
	@file GPSdFileFormat.hpp

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

#ifndef SYNTHESE_pt_GPSdFileFormat_hpp__
#define SYNTHESE_pt_GPSdFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "NoExportPolicy.hpp"
#include "PermanentThreadImporterTemplate.hpp"

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace data_exchange
	{
		class GPSdFileFormat:
			public impex::FileFormatTemplate<GPSdFileFormat>
		{

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Test poller.
			//////////////////////////////////////////////////////////////////////////
			/// @author RCS
			/// @ingroup m61
			class Importer_:
				public impex::PermanentThreadImporterTemplate<GPSdFileFormat>
			{
			public:
				static const std::string PARAMETER_ADDRESS;
				static const std::string PARAMETER_PORT;

			private:
				std::string _address;
				int _port;

				typedef enum
				{
					OFFLINE,
					ONLINE,
					WAITING
				} GpsStatus;

				mutable GpsStatus _gpsStatus;
				mutable boost::asio::io_service _ios;  
				mutable boost::asio::ip::tcp::socket _socket;
				mutable boost::shared_ptr<geos::geom::Point> _lastPosition;
				mutable pt::StopPoint* _lastStopPoint;
				mutable boost::posix_time::ptime _lastStorage;

				bool _updateFromGps(double &lat, double &lon) const;
				bool _loadPositionJSON(boost::asio::streambuf &ss,
					double &lat,
					double &lon) const;


			protected:

				virtual void _onStart() const;

				virtual void _loop() const;

				virtual boost::posix_time::time_duration _getWaitingTime() const;

				virtual void _onStop() const;

			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author RCS
				/// @date 2013
				/// @since 3.9.0
				virtual util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author RCS
				/// @date 2013
				/// @since 3.9.0
				virtual void setFromParametersMap(
					const util::ParametersMap& map,
					bool doImport
				);
			};

			typedef impex::NoExportPolicy<GPSdFileFormat> Exporter_;
		};
}	}

#endif // SYNTHESE_pt_GPSdFileFormat_hpp__


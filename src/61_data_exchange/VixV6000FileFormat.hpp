
/** VixV6000FileFormat class header.
	@file VixV6000FileFormat.hpp

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

#ifndef SYNTHESE_pt_VixV6000FileFormat_hpp__
#define SYNTHESE_pt_VixV6000FileFormat_hpp__

#include "FileFormatTemplate.h"
#include "NoImportPolicy.hpp"
#include "PermanentThreadExporterTemplate.hpp"

#include "vix/VIX-CIntSurvMsg.hpp"
#include "vix/VIX-timeutil.hpp"

namespace synthese
{
	class SerialReader;

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Test poller.
		//////////////////////////////////////////////////////////////////////////
		/// @author RCS
		/// @ingroup m61
		class VixV6000FileFormat:
			public impex::FileFormatTemplate<VixV6000FileFormat>
		{
		public:
			typedef impex::NoImportPolicy<VixV6000FileFormat> Importer_;


			//////////////////////////////////////////////////////////////////////////
			class Exporter_:
				public impex::PermanentThreadExporterTemplate<VixV6000FileFormat>
			{
			public:
				static const std::string PARAMETER_COM_PORT_NUMBER;
				static const std::string PARAMETER_COM_PORT_RATE;
				static const std::string PARAMETER_DATASOURCE_ID;
				static const std::string PARAMETER_PATCH_DIRECTION_BELFORT;

			private:
				typedef enum
				{
					INCONSISTENT_PARAMETERS,
					ONLINE_POLLING,
					ONLINE_SELECTING,
					OFFLINE
				} Status;

				int _comPortNb;
				int _comPortRate;
				boost::shared_ptr<const impex::DataSource> _dataSource;
				bool _patchDirectionBelfort;

				mutable Status _status;
				mutable std::auto_ptr<SerialReader> _srt;
				mutable unsigned long long _timeNextMessage;
				mutable TimeUtil _tu;

				CIntSurvMsg _getMessage() const;

			protected:
				virtual void _onStart() const;

				virtual void _loop() const;

				virtual boost::posix_time::time_duration _getWaitingTime() const;

				virtual void _onStop() const;

			public:
				Exporter_(const impex::Export& export_);

				void startPolling() const;

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
				virtual void setFromParametersMap(const util::ParametersMap& map);
			};
		};
}	}

#endif // SYNTHESE_pt_VixV6000FileFormat_hpp__


////////////////////////////////////////////////////////////////////////////////
/// DisplayGetNagiosStatusFunction class header.
///	@file DisplayGetNagiosStatusFunction.h
///	@author Hugues Romain
///	@date 2008-12-15 0:01
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_CPUGetWiredScreensFunction_H__
#define SYNTHESE_CPUGetWiredScreensFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreenCPU;

		////////////////////////////////////////////////////////////////////////
		///	CPUGetWiredScreensFunction public function class.
		/// @author Hugues Romain
		/// @date 2009
		/// @ingroup m54Functions refFunctions
		///
		/// This function returns informations about the display screen wired
		/// to the specified CPU.
		///
		/// The response follows a light XML schema :
		///
		///	@code
		/// <?xml version='1.0' encoding='UTF-8'?>
		/// <displayScreens>
		/// <displayscreen id="123412341234" comPort="3" />
		/// </displayScreens>
		/// @endcode
		///
		/// This function is also in charge of keeping the contact between the
		/// cpu and the server. It writes a fake monitoring entry in the database
		/// indicating that the cpu is still alive.
		///
		class CPUGetWiredScreensFunction
			:	public util::FactorableTemplate<server::Function, CPUGetWiredScreensFunction>
		{
		public:
			static const std::string PARAMETER_CPU_ID;
			static const std::string PARAMETER_CPU_MAC_ADDRESS;

			static const std::string DISPLAY_SCREEN_XML_TAG;
			static const std::string DISPLAY_SCREENS_XML_TAG;
			static const std::string DISPLAY_SCREEN_ID_XML_FIELD;
			static const std::string DISPLAY_SCREEN_COMPORT_XML_FIELD;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreenCPU>	_cpu;
			//@}


			////////////////////////////////////////////////////////////////////
			///	Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			util::ParametersMap _getParametersMap() const;



			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			void _setFromParametersMap(const util::ParametersMap& map);


		public:
			////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			///	@param stream Stream to write the output on
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			////////////////////////////////////////////////////////////////////
			///	Defines the security policy of the action.
			///	@author Hugues Romain
			///	@date 2008
			/// At the time, the function can be used without restrictions.
			virtual bool isAuthorized(const server::Session* session) const;



			virtual std::string getOutputMimeType() const;

			void setCPU(util::RegistryKeyType id);
			void setCPU(const std::string& mac);
		};
	}
}

#endif // SYNTHESE_DisplayGetNagiosStatusFunction_H__

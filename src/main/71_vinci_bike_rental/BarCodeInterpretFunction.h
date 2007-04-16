
/** BarCodeInterpretFunction class header.
	@file BarCodeInterpretFunction.h

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

#ifndef SYNTHESE_BarCodeInterpretFunction_H__
#define SYNTHESE_BarCodeInterpretFunction_H__

#include "01_util/UId.h"

#include "11_interfaces/RequestWithInterfaceAndRequiredSession.h"

namespace synthese
{
	namespace vinci
	{
		class VinciBike;
		class VinciContract;

		/** BarCodeInterpretFunction class.
			@ingroup m71Functions refFunctions

			The action depends on the preceding displayed page and from the kind of number readed :
				- bike id  :
					- if the bike is rented, and if the current page is the corresponding contract, then directly return the bike
					- if the bike is rented, and if the current page is not the corresponding contract, then display the contract
					- if the bike is not rented, and if the current page is a contract, then directly rent the bike
					- if the bike is not rented, and if the current page is not a contract, then display the bike page
				- contract id :
					- if the contract contains one or more rented bikes and if the contract is already displayed, then the bikes are directly returned
					- in other cases, display the contract page
				- other text :
					- display the contract search by user name and search the entered text (this function can be used to search a customer from text coming from a manual typing)
		*/
		class BarCodeInterpretFunction : public interfaces::RequestWithInterfaceAndRequiredSession
		{
		public:
			static const std::string PARAMETER_READED_CODE;
			static const std::string PARAMETER_LAST_PAGE;
			static const std::string PARAMETER_LAST_OBJECT_ID;

		protected:
			//! \name Page parameters
			//@{
				std::string							_lastPage;
				uid									_lastId;
				int									_tableId;
				boost::shared_ptr<VinciBike>		_bike;
				boost::shared_ptr<VinciContract>	_contract;
				std::string							_strCode;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;
		};
	}
}

#endif // SYNTHESE_BarCodeInterpretFunction_H__

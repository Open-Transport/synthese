
/** FareType class header.
	@file FareType.hpp

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

#ifndef SYNTHESE_FareType_H__
#define SYNTHESE_FareType_H__

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>

namespace synthese
{
	namespace fare
	{
		/** FareType handling class
			@ingroup m30
		*/
		class FareType
		{
		 public:

			enum FareTypeNumber
			{
				FARE_TYPE_UNKNOWN = -1,
				FARE_TYPE_FLAT_RATE = 0,
//				FARE_TYPE_SECTION = 1,
				FARE_TYPE_DISTANCE = 2
//				FARE_TYPE_ZONAL = 3
			};
			typedef std::vector<std::pair<boost::optional<FareTypeNumber>, std::string> > TypesList;

			typedef struct Slice
			{
				int min;
				int max;
				double price;
			} Slice;
			typedef std::vector<Slice> Slices;

			static std::string GetTypeName(FareTypeNumber value);
			static TypesList GetTypesList();

		 protected:

			FareTypeNumber _typeNumber; //!< Fare type number
			double _accessPrice;
			Slices _slices;
			bool _isUnitPrice;
//			std::string matrix;
//			std::string subFares;

		public:

			FareType(FareTypeNumber typeNumber);



			virtual double fareCalculation()
			{
				return _accessPrice;
			}



			//! @name Getters
			//@{
				const FareTypeNumber& getTypeNumber() const { return _typeNumber; }
				double getAccessPrice() const { return _accessPrice; }
				bool getIsUnitPrice() const { return _isUnitPrice; }
				const Slices& getSlices() const { return _slices; }
			//@}

			//! @name Setters
			//@{
				void setAccessPrice(const double accessPrice) { _accessPrice = accessPrice; }
				void setIsUnitPrice(const bool isUnitPrice) { _isUnitPrice = isUnitPrice; }
				void setSlices(const Slices& slices) {_slices = slices; }
			//@}
		};
	}
}

#endif

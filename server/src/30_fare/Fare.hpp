
/** Fare class header.
	@file Fare.hpp

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

#ifndef SYNTHESE_ENV_FARE_H
#define SYNTHESE_ENV_FARE_H

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "FareType.hpp"
#include "ServicePointer.h"
#include "StringField.hpp"

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace fare
	{
		FIELD_ENUM(FareTypeEnum, fare::FareType::FareTypeNumber)
		FIELD_STRING(Currency)
		FIELD_INT(PermittedConnectionsNumber)
		FIELD_BOOL(RequiredContinuity)
		FIELD_INT(ValidityPeriod)
		FIELD_DOUBLE(Access)
		FIELD_STRING(Slices)
		FIELD_DOUBLE(UnitPrice)
		FIELD_STRING(Matrix)
		FIELD_STRING(SubFares)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(FareTypeEnum),
			FIELD(Currency),
			FIELD(PermittedConnectionsNumber),
			FIELD(RequiredContinuity),
			FIELD(ValidityPeriod),
			FIELD(Access),
			FIELD(Slices),
			FIELD(UnitPrice),
			FIELD(Matrix),
			FIELD(SubFares)
		> FareSchema;

		/** Fare handling class
			@ingroup m30
		*/
		class Fare:
			public virtual Object<Fare, FareSchema>
		{
		private:
			static const std::string ROWS_SEPARATOR;
			static const std::string FIELDS_SEPARATOR;

			boost::shared_ptr<FareType> _type; //!< Fare type

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key id of the object (optional)
			/// @author Gaël Sauvanet
			/// @date 2011
			Fare(
				util::RegistryKeyType key = 0
			);



			//////////////////////////////////////////////////////////////////////////
			/// Set the FareType according to the corresponding FareType number
			/// @param number the number of the FareType
			/// @author Gaël Sauvanet
			/// @date 2011
			void setTypeNumber(FareType::FareTypeNumber number);



			//! @name Getters
			//@{
				virtual std::string getName() const { return get<Name>(); }
				const boost::shared_ptr<FareType> getType() const { return _type; }
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			//@}
			
			static FareType::Slices GetSlicesFromSerializedString(
				const std::string& serializedString
			);
			static std::string SerializeSlices(
				const FareType::Slices& object
			);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif

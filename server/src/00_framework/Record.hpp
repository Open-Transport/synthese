
/** Record class header.
	@file Record.hpp

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

#ifndef SYNTHESE__Record_hpp__
#define SYNTHESE__Record_hpp__

#include "Exception.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <vector>

namespace geos
{
	namespace geom
	{
		class Geometry;
		class GeometryFactory;
	}
}

namespace synthese
{
	/** Record class.
		@ingroup m
	*/
	class Record
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Missing parameter at parsing exception.
		/// @ingroup m01Exception refException
		class MissingParameterException:
			public Exception
		{
		private:
			const std::string _field;

		public:
			MissingParameterException(
				const std::string& field
			):	Exception("Missing parameter in request parsing : " + field),
				_field(field)
			{}

			virtual ~MissingParameterException() throw() {}

			const std::string& getField() const { return _field; }
		};

		Record();


		//////////////////////////////////////////////////////////////////////////
		/// Gets the value of a field
		/// @param fieldName name of the parameter to read
		/// @param exceptionIfMissing throws an exception if the parameter is undefined, else returns an empty string
		/// @return the value of the parameter
		virtual std::string getValue(
			const std::string& fieldName,
			bool exceptionIfMissing = true
		) const = 0;



		typedef std::vector<std::string> FieldNames;

		//////////////////////////////////////////////////////////////////////////
		/// Gets the list of the available fields
		virtual FieldNames getFieldNames() const = 0;



		//////////////////////////////////////////////////////////////////////////
		/// Tests if the specified parameter is defined in the map.
		/// @param parameterName key of the parameter to test
		/// @return true if the parameter is present in the map even if its value
		/// is empty
		/// @author Hugues Romain
		/// @since 3.2.0
		/// @date 2010
		virtual bool isDefined(
			const std::string& parameterName
		) const = 0;



		//////////////////////////////////////////////////////////////////////////
		/// Gets the value of an optional parameter and converts it into C type if
		/// available.
		/// @param parameterName key of the parameter to get
		/// @return the parameter value converted into the C type, undefined if
		/// impossible.
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// If the parameter does not exist, is empty, or cannot be converted into the type
		/// specified by the template parameter C, then an undefined object is
		/// returned
		template<class C>
		boost::optional<C> getOptional(
			const std::string& parameterName,
			bool trim = true
		) const;




		//////////////////////////////////////////////////////////////////////////
		/// Tests if the specified parameter is defined, non empty and non equal to 0.
		/// @param parameterName key of the parameter to test
		/// @return true if the parameter is present in the map, non empty, and non equal to 0
		/// @author Hugues Romain
		/// @since 3.3.0
		/// @date 2011
		bool isTrue(const std::string& parameterName) const;



		//////////////////////////////////////////////////////////////////////////
		/// Gets the value of a compulsory parameter and converts it into C type.
		/// @param parameterName key of the parameter to get
		/// @return the parameter value converted into the C type
		/// @author Hugues Romain
		/// @throws MissingParameterException if the parameter does not exist or
		/// cannot be converted into the type specified by the template parameter C.
		template<class C>
		C get(
			const std::string& parameterName,
			bool trim = true
		) const;



		//////////////////////////////////////////////////////////////////////////
		/// Gets the value of an optional parameter and converts it into C type if
		/// available.
		/// @param parameterName key of the parameter to get
		/// @param defaultValue value to return if the parameter is undefined
		/// @return the parameter value converted into the C type, a default value
		/// if impossible.
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// If the parameter does not exist, is empty, or cannot be converted into
		/// the type specified by the template parameter C, then a default value
		/// is returned
		template<class C>
		C getDefault(
			const std::string& parameterName,
			const C defaultValue,
			bool trim = true
		) const;



		//////////////////////////////////////////////////////////////////////////
		/// Gets the value of an optional parameter and converts it into C type if
		/// available.
		/// @param parameterName key of the parameter to get
		/// @return the parameter value converted into the C type, a default value
		/// if impossible. The value will be trimmed.
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// If the parameter does not exist, is empty, or cannot be converted into
		/// the type specified by the template parameter C, then a the object
		/// constructed by default constructor returned.
		template<class C>
		C getDefault(
			const std::string& parameterName
		) const;



		//////////////////////////////////////////////////////////////////////////
		/// Trims a string considering spaces, tabs, and carriage returns as blank characters.
		/// @param value the string to trim
		/// @return the trimmed string
		static std::string Trim(const std::string& value);



		//////////////////////////////////////////////////////////////////////////
		/// Reads a WKT column and transform it into geometry.
		/// @param col the column to read
		/// @return the geometry object corresponding to the WKT data
		/// @pre the column must store WKT data
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.2.0
		boost::shared_ptr<geos::geom::Geometry> getGeometryFromWKT(
			const std::string& col,
			boost::optional<const geos::geom::GeometryFactory&> factory =
				boost::optional<const geos::geom::GeometryFactory&>()
		) const;
	};


	template<class C>
	boost::optional<C> Record::getOptional(
		const std::string& parameterName,
		bool trim
	) const {
		try
		{
			if(!isDefined(parameterName))
			{
				return boost::optional<C>();
			}
			std::string value(
				trim ?
				Trim(getValue(parameterName)) :
				getValue(parameterName)
			);
			if(value.empty())
			{
				return boost::optional<C>();
			}
			return boost::lexical_cast<C>(value);
		}
		catch(boost::bad_lexical_cast&)
		{
			return boost::optional<C>();
		}
	}



	template<class C>
	C Record::getDefault(
		const std::string& parameterName,
		const C defaultValue,
		bool trim
	) const {
		try
		{
			if(!isDefined(parameterName))
			{
				return defaultValue;
			}
			std::string value(
				trim ?
				Trim(getValue(parameterName)) :
				getValue(parameterName)
			);
			if(value.empty())
			{
				return defaultValue;
			}
			return boost::lexical_cast<C>(value);
		}
		catch(boost::bad_lexical_cast&)
		{
			return defaultValue;
		}
	}



	template<class C>
	C Record::getDefault(
		const std::string& parameterName
	) const {
		return getDefault(parameterName, C(), true);
	}



	template<class C>
	C Record::get(
		const std::string& parameterName,
		bool trim
	) const {
		try
		{
			std::string value(
				trim ?
				Trim(getValue(parameterName)) :
				getValue(parameterName)
			);
			return boost::lexical_cast<C>(value);
		}
		catch(boost::bad_lexical_cast&)
		{
			throw MissingParameterException(parameterName);
		}
	}

}

#endif // SYNTHESE__Record_hpp__


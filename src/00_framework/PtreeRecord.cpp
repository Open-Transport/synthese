
/** PtreeRecord class implementation.
	@file PtreeRecord.cpp

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

#include "PtreeRecord.hpp"

#include <boost/foreach.hpp>

using namespace boost::property_tree;
using namespace std;

namespace synthese
{
	PtreeRecord::PtreeRecord(
		const boost::property_tree::ptree& tree
	):	_tree(tree)
	{}



	//////////////////////////////////////////////////////////////////////////
	/// Gets the value of a field
	/// @param fieldName name of the parameter to read
	/// @param exceptionIfMissing throws an exception if the parameter is undefined, else returns an empty string
	/// @return the value of the parameter
	std::string PtreeRecord::getValue(
		const std::string& fieldName,
		bool exceptionIfMissing /*= true */
	) const {

		return
			exceptionIfMissing ?
			_tree.get<string>(fieldName) :
			_tree.get<string>(fieldName, string());
	}



	//////////////////////////////////////////////////////////////////////////
	/// Gets the list of the available fields
	Record::FieldNames PtreeRecord::getFieldNames() const
	{
		Record::FieldNames result;
		BOOST_FOREACH(const ptree::value_type& item, _tree)
		{
			result.push_back(item.first);
		}
		return result;
	}



	//////////////////////////////////////////////////////////////////////////
	/// Tests if the specified parameter is defined in the map.
	/// @param parameterName key of the parameter to test
	/// @return true if the parameter is present in the map even if its value
	/// is empty
	/// @author Hugues Romain
	/// @since 3.9.0
	/// @date 2013
	bool PtreeRecord::isDefined( const std::string& parameterName ) const
	{
		return _tree.count(parameterName) > 0;
	}
}


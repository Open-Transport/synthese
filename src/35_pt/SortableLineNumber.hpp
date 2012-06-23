//////////////////////////////////////////////////////////////////////////////////////////
/// SortableLineNumber class header.
/// @file SortableLineNumber.hpp
/// @author Hugues Romain
/// @date 2012
///
/// This file belongs to the SYNTHESE project (public transportation specialized software)
/// Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_SortableLineNumber_H__
#define SYNTHESE_SortableLineNumber_H__

#include <string>

namespace synthese
{
	namespace pt
	{
		class SortableLineNumber
		{
		private:
			std::string _value;
			std::string _begin; // = _value except for 12s it is 12
			std::string _end; // = "" except for 12s it is  s
			long int _numericalValue; // = 12 for 12 and 12s, = -1 for A
			bool _lettersBeforeNumbers;

			typedef enum {
				isAnInteger, // example : 12
				beginIsInteger, // example : 12s
				beginIsNotInteger // example : A, or T1
			} numberType;
			numberType _numberType;

		public:
			SortableLineNumber(std::string str, bool lettersBeforeNumbers);
			bool operator<(SortableLineNumber const &otherNumber) const;
			bool operator!=(SortableLineNumber const &otherNumber) const;
		};
	}
}

#endif // SYNTHESE_SortableLineNumber_H__

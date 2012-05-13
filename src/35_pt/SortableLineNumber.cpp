////////////////////////////////////////////////////////////////////////////////
/// SortableLineNumber class implementation.
///	@file SortableLineNumber.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "SortableLineNumber.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace synthese
{
	namespace pt
	{
		SortableLineNumber::SortableLineNumber(string str, bool lettersBeforeNumbers):
			_lettersBeforeNumbers(lettersBeforeNumbers)
		{
			_numericalValue = -1;
			_value = str;

			char * buffer = strdup(_value.c_str());
			char * pEnd;

			errno = 0;

			_numericalValue = strtol (buffer, &pEnd, 10);
			if (errno != 0 || pEnd == buffer) // Is A form
			{
				_numberType = beginIsNotInteger;
				_begin = _value;
				_end = "";
			}
			else
			{
				if(*pEnd != 0 ) // Is 12s form so pEnd is "s";
				{
					_numberType = beginIsInteger;
					_end = string(pEnd);
					*pEnd = '\0';
					_begin = string(buffer);
				}
				else // Is 12 form
				{
					_numberType = isAnInteger;
					_begin = _value;
					_end = "";
				}
			}
		}

		bool SortableLineNumber::operator<(SortableLineNumber const &otherNumber) const
		{
			if((_numberType != beginIsInteger)
					&& (otherNumber._numberType != beginIsInteger)) // No number have form "12S"
			{
				if((_numberType == beginIsNotInteger)
						&& (otherNumber._numberType == beginIsNotInteger)) // They have both form "A"
				{
					return _value < otherNumber._value;
				}
				else if((_numberType == beginIsNotInteger)
						&& (otherNumber._numberType == beginIsNotInteger)) // One is "A" form, other is "23" form
				{
					return _lettersBeforeNumbers;
				}
				else // The two numbers have form 12
				{
					return _numericalValue < otherNumber._numericalValue;
				}
			}
			else if(_begin != otherNumber._begin) // At least one number have form 12S, and the other one have a different begin (eg. 13)
			{
				if(	_numberType == isAnInteger ||
					otherNumber._numberType == isAnInteger ||
					_numberType == beginIsInteger ||
					otherNumber._numberType == beginIsInteger
				) // here is a "13" type and an "12s" form
				{
					return _numericalValue < otherNumber._numericalValue;
				}
				else // There is a "A" type and an "12s" form
				{
					return _lettersBeforeNumbers;
				}
			}
			else // Case 12/12s or 12s/12k : sort based on end
			{
				return _end < otherNumber._end;
			}
		}
	}
}

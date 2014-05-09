////////////////////////////////////////////////////////////////////////////////
/// Exception class header.
///	@file Exception.h
///	@author Hugues Romain
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

#ifndef SYNTHESE_UTIL_KILLPROCESSEXCEPTION_H
#define SYNTHESE_UTIL_KILLPROCESSEXCEPTION_H

#include <string>
#include <iostream>

/// @defgroup refExceptions KillProcessExceptions
///	@ingroup ref

namespace synthese
{
	//lint --e{1712}

	////////////////////////////////////////////////////////////////////
	/// KillProcessException exception class
	///	@ingroup m01
	class KillProcessException : public std::exception
	{
	 private:

		const std::string _message; //!< Error message


	 public:

		KillProcessException(const std::string& message ) throw ();
		KillProcessException(const synthese::KillProcessException& ref );

		virtual ~KillProcessException () throw ();


		//! @name Getters/Setters
		//@{
		const std::string& getMessage () const;
		//@}

		//! @name Query methods
		//@{
		const char* what () const throw ();

	 private:

		KillProcessException& operator= ( const KillProcessException& rhs );

	};
}

#endif

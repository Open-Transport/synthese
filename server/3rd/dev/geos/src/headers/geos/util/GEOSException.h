/**********************************************************************
 * $Id: GEOSException.h 2556 2009-06-06 22:22:28Z strk $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_UTIL_GEOSEXCEPTION_H
#define GEOS_UTIL_GEOSEXCEPTION_H

#include <geos/export.h>
#include <stdexcept>
#include <string>

namespace geos {
namespace util { // geos.util

/**
 *
 * \brief Base class for all GEOS exceptions.
 *
 * Exceptions are thrown as pointers to this type.
 * Use toString() to get a readable message.
 */
class GEOS_DLL GEOSException: public std::exception {

	std::string _msg;

public:

	GEOSException()
		:
		_msg("Unknown error")
	{}

	GEOSException(std::string const& msg)
		:
		_msg(msg)
	{}

	GEOSException(std::string const& name, std::string const& msg)
		:
		_msg(name+": "+msg)
	{}

	virtual ~GEOSException() throw()
	{}

	const char* what() const throw()
	{
		return _msg.c_str();
	}

};

} // namespace geos.util
} // namespace geos

#endif // GEOS_UTIL_GEOSEXCEPTION_H

/**********************************************************************
 * $Log$
 * Revision 1.4  2006/05/22 15:08:01  strk
 * Print colon after exception name (as it has always been)
 *
 * Revision 1.3  2006/04/05 09:34:20  strk
 * GEOSException derived from std::exception again, hopefully the correct way now
 *
 * Revision 1.2  2006/04/04 08:16:46  strk
 * Changed GEOSException hierarchy to be derived from std::runtime_exception.
 * Removed the GEOSException::toString redundant method (use ::what() instead)
 *
 * Revision 1.1  2006/03/09 16:46:49  strk
 * geos::geom namespace definition, first pass at headers split
 *
 **********************************************************************/

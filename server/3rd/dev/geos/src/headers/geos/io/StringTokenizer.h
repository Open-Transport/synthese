/**********************************************************************
 * $Id: StringTokenizer.h 2775 2009-12-03 19:38:12Z mloskot $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: ORIGINAL WORK
 *
 **********************************************************************/

#ifndef GEOS_IO_STRINGTOKENIZER_H
#define GEOS_IO_STRINGTOKENIZER_H

#include <geos/export.h>

#include <string>

namespace geos {
namespace io {

class GEOS_DLL StringTokenizer {
public:
	enum {
		TT_EOF,
		TT_EOL,
		TT_NUMBER,
		TT_WORD
	};
	//StringTokenizer();
	StringTokenizer(const std::string& txt);
	~StringTokenizer() {};
	int nextToken();
	int peekNextToken();
	double getNVal();
	std::string getSVal();
private:
	const std::string &str;
	std::string stok;
	double ntok;
	std::string::const_iterator iter;

    // Declare type as noncopyable
    StringTokenizer(const StringTokenizer& other);
    StringTokenizer& operator=(const StringTokenizer& rhs);
};

} // namespace io
} // namespace geos

#endif // #ifndef GEOS_IO_STRINGTOKENIZER_H

/**********************************************************************
 * $Log$
 * Revision 1.1  2006/03/20 18:18:14  strk
 * io.h header split
 *
 **********************************************************************/

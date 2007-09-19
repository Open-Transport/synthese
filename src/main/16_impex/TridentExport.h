
/** tridentexport class header.
	@file tridentexport.h

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


#ifndef SYNTHESE_IMPEX_TRIDENTEXPORT_H
#define SYNTHESE_IMPEX_TRIDENTEXPORT_H

#include "01_util/UId.h"


#include <iostream>


namespace synthese
{

namespace env
{
    class Service;
}


namespace impex
{



/** Trident/Chouette export.
 */
    
class TridentExport
{
public:

    
private:
    
    TridentExport ();
    ~TridentExport();

    
public:

    /** -> ChouettePTNetwork
     */
    static void Export (std::ostream& os, 
			const uid& commercialLineId);

private:
    
    static std::string TridentId (const std::string& peer, const std::string clazz, const uid& id);
    static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
    static std::string TridentId (const std::string& peer, const std::string clazz, const env::Service* srv);


};







}
}


#endif


/** ThreadExec class header.
	@file ThreadExec.h

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


#ifndef SYNTHESE_UTIL_THREADEXEC_H
#define SYNTHESE_UTIL_THREADEXEC_H

#include <string>
#include <iostream>


#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>


namespace synthese
{
namespace util
{




/** Abstract class defining thread execution in terms of initialization
    and thread atomic loop.

@ingroup m01
*/
class ThreadExec
{
 public:


 private:

 protected:

 public:

    ThreadExec ();
    virtual ~ThreadExec ();


    virtual void initialize ();

    /** Thread execution loop.
	It is assumed that one execution loop is "short" enough and non-blocking
	so that thread life can be monitored properly.
    */
    virtual void loop () = 0;
    virtual void finalize ();

    void loops (int nbLoops = 5);

 private:

};




}

}
#endif


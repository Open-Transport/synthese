////////////////////////////////////////////////////////////////////////////////
/// ThreadAware class header.
///	@file ThreadAware.h
///	@author Marc Jambert
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

#ifndef SYNTHESE_UTIL_THREADAWARE_H
#define SYNTHESE_UTIL_THREADAWARE_H


#include <boost/thread/thread.hpp>




namespace synthese
{
namespace util
{


/** Base class that gurantees that the created object is aware of the thread which ordered
    its construction.

@ingroup m01
*/
class ThreadAware
{
private:

    const boost::thread _ownerThread;

 protected:

    ThreadAware ();
    virtual ~ThreadAware ();

 public:

    const boost::thread& getOwnerThread () const { return _ownerThread; }

    /** Returns wheter or not this call is done inside this object owner thread.
     */
    bool insideOwnerThread () const;

};



}

}
#endif


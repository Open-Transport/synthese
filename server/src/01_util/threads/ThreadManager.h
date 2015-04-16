
/** ThreadManager class header.
	@file ThreadManager.h

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


#ifndef SYNTHESE_UTIL_THREADMANAGER_H
#define SYNTHESE_UTIL_THREADMANAGER_H

#include <vector>
#include <string>
#include <map>
#include <iostream>




namespace synthese
{
namespace util
{


    class ManagedThread;


/** Thread manager.
@todo add thread group management.

@ingroup m01
*/
class ThreadManager
{
 public:


 private:

    static bool _MonothreadEmulation;

    typedef struct {
	ManagedThread* thread;
	unsigned long nbLoopsPrev;
	unsigned long nbLoopsPrevPrev;
    } ManagedThreadEntry;

    std::vector<std::string> _names;  //!< Thread names ordered by registration.
    std::map<std::string, ManagedThreadEntry> _threads;  //!< Managed threads.


    ThreadManager (bool monothreadEmulation = true);

 protected:

 public:

    ~ThreadManager ();

    static ThreadManager* Instance ();

    void run ();

    static bool GetMonothreadEmulation ();
    static void SetMonothreadEmulation (bool monothreadEmulation);

 private:

    void checkForDeadThreads ();

    void add (ManagedThread* thread, bool autoRespawn = false);
    void remove (ManagedThread* thread);

    friend class ManagedThread;


};




}

}
#endif



/** SendTokenThreadExec class header.
	@file SendTokenThreadExec.h

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


#ifndef SYNTHESE_DBRING_SENDTOKENTHREADEXEC_H
#define SYNTHESE_DBRING_SENDTOKENTHREADEXEC_H

#include "03_db_ring/TransmissionStatusMap.h"
#include "03_db_ring/Token.h"

#include "01_util/threads/ThreadExec.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>



namespace synthese
{


namespace dbring
{



/** Send token thread execution.

@ingroup m03
*/
class SendTokenThreadExec : public synthese::util::ThreadExec
{
 private:

    const NodeId _emitterNodeId;
    const NodeInfo _nodeInfo;
    std::stringstream _tokenBuffer;
    TransmissionStatusMap& _transmissionStatusMap;

 public:

    SendTokenThreadExec (const NodeId& emitterNodeId, 
			 const NodeInfo& nodeInfo, 
			 const TokenSPtr token, 
			 TransmissionStatusMap& transmissionStatusMap);


    /** Execution body.
     */
    void loop ();

};


}
}



#endif


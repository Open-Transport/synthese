/** SCOMModule class header.
	@file SCOMModule.h

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

#ifndef SYNTHESE_SCOMModule_h__
#define SYNTHESE_SCOMModule_h__

#include "ModuleClassTemplate.hpp"

namespace synthese
{

	/**	SCOM real time XML fetcher

	  This module reads a TCP socket from an SCOM server (Ineo SAE) and stores
	  its data internally.

	  The data is stored only for a certain amount of time, after what this data will be destroyed.
	  There is no persistent storage as this data is real time only.

	  TODO : SCOM infos

	  TODO : Format infos

	  TODO : Use
	@{
	*/

	//////////////////////////////////////////////////////////////////////////
	/// 62 SCOM Module namespace.
	///	@author Bastien Noverraz (TL)
	///	@date 2014
	//////////////////////////////////////////////////////////////////////////
	namespace scom
	{
		class SCOMSocketReader;
		class SCOMData;

		/** 62 SCOM module class.
		*/
		class SCOMModule:
			public server::ModuleClassTemplate<SCOMModule>
		{
			friend class server::ModuleClassTemplate<SCOMModule>;

		public:

			//! Connect to the SCOM server and fetch its data
			static void Start();

			//! Close the socket connection
			static void End();

			// TEST
			static std::string Test ();

			/** Does nothing
			@{
			*/
			static void PreInit();
			static void Init();
			static void InitThread();
			static void CloseThread();
			/** @} */

		private:
			// The socket listener
			static SCOMSocketReader* _sr;
			// The data gatherer
			static SCOMData* _sd;
		};
	}
	/** @} */
}

#endif // SYNTHESE_SCOMModule_h__

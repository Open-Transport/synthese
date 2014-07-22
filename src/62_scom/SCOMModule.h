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

	/**	Here comes a one-line definition of the module.

		Here, a full documentation about de module abilities and how to use it should be written.
		Please, do it.

	@{
	*/

	//////////////////////////////////////////////////////////////////////////
	/// 62 SCOM Module namespace.
	///	@author Exemple
	///	@date 2014
	//////////////////////////////////////////////////////////////////////////
	namespace scom
	{
		/** 62 SCOM module class.
		*/
		class SCOMModule:
			public server::ModuleClassTemplate<SCOMModule>
		{
			friend class server::ModuleClassTemplate<SCOMModule>;

		public:

			/** First step of initialization of the module.
				This method is launched when the server starts.
				Must be implemented for each instantiation.
			*/
			static void PreInit();

			/** Second step of initialization of the module.
				This method is launched after PreInit is launched for each module.
				Must be implemented for each instantiation.
			*/
			static void Init();

			/** Third step of initialization of the module.
				This method is launched after Init is launched for each module.
				Must be provided by each instance if needed. Default does nothing.
			*/
			static void Start();

			/** Ending of the module.
				This method is launched when the server stops.
				Must be implemented for each instantiation.
			*/
			static void End();

			/** Operations to do at thread creation.
				@param thread the created thread
			*/
			static void InitThread();

			/** Operations to do at thread deletion.
				@param the deleted thread
			*/
			static void CloseThread();
		};
	}
	/** @} */
}

#endif // SYNTHESE_SCOMModule_h__

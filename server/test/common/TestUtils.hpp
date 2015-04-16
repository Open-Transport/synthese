/** Utilities for tests.
	@file TestUtils.hpp
	@author Sylvain Pasche
	@date 2011

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

#ifndef SYNTHESE_common_TestUtils_hpp__
#define SYNTHESE_common_TestUtils_hpp__

#include "01_util/Env.h"
#include "01_util/Log.h"
#include "15_server/ModuleClassTemplate.hpp"

#include <boost/test/unit_test.hpp>

using synthese::server::ModuleClass;
using synthese::server::ModuleClassTemplate;


struct GlobalFixture
{
	GlobalFixture()
	{
#ifdef _MSC_VER
		// Disable leak logs on Windows (they are set by Boost.Test).
		// TODO: fix the leaks and then remove this line
		_CrtSetDbgFlag(~_CRTDBG_LEAK_CHECK_DF & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));

		// Useful for attaching debugger to the test at startup.
		if (::getenv("SYNTHESE_PAUSE"))
			::system("pause");
#endif
	}

	~GlobalFixture()
	{
#ifdef _MSC_VER
		// For debugging, to keep the cmd window open.
		if (::getenv("SYNTHESE_PAUSE"))
			system("pause");
#endif
	}
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);

/// Register a registrable in the environment on construction and unregister it on destruction.
template <class Registrable>
struct ScopedRegistrable
{
	ScopedRegistrable()
	{
		synthese::util::Env::Integrate<Registrable>();
	}
	~ScopedRegistrable()
	{
		synthese::util::Env::Unregister<Registrable>();
	}
};

/// Register a factory on construction and unregister it on destruction.
template <class FactoryTemplate>
struct ScopedFactory
{
	ScopedFactory()
	{
		FactoryTemplate::integrate();
	}
	~ScopedFactory()
	{
		FactoryTemplate::unregister();
	}
};

/// Calls the module PreInit() and Init() methods on construction and End() on destruction.
template <class Module>
struct ScopedModule
{
	ScopedModule()
	{
		ModuleClassTemplate<Module>::PreInit();
		ModuleClassTemplate<Module>::Init();
	}
	~ScopedModule()
	{
		ModuleClassTemplate<Module>::End();
	}
};

#endif // SYNTHESE_common_TestUtils_hpp__

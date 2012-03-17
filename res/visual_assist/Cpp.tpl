a::#d:
#define 
a::#u:
#undef 
a::#p:
#pragma 
a::#in:
#include "$end$"
a::#in:
#include <$end$>
a::#im:
#import "$end$"
a::#im:
#import <$end$>
a::#if:
#ifdef $end$
$selected$
#endif
a::#if:
#if 0
$selected$$end$
#endif
a::#if:
#if defined($end$)
$selected$
#endif
a::#if:
#ifdef _DEBUG
$selected$$end$
#endif
a::#ifn:
#ifndef $end$
$selected$
#endif
a::#el:
#else
a::#eli:
#elif 
a::#en:
#endif
a::A:
ASSERT($end$)
a::b:
bool
a::B:
BOOL
a::C:
CHAR
a::CR:
CRITICAL_SECTION
a::D:
DWORD
a::DW:
DWORD
a::DW:
DWORDLONG
a::DW:
DWORD_PTR
a::DW:
DWORD32
a::DW:
DWORD64
a::r:
return
a::T:
TRUE
a::t:
true
a::F:
FALSE
a::f:
false
a::FL:
FLOAT
a::fl:
float
a::HA:
HANDLE
a::HB:
HBRUSH
a::HBI:
HBITMAP
a::HC:
HCURSOR
a::HD:
HDESK
a::HF:
HFILE
a::HFO:
HFONT
a::HGD:
HGDIOBJ
a::HGL:
HGLOBAL
a::HI:
HICON
a::HIN:
HINSTANCE
a::HM:
HMODULE
a::HPA:
HPALETTE
a::HRE:
HRESULT
a::H:
HWND
a::I:
INT
a::IN:
INT
a::IN:
INT_PTR
a::IN:
INT32
a::IN:
INT64
a::L:
LONG
a::LO:
LONG
a::LO:
LONGLONG
a::LO:
LONG_PTR
a::LO:
LONG32
a::LO:
LONG64
a::ll:
long long
a::LP:
LPARAM
a::LPB:
LPBYTE
a::LPC:
LPCSTR
a::LPC:
LPCTSTR
a::LPC:
LPCWSTR
a::LPS:
LPSTR
a::LPT:
LPTSTR
a::LPW:
LPWSTR
a::LR:
LRESULT
a::TC:
TCHAR
a::U:
UINT
a::UI:
UINT
a::UI:
UINT_PTR
a::UI:
UINT32
a::UI:
UINT64
a::UL:
ULONG
a::ULO:
ULONG
a::ULO:
ULONGLONG
a::ULO:
ULONG_PTR
a::ULO:
ULONG32
a::ULO:
ULONG64
a::WC:
WCHAR
a::WI:
WINAPI
a::W:
WORD
a::WP:
WPARAM
a::N:
NULL
a::nn:
!= NULL
a::n1:
!= 1
a::n0:
!= 0
a::nt:
!= true
a::nf:
!= false
a::nt:
!= TRUE
a::nf:
!= FALSE
a:GUID IMPLEMENT_OLECREATE:guid:
// {$GUID_STRING$} 
IMPLEMENT_OLECREATE($GUID_Class$, $GUID_ExternalName$, 
$GUID_DEFINITION$);

a:DEFINE_GUID:guid:
// {$GUID_STRING$} 
DEFINE_GUID($GUID_Name$, 
$GUID_DEFINITION$);

a:GUID struct instance:guid:
// {$GUID_STRING$} 
static const GUID $GUID_InstanceName$ = 
{ $GUID_STRUCT$ };

a:GUID string:guid:
"{$GUID_STRING$}"
a::usi:
using namespace $end$;

a::class:
class $end$
{
public:
protected:
private:
};

a:class with prompt for name:class:
class $Class_name$
{
public:
	$Class_name$();
	~$Class_name$();
protected:
	$end$
private:
};

a:dynamic cast, run code on valid cast:dyna:
$New_type$ *$New_pointer$ = dynamic_cast<$New_type$ *>($Cast_this$);
if($New_pointer$)
{
	$end$
}

a:dynamic cast, return on invalid cast:dyna:
$New_type$ *$New_pointer$ = dynamic_cast<$New_type$ *>($Cast_this$);
if($New_pointer$ == 0)
	return;

a::struct:
struct $end$ 
{
};

a::switch:
switch($end$)
{
	$selected$
}

a:switch one case:switch:
switch($end$)
{
case :
	$selected$
	break;
default:
    break;
}

a:switch two cases:switch:
switch($end$)
{
case :
	break;
case :
    break;
default:
    break;
}

a:switch four cases:switch:
switch($end$)
{
case :
	break;
case :
	break;
case :
    break;
case :
    break;
default:
    break;
}

a:switch eight cases:switch:
switch($end$)
{
case :
	break;
case :
	break;
case :
    break;
case :
    break;
case :
	break;
case :
	break;
case :
    break;
case :
    break;
default:
    break;
}

a:://-:
// $end$ [$MONTH$/$DAY$/$YEAR$ %USERNAME%]
a::///:
//////////////////////////////////////////////////////////////////////////

a::/*-:
/*
 *	$end$
 */
a:atoi()::
atoi($selected$)$end$
a:_T()::
_T($selected$)$end$
a:_T(''):tc:
_T('$Letter$')
a:_T(""):tc:
_T("$String$")
a:_T($clipboard$):tcc:
_T($clipboard$)
a:( ... )::
($selected$)
a:{ ... }::
{
	$selected$$end$
}

a:if () { ... }:if:
if ($end$)
{
	$selected$
}

a:if () { ... } else { }:if:
if ($end$)
{
	$selected$
} 
else
{
}

a:if () { } else { ... }::
if ($end$)
{
} 
else
{
	$selected$
}

a:while () { ... }:while:
while ($end$)
{
	$selected$
}

a:for () { ... }:for:
for ($end$)
{
	$selected$
}

a:for loop forward:forr:
for (int $Index$ = 0; $Index$ < $Length$ ; $Index$++)
{
	$end$
}

a:for loop reverse:forr:
for (int $Index$ = $Length$ - 1; $Index$ >= 0 ; $Index$--)
{
	$end$
}

a:for loop across STL container with const_iterator:forr:
for($container_type$::const_iterator it = $container$.begin(); it != $container$.end(); ++it)
{
    $end$
}
a:do { ... } while ():do:
do 
{
	$selected$
} while($end$);

a::catch:
catch ($end$) {
	$selected$
}

a:try { ... } catch {}:try:
try
{
	$selected$
}
catch (CException* e)
{
	$end$
}

a:try { ... } catch {} catch {} catch {}:try:
try
{
	$selected$
}
catch (CMemoryException* e)
{
	$end$
}
catch (CFileException* e)
{
}
catch (CException* e)
{
}

a:TRY { ... } CATCH {}:TRY:
TRY {
	$selected$
}
CATCH (CMemoryException, e)
{
	$end$
}
END_CATCH

a:#ifdef ... #endif::
#ifdef $end$
$selected$
#endif

a:#ifdef guard in a header::

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

$selected$
#endif // SYNTHESE_$FILE_BASE$_H__

a:// TODO::
// TODO: 
a:Empty File header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

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

#ifndef SYNTHESE_$module$_$FILE_BASE$_$FILE_EXT$__
#define SYNTHESE_$module$_$FILE_BASE$_$FILE_EXT$__

namespace synthese
{
	namespace $module$
	{
		/** $FILE_BASE$ class.
			@ingroup m$module_number$
		*/
		class $FILE_BASE$
		{
		public:
			$FILE_BASE$();

		};
	}
}

#endif // SYNTHESE_$module$_$FILE_BASE$_$FILE_EXT$__


a:Empty CPP implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

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

#include "$FILE_BASE$.hpp"

namespace synthese
{
	namespace $Module$
	{

}	}


a:12 Right Header::

/**	$FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$Module$_$FILE_BASE$_H__
#define SYNTHESE_$Module$_$FILE_BASE$_H__

#include "RightTemplate.h"

namespace synthese
{
	namespace $Module$
	{
		/** $FILE_BASE$ Right Class.
			@ingroup m$ModuleNumber$Rights refRights

			Here are the different right levels :

			Private rights :
				- USE : 
				- READ : 
				- WRITE : 
				- DELETE : 
				
			Public rights :
				- USE : 
				- READ : 
				- WRITE : 
				- DELETE : 
				
			Perimeter definition :
				- 
		*/
		class $FILE_BASE$ : public security::RightTemplate<$FILE_BASE$>
		{
		public:
			/** Displays the interpretation of the string perimeter of the right.
				@return description of the perimeter
			*/
			std::string	displayParameter(util::Env& env)	const;
			

			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(const std::string& perimeter, util::Env& env) const;
		};
}	}

#endif // SYNTHESE_$Module$_$FILE_BASE$_H__

a:12 Right Implementation::

/**	$FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "$FILE_BASE$.hpp"

#include "Env.h"
#include "SecurityConstants.hpp"

using namespace std;

namespace synthese
{
	using namespace $namespace$;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, $FILE_BASE$>::FACTORY_KEY("$Factory_key$");
	}

	namespace security
	{
		template<> const string RightTemplate<$FILE_BASE$>::NAME("$Name$");
		template<> const bool RightTemplate<$FILE_BASE$>::USE_PRIVATE_RIGHTS($Use_private_rights$);

		template<>
		ParameterLabelsVector RightTemplate<$FILE_BASE$>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(all)"));

			return m;
		}
	}

	namespace $namespace$
	{
		string $FILE_BASE$::displayParameter(Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";
			
			return _parameter;
		}



		bool $FILE_BASE$::perimeterIncludes(const string& perimeter, Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
}	}

a:00 Exception Implementation:EXC:

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.cpp

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

#include "$FILE_BASE$.hpp"

namespace synthese
{
	namespace $Namespace$
	{
		$FILE_BASE$::$FILE_BASE$(const std::string& message)
			: synthese::util::Exception(message)
		{
		}

		$FILE_BASE$::~$FILE_BASE$() throw()
		{
		}
	}
}

a:00 Exception Header:EXH:

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.h
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H
#define SYNTHESE_$FILE_BASE$_H

#include "01_util/Exception.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace $Namespace$
	{
		/** $Description$
			@ingroup $Module_code$Exceptions refExceptions
		*/
		class $FILE_BASE$ : public synthese::util::Exception
		{
		public:

			/** Constructor.
				@param message Message to diplay when the exception occurs.
			*/
			$FILE_BASE$(const std::string& message);

			/** Destructor.
			*/
			~$FILE_BASE$() throw ();
		};
	}
}

#endif


a:14 Admin Page header::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class header.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "AdminInterfaceElementTemplate.h"

#include "ResultHTMLTable.h"

namespace synthese
{
	namespace $Module$
	{
		//////////////////////////////////////////////////////////////////////////
		/// $FILE_BASE$ Admin compound class.
		///	@ingroup m$Module_number$Admin refAdmin
		///	@author %USERNAME%
		///	@date $YEAR$
		class $FILE_BASE$:
			public admin::AdminInterfaceElementTemplate<$FILE_BASE$>
		{
		public:
			/// @name Parameter identifiers
			//@{
			// static const std::string PARAM_SEARCH_;
			//@}

		private:
			/// @name Search parameters
			//@{
			// boost::optional<std::string>				_searchXxx;
			// html::ResultHTMLTable::RequestParameters	_requestParameters;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class. 
			/// @return true if the other page can be considered as the same than the
			///         current one.
			/// @author %USERNAME%
			/// @date $YEAR$
			/// @todo A DEFAULT IMPLEMENTATION RETURNS ALWAYS TRUE.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the current user
			/// @author %USERNAME%
			/// @date $YEAR$
			/// @todo A DEFAULT IMPLEMENTATION DOES NOT BUILD ANY TAB.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author %USERNAME%
			///	@date $YEAR$
			$FILE_BASE$();
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author %USERNAME%
			///	@date $YEAR$
			void setFromParametersMap(
				const util::ParametersMap& map
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author %USERNAME%
			///	@date $YEAR$
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author %USERNAME%
			///	@date $YEAR$
			void display(
				std::ostream& stream,
				const admin::AdminRequest& _request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Authorization check.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author %USERNAME%
			///	@date $YEAR$
			bool isAuthorized(
				const security::User& user
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages of the current class to put directly under
			/// a module admin page in the pages tree.
			///	@param module The module
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the module page in the page
			///	@author %USERNAME%
			///	@date $YEAR$
			/// @todo A DEFAULT IMPLEMENTATION RETURNS NOTHING.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages to put directly under the current page in
			/// the pages tree.
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the current page
			///	@author %USERNAME%
			///	@date $YEAR$
			/// @todo A DEFAULT IMPLEMENTATION RETURNS NOTHING.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author %USERNAME%
			///	@date $YEAR$
			/// @todo A DEFAULT IMPLEMENTATION RETURNS THE TITLE STATIC VARIABLE.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual std::string getTitle() const;
		};
}	}

#endif // SYNTHESE_$FILE_BASE$_H__


a:14 Admin Implementation::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class implementation.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "$FILE_BASE$.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "$ModuleClass$.h"
#include "$RightClass$.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace $Namespace$;

	template<> const string FactorableTemplate<AdminInterfaceElement, $FILE_BASE$>::FACTORY_KEY = "$FILE_BASE$";

	namespace admin
	{
		template<>
		const string AdminInterfaceElementTemplate<$FILE_BASE$>::ICON = "$Icon$";

		template<>
		const string AdminInterfaceElementTemplate<$FILE_BASE$>::DEFAULT_TITLE = "$Default_Title$";
	}

	namespace $Namespace$
	{
		// const string $FILE_BASE$::PARAM_SEARCH_XXX = "xx";



		$FILE_BASE$::$FILE_BASE$()
			: AdminInterfaceElementTemplate<$FILE_BASE$>()
		{ }


		
		void $FILE_BASE$::setFromParametersMap(
			const ParametersMap& map
		){
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap $FILE_BASE$::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}


		
		bool $FILE_BASE$::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<$RightClass$>(READ);
		}



		void $FILE_BASE$::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// PROPERTIES TAB
			// if (openTabContent(stream, TAB_PROPERTIES))
			{
				/// @todo Implement the display by streaming the output to the stream variable
			}
			
			////////////////////////////////////////////////////////////////////
			/// END TABS
			// closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks $FILE_BASE$::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	dynamic_cast<const $ModuleClass$*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks $FILE_BASE$::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const $FILE_BASE$* ua(
			//	dynamic_cast<const $FILE_BASE$*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<$FILE_BASE$> p(getNewOtherPage<$FILE_BASE$>());
			//	links.push_back(p);
			// }
			
			return links;
		}


		std::string $FILE_BASE$::getTitle() const
		{
			// return object_var.get() ? object_var->getName() : DEFAULT_TITLE;
			return string();
		}



		bool $FILE_BASE$::_hasSameContent(const AdminInterfaceElement& other) const
		{
			// return _value == static_cast<const $FILE_BASE$&>(other)._value;
			return true;
		}



		void $FILE_BASE$::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();

			// _tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, profile.isAuthorized<$RightClass$>(WRITE, UNKNOWN_RIGHT_LEVEL)));

			_tabBuilded = true;
		}
}	}


a:15 Action Header::

//////////////////////////////////////////////////////////////////////////
/// $FILE_BASE$ class header.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace $Namespace$
	{
		//////////////////////////////////////////////////////////////////////////
		/// $Module_number$.15 Action : $FILE_BASE$.
		/// @ingroup m$Module_number$Actions refActions
		///	@author %USERNAME%
		///	@date $YEAR$
		/// @since $Version$
		//////////////////////////////////////////////////////////////////////////
		/// Key : $FILE_BASE$
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class $FILE_BASE$:
			public util::FactorableTemplate<server::Action, $FILE_BASE$>
		{
		public:
			/// @todo Parameters list
			// static const std::string PARAMETER_xxx;

		private:
			/// @todo Attributes list

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				// void setObject(boost::shared_ptr<Object> value) { _object = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:15 Action Implementation::

//////////////////////////////////////////////////////////////////////////
/// $FILE_BASE$ class implementation.
/// @file $FILE_BASE$.$FILE_EXT$
/// @author %USERNAME%
/// @date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "$FILE_BASE$.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "$RightClass$.h"
#include "$LogClass$.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, $Module$::$FILE_BASE$>::FACTORY_KEY = "$FILE_BASE$";

	namespace $Module$
	{
		/// @todo Parameters constants definition
		// const string $FILE_BASE$::PARAMETER_xxx = Action_PARAMETER_PREFIX + "xxx";
		
		
		
		ParametersMap $FILE_BASE$::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}
		
		
		
		void $FILE_BASE$::_setFromParametersMap(const ParametersMap& map)
		{
			// _xxx = map.get<string>(PARAMETER_xxx);
			// if (_xxx.empty())
			//	throw ActionException("Bad value for xxx parameter ");	
			// 

			// try
			// {
			//	_xxx = yyyTableSync::Get(map.get<RegistryKeyType>(PARAMETER_xxx), *_env);
			// }
			// catch(ObjectNotFoundException<yyy>& e)
			// {
			//	throw ActionException<yyy>("yyy", e, &this);
			// }

		}
		
		
		
		void $FILE_BASE$::run(
			Request& request
		){

		}
		
		
		
		bool $FILE_BASE$::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<$RightClass$>($NeededRightLevel$);
		}
}	}


a:10 Table Sync Header::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class header.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_$FILE_BASE$_$FILE_EXT$__
#define SYNTHESE_$FILE_BASE$_$FILE_EXT$__

#include "$ObjectClass$.hpp"

#include "DB$NoSync_Registry$TableSyncTemplate.hpp"

namespace synthese
{
	namespace $namespace$
	{
		//////////////////////////////////////////////////////////////////////////
		///	$ObjectClass$ table synchronizer.
		///	@ingroup m$module_number$LS refLS
		///	@author %USERNAME%
		///	@date $YEAR$
		/// @since $Version$
		class $FILE_BASE$:
			public db::DB$NoSync_Registry$TableSyncTemplate<$FILE_BASE$,$ObjectClass$>
		{
		public:
			//! @name Services
			//@{		
				//////////////////////////////////////////////////////////////////////////
				///	$ObjectClass$ search.
				///	@param env Environment to populate
				///	@param parameterId optional ID of a foreign key to filter on (deactivated if undefined)
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByName The result records must be ordered by their name
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author %USERNAME%
				///	@date $YEAR$
				/// @since $Version$
				static SearchResult Search(
					util::Env& env,
					// boost::optional<util::RegistryKeyType> parameterId = boost::optional<util::RegistryKeyType>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByName = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
			//@}
		};
}	}

#endif // SYNTHESE_$FILE_BASE$_$FILE_EXT$__


a:10 Table Sync implementation::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class implementation.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "$FILE_BASE$.hpp"

#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace $namespace$;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,$FILE_BASE$>::FACTORY_KEY = "$Table_factory_key$";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<$FILE_BASE$>::TABLE(
			"$Full_table_name$"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<$FILE_BASE$>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		const DBTableSync::Indexes DBTableSyncTemplate<$FILE_BASE$>::GetIndexes()
		{
			DBTableSync::Indexes r;
			// r.push_back(
			//	DBTableSync::Index(
			//		$FILE_BASE$::COL_NAME.c_str(),
			// "")	);
			return r;
		}



		template<> void DBDirectTableSyncTemplate<$FILE_BASE$,$ObjectClass$>::Load(
			$ObjectClass$* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			}
			object->loadFromRecord(*rows, env);
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void DBDirectTableSyncTemplate<$FILE_BASE$,$ObjectClass$>::Save(
			$ObjectClass$* object,
			optional<DBTransaction&> transaction
		){
			// Query
			ReplaceQuery<$FILE_BASE$> query(*object);
			ParametersMap map;
			object->toParametersMap(map);
			query.setValues(map);
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<$FILE_BASE$,$ObjectClass$>::Unlink(
			$ObjectClass$* obj
		){
			obj->unlink();
		}



		template<> bool DBTableSyncTemplate<$FILE_BASE$>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return true;
		}



		template<> void DBTableSyncTemplate<$FILE_BASE$>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<$FILE_BASE$>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<$FILE_BASE$>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}
	
	
	
	namespace $namespace$
	{
		$FILE_BASE$::SearchResult $FILE_BASE$::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<$FILE_BASE$> query;
			// if(parameterId)
			// {
			// 	query.addWhereField(COL_PARENT_ID, *parentFolderId);
			// }
			// if(orderByName)
			// {
			// 	query.addOrderField(COL_NAME, raisingOrder);
			// }
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}


a:10 Inherited Table Sync Header::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class header.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_$Namespace$_$FILE_BASE$_$FILE_EXT$__
#define SYNTHESE_$Namespace$_$FILE_BASE$_$FILE_EXT$__

#include "DBInherited$NoSync_or_Registry$TableSyncTemplate.h"

#include "$Parent_Table_Sync$.hpp"
#include "$Class$.h"

namespace synthese
{
	namespace $Namespace$
	{
		//////////////////////////////////////////////////////////////////////////
		///	$FILE_BASE$ class.
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m$Module$ILS refILS
		///	@author %USERNAME%
		///	@since $Version$
		///	@date $YEAR$
		class $FILE_BASE$:
			public db::SQLiteInherited$NoSync_or_Registry$TableSyncTemplate<
				$Parent_Table_Sync$,
				$FILE_BASE$,
				$Class$
			>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Search of $Class$ objects.
			/// @param env Environment to populate when loading objects
			/// @param first first element to return
			/// @param number maximal number of elements to return
			/// @param orderById order the returned elements by their id
			/// @param raisingOrder order ascendantly or not
			/// @param linkLevel automatic load of objects linked by foreign key
			///	@author %USERNAME%
			/// @since $Version$
			/// @date $YEAR$
			static SearchResult Search(
				util::Env& env,
				std::size_t first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderById = false,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
}	}

#endif // SYNTHESE_$Namespace$_$FILE_BASE$_$FILE_EXT$__


a:10 Inherited Table Sync Implementation::

//////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class implementation.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "$FILE_BASE$.hpp"

#include "ReplaceQuery.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace $Namespace$;
	using namespace util;

	template<>
	const string FactorableTemplate<$Parent_Table_Sync$,$FILE_BASE$>::FACTORY_KEY = "$FILE_BASE$";

	namespace db
	{
		template<>
		void SQLiteInheritedTableSyncTemplate<$Parent_Table_Sync$,$FILE_BASE$,$Class$>::Load(
			$Class$* obj,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
			}
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<$Parent_Table_Sync$,$FILE_BASE$,$Class$>::Unlink(
			$Class$* obj
		){
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<$Parent_Table_Sync$,$FILE_BASE$,$Class$>::Save(
			$Class$* obj,
			optional<SQLiteTransaction&> transaction
		){
			// The query
			ReplaceQuery<$Parent_Table_Sync$> query(*obj);
			// query.addField(object->getName());
			query.execute(transaction);
		}
	}

	namespace $Namespace$
	{
		$FILE_BASE$::SearchResult $FILE_BASE$::Search(
			Env& env,
			size_t first,
			boost::optional<std::size_t> number,
			bool orderById,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<$Parent_Table_Sync$> query;
			
			// Ordering
			if(orderById)
			{
				query.addOrderField(TABLE_COL_ID, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}


a:15 Service Header::

//////////////////////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class header.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace $Namespace$
	{
		//////////////////////////////////////////////////////////////////////////
		///	$Module_number$.15 Function : $FILE_BASE$.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/$Wiki_page$
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m$Module_number$Functions refFunctions
		///	@author %USERNAME%
		///	@date $YEAR$
		/// @since $Version$
		class $FILE_BASE$:
			public util::FactorableTemplate<server::Function,$FILE_BASE$>
		{
		public:
			/// @todo request parameter names declaration
			// eg : static const std::string PARAMETER_xxx;
			
		protected:
			//! \name Page parameters
			//@{
				/// @todo Parsed parameters declaration
				// eg : const void*	_object;
				// eg : ParametersMap			_parameters;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/$Wiki_page$#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author %USERNAME%
			/// @date $YEAR$
			/// @since $Version$
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/$Wiki_page$#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author %USERNAME%
			/// @date $YEAR$
			/// @since $Version$
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author %USERNAME%
			/// @date $YEAR$
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author %USERNAME%
			/// @date $YEAR$
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author %USERNAME%
			/// @date $YEAR$
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_$FILE_BASE$_H__


a:15 Service Implementation::

//////////////////////////////////////////////////////////////////////////////////////////
///	$FILE_BASE$ class implementation.
///	@file $FILE_BASE$.$FILE_EXT$
///	@author %USERNAME%
///	@date $YEAR$
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "$FILE_BASE$.hpp"

#include "$RightClass$.h"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,$module$::$FILE_BASE$>::FACTORY_KEY = "$FILE_BASE$";
	
	namespace $module$
	{
		/// @todo Parameter names declarations
		//const string $FILE_BASE$::PARAMETER_PAGE = "p";
		


		ParametersMap $FILE_BASE$::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return map;
		}



		void $FILE_BASE$::_setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);
		}

		ParametersMap $FILE_BASE$::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			/// @todo Fill it
			return map;
		}
		
		
		
		bool $FILE_BASE$::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<$RightClass$>($RequiredRightLevel$);
		}



		std::string $FILE_BASE$::getOutputMimeType() const
		{
			return "text/html";
		}
}	}

a:GNU GPL Licence for header:GPLH:

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

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


a:GNU GPL Licence for implementation:GPLC:

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

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


a:Vinci GPL H:VinciGPLH:

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 

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


a:Vinci GPL Implementation:VinciGPLC:

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	
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


a:12_security:12_:
12_security
a:Module Class Header::

/** $Module_Number$ $Namespace$ module class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$
	@since $Version$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// $Module_Number$ $Namespace$ Module namespace.
	///	@author %USERNAME%
	///	@date $YEAR$
	/// @ingroup m$Module_Number$
	/// @since $Version$
	namespace $Namespace$
	{
		/**	@defgroup m$Module_Number$Exceptions $Module_Number$.01 Exceptions
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$LS $Module_Number$.10 Table synchronizers
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Pages $Module_Number$.11 Interface Pages
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Library $Module_Number$.11 Interface Library
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Rights $Module_Number$.12 Rights
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Logs $Module_Number$.13 DB Logs
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Admin $Module_Number$.14 Administration pages
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Actions $Module_Number$.15 Actions
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Functions $Module_Number$.15 Functions
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$File $Module_Number$.16 File formats
			@ingroup m$Module_Number$
	
			@defgroup m$Module_Number$Alarm $Module_Number$.17 Messages recipient
			@ingroup m$Module_Number$
			
			@defgroup m$Module_Number$ $Module_Number$ $Namespace$
			@ingroup m$Module_First_Number$
			
			@todo Write Module documentation
			
			@{
		*/


		//////////////////////////////////////////////////////////////////////////
		/// $Module_Number$ $Namespace$ Module class.
		///	@author %USERNAME%
		///	@date $YEAR$
		/// @since $Version$
		class $FILE_BASE$:
			public server::ModuleClassTemplate<$FILE_BASE$>
		{
		private:
			
		public:

		};
	}
	/** @} */
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Module Class Implementation::

/** $ModuleNumber$_$Namespace$ module class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$
	@since $Version$

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

#include "$FILE_BASE$.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace $Namespace$;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,$FILE_BASE$>::FACTORY_KEY = "$ModuleNumber$_$Namespace$";
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<$FILE_BASE$>::NAME = "$Namespace$";

		template<> void ModuleClassTemplate<$FILE_BASE$>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<$FILE_BASE$>::Init()
		{
		}

		template<> void ModuleClassTemplate<$FILE_BASE$>::End()
		{
		}
	}

	namespace $Namespace$
	{

	}
}



a:GetOpenFileName::
OPENFILENAME ofn;
ZeroMemory(&ofn, sizeof(OPENFILENAME)); 

ofn.lStructSize       = sizeof(OPENFILENAME);
ofn.hwndOwner         = $OwnerWindowHandle$;         
ofn.hInstance         = GetModuleHandle(NULL);  
ofn.lpstrFilter       = $lpstrFilter$;     
ofn.lpstrCustomFilter = $lpstrCustomFilter$;     
ofn.nMaxCustFilter    = 0;
ofn.nFilterIndex      = 0;
ofn.lpstrFile         = $lpstrFile$;     
ofn.nMaxFile          = sizeof($lpstrFile$) / sizeof($lpstrFile$[0]);
ofn.lpstrFileTitle    = NULL;          
ofn.nMaxFileTitle     = 0;     
ofn.lpstrInitialDir   = $lpstrInitialDir$;     
ofn.lpstrTitle        = $OpenFile_WindowTitle$;
ofn.nFileOffset       = 0;            
ofn.nFileExtension    = 0;
ofn.lpstrDefExt       = $Default_FileExtension$; 
ofn.lCustData         = 0;           
ofn.lpfnHook          = NULL; 
ofn.lpTemplateName    = NULL; 
ofn.Flags             = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

bRet = GetOpenFileName(&ofn);   

a:GetSaveFileName::
OPENFILENAME ofn;
ZeroMemory(&ofn, sizeof(OPENFILENAME)); 

ofn.lStructSize       = sizeof(OPENFILENAME);
ofn.hwndOwner         = $OwnerWindowHandle$;         
ofn.hInstance         = GetModuleHandle(NULL);  
ofn.lpstrFilter       = $lpstrFilter$;     
ofn.lpstrCustomFilter = $lpstrCustomFilter$;     
ofn.nMaxCustFilter    = 0;
ofn.nFilterIndex      = 0;
ofn.lpstrFile         = $lpstrFile$;     
ofn.nMaxFile          = sizeof($lpstrFile$) / sizeof($lpstrFile$[0]);
ofn.lpstrFileTitle    = NULL;          
ofn.nMaxFileTitle     = 0;     
ofn.lpstrInitialDir   = $lpstrInitialDir$;     
ofn.lpstrTitle        = $OpenFile_WindowTitle$;
ofn.nFileOffset       = 0;            
ofn.nFileExtension    = 0;
ofn.lpstrDefExt       = $Default_FileExtension$; 
ofn.lCustData         = 0;           
ofn.lpfnHook          = NULL; 
ofn.lpTemplateName    = NULL; 
ofn.Flags             = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

bRet = GetSaveFileName(&ofn);

a:Win32 console application::
#include <stdio.h>
int main(int argc, char *argv[]) 
{
    $end$
	return 0;
}

a:Win32 standard application::
#include <windows.h>
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("$end$");
    HWND         hwnd;
    MSG          msg;
    WNDCLASSEX   wndclassex = {0};
    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = WndProc;
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.hInstance     = hInstance;
    wndclassex.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclassex.lpszMenuName  = NULL;
    wndclassex.lpszClassName = szAppName;
    wndclassex.hIconSm       = wndclassex.hIcon;
	
    if (!RegisterClassEx (&wndclassex))
    {
        MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
        return 0;
    }
    hwnd = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, 
		                  szAppName, 
        		          TEXT ("WindowTitle"),
                		  WS_OVERLAPPEDWINDOW,
		                  CW_USEDEFAULT, 
        		          CW_USEDEFAULT, 
                		  CW_USEDEFAULT, 
		                  CW_USEDEFAULT, 
        		          NULL, 
                		  NULL, 
		                  hInstance,
        		          NULL); 
						  
    ShowWindow (hwnd, iCmdShow);
    UpdateWindow (hwnd);
	
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        return (0);
		
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        TextOut (hdc, 0, 0, "A Window!", 27);
        EndPaint (hwnd, &ps);
        return (0);
		
    case WM_DESTROY:
        PostQuitMessage (0);
        return (0);
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

a:Win32 modal dialog::
#include <windows.h>
#include "resource.h"
BOOL CALLBACK DialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   return (int) DialogBox (hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
}
BOOL CALLBACK DialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_INITDIALOG:
        $end$
        return (TRUE);
		
    case WM_DESTROY:
    case WM_CLOSE:
        EndDialog(hDlg,0);
        return (TRUE);
		
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            SendMessage(hDlg, WM_CLOSE, 0, 0);
            return (TRUE);
			
        case IDOK:
            SendMessage(hDlg, WM_CLOSE, 0, 0);
            return (TRUE);
        }
        return (FALSE);
    }
    return (FALSE);
}

a:Win32 modeless dialog with menu/accelerators::
#include <windows.h>
#include "resource.h"
HWND hDialog;
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccel;
	hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	ShowWindow(hDialog, nCmdShow);
	UpdateWindow(hDialog);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hDialog, hAccel, &msg))
		{
			if(!IsDialogMessage(hDialog, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}			
		}
	}
	return msg.wParam;
}
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (TRUE);
		
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            DestroyWindow (hwndDlg);
            return (TRUE);
			
        default:
            break;
        }
        return (FALSE);
		
    case WM_CLOSE:
    case WM_DESTROY:
        EndDialog (hwndDlg, 0);
        return (TRUE);
    }
    return (FALSE);
}

a:Dialog procedure::
BOOL CALLBACK $end$ (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
        {
			
        }
		return (TRUE);
	case WM_CLOSE:
        {
            EndDialog(hDlg,0);
        }
		return (TRUE);
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
            {
                SendMessage(hDlg, WM_CLOSE, 0, 0);
            }
            return (TRUE);
		case IDOK:
            {
                
            }
			return (TRUE);
		}
		return (FALSE);
	}
	return (FALSE);
}

a:Window procedure::
LRESULT CALLBACK $end$ (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    switch (message)
    {
    case WM_CREATE:
        return (0);
		
    case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps);
        
        EndPaint (hwnd, &ps);
        return (0);
		
    case WM_DESTROY:
        PostQuitMessage (0);
        return (0);
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

a:Ultimate DLL header::
#ifndef _DEFINED_$GUID_STRING$ 
#define _DEFINED_$GUID_STRING$ 
#if _MSC_VER > 1000 
    #pragma once  
#endif // _MSC_VER > 1000
#ifdef __cplusplus 
extern "C" 
{ 
#endif // __cplusplus  
#ifdef _COMPILING_$clipboard$ 
    #define LIBSPEC __declspec(dllexport)  
#else 
    #define LIBSPEC __declspec(dllimport) 
#endif // _COMPILING_uniqueheadername 
//
// Enter your Function-declarations as follows:
//
// LIBSPEC linkagetype resulttype name(parameters); 
// 
$end$
#undef LIBSPEC 
#ifdef __cplusplus 
} 
#endif // __cplusplus 
#endif // _DEFINED_uniqueheadername

readme:
Autotext item used for refactoring: Change Signature, Create Implementation, and Move Implementation to Source File.
Delete this item to restore the default upon next use.

a:Refactor Create Implementation::



$SymbolType$ $SymbolContext$( $ParameterList$ ) $MethodQualifier$
{
	$end$$MethodBody$
}

readme:
Autotext item used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Document Method::


/** $SymbolName$.
	@param $MethodArgName$
	@return $SymbolType$
	@author %USERNAME%
	@date $YEAR$
	
*/

readme:
Autotext item used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Encapsulate Field::
	$end$$SymbolType$ $GeneratedPropertyName$() const { return $SymbolName$; }
	void $GeneratedPropertyName$($SymbolType$ val) { $SymbolName$ = val; }

readme:
Autotext item used for refactoring.
Delete this item to restore the default upon next use.

a:Refactor Extract Method::

$end$$SymbolType$ $SymbolContext$( $ParameterList$ )
{
	$MethodBody$
}


a:shared pointer:sha:
boost::shared_ptr<>
a:sha2:s:
boost::shared_ptr<$selected$>

readme:
VA Snippet used by Surround With #ifdef.
Delete this item to restore the default upon next use.

a:#ifdef (VA X):#if:
#ifdef $condition=_DEBUG$$end$
$selected$
#endif // $condition$


readme:
VA Snippet used by Surround With #region.
Delete this item to restore the default upon next use.

a:#region (VA X):#r:
#pragma region $end$$regionName$
$selected$
#pragma endregion $regionName$


readme:
Delete this item to restore the default when the IDE starts.

a:{...}::
{
	$end$$selected$
}


readme:
Delete this item to restore the default when the IDE starts.

a:(...)::
($selected$)

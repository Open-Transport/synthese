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
a:File header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

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


a:File header detailed::
/********************************************************************
	created:	$DATE$
	created:	$DAY$:$MONTH$:$YEAR$   $HOUR$:$MINUTE$
	filename: 	$FILE$
	file path:	$FILE_PATH$
	file base:	$FILE_BASE$
	file ext:	$FILE_EXT$
	author:		%USERNAME%
	
	purpose:	$end$
*********************************************************************/

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

a:Interface Page Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace $namespace$
	{
		/** $FILE_BASE$ Interface Page Class.
			@ingroup m$module_number$Pages refPages
			@author %USERNAME%
			@date $YEAR$

			@code $Factory_key$ @endcode

			Parameters :
				- 0 :
				- 1 :

			Object : $ObjectClass$
		*/
		class $FILE_BASE$
			: public util::FactorableTemplate<interfaces::InterfacePage, $FILE_BASE$>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream
				// Add fields here
				, interfaces::VariablesMap& variables
				, const server::Request* request = NULL
			) const;
			
			$FILE_BASE$();
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Interface Page Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "$FILE_BASE$.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, $Module$::$FILE_BASE$>::FACTORY_KEY("$Factory_key$");
	}

	namespace $Module$
	{

		void $FILE_BASE$::display(
			std::ostream& stream
			// Additional parameters
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			const void* object;

			/// @todo Implement the building of the parameter vector with parameters of the function

			InterfacePage::display(
				stream
				, pv
				, variables
				, static_cast<const void*>(object)
				, request
			);
		}
	}
}


a:Interface Library Element Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace $Module$
	{
		/** $FILE_BASE$ Library Interface Element Class.
			@author %USERNAME%
			@date $YEAR$
			@ingroup m$Module_Number$Library refLibrary

			@code $Factory_key$ @endcode

			Parameters :
				- 0 :
				- 1 :

			Object : $ObjectClass$

		*/
		class $FILE_BASE$
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, $FILE_BASE$>
		{
			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _parameter1;
			//...

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param variables Runtime variables
				@param object Object to read at the display
				@param request Source request
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
				) const;
				
			~$FILE_BASE$();
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Interface Library Element Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "$FILE_BASE$.h"

#include "11_interfaces/ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, $Module$::$FILE_BASE$>::FACTORY_KEY("$Factory_key$");
	}

	namespace $Module$
	{
		void $FILE_BASE$::storeParameters(ValueElementList& vel)
		{
			_parameter1 = vel.front();
			/// @todo control and Fill the parameters init
		}

		string $FILE_BASE$::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			/// @todo output on the stream. Read the parameters like this :
			stream << _parameter1->getValue(parameters,variables,object,request);

			return string();
		}

		$FILE_BASE$::~$FILE_BASE$()
		{
		}
	}
}


a:Right Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$Module$_$FILE_BASE$_H__
#define SYNTHESE_$Module$_$FILE_BASE$_H__

#include "12_security/RightTemplate.h"

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
			std::string	displayParameter()	const;
			

			/** Test of inclusion of an other perimeter by the current one.
				@param perimeter to compare with
				@return true if the current perimeter includes the compared one, false else
			*/
			bool perimeterIncludes(const std::string& perimeter) const;
		};
	}
}

#endif // SYNTHESE_$Module$_$FILE_BASE$_H__


a:Right Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "$FILE_BASE$.h"

#include "12_security/Constants.h"

using namespace std;

namespace synthese
{
	using namespace $Module$;
	using namespace security;

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

	namespace $Module$
	{
		string $FILE_BASE$::displayParameter() const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";
			
			return _parameter;
		}

		bool $FILE_BASE$::perimeterIncludes(const string& perimeter) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
	}
}


a:Exception Implementation:EXC:

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.cpp

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

#include "$FILE_BASE$.h"

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

a:Exception Header:EXH:

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.h
	@author %USERNAME%
	@date $YEAR$

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


a:Admin Page header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "32_admin/AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace $Module$
	{
		/** $FILE_BASE$ Class.
			@ingroup m$Module_number$Admin refAdmin
			@author %USERNAME%
			@date $YEAR$
		*/
		class $FILE_BASE$ : public admin::AdminInterfaceElementTemplate<$FILE_BASE$>
		{
		public:
			$FILE_BASE$();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author %USERNAME%
				@date $YEAR$
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author %USERNAME%
				@date $YEAR$
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;
			
			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author %USERNAME%
				@date $YEAR$
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
			
			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author %USERNAME%
				@date $YEAR$
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
			) const;
			
			/** Sub pages getter.
				@param currentPage Currently displayed page
				@param request User request
				@return PageLinks each subpage of the current page
				@author %USERNAME%
				@date $YEAR$
				REMOVE IF YOU WANT TO USE THE STANDARD INHERITANCE METHOD DESIGNED IN ADMININTERFACEELEMENT
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
				
			) const;
			
			/** Title generator.
				@return The title of the page
				@author %USERNAME%
				@date $YEAR$
				REMOVE IF YOU WANT ALWAYS USE THE DEFAULT_TITLE
			*/
			virtual std::string getTitle() const;
			
			/** Parameter name getter.
				@return The name of the parameter of the page
				@author %USERNAME%
				@date $YEAR$
				REMOVE IF YOU DONT USE A PARAMETER
			*/
			virtual std::string getParameterName() const;
			
			/** Parameter value getter.
				@return The value of the parameter of the page
				@author %USERNAME%
				@date $YEAR$
				REMOVE IF YOU DONT USE A PARAMETER
			*/
			virtual std::string getParameterValue() const;
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Admin Page Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "$FILE_BASE$.h"
#include "$Superior_Page_Class$.h"
#include "$Module_Class$.h"

#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace $Namespace$;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, $FILE_BASE$>::FACTORY_KEY("$FILE_BASE$");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<$FILE_BASE$>::ICON("$Icon$");
		template<> const string AdminInterfaceElementTemplate<$FILE_BASE$>::DEFAULT_TITLE("$Default_Title$");
	}

	namespace $Namespace$
	{
		$FILE_BASE$::$FILE_BASE$()
			: AdminInterfaceElementTemplate<$FILE_BASE$>()
		{ }
		
		void $FILE_BASE$::setFromParametersMap(const ParametersMap& map)
		{
			/// @todo Initialize internal attributes from the map
			// Exception example:
			// throw AdminParametersException("Parameter not found");
			// Example : _pageLink.name=object.getName();
			// Example : _pageLink.parameterName=QueryString::PARAMETER_OBJECT_ID;
			// Example : _pageLink.parameterValue=Conversion::ToString(id);
		}
		
		void $FILE_BASE$::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
		}

		bool $FILE_BASE$::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return false;
		}
		
		AdminInterfaceElement::PageLinks $FILE_BASE$::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or leave empty
			// Example
			// if(parentLink.factoryKey == $Superior_Page_Class$::FACTORY_KEY && parentLink.parameterValue == $Module_Class$::FACTORY_KEY)
			//	links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks $FILE_BASE$::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or remove the method to get the default behaviour
			return links;
		}


		std::string $FILE_BASE$::getTitle() const
		{
			return $object_var$.get() ? $object_var$->getName() : DEFAULT_TITLE;
		}

		std::string $FILE_BASE$::getParameterName() const
		{
			return $object_var$.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string $FILE_BASE$::getParameterValue() const
		{
			return $object_var$.get() ? Conversion::ToString($object_var$->getKey()) : string();
		}
	}
}


a:Action Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "30_server/Action.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace $Module$
	{
		/** $FILE_BASE$ action class.
			@ingroup m$module_number$Actions refActions
		*/
		class $FILE_BASE$
			: public util::FactorableTemplate<server::Action, $FILE_BASE$>
		{
		public:
			/// @todo Parameters list
			// static const std::string PARAMETER_xxx;

		private:
			/// @todo Attributes list

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();
			
			$FILE_BASE$();
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Action Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "$FILE_BASE$.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, $Module$::$FILE_BASE$>::FACTORY_KEY("$FILE_BASE$");
	}

	namespace $Module$
	{
		/// @todo Parameters constants definition
		// const string $FILE_BASE$::PARAMETER_xxx = Action_PARAMETER_PREFIX + "xxx";
		
		
		
		$FILE_BASE$::$FILE_BASE$()
			: util::FactorableTemplate<Action, $FILE_BASE$>()
		{
		}
		
		
		
		ParametersMap $FILE_BASE$::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}
		
		
		
		void $FILE_BASE$::_setFromParametersMap(const ParametersMap& map)
		{
			// _xxx = map.getString(PARAMETER_xxx, true, FACTORY_KEY);
			// if (_xxx.empty())
			//	throw ActionException("Bad value for xxx parameter ");	
			// 
		}
		
		
		
		void $FILE_BASE$::run()
		{
		}
	}
}


a:Table Sync Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.h
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace $module$
	{
		class $class$;

		/** $class$ table synchronizer.
			@ingroup m$module_number$LS refLS
		*/
		class $FILE_BASE$ : public db::SQLiteTableSyncTemplate<$FILE_BASE$,$class$>
		{
		public:
			/// @todo Column list
			// eg : static const std::string COL_NAME;
			
			$FILE_BASE$();


			/** $class$ search.
				(other search parameters)
				@param first First $class$ object to answer
				@param number Number of $class$ objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<$class$> Founded $class$ objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<$class$> > search(
				// other search parameters ,
				int first = 0, int number = 0);


		protected:

			/** Action to do on $class$ creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (
				db::SQLite* sqlite
				, db::SQLiteSync* sync
				, const db::SQLiteResultSPtr& rows
				, bool isItFirstSync = false
				);

			/** Action to do on $class$ creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated(
				db::SQLite* sqlite
				, db::SQLiteSync* sync
				, const db::SQLiteResultSPtr& rows
			);
			
			/** Action to do on $class$ deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved(
				db::SQLite* sqlite
				, db::SQLiteSync* sync
				, const db::SQLiteResultSPtr& rows
			);
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Table sync implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.cpp
	@author %USERNAME%
	@date $YEAR$

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

#include <sstream>

#include "$FILE_BASE$.h"
#include "$class$.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace $module$;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,$FILE_BASE$>::FACTORY_KEY("$Factory_key$");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<$FILE_BASE$>::TABLE_NAME("$table_name$");
		template<> const int SQLiteTableSyncTemplate<$FILE_BASE$>::TABLE_ID($table_id$);
		template<> const bool SQLiteTableSyncTemplate<$FILE_BASE$>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<$FILE_BASE$,$class$>::load(
			$class$* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			/// @todo Set all other attributes from the row

		}



		template<> void SQLiteDirectTableSyncTemplate<$FILE_BASE$,$class$>::save(
			$class$* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<$FILE_BASE$,$class$>::_link(
			$class$* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
			/// @todo Fill it
		}


		template<> void SQLiteDirectTableSyncTemplate<$FILE_BASE$,$class$>::_unlink(
			$class$* obj
		){
			/// @todo Fill it
		}
	}
	
	
	
	namespace $module$
	{
		// const std::string $FILE_BASE$::COL_xx("xx");
		/// @todo Other fields



		$FILE_BASE$::$FILE_BASE$()
			: SQLiteTableSyncTemplate<$FILE_BASE$>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			/// @todo Other fields (types ar INTEGER, TEXT)
		}



		void $FILE_BASE$::rowsAdded(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
			, bool isItFirstSync
		){
			while(rows->next())
			{
			}
		}


		
		void $FILE_BASE$::rowsUpdated(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
			while(rows->next())
			{
			}
		}



		void $FILE_BASE$::rowsRemoved(
			SQLiteQueueThreadExec* sqlite
			, SQLiteSync* sync
			, const SQLiteResultSPtr& rows
		){
			while(rows->next())
			{
			}
		}



		vector<shared_ptr<$class$> > $FILE_BASE$::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			/// @todo Fill Where criteria
			// if (!name.empty())
			// 	query << " AND " << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
				;
			//if (orderByName)
			//	query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<$class$> > objects;
				while (rows->next ())
				{
					shared_ptr<$class$> object(new $class$);
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}


a:Inherited Table Sync Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

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

#ifndef SYNTHESE_$Module$_$FILE_BASE$_$FILE_EXT$__
#define SYNTHESE_$Module$_$FILE_BASE$_$FILE_EXT$__

#include "02_db/SQLiteInheritedTableSyncTemplate.h"

#include "$Parent_table_sync$.h"
#include "$Class$.h"

namespace synthese
{
	namespace $Module$
	{
		/** $FILE_BASE$ class.
			@ingroup m$Module_number$ILS refILS
		*/
		class $FILE_BASE$
			: public db::SQLiteInheritedTableSyncTemplate<$Parent_table_sync$, $FILE_BASE$, $Class$>
		{
		public:
			/** Constructor.
			*/
			$FILE_BASE$();
		};
	}
}

#endif // SYNTHESE_$Module$_$FILE_BASE$_$FILE_EXT$__


a:Inherited Table Sync Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

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

#include "$FILE_BASE$.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace $Module$;

	template<>
	const string util::FactorableTemplate<$Parent_table_sync$, $FILE_BASE$>::FACTORY_KEY("$FILE_BASE$");

	namespace db
	{

		template<>
		void SQLiteInheritedTableSyncTemplate<$Parent_table_sync$,$FILE_BASE$,$Class$>::_Link($Class$* obj, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			/// @todo Implement Link
		}

		template<>
		void SQLiteInheritedTableSyncTemplate<$Parent_table_sync$,$FILE_BASE$,$Class$>::_Unlink($Class$* obj)
		{
			/// @todo Implement Unlink
		}
	}

	namespace $Module$
	{

		$FILE_BASE$::$FILE_BASE$()
			: SQLiteInheritedNoSyncTableSyncTemplate<$Parent_table_sync$, $FILE_BASE$, $Class$>()
		{

		}
	}
}
a:Function Header::

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.h
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "01_util/FactorableTemplate.h"

#include "30_server/Function.h"

namespace synthese
{
	namespace $module$
	{
		/** $FILE_BASE$ Function class.
			@author %USERNAME%
			@date $YEAR$
			@ingroup m$module_number$Functions refFunctions
		*/
		class $FILE_BASE$ : public util::FactorableTemplate<server::Function,$FILE_BASE$>
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
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			/// @todo Getters/Setters for parsed parameters
			
		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;
		};
	}
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:Function Implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.cpp
	@author %USERNAME%
	@date $YEAR$

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

#include "01_util/Conversion.h"

#include "30_server/RequestException.h"
#include "30_server/RequestMissingParameterException.h"

#include "$FILE_BASE$.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<Function,$module$::$FILE_BASE$>::FACTORY_KEY("$FILE_BASE$");
	
	namespace $module$
	{
		/// @todo Parameter names declarations
		//const string $FILE_BASE$::PARAMETER_PAGE("rub");
		
		ParametersMap $FILE_BASE$::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void $FILE_BASE$::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			/// @todo Parameters parsing
			// eg
			//it = map.find(PARAMETER_PAGE);
			// if (it == map.end())
			//	throw RequestMissingParameterException(PARAMETER_PAGE, FACTORY_KEY);
			//try
			//{
			//	AdminInterfaceElement* page = (it == map.end())
			//		? Factory<AdminInterfaceElement>::create<HomeAdmin>()
			//		: Factory<AdminInterfaceElement>::create(it->second);
			//	page->setFromParametersMap(map);
			//	_page = page;
			//}
			//catch (FactoryException<AdminInterfaceElement> e)
			//{
			//	throw RequestException("Admin page " + it->second + " not found");
			//}
		}

		void $FILE_BASE$::_run( std::ostream& stream ) const
		{
			/// @todo Fill it
		}
	}
}


a:CPP implementation::

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

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

#include "$FILE_BASE$.h"

namespace synthese
{
	namespace $Module$
	{

	}
}


a:GNU GPL Licence for header:GPLH:

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$

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


a:GNU GPL Licence for implementation:GPLC:

/** $FILE_BASE$ class implementation.
	@file $FILE_BASE$.$FILE_EXT$

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

/** $FILE_BASE$ class header.
	@file $FILE_BASE$.$FILE_EXT$
	@author %USERNAME%
	@date $YEAR$

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

#ifndef SYNTHESE_$FILE_BASE$_H__
#define SYNTHESE_$FILE_BASE$_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	/**	@defgroup m$Module_Number$Actions $Module_Number$ Actions
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Pages $Module_Number$ Pages
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Functions $Module_Number$ Functions
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Exceptions $Module_Number$ Exceptions
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Alarm $Module_Number$ Messages recipient
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$LS $Module_Number$ Table synchronizers
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Admin $Module_Number$ Administration pages
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Rights $Module_Number$ Rights
		@ingroup m$Module_Number$

		@defgroup m$Module_Number$Logs $Module_Number$ DB Logs
		@ingroup m$Module_Number$
		
		@defgroup m$Module_Number$ $Module_Title$
		@ingroup m$Module_First_Number$
		
		(Module documentation)
		
		@{
	*/

	/** $Module_Number$ $Module_Title$ Module namespace.
		@author %USERNAME%
		@date $YEAR$
	*/
	namespace $Namespace$
	{

		/** $Module_Number$ $Module_Title$ Module class.
			@author %USERNAME%
			@date $YEAR$
		*/
		class $FILE_BASE$ : public util::ModuleClass
		{
		private:
			// static Object::Registry _registry;	//!< Objects registry
			
			
		public:
			/** Initialization of the $Module_Number$ $Module_Title$ module after the automatic database loads.
				@author %USERNAME%
				@date $YEAR$
			*/			
			void initialize();
			
			// static Object::Registry& getObjects();
		};
	}
	/** @} */
}

#endif // SYNTHESE_$FILE_BASE$_H__


a:shared pointer:sha:
boost::shared_ptr<>
a:sha2:s:
boost::shared_ptr<$selected$>

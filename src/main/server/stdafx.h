// stdafx.h : Fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once


#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // certains constructeurs CString seront explicites

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  // Exclure les en-t�tes Windows rarement utilis�s
#endif

#include <afx.h>
#include <afxwin.h>         // Composants MFC principaux et standard
#include <afxext.h>         // Extensions MFC
#include <afxdtctl.h>  // Prise en charge des MFC pour les contr�les communs Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>   // Prise en charge des MFC pour les contr�les communs Windows
#endif // _AFX_NO_AFXCMN_SUPPORT


// TODO : faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme


// stdafx.h : Fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once


#include <iostream>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // certains constructeurs CString seront explicites

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  // Exclure les en-têtes Windows rarement utilisés
#endif

#include <afx.h>
#include <afxwin.h>         // Composants MFC principaux et standard
#include <afxext.h>         // Extensions MFC
#include <afxdtctl.h>  // Prise en charge des MFC pour les contrôles communs Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>   // Prise en charge des MFC pour les contrôles communs Windows
#endif // _AFX_NO_AFXCMN_SUPPORT


// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme


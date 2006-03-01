/*! \file cFormatFichier.h
\brief En-tête classe Format de fichier
\author Hugues Romain 
\date 2000-2003
*/

#ifndef SYNTHESE_CFORMATFICHIERS_H
#define SYNTHESE_CFORMATFICHIERS_H

// Format du fichier des formats
#define FORMATSEXTENSION "formats.per"

typedef int tNumeroElementFormat;
class cFormatFichier;

#include "cTexte.h"
#include <iostream>
#include "cFichier.h"



/** Format de fichier
 @ingroup m01
*/
class cFormatFichier
{
        int* vColonnePosition; //!< Tableau des positions de colonnes de données
        int* vColonneLongueur; //!< Tableau des longueurs de colonnes de données
        int* vLignePosition;  //!< Tableau des positions des identificateurs de types de lignes
        int* vLigneLongueur;  //!< Tableau des longueurs des identificateurs de types de lignes
        cTexte* vLigneMarqueur;  //!< Tableau des identificateurs de types de lignes
        int vNombreLignes;  //!< Nombre d'identificateurs de types de lignes

    public:
        //! \name Calculateurs
        //@{
        cTexte Extrait( const cTexte& Texte, int n, int NumeroColonne = 0 ) const;
        cTexte ExtraitComplet( const cTexte& Texte, int n ) const;
        int GetColonneLargeur( int n ) const;
        int GetColonnePosition( int n, int NumeroColonne = 0 ) const;
        int GetNombre( const cTexte& Texte, int n, int NumeroColonne = 0 ) const;
        int LireFichier( ifstream& Fichier, char* Tampon ) const;
        int LireFichier( ifstream& Fichier, cTexte& Tampon ) const;
        int Longueur( const cTexte& Texte, int n ) const;
        //@}


        //! \name Constructeur et fonctions de construction
        //@{
        cFormatFichier( const cTexte& NomFichier, const char* Label, int NombreFormatsLignes, int NombreFormatsColonnes );
        ~cFormatFichier();
        //@}

};

#endif

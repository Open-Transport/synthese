/*! \file cFormatFichier.cpp
\brief Impl�mentation classe format de fichier
\author Hugues Romain
\date 2000-2003
*/

#include "cFormatFichier.h"

/*!  \brief Fonction de construction puisant les informations dans un fichier .formats.per
 \param NomFichier Nom du fichier (sans l'extension formats.per qui est ajout�e automatiquement : ne pas oublier le . si le fichier est nomm�)
 \param Label indiquant la section du fichier qui d�crit le format voulu
 \param NombreFormatsLignes Nombre de types de lignes dans le format
 \param NombreFormatsColonnes Nombre de types de colonnes dans le format
*/
cFormatFichier::cFormatFichier( const cTexte& NomFichier, const char* Label, int NombreFormatsLignes, int NombreFormatsColonnes )
{
    // Variables locales
    int i;
    cTexte Tampon( TAILLETAMPON, true );

    // FichierLOG << "# Lecture du format " << Label << endl;

    // Allocation
    vLignePosition = new int[ NombreFormatsLignes ];
    vLigneLongueur = new int[ NombreFormatsLignes ];
    vLigneMarqueur = new cTexte[ NombreFormatsLignes ];
    vColonneLongueur = new int[ NombreFormatsColonnes ];
    vColonnePosition = new int[ NombreFormatsColonnes ];
    vNombreLignes = NombreFormatsLignes;

    // Ouverture du fichier
    ifstream Fichier;
    cTexte NomFichierComplet;
    NomFichierComplet << NomFichier << FORMATSEXTENSION;
    Fichier.open( NomFichierComplet.Texte() );

    if ( !Fichier.is_open() )
    {
        cout << "*** ERREUR Impossible d'ouvrir le fichier " << NomFichierComplet.Texte() << "\n";
        //  FichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << NomFichierComplet.Texte() << "\n";
        //  FichierLOG.flush();
        //   return false;
    }

    if ( Tampon.RechercheSection( Fichier, cTexte( Label ), TYPESection ) )
    {
        Tampon.ProchaineSection( Fichier, TYPESousSection );
        i = 0;
        while ( Tampon.LireLigne( Fichier ) == TYPEAutre )
        {
            vLignePosition[ i ] = Tampon.GetNombre( 2 );
            vLigneLongueur[ i ] = Tampon.GetNombre( 2, 2 );
            vLigneMarqueur[ i ] = Tampon.Extrait( 4, vLigneLongueur[ i ] );
            i++;
        }
        //   FichierLOG << TXT(i) << " lignes de format\n";
        i = 0;

        while ( Tampon.LireLigne( Fichier ) == TYPEAutre )
        {
            vColonnePosition[ i ] = Tampon.GetNombre( 2 );
            vColonneLongueur[ i ] = Tampon.GetNombre( 2, 2 );
            i++;
        }
        //    FichierLOG << TXT(i) << " colonnes de format\n\n";

    }

    // Fermeture du fichier
    Fichier.close();
}

/*! \brief Destructeur
*/
cFormatFichier::~cFormatFichier()
{
    delete [] vLignePosition;
    delete [] vLigneLongueur;
    delete [] vColonneLongueur;
    delete [] vColonnePosition;
    delete [] vLigneMarqueur;
}

/*! \brief lecture d'un fichier en appliquant le format, avec �criture dans un char*
 \deprecated Eviter l'usage de cette m�thode
 \warning L'allocation de la cha�ne de caract�res n'est pas g�r�e par cette fonctions
 \param Fichier Ficher � lire
 \retval Tampon Chaine de caract�res qui la ligne de texte lue dans le fichier
 \return Type de ligne de donn�e lue si format reconnu, valeur n�gative d�crivant le contexte sinon
*/ 
/*int cFormatFichier::LireFichier(ifstream& Fichier, char* Tampon) const
{
 TypeSection TS;
 tNumeroElementFormat i;
 
 TS = Tampon. LireLigne(Fichier, Tampon);
 
 switch (TS)
 {
 case TYPEVide:
 case TYPESection:
 case TYPESousSection:
 case TYPELien:
  return(-TS);
 case TYPEAutre:
  for (i=0; i!=vNombreLignes; i++)
  {
   if (strlen(Tampon)>=(vLignePosition[i] + vLigneLongueur[i]))
    if (CompareChaine(Tampon + vLignePosition[i], vLigneMarqueur[i].Texte(), vLigneLongueur[i]))
     return(i);
  }
  return(-TS);
 }
 
 return(-TYPEVide);
}*/


/*! \brief lecture d'un fichier en appliquant le format
 \warning Eviter l'usage de cette m�thode
 \param Fichier Ficher � lire
 \retval Tampon Chaine de caract�res qui la ligne de texte lue dans le fichier
 \return Type de ligne de donn�e lue si format reconnu, valeur n�gative d�crivant le contexte sinon
*/
int cFormatFichier::LireFichier( ifstream& Fichier, cTexte& Tampon ) const
{
    TypeSection TS;
    tNumeroElementFormat i;

    TS = Tampon.LireLigne( Fichier );

    switch ( TS )
    {
        case TYPEVide:
        case TYPESection:
        case TYPESousSection:
        case TYPELien:
            return ( -TS );

        case TYPEAutre:
            for ( i = 0; i != vNombreLignes; i++ )
                if ( Tampon.Taille() >= ( vLignePosition[ i ] + vLigneLongueur[ i ] ) )
                    if ( Tampon.Compare( vLigneMarqueur[ i ], vLigneLongueur[ i ], vLignePosition[ i ] ) )
                        return ( i );
            return ( -TS );
    }

    return ( -TYPEVide );
}

int cFormatFichier::GetNombre( const cTexte& Texte, int n, int NumeroColonne ) const
{
    return ( Texte.GetNombre( vColonneLongueur[ n ], vColonnePosition[ n ] + NumeroColonne * vColonneLongueur[ n ] ) );
}



/*! \brief Extraction du texte d'une ligne de fichier en fonction du format
 \param __Texte La ligne de texte compl�te extraite du fichier
 \param __n Le type de ligne d�crit par l'index du type au sein du format
 \param __Num�roColonne (s'il y a lieu) Le num�ro de la colonne � extraire
 \return Le texte demand�
 \author Hugues Romain
 \date 2000-2005
*/
cTexte cFormatFichier::Extrait( const cTexte& __Texte, int __n, int __NumeroColonne ) const
{
    cTexte __Retour;
    if ( vColonneLongueur[ __n ] )
        __Retour = __Texte.Extrait( vColonnePosition[ __n ] + __NumeroColonne * vColonneLongueur[ __n ], vColonneLongueur[ __n ] );
    else
        __Retour = __Texte.Extrait( vColonnePosition[ __n ] );
    return __Retour;
}

cTexte cFormatFichier::ExtraitComplet( const cTexte& Texte, int n ) const
{
    return ( Texte.Extrait( vColonnePosition[ n ] ) );
}

int cFormatFichier::GetColonneLargeur( int n ) const
{
    return ( vColonneLongueur[ n ] );
}

int cFormatFichier::GetColonnePosition( int n, int NumeroColonne ) const
{
    return ( vColonnePosition[ n ] + NumeroColonne * vColonneLongueur[ n ] );
}

/*! \brief Renvoie la longueur des donn�es
 \return le nombre d'�l�ments de donn�es si le format est en colonnes, la taille du texte hors identificateur sinon
 \param n Identificateur du type de la ligne
 \param Texte Ligne de texte � analyser correspondant � la ligne du fichier
*/
int cFormatFichier::Longueur( const cTexte& Texte, int n ) const
{
    // Si donn�es en colonnes
    if ( vColonneLongueur[ n ] )
        // Nombre d'�l�ments de donn�es
        return ( Texte.Taille() - vColonnePosition[ n ] ) / vColonneLongueur[ n ];
    else
        // Taille du texte de donn�es
        return Texte.Taille() - vColonnePosition[ n ];
}

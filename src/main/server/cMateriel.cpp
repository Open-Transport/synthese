/*------------------------------------*
|                                    |
|  APDOS - SYNTHESE v0.6             |
|  � Hugues Romain 2000/2002         |
|  CMateriel.cpp                     |
|  Implementation Classe Materiel    |
|                                    |
*------------------------------------*/

#include "cMateriel.h"

/*! \brief Constructeur
 \author Hugues Romain
 \date 2001
*/
cMateriel::cMateriel( int newCode )
{
    vCode = newCode;
}



/*! \brief Modificateur de la vitesse moyenne maximale du mat�riel roulant
 \author Hugues Romain
 \date 2001
*/
bool cMateriel::setVitesseMoyenneMax( tVitesseKMH newVitesseMoyenneMax )
{
    vVitesseMoyenneMax = newVitesseMoyenneMax;
    return ( true );
}

/*! \brief Destructeur
*/
cMateriel::~cMateriel()
{}

void cMateriel::setLibelleSimple( const cTexte& newNom )
{
    vLibelleSimple = newNom;
}

void cMateriel::setArticle( const cTexte& newArticle )
{
    vArticle = newArticle;
}


void cMateriel::setDoc( const cTexte &newVal )
{
    vDoc.Vide();
    vDoc << newVal;
}


void cMateriel::setIndicateur( const cTexte &newVal )
{
    vIndicateur.Vide();
    vIndicateur << newVal;
}

tVitesseKMH cMateriel::VitesseMoyenneMax() const
{
    return ( vVitesseMoyenneMax );
}

const cTexte& cMateriel::getIndicateur() const
{
    return ( vIndicateur );
}

int cMateriel::Code() const
{
    return ( vCode );
}

const cTexte& cMateriel::getLibelleSimple() const
{
    return ( vLibelleSimple );
}




/*! \brief Indique si le mat�riel repr�sente les jonctions � pied
 \todo Remplacer le define par un param�tre
*/
bool cMateriel::EstAPied() const
{
    return vCode == MATERIELPied;
}



/*! \brief Accesseur Article pr�c�dant le nom du mat�riel
*/
const cTexte& cMateriel::getArticle() const
{
    return vArticle;
}


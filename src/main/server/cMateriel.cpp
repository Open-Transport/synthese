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
bool cMateriel::setVitesseMoyenneMax( int newVitesseMoyenneMax )
{
    vVitesseMoyenneMax = newVitesseMoyenneMax;
    return ( true );
}

/*! \brief Destructeur
*/
cMateriel::~cMateriel()
{}

void cMateriel::setLibelleSimple( const std::string& newNom )
{
    vLibelleSimple = newNom;
}

void cMateriel::setArticle( const std::string& newArticle )
{
    vArticle = newArticle;
}


void cMateriel::setDoc( const std::string &newVal )
{
    vDoc = newVal;
}


void cMateriel::setIndicateur( const std::string &newVal )
{
    vIndicateur = newVal;
}

int cMateriel::VitesseMoyenneMax() const
{
    return ( vVitesseMoyenneMax );
}

const std::string& cMateriel::getIndicateur() const
{
    return ( vIndicateur );
}

int cMateriel::Code() const
{
    return ( vCode );
}

const std::string& cMateriel::getLibelleSimple() const
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
const std::string& cMateriel::getArticle() const
{
    return vArticle;
}



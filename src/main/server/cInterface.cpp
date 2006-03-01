/*! \file cInterface.cpp
\brief Impl�mentation classe interface
*/



#include "cInterface.h"


const size_t cInterface::ALL_DAY_PERIOD = 0;

/*! \brief Constructeur
 \author Hugues Romain
 \date 2001-2005
 
Les objets interface sont initialis�s par les valeurs par d�faut suivantes :
 - Index inconnu
 - Cr�ation des objets d'interface vide
*/
cInterface::cInterface( const size_t& id )
        : _id( id )
{
    _Element = new cInterface_Objet_AEvaluer_PageEcran[ INTERFACESNOMBREELEMENTS ];
}



/*! \brief Destructeur
 \author Hugues Romain
 \date 2001-2005
 
Les objets interface sont d�truits de la mani�re suivante :
 - Destruction des objets d'interface
*/
cInterface::~cInterface()
{
    delete[] _Element;
}



/*! \brief Accesseur �l�ments standard d'interface
 \param __Index Index de l'�l�ment � fournir
 \return L'�l�ment demand�
 \author Hugues Romain
 \date 2005
*/
const cInterface_Objet_AEvaluer_PageEcran& cInterface::operator[] ( int __Index ) const
{
    return _Element[ __Index ];
}


cInterface_Objet_AEvaluer_PageEcran& cInterface::Element( int __Index )
{
    return _Element[ __Index ];
}








/**
 * Modificateur des pr�fixes de messages d'alerte
 * \param __Niveau Niveau des messages sur lesquels seront appliqu�s le pr�fixe
 * \param __Message Pr�fixe � ajouter aux messages du niveau sp�cifi�
 * \return true si le message a bien �t� enregistr� (test sur la taille de texte enregistr� uniquement)
 */
bool cInterface::SetPrefixeAlerte( int __Niveau, const cTexte& __Message )
{
    if ( _PrefixesAlerte.SetElement( __Message, __Niveau ) != __Niveau )
        return false;
    else
        return _PrefixesAlerte[ __Niveau ].Taille() == __Message.Taille();
}



/**
 * Accesseur pr�fixe de message d'alerte sans contr�le du niveau demand�
 * \param __Niveau Niveau d'alerte du message
 * \return Le pr�fixe � apposer au message d'alerte
 * \warning Le niveau d'alerte n'est pas control�. Une valleur erronn�e peut g�n�rer une erreur
 */
const cTexte& cInterface::getPrefixeAlerte( int __Niveau ) const
{
    return _PrefixesAlerte[ __Niveau ];
}


/*! \brief Accesseur pointeur vers période de la journée avec contrôle
 \param __Index Numéro de la période
 \author Hugues Romain
 \date 2005
*/
const cPeriodeJournee* cInterface::GetPeriode( size_t __Index ) const
{
    return ( __Index < _Periode.size() ) ? _Periode.at( __Index ) : NULL;
}


void cInterface::AddPeriode( cPeriodeJournee* __Element )
{
    _Periode.push_back( __Element );
}


/*! \file cInterface..h
 \brief Fonctions  des classes d'interface
*/



/*! \brief Accesseur Index
 \return L'index de l'interface
 \author Hugues Romain
 \date 2005
*/
const size_t& cInterface::Index() const
{
    return _id;
}


/*! \brief Contrôle de la validité de l'objet
 \return true si l'objet est utilisable, false sinon
 \author Hugues Romain
 \date 2005
 
L'objet est valide si :
 - Au moins une période de la journée est décrite
*/
bool cInterface::OK() const
{
    return _Periode.size() > 0;
}



/*! \brief Modificateur libellé jour de semaine
 \param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
 \param __Libelle Libellé du jour de la semaine
 \return true si la modification a été effectuée avec succès
 \author Hugues Romain
 \date 2005
*/
bool cInterface::SetLibelleJour( int __Index, const cTexte& __Libelle )
{
    if ( __Index < 0 || __Index >= synthese::time::DAYS_PER_WEEK )
        return false;

    _LibelleJourSemaine[ __Index ] = __Libelle;
    return true;
}



/*! \brief Modificateur libellé mois
 \param __Index Index du mois
 \param __Libelle Libellé du mois
 \return true si la modification a été effectuée avec succès
 \author Hugues Romain
 \date 2005
*/
bool cInterface::SetLibelleMois( int __Index, const cTexte& __Libelle )
{
    if ( __Index < 0 || __Index > synthese::time::MONTHS_PER_YEAR )
        return false;

    _LibelleMois[ __Index ] = __Libelle;
    return true;
}


/*! \brief Accesseur libellé de jour de la semaine
 \param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
 \return Libellé du jour demandé
 \author Hugues Romain
 \date 2005
*/ /*
 const cTexte& cInterface::LibelleJourSemaine(int __Index) const
{
 return _LibelleJourSemaine[__Index];
}*/




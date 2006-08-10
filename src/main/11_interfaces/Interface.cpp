#include "Interface.h"


#include "cInterface_Objet_AEvaluer_PageEcran.h"


namespace synthese
{

namespace interfaces
{


const size_t Interface::ALL_DAY_PERIOD = 0;

/*! \brief Constructeur
 \author Hugues Romain
 \date 2001-2005
 
Les objets interface sont initialis�s par les valeurs par d�faut suivantes :
 - Index inconnu
 - Cr�ation des objets d'interface vide
*/
Interface::Interface( const uid& id )
    : synthese::util::Registrable<uid,synthese::interfaces::Interface> (id)
{
    _Element = new cInterface_Objet_AEvaluer_PageEcran[ INTERFACESNOMBREELEMENTS ];
}



/*! \brief Destructeur
 \author Hugues Romain
 \date 2001-2005
 
Les objets interface sont d�truits de la mani�re suivante :
 - Destruction des objets d'interface
*/
Interface::~Interface()
{
    delete[] _Element;
}



/*! \brief Accesseur �l�ments standard d'interface
 \param __Index Index de l'�l�ment � fournir
 \return L'�l�ment demand�
 \author Hugues Romain
 \date 2005
*/
const cInterface_Objet_AEvaluer_PageEcran& Interface::operator[] ( int __Index ) const
{
    return _Element[ __Index ];
}


cInterface_Objet_AEvaluer_PageEcran& Interface::Element( int __Index )
{
    return _Element[ __Index ];
}








/**
 * Modificateur des pr�fixes de messages d'alerte
 * \param __Niveau Niveau des messages sur lesquels seront appliqu�s le pr�fixe
 * \param __Message Pr�fixe � ajouter aux messages du niveau sp�cifi�
 * \return true si le message a bien �t� enregistr� (test sur la taille de texte enregistr� uniquement)
 */
bool Interface::SetPrefixeAlerte( int __Niveau, const std::string& __Message )
{
    if ( _PrefixesAlerte.SetElement( __Message, __Niveau ) != __Niveau )
        return false;
    else
        return _PrefixesAlerte[ __Niveau ].size () == __Message.size ();
}



/**
 * Accesseur pr�fixe de message d'alerte sans contr�le du niveau demand�
 * \param __Niveau Niveau d'alerte du message
 * \return Le pr�fixe � apposer au message d'alerte
 * \warning Le niveau d'alerte n'est pas control�. Une valleur erronn�e peut g�n�rer une erreur
 */
const std::string& Interface::getPrefixeAlerte( int __Niveau ) const
{
    return _PrefixesAlerte[ __Niveau ];
}


/*! \brief Accesseur pointeur vers période de la journée avec contrôle
 \param __Index Numéro de la période
 \author Hugues Romain
 \date 2005
*/
const synthese::time::HourPeriod* Interface::GetPeriode( size_t __Index ) const
{
    return ( __Index < _Periode.size() ) ? _Periode.at( __Index ) : NULL;
}


void Interface::AddPeriode( synthese::time::HourPeriod* __Element )
{
    _Periode.push_back( __Element );
}



/*! \brief Contrôle de la validité de l'objet
 \return true si l'objet est utilisable, false sinon
 \author Hugues Romain
 \date 2005
 
L'objet est valide si :
 - Au moins une période de la journée est décrite
*/
bool Interface::OK() const
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
bool Interface::SetLibelleJour( int __Index, const std::string& __Libelle )
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
bool Interface::SetLibelleMois( int __Index, const std::string& __Libelle )
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
 const std::string& Interface::LibelleJourSemaine(int __Index) const
{
 return _LibelleJourSemaine[__Index];
}*/




}
}




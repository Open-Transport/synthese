/*!	\file cInterface.cpp
	\brief Impl�mentation classe interface
*/



#include "cInterface.h"


const size_t cInterface::ALL_DAY_PERIOD = 0;

/*!	\brief Constructeur
	\author Hugues Romain
	\date 2001-2005

Les objets interface sont initialis�s par les valeurs par d�faut suivantes :
 - Index inconnu
 - Cr�ation des objets d'interface vide
*/
cInterface::cInterface()
{
	_Index = INCONNU;
	_Element = new cInterface_Objet_AEvaluer_PageEcran[INTERFACESNOMBREELEMENTS];
}



/*!	\brief Destructeur
	\author Hugues Romain
	\date 2001-2005

Les objets interface sont d�truits de la mani�re suivante :
 - Destruction des objets d'interface
*/
cInterface::~cInterface()
{
	delete[] _Element;
}



/*!	\brief Accesseur �l�ments standard d'interface
	\param __Index Index de l'�l�ment � fournir
	\return L'�l�ment demand�
	\author Hugues Romain
	\date 2005
*/
const cInterface_Objet_AEvaluer_PageEcran& cInterface::operator[](tIndex __Index) const
{
	return _Element[__Index];
}


cInterface_Objet_AEvaluer_PageEcran& cInterface::Element(tIndex __Index)
{
	return _Element[__Index];
}








/**
 * Modificateur des pr�fixes de messages d'alerte
 * \param __Niveau Niveau des messages sur lesquels seront appliqu�s le pr�fixe
 * \param __Message Pr�fixe � ajouter aux messages du niveau sp�cifi�
 * \return true si le message a bien �t� enregistr� (test sur la taille de texte enregistr� uniquement)
 */
bool cInterface::SetPrefixeAlerte(tIndex __Niveau, const cTexte& __Message)
{
	if (_PrefixesAlerte.SetElement(__Message, __Niveau) != __Niveau)
		return false;
	else
		return _PrefixesAlerte[__Niveau].Taille() == __Message.Taille();
}



/**
 * Accesseur pr�fixe de message d'alerte sans contr�le du niveau demand�
 * \param __Niveau Niveau d'alerte du message
 * \return Le pr�fixe � apposer au message d'alerte
 * \warning Le niveau d'alerte n'est pas control�. Une valleur erronn�e peut g�n�rer une erreur
 */
const cTexte& cInterface::getPrefixeAlerte(tIndex __Niveau) const
{
	return _PrefixesAlerte[__Niveau];
}


/*!	\brief Accesseur pointeur vers période de la journée avec contrôle
	\param __Index Numéro de la période
	\author Hugues Romain
	\date 2005
*/
const cPeriodeJournee* cInterface::GetPeriode(size_t __Index) const
{
	return (__Index < _Periode.size()) ? _Periode.at(__Index) : NULL;
}


void cInterface::AddPeriode(cPeriodeJournee* __Element)
{
	_Periode.push_back(__Element);
}


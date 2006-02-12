/** En-t�te classe interpr�teur de textes.
	@file Interpretor.h
*/

#ifndef SYNTHESE_INTERPRETOR_H
#define SYNTHESE_INTERPRETOR_H

#include <utility>
#include <string>
#include <vector>
#include "cAssociator.h"

using namespace std;


/** @defgroup m07 07 Interpr�teur de textes
	@{

	@section m07integr Int�gration du module Interpr�teur de textes

	Le module interpr�teur de texte impl�mente une liste permettant la recherche via l'associateur bas� sur un r�seau de neurones.

	Il est pr�vu pour �tre utilis� dans toute liste susceptible d'�tre interrog�e par une requ�te utilisateur inexacte :
		- Liste des communes dans chaque environnement : cInterpreteur<cCommune*>
		- Liste des arr�ts/adresses/lieux dans chaque commune : cInterpreteur<cAccesPADe*>

	Les m�thodes de recherche correspondantes doivent �tre mises � jour pour utiliser l'interpr�teur. Les m�thodes suivantes sont concern�es: 
		- cEnvironnement::TextToCommune
		- cCommune::textToPADe

	Les m�thode d'ajout d'�l�ment sont impact�es par l'ajout d'une commande _interpreteur.Ajout() en fin de code :
		- cEnvironnement::Enregistre(cCommune*)
		- cCommune::addDesignation

	Les m�thodes de suppression seront uitilis�es dans le cadre de la mise en service des modifications temps r�el de la base.
*/




/** Classe interpr�teur de textes (template g�n�rique).

	L'interpr�teur g�n�rique fonctionne comme un map o� les objets sont doublement index�s :
		- un inde textuel permet principalement une m�thode d'interrogation d'apr�s un texte entr� inexact
		- un index num�rique simple
	
	Un m�me �l�ment peut �tre index� sous plusieurs indexs textes, mais sous un seul index num�rique.

	La recherche d'un �l�ment peut se faire de deux mani�res :
		- par la fourniture directe de l'index
		- par la recherche pr�alable de l'index d'apr�s un texte � rechercher

	CR : tu g�res tes attributs comme tu veux (avec peut etre une liste normale d'�l�ments en plus pour s'en servir comme tableau avec op�rateur [])
*/
template <class T>
class Interpretor
{
public:

	/** Score d'un r�sultat (� d�placer dans associator je pense) */
	typedef int Score;

	/** Index */
	typedef int Index;

	/** R�sultat */
	typedef pair<Index, Score> Result;

	/** Cl� */
	typedef string Key;


protected:
	//! @name Attributs
	//{
		cAssociator		_associator;	//!< R�seau de neurones support de l'interpr�tation (CR : met le bon typage du dump : il fait index_texte=>index_numerique)
		vector<T>	_vector;		//!< Liste d'objets index�e num�riquement
	//}

public:
	//! @name Modificateurs
	//@{
		Index add(const Key& key, const T& element, Index id=INCONNU);
		void erase(const Key& key);
		bool erase(Index id);
	//@}


	//! @name Accesseurs
	//@{
		T& operator[](Index id);
		Index size() const;
		vector<Result> search(const Key&, Index) const;
	//@}

	//!	@name Constructeur et destructeur.
	//@{
		Interpretor();
		~Interpretor();
	//@}

};

/** @} */

#endif
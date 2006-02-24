/** En-tête classe interpréteur de textes.
	@file Interpretor.h
*/

#ifndef SYNTHESE_INTERPRETOR_H
#define SYNTHESE_INTERPRETOR_H

#include <utility>
#include <string>
#include <map>
#include <set>
#include "cAssociator.h"

namespace interpretor
{

	/** Score d'un résultat (à déplacer dans associator je pense) */
	typedef int Score;

	/** Résultat */
	typedef pair<size_t, Score> Result;

	/** Clé */
	typedef std::string Key;

	/** ID inconnu */
	const size_t UNKNOWN_INDEX = -1;


/** @defgroup m07 07 Interpréteur de textes
	@{

	@section m07integr Intégration du module Interpréteur de textes

	Le module interpréteur de texte implémente une liste permettant la recherche via l'associateur basé sur un réseau de neurones.

	Il est prévu pour être utilisé dans toute liste susceptible d'être interrogée par une requête utilisateur inexacte :
		- Liste des communes dans chaque environnement : cInterpreteur<cCommune*>
		- Liste des arrêts/adresses/lieux dans chaque commune : cInterpreteur<cAccesPADe*>

	Les méthodes de recherche correspondantes doivent être mises à jour pour utiliser l'interpréteur. Les méthodes suivantes sont concernées: 
		- cEnvironnement::TextToCommune
		- cCommune::textToPADe

	Les méthode d'ajout d'élément sont impactées par l'ajout d'une commande _interpreteur.Ajout() en fin de code :
		- cEnvironnement::Enregistre(cCommune*)
		- cCommune::addDesignation

	Les méthodes de suppression seront utilisées dans le cadre de la mise en service des modifications temps réel de la base.
*/






/** Classe interpréteur de textes (template générique).

	L'interpréteur générique fonctionne comme un map où les objets sont doublement indexés :
		- un inde textuel permet principalement une méthode d'interrogation d'après un texte entré inexact
		- un index numérique simple
	
	Un même élément peut être indexé sous plusieurs indexs textes, mais sous un seul index numérique.

	La recherche d'un élément peut se faire de deux manières :
		- par la fourniture directe de l'index
		- par la recherche préalable de l'index d'après un texte à rechercher

	CR : tu gères tes attributs comme tu veux (avec peut etre une liste normale d'éléments en plus pour s'en servir comme tableau avec opérateur [])
*/
template <class T>
class Interpretor
{
public:
	typedef std::map<size_t, T> MapType;

protected:
	//! @name Attributs
	//{
		cAssociator		_associator;	//!< Réseau de neurones support de l'interprétation (CR : mettre le bon typage : il fait index_texte=>index_numerique)
		MapType	_map;		//!< Liste d'objets indexée numériquement
	//}

public:

	//!	@name Accesseurs
	//@{
		const MapType& getMap() const { return _map; }

		/** Accesseur d'après l'index numérique
			@param __Index Index de l'objet à accéder
			@return Objet demandé
		*/
		T& at(size_t id) const
		{
			return _map[id];
		}


	//@}

	//!	@name Constructeur et destructeur.
	//@{
		Interpretor() {}
		~Interpretor() {}
	//@}


	//!	@name Modificateurs
	//@{
		/** Adding of an element in the list, without specified index.
			@param key Text key of the element
			@param element Element to be added
			@return Attributed index
			@todo Put a critical section
		*/
		size_t add(const Key& key, const T& element)
		{
			size_t id;
			if (_map.size())
			{
				MapType::const_iterator iter = _map.end();
				--iter;
				id = iter->first + 1;
			}
			else
				id = 0;
		
			// Adding of the element
			add(key, element, id);

			// returns the id
			return id;
		}
		

		/** Ajout d'élément dans le réseau.

			@param key Clé texte de l'élément
			@param element Elément à ajouter
			@param id Index de l'élément (vide/INCONNU = auto : création d'un index si nouvel élément, rien si élement déjà existant)

			Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une clé texte dans le réseau vers celui-ci.
			Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'élément dans la liste, puis à l'ajout de la clé texte dans le réseau.
			Si l'index n'est pas fourni, et si aucun élément de la liste n'est identique à l'élément fourni, alors l'élément est ajouté en queue de liste
			Si l'index n'est pas fourni et si un élément de la liste est identique, alors une clé texte est ajoutée au réseau vers cet élement
		*/
		void add(const Key& key, const T& element, size_t id)
		{

			// storage in map contenor
			_map[id] = element;

			// Etablissement du lien dans le réseau de neurone
			// TODO CR

		}


		/** Suppression d'un lien de correspondance clé => élément.

			La suppression par clé texte ne supprime que le lien texte => élément et non la totalité des liens vers l'élément pointé par la clé texte.
			
			@warning Un élément peut ne plus posséder de clé texte vers celui-ci si la dernière a été supprimée.

			@param __Index Index de l'élément à supprimer
			@return true si Suppression OK (peut être changé)

			@todo CR : Implémenter, ça ne concerne que le réseau de neurone
		*/
		void erase(const Key& key)
		{
			
		}


		/** Suppression d'élément dans le réseau d'après son index.

			La suppression par élément supprime la totalité des liens vers l'élément.

			@param id Index de l'élément à supprimer
			@return true si Suppression OK (peut être changé)

			@todo CR : faire la partie réseau neurone
		*/
		bool erase(size_t id)
		{
			// CR : ici l'effacement de toutes les clés dans le réseau
			
			_map.erase(id);
		}
	//@}

	//!	@name Calculateur
	//@{
		/** Interprétation de texte.

			L'interprétation s'effectue par action de l'associateur sur le réseau de neurones

			(détailler le fonctionnement à l'intérieur du source avec tags doxygen pour prise en compte)

			@param key Texte à interpréter
			@param size Nombre de solutions à produire
			@return couple index élément trouvé / score, trié dans l'ordre drécroissant de score

			@todo CR : A implémenter
		*/
		set<Result> search(const Key& key, size_t size) const
		{
			set<Result> result;

			return result;
		}
	//@}




};

/** @} */

}


#endif

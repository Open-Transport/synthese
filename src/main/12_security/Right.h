
#ifndef SYNTHESE_SECURITY_RIGHT_TEMPLATE_H
#define SYNTHESE_SECURITY_RIGHT_TEMPLATE_H

#include <string>

namespace synthese
{
	namespace security
	{
		/** Habilitation (abstraite).
			@ingroup m05

			Une habilitation est un droit d'effectuer une ou plusieurs opération(s) sur un périmètre donné.

			Une habilitation est définie par :
				-# un périmètre d'application dont l'usage est précisé par les sous classes. Exemple d'utilisations possibles :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de périmètre
				-# un niveau de droit sur les objets appartenant à l'utilisateur (droits privés)
				-# un niveau de droit sur les objets n'appartenant pas à l'utilisateur (droits publics)

			Les niveaux de droits sont les suivants :
				- FORBIDDEN : interdiction (utile pour annuler une habilitation héritée)
				- USE : droit d'utiliser une fonction
				- READ : accès en lecture
				- WRITE : accès en écriture, effacement non permis
				- DELETE : accès en écriture, effacement permis
				- NB : Chaque niveau de droit inclut les niveaux précédents.

			Chaque module contient des définitions d'habilitations (sous-classes enregistrées dans Factory<Right>) qui implémentent le contrôle des droits et la génération d'une liste de paramètres possibles.
		*/
		class Right
		{
		public:
			/** Available vality areas map.
				Key : heading of areas (eg: lines, stops...)
				Value : set of allowed strings representing an area
			*/
			typedef map<std::string, set<std::string>> AvailableValidityAreasMap;

			/** Niveaux d'habilitation. */
			typedef enum {
				FORBIDDEN		//!< Interdiction
				, USE				//!< Utilisation de fonction autorisée
				, READ			//!< Lecture directe de données autorisée
				, WRITE			//!< Ecriture directe de donnée autorisée
				, DELETE		//!< Suppression de donnée autorisée
			} Level;

		private:
			const std::string _parameter;
			const Level _privateRightLevel;
			const Level _publicRightLevel;

		public:
			// Can be private with friend factory class. To see later...
			//static std::string getModuleName() = 0;
			//static std::string getOperationName() = 0;
			//static AvailableValidityAreasMap getAvailableValidityAreas() const = 0;
			//static bool validateParameter(std::string parameter) const = 0;

			Right(std::string parameter, Level level)
				: _parameter(parameter), _level(level)
			{}
			
			Level getLevel() const { return _level }
		};
	}
}
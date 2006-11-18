
#ifndef SYNTHESE_SECURITY_RIGHT_TEMPLATE_H
#define SYNTHESE_SECURITY_RIGHT_TEMPLATE_H

#include <string>

namespace synthese
{
	namespace security
	{
		/** Habilitation (abstraite).
			@ingroup m05

			Une habilitation est un droit d'effectuer une ou plusieurs op�ration(s) sur un p�rim�tre donn�.

			Une habilitation est d�finie par :
				-# un p�rim�tre d'application dont l'usage est pr�cis� par les sous classes. Exemple d'utilisations possibles :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de p�rim�tre
				-# un niveau de droit sur les objets appartenant � l'utilisateur (droits priv�s)
				-# un niveau de droit sur les objets n'appartenant pas � l'utilisateur (droits publics)

			Les niveaux de droits sont les suivants :
				- FORBIDDEN : interdiction (utile pour annuler une habilitation h�rit�e)
				- USE : droit d'utiliser une fonction
				- READ : acc�s en lecture
				- WRITE : acc�s en �criture, effacement non permis
				- DELETE : acc�s en �criture, effacement permis
				- NB : Chaque niveau de droit inclut les niveaux pr�c�dents.

			Chaque module contient des d�finitions d'habilitations (sous-classes enregistr�es dans Factory<Right>) qui impl�mentent le contr�le des droits et la g�n�ration d'une liste de param�tres possibles.
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
				, USE				//!< Utilisation de fonction autoris�e
				, READ			//!< Lecture directe de donn�es autoris�e
				, WRITE			//!< Ecriture directe de donn�e autoris�e
				, DELETE		//!< Suppression de donn�e autoris�e
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
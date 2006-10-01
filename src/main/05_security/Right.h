
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
				-# un module d'application :
					- L'un des namespace de SYNTHESE
					- @c * = Tous les modules
				-# une op�ration
					- L'une des fonctions publiques SYNTHESE du module s�lectionn�
					- @c * = Toutes les fonctions SYNTHESE du module s�lectionn�
				-# un p�rim�tre d'application qui pourra �tre interpr�t� par la fonction SYNTHESE. Exemples :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de p�rim�tre
					- @c belong = Crit�re d'appartenance : droit applicable uniquement sur les objets appartenant � l'utilisateur (Ex : possibilit� d'annuler uniquement une r�servation au nom de l'utilisateur)
				-# le droit obtenu. Chaque niveau de droit inclut les niveaux pr�c�dents.
					@copydoc synthese::security::Right::Level

			@todo Make it registrable to bring list of available right templates by module in interface.
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
				USE			//!< Utilisation de fonction autoris�e
				, READ		//!< Lecture directe de donn�es autoris�e
				, WRITE		//!< Ecriture directe de donn�e autoris�e
				, DELETE	//!< Suppression de donn�e autoris�e
			} Level;

		private:
			const std::string _parameter;
			const Level _level;

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
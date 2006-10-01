
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
				-# un module d'application :
					- L'un des namespace de SYNTHESE
					- @c * = Tous les modules
				-# une opération
					- L'une des fonctions publiques SYNTHESE du module sélectionné
					- @c * = Toutes les fonctions SYNTHESE du module sélectionné
				-# un périmètre d'application qui pourra être interprété par la fonction SYNTHESE. Exemples :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de périmètre
					- @c belong = Critère d'appartenance : droit applicable uniquement sur les objets appartenant à l'utilisateur (Ex : possibilité d'annuler uniquement une réservation au nom de l'utilisateur)
				-# le droit obtenu. Chaque niveau de droit inclut les niveaux précédents.
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
				USE			//!< Utilisation de fonction autorisée
				, READ		//!< Lecture directe de données autorisée
				, WRITE		//!< Ecriture directe de donnée autorisée
				, DELETE	//!< Suppression de donnée autorisée
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
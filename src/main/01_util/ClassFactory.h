#ifndef SYNTHESE_UTIL_CLASS_FACTORY_H
#define SYNTHESE_UTIL_CLASS_FACTORY_H

#include <map>
#include <string>

namespace synthese
{

    
    namespace util
    {

		/** Class factory.
		@ingroup m01

		@todo Translate following explanation :
			- in english
			- replace A by ClassFactory
			- replace AT by ClassFactoryTemplate
			- integrate the pair key instead of string 

		http://www.cppfrance.com/codes/ANSI-ISO-CPLUSPLUS-081-CLASSE-FABRIQUE-USINE_9716.aspx
		
		Voici une construction de "classe fabrique" (ou usine), c'est � dire une classe capable de fournir de nouveaux objet dont la la classe est d�termin� dynamiquement.
		Ici, la classe est coisie en fonction du premier paramettre pass� aux programme.
		La class 'A' est abstraite et servira � d�finir l'interface commun aux diff�rente classes de la fabrique. Ces classe sont d�clar� � la fin du code, pour vous montrer qu'elle sont facilement extenssible (nouvelles classe ...)
		Je vai encore apporter plus d'explications par la suite (mises � jours prochaines)... 

		Je d�clare une classe template, 'AT', dont h�riteront nos classe CA (A1, A2, ...).
		La classe 'A', dont h�rite 'AT' (oui oui !) contient donc les fonction d'interface commune aux 'CA', ici une fonction virtuel qu'impl�mentera chaque class 'CA'.
		La fonction statique permet d'obtenir l'instanciation d'une classe 'CA' � partir de son "nom" ("C1", "C2", ...). C'est le but de la manoeuvre :-)
		En priv�, 'A' poss�de une varribale statique lui permettant de stoquer une table de correspondance entre une chaine de caract�res et un pointeur de fonction retournant l'adresse d'un nouvelle objet de classe 'A'. En pratique, pour chaque classe 'CA', il y aura une fonction qui instantiera un objet de classe 'CA' correspondant et en retournera un pointeur en tant 'A'.
		Bon, et pour faciliter l'enregistrement de chaque classe 'CA', une classe template imbriqu�, 'A::Register&lt;CA&gt;', amie bien entendu ;-)
		Bon, revenons maintenant � 'AT&lt;CA&gt;', dont h�rite les 'CA'.
		En public, elle poss�de une fonction statique (une fonction par 'CA' !), celle-la m�me dont l'adresse est place dans la table de correspondance ('A::a_creators') !
		En priv�, une fonction statique, 'name', qui retournera le nom de la classe. Pour chaque class 'CA', nous sp�cialiserons cette fonction (patience ;).
		Et !, une variable statique (une variable par 'CA' !), un objet de classe 'A::Register&lt;CA&gt;'.
		Bien entendu, on est tous amis ;-)
		Maintenant, que fait cette fameuse classe 'Register&lt;CA&gt;' ? H� bien, � �a cr�ation (avant l'entr� dans la fonction main donc, puisse que statique de 'AT&lt;CA&gt;'), elle enregistre la classe 'CA', en ins�rant un couple cr�� � partir du nom retourn� par 'AT&lt;CA&gt;::name()' (sp�cialis�e !) et l'adresse de la fonction 'AT::&lt;CA&gt;::get_new' (statique !). Ainsi, l'insertion est automatique pour toute classe 'CA', c'est � dire h�ritant de 'AT&lt;CA&gt;' et pour laquelle 'AT&lt;CA&gt;::name' est sp�cialis� (plus encore une petite chose...;).
		Ne pas oublier de d�finir les membre statique, en locurrence 'AT&lt;CA&gt;::m_register'.
		La fonction 'AT&lt;CA&gt;::get_new()' est trivialement simple.
		D�finition, maintenant de notre table de correspondance, 'A::a_creators'.
		La fonction la plus important, 'A::get_new( class_name )' est celle qui vat chercher dans la table de correspondance, la fonction ad�quat pour la cr�ation de l'objet dont la classe sera d�termin� par 'class_name'. Pour cela, nous utilisont la fonction 'map&lt;&gt;::find( key )'. D�s lors que l'it�rateur retourn� est "valide", nous utilison la seconde partie de l'�l�ment, � savoir le pointeur de fonction, pour appeler cette fonction et retourner, directement, l'adresse ainsi obtenu, du nouvelle objet. Ici, le chois est fait, en cas d'�chec, de retourner un pointeur nul. Mais vous pourriez �galement �mettre une exeption.
		Dans la fonction 'main( argc, arv )', nous appelons, tout simplement 'A::get_new' en passant un 'std::string' construit � partir du 1er argument ('argv[1]'). Pour "d�monstration", nous envoyons via le flux de sortie, le "nom" de la classe, ici obtenu par l'appel de la fonction virtuelle 'A::cout_name'.
		Il ne nous reste plus qu'� concr�tiser quelques classe 'CA'.
		C'est tr�s simple, chaque classe 'CA' doit h�riter de 'AT&lt;CA' (par exemple 'C1' h�rite de 'AT&lt;C1&gt;'). Elle doit impl�menter (de pr�f�rence ;) les fonctions virtuelles (ici, 'cout_name'). Il faut sp�cialiser la fonction 'AT&lt;CA&gt;::name', de sorte � lui donner un nom (ici, nous respectons le nom c++ de la classe ("A1", "A2", ...), mais vous pourriez mettre "bannane" et "abricot"). C'est ce nom qui est utilis� dans la table de correspondance. Et finalement, petit d�tail de grande importance, "instancier" la classe 'AT&lt;CA', sans quoi vous n'aurrez de d�finition des membres en question dans aucun module (.obj ou .o) � l'�dition des liens !
		Bon, h� bien voila, n'h�sitez pas � me poser des questions, de pr�f�rence sur le ng fr.comp.lang.c++ :-) 

		The Superclass must provide :
			- Public typedef FactoryFunctionPointer : the pointer type of the factory function. Example : typedef SuperClass *(*FactoryFunctionPointerType)();
			- Public method called get_new : the factory fonction


		*/
		class ClassFactory
		{ 
			public: 
		  
				static ClassFactory * get_new( const std::string& class_name ); 
		  
				virtual void cout_name() = 0; 
		  
			private: 
		  
				typedef ClassFactory *(*FactoryFunctionPointer)();
				typedef std::map<std::string, FactoryFunctionPointer > ClassFactoryFunctionsMap;
				static ClassFactoryFunctionsMap _factoryFunctions;
		  
				template<class CA> 
				class Register 
				{
					public: 
					Register(); 
				};
		  
			template<class CA> 
			friend class Register; 
		};


		template<class CA> 
		class ClassFactoryTemplate : public ClassFactory
		{ 
			public: 
		  
				static ClassFactory * get_new(); 
		  
			private: 
		  
				static char const * name(); 
				static ClassFactory::Register<CA> m_register; 
		  
			friend class ClassFactory::Register<CA>; 
		}; 
		  
		template<class CA> 
		ClassFactory::Register<CA>::Register() 
		{ 
			_factoryFunctions.insert( 
				ClassFactoryFunctionsMap::value_type( 
					std::string(ClassFactoryTemplate<CA>::name())
					, & ClassFactoryTemplate<CA>::get_new 
				) 
			);
		} 

		template<class CA> 
		ClassFactory::Register<CA> ClassFactoryTemplate<CA>::m_register; 
		  
		template<class CA>
		ClassFactory * ClassFactoryTemplate<CA>::get_new() 
		{
			return new CA;
		}
		  
		ClassFactory::ClassFactoryFunctionsMap ClassFactory::_factoryFunctions; 
		  
		ClassFactory* ClassFactory::get_new( std::string const & class_name ) 
		{ 
			ClassFactoryFunctionsMap::iterator i = _factoryFunctions.find( class_name ); 
			if( i != _factoryFunctions.end() ) 
				return (*i).second(); 
			else 
				return static_cast<ClassFactory*>(0); //! @todo Throw an exception
		} 
	}
}

#endif

#ifndef SYNTHESE_util_Factory_h__
#define SYNTHESE_util_Factory_h__

#include <map>
#include <string>
#include <iostream>

namespace synthese
{
	namespace util
	{
		/** Generic class factory.

			The features of the generic class factory are :
				- Auto instantiation of the factory at the fist class registration
				- Iterator for accessing to the registered subclasses list
				- Creation of objects from the iterator or directly from a string key
		*/
		template <class RootObject>
		class Factory
		{
		public:
			class Iterator;

		private:

			/** Interface for auto-generated creators. */
			class CreatorInterface
			{
			private:
				virtual RootObject* create() = 0;
				friend class Factory;
				friend class Factory::Iterator;
			};

			/** Auto-generated creator for each registered subclass. */
			template <class T>
			class Creator : public CreatorInterface
			{
			private:
				RootObject* create()
				{
					return static_cast<RootObject*>(new T);
				}
			};

			/** Registered subclasses map type. */
			typedef std::map<std::string, CreatorInterface*> Map;

			/** The registered subclasses map.

			The pointer permits to avoid a preliminary instantiation of the factory before the adding of the subclasses. 
			*/
			static Map* _registeredCreator;

		public:

			static size_t size()
			{
				return (_registeredCreator == NULL) ? 0 : _registeredCreator->size();
			}

			/** Subclass registration.
				@param key Text key of the subclass
				@return the key if ok, empty string if the subclass is already registered
			*/
			template <class T>
				static std::string integrate(const typename Map::key_type& key)
			{
				std::cerr << key << "\n";

				// The first integration allocates the static map
				if (_registeredCreator == NULL)
					_registeredCreator = new Map;

				// If the key is already used then return false (it would be better to use exceptions)
				if(_registeredCreator->find(key) != _registeredCreator->end())
					return "";

				// Saving of the auto generated builder
				CreatorInterface* creator = new Creator<T>;
				_registeredCreator->insert(std::pair<typename Map::key_type, CreatorInterface*>(key, creator));
				return key;
			}

			template <class T>
				static typename Map::key_type getKey()
			{
				// If no registered classes, then always return empty
				if (_registeredCreator == NULL)
					return "";

				// Search for a creator for the T class
				typename Map::const_iterator it;
				for (it = _registeredCreator->begin(); it != _registeredCreator->end(); ++it)
					if (dynamic_cast<Creator<T>*>(it->second) != NULL)
						return it->first;

				// No such creator was founded : empty return
				return "";

			}


			static bool contains( const typename Map::key_type& key )
			{
				// Search of the key of the wished class in the map
				typename Map::iterator it = _registeredCreator->find(key);

				// The key is not found
				return it != _registeredCreator->end();
			}

			static RootObject* create(const typename Map::key_type& key)
			{
				// The factory "single object" was never filled
				if (_registeredCreator == NULL)
					return NULL;

				// Search of the key of the wished class in the map
				typename Map::iterator it = _registeredCreator->find(key);

				// The key is not found
				if(it == _registeredCreator->end())
					return NULL;

				// The key is found : return of an instance of the object
				return it->second->create();
			}

			static void destroy()
			{
				delete _registeredCreator;
			}

			class Iterator
			{
			private:
				typename Factory::Map::const_iterator _it;
				RootObject* _obj;

			public:
				Iterator(const typename Factory::Map::const_iterator& it)
					: _it(it), _obj(NULL)
				{	}

				~Iterator()
				{
					delete _obj;
				}

				/** Temporary object creator.
				Use it to run static members of the subclasses without having to handle instanciation.
				@return Pointer to a temporary object of the current subclass, which will be deleted at the next iteration.
				@warning Do not use the pointer after an iteration. If you want a persistent object, use getObject method instead.
				*/
				RootObject* operator*()
				{
					if (_obj == NULL)
						_obj = _it->second->create();
					return _obj;
				}

				RootObject* operator->()
				{
					return operator*();
				}

				/** Iteration.
				*/
				void operator++()
				{
					delete _obj;
					++_it;
					_obj = NULL;
				}

				bool operator==(const Iterator& obj)
				{
					return obj._it == _it;
				}

				bool operator!=(const Iterator& obj)
				{
					return obj._it != _it;
				}

				/** Persistent object creator.
				@return Pointer to an object of the current subclass.
				@warning To avoid memory leaks, do not forget to delete the returned object.
				*/
				RootObject* getObject()
				{
					return _it->second->create();
				}

				/** Key getter.
				@return The key of the current subclass.
				*/
				const typename Factory::Map::key_type& getKey()
				{
					return _it->first;
				}
			};

			/** First iterator on the subclasses.
			@return First iterator on the subclasses.
			*/
			static Iterator begin()
			{		
				return Iterator( _registeredCreator->begin() );
			}

			/** Last iterator on the subclasses.
			@return Last iterator on the subclasses.
			*/
			static Iterator end()
			{
				return Iterator( _registeredCreator->end() );
			}
		};

		template <class T>
			typename Factory<T>::Map* Factory<T>::_registeredCreator = NULL;

	}
}

#endif // Factory_h__

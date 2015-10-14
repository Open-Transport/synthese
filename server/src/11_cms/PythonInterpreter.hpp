
/** PythonInterpreter class header.
		@file PythonInterpreter.hpp

		This file belongs to the SYNTHESE project (public transportation specialized software)
		Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

		This program is free software; you can redistribute it and/or
		modify it under the terms of the GNU General Public License
		as published by the Free Software Foundation; either version 2
		of the License, or (at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_cms_PythonInterpreter_hpp__
#define SYNTHESE_cms_PythonInterpreter_hpp__

#include <Python.h>
#include <string>

#include "ParametersMap.h"


namespace synthese
{
	namespace cms
	{

		/** PythonInterpreter class.
		@ingroup m11
		*/
		class PythonInterpreter
		{

		public:

			static const std::string INTERPRETER_NAME;

			static const std::string MODULE_NAME;


			static void Initialize();

			static void End();

			static PyObject* BindingService(PyObject* self, PyObject* args);

			static PyObject* BindingAction(PyObject* self, PyObject* args);

			static PyObject* BindingInclude(PyObject* self, PyObject* args);

			static void Execute(
				const std::string& script,
				std::ostream& stream,
				const util::ParametersMap& parameters,
				util::ParametersMap& variables
			);


		private:

			// Declaration of bindings to SYNTHESE APIs
			static PyMethodDef PYTHON_BINDINGS[];

			// Thread state during Python initialization, preserved for cleanup
			static PyThreadState* _initialThreadState;

			PythonInterpreter();

			~PythonInterpreter();

			static PyObject* BuildDictionaryFromParametersMap(const synthese::util::ParametersMap& map);

			static void _BuildDictionaryFromParametersMap(PyObject* dictionary, const synthese::util::ParametersMap& map);

			static boost::shared_ptr<synthese::util::ParametersMap> BuildParametersMapFromDictionary(PyObject* dictionary);

			static void _BuildParametersMapFromDictionary(boost::shared_ptr<synthese::util::ParametersMap> map, PyObject* dictionary);


			// This struct stores the variables required for each thread-specific instance of the Python interpreter
			struct ThreadSpecificInterpreter
			{

				PyThreadState* _threadState;

				PyObject* _mainModule;

				PyObject* _sysModule;

				PyObject* _globals;

				ThreadSpecificInterpreter();

				~ThreadSpecificInterpreter();

			};

		};

	}
}

#endif // SYNTHESE_cms_PythonInterpreter_hpp__

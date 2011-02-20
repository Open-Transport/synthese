////////////////////////////////////////////////////////////////////////////////
/// Function class header.
///	@file Function.h
///	@author Hugues Romain
///	@date 2008-12-26 18:00
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_Function_h__
#define SYNTHESE_Function_h__

#include "FactoryBase.h"
#include "ParametersMap.h"
#include "Env.h"

#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////
/// @defgroup refFunctions Functions
///	@ingroup ref

namespace synthese
{
	namespace security
	{
		class Profile;
	}

	namespace server
	{
		class Session;
		class Request;

		//////////////////////////////////////////////////////////////////////////
		/// Public function abstract class to use in a Request.
		///
		/// A public function is a feature that can be used by passing a SYNTHESE Request,
		/// and that produces an output.
		/// NB : Not to be confused with public action, that produces no output and can be launched
		/// before a public function (see Action class)
		///
		/// The Function class is the root class of a factory which is derived by each public
		/// function class.
		///
		/// A public function object is a mapping between a request object (see Request class)
		/// and some features of SYNTHESE.
		/// Warning : the public function object is not supposed to contain any feature itself.
		///
		/// The public function object can be used by two symmetrical ways :
		///		- run a feature from a request : the key of the child class, contained in the request,
		///		determines which subclass handles the requested feature. It provides a method in charge
		///		of the control of the provided arguments of the feature. (see _setFromParametersMap method)
		///		- produce a request object from a feature : a method builds the whole request string from
		///		the parameters stored previously in the object by the setters. (see _getParametersMap method)
		///
		/// @ingroup m15
		//////////////////////////////////////////////////////////////////////////
		class Function
		:	public util::FactoryBase<Function>
		{
		public:

		protected:
			server::ParametersMap _savedParameters;
			boost::shared_ptr<util::Env>	_env;
			
			//////////////////////////////////////////////////////////////////////////
			///	Constructor.
			//////////////////////////////////////////////////////////////////////////
			Function(
				boost::shared_ptr<util::Env> env = util::Env::GetOfficialEnvSPtr()
			):	util::FactoryBase<Function>(),
				_env(env)
			{
			}

		public:
			//! @name Getters
			//@{
				boost::shared_ptr<util::Env> getEnv() const { return _env; }
				const server::ParametersMap& getSavedParameters() const { return _savedParameters; }
			//@}

			//! @name Setters
			//@{
				void setEnv(boost::shared_ptr<util::Env> value){ _env = value; }
				void setSavedParameters(const ParametersMap& value){ _savedParameters = value; }
				void removeSavedParameter(const std::string& key){ _savedParameters.remove(key); }
			//@}



			/** Conversion from attributes to generic parameter maps.
				@return The generated parameters map
			*/
			virtual ParametersMap _getParametersMap() const = 0;


			/** Gets the Mime type of the data produced by the function.
				@return Mime type of the data produced by the function
			*/
			virtual std::string getOutputMimeType() const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the default name of the generated file.
			/// @return default name of the generated file
			///    empty = display directly the result in the browser
			virtual std::string getFileName() const { return std::string(); }



			/** Copy of the function parameters.
				@param function
				@author Hugues Romain
				@date 2007				
			*/
			virtual void _copy(const Function& function) {}


			/** Conversion from generic parameters map to attributes.
				@param map The map to interpret (comes from _parseString())
				@throw RequestException if a parameter is missing or has corrupted value
			*/
			virtual void _setFromParametersMap(const ParametersMap& map) = 0;



			/** Method to run before display when no session is detected.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behavior is to do nothing and continue the execution without any session.
			*/
			virtual bool _runBeforeDisplayIfNoSession(std::ostream& stream) { return false; }


			/** Authorization control.
				@param session Session to read
				@return True if the function run is authorized
				@author Hugues Romain
				@date 2007
			*/
			virtual bool isAuthorized(
				const Session* session
			) const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Function to display, defined by each subclass.
			///	@param stream Stream to write the output on.
			/// @param request The request which has launched the function
			virtual void run(
				std::ostream& stream,
				const Request& request
			) const = 0;
		};
}	}

#endif // SYNTHESE_Function_h__

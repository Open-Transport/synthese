
/** WFSService class implementation.
	@file WFSService.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "WFSService.hpp"
#include "DBModule.h"
#include "CoordinatesSystem.hpp"
#include "WFSType.hpp"

#include <geos/geom/Envelope.h>
#include <geos/geom/Point.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace db;

	template<> const string util::FactorableTemplate<Function,map::WFSService>::FACTORY_KEY("WFS");

	namespace map
	{
		const string WFSService::PARAMETER_VERSION("VERSION");
		const string WFSService::VALUE_VERSION_1_1_0("1.1.0");

		const string WFSService::PARAMETER_REQUEST("REQUEST");
		const string WFSService::VALUE_REQUEST_GetCapabilities("GetCapabilities");
		const string WFSService::VALUE_REQUEST_DescribeFeatureType("DescribeFeatureType");
		const string WFSService::VALUE_REQUEST_GetFeature("GetFeature");

		const string WFSService::PARAMETER_OUTPUTFORMAT("OUTPUTFORMAT");
		const string WFSService::VALUE_OUTPUTFORMAT_GML_3_1_1("text/xml; subtype=gml/3.1.1");

		const string WFSService::PARAMETER_RESULTTYPE("RESULTTYPE");
		const string WFSService::VALUE_RESULTTYPE_RESULTS("results");
		const string WFSService::VALUE_RESULTTYPE_HITS("hits");

		const string WFSService::PARAMETER_SRSNAME("SRSNAME");

		const string WFSService::PARAMETER_TYPENAME("TYPENAME");

		const string WFSService::PARAMETER_BBOX("BBOX");

		WFSService::WFSService():
			_version(VALUE_VERSION_1_1_0),
			_request(VALUE_REQUEST_GetCapabilities),
			_outputFormat(VALUE_OUTPUTFORMAT_GML_3_1_1),
			_outputResults(true),
			_srs(&CoordinatesSystem::GetStorageCoordinatesSystem())
		{}



		ParametersMap WFSService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_VERSION, _version);
			map.insert(PARAMETER_REQUEST, _request);
			map.insert(PARAMETER_OUTPUTFORMAT, _outputFormat);
			map.insert(PARAMETER_RESULTTYPE, _outputResults ? VALUE_RESULTTYPE_RESULTS : VALUE_RESULTTYPE_HITS);

			// SRS name
			if(_srs)
			{
				map.insert(PARAMETER_SRSNAME, "EPSG:" + lexical_cast<string>(_srs->getSRID()));
			}

			// Type name
			stringstream typeName;
			bool first(true);
			BOOST_FOREACH(const boost::shared_ptr<WFSType>& type, _types)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					typeName << ",";
				}
				typeName << type->getFactoryKey();
			}
			map.insert(PARAMETER_TYPENAME, typeName.str());

			// BBox
			if(_envelope && _srs)
			{
				boost::shared_ptr<Point> minPoint(
					CoordinatesSystem::GetStorageCoordinatesSystem().createPoint(_envelope->getMinX(), _envelope->getMinY())
				);
				boost::shared_ptr<Point> maxPoint(
					CoordinatesSystem::GetStorageCoordinatesSystem().createPoint(_envelope->getMaxX(), _envelope->getMaxY())
				);

				boost::shared_ptr<Point> convMinPoint(
					_srs->convertPoint(*minPoint)
				);
				boost::shared_ptr<Point> convMaxPoint(
					_srs->convertPoint(*maxPoint)
				);

				stringstream s;
				s << convMinPoint->getX() << " " << convMinPoint->getY() << " " <<
					convMaxPoint->getX() << " " << convMaxPoint->getY();
				map.insert(PARAMETER_BBOX, s.str());
			}

			return map;
		}



		void WFSService::_setFromParametersMap(const ParametersMap& map)
		{
			// Version
			_version = map.getDefault<string>(PARAMETER_VERSION, VALUE_VERSION_1_1_0);
			if(_version != VALUE_VERSION_1_1_0)
			{
				throw RequestException("Only version "+ VALUE_VERSION_1_1_0 + " of WFS protocol is allowed.");
			}

			// Request
			_request = map.get<string>(PARAMETER_REQUEST);
			if(	_request != VALUE_REQUEST_GetFeature &&
				_request != VALUE_REQUEST_DescribeFeatureType &&
				_request != VALUE_REQUEST_GetCapabilities
			){
				throw RequestException("SYNTHESE is a basic WFS service. Only "+ VALUE_REQUEST_GetCapabilities +","+ VALUE_REQUEST_DescribeFeatureType +", and "+ VALUE_REQUEST_GetFeature +" request are allowed.");
			}

			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUTFORMAT, VALUE_OUTPUTFORMAT_GML_3_1_1);
			if(_outputFormat != VALUE_OUTPUTFORMAT_GML_3_1_1)
			{
				throw RequestException("Only "+ VALUE_OUTPUTFORMAT_GML_3_1_1 +" output format is allowed.");
			}

			// Result type
			string outputResults(map.getDefault<string>(PARAMETER_RESULTTYPE, VALUE_RESULTTYPE_RESULTS));
			if(outputResults != VALUE_RESULTTYPE_RESULTS)
			{
				throw RequestException("Only "+ VALUE_RESULTTYPE_RESULTS +" result type is allowed");
			}
			_outputResults = (outputResults == VALUE_RESULTTYPE_RESULTS);

			// SRS
			if(map.getOptional<string>(PARAMETER_SRSNAME))
			{
				string srsName(map.get<string>(PARAMETER_SRSNAME));
				if(srsName.size() < 6)
				{
					throw RequestException("Invalid SRS name");
				}

				try
				{
					CoordinatesSystem::SRID srid(lexical_cast<CoordinatesSystem::SRID>(srsName.substr(5)));
					_srs = &CoordinatesSystem::GetCoordinatesSystem(srid);
				}
				catch(bad_lexical_cast)
				{
					throw RequestException("Invalid SRS name");
				}
				catch(CoordinatesSystem::CoordinatesSystemNotFoundException)
				{
					throw RequestException("Invalid SRID");
				}
			}

			// Type name
			string typeNames(map.get<string>(PARAMETER_TYPENAME));
			vector<string> typeNamesVector;
			split(typeNamesVector, typeNames, is_any_of(",; ") );
			BOOST_FOREACH(const string& typeName, typeNamesVector)
			{
				if(!Factory<WFSType>::contains(typeName))
				{
					throw RequestException("Type "+ typeName +" does not exists.");
				}
				_types.push_back(boost::shared_ptr<WFSType>(Factory<WFSType>::create(typeName)));
			}

			// BBox
			if(map.isDefined(PARAMETER_BBOX))
			{
				string bbox(map.get<string>(PARAMETER_BBOX));
				vector<string> bboxVector;
				split(bboxVector, bbox, is_any_of(",; ") );
				if(bboxVector.size() != 4)
				{
					throw RequestException("Malformed BBOX");
				}

				try
				{
					boost::shared_ptr<Point> minPoint(
						_srs->getGeometryFactory().createPoint(
							Coordinate(
								lexical_cast<double>(bboxVector[0]),
								lexical_cast<double>(bboxVector[1])
					)	)	);
					boost::shared_ptr<Point> maxPoint(
						_srs->getGeometryFactory().createPoint(
							Coordinate(
								lexical_cast<double>(bboxVector[2]),
								lexical_cast<double>(bboxVector[3])
					)	)	);

					boost::shared_ptr<Point> wgsMinPoint(
						CoordinatesSystem::GetStorageCoordinatesSystem().convertPoint(*minPoint)
					);
					boost::shared_ptr<Point> wgsMaxPoint(
						CoordinatesSystem::GetStorageCoordinatesSystem().convertPoint(*maxPoint)
					);

					_envelope = Envelope(
						*wgsMinPoint->getCoordinate(),
						*wgsMaxPoint->getCoordinate()
					);
				}
				catch(bad_lexical_cast)
				{
					throw RequestException("Malformed BBOX");
				}

			}

		}



		ParametersMap WFSService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// GetFeature service
			if(_request == VALUE_REQUEST_GetFeature)
			{
				// Header
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<wfs:FeatureCollection	" <<
						"xmlns=\"https://extranet.rcsmobility.com/projects/synthese\" " <<
						"xmlns:synthese=\"https://extranet.rcsmobility.com/projects/synthese\" " <<
						"xmlns:wfs=\"http://www.opengis.net/wfs\" " <<
						"xmlns:gml=\"http://www.opengis.net/gml\" " <<
						"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " <<
						"xsi:schemaLocation=\"https://extranet.rcsmobility.com/projects/synthese http://" << request.getHostName() << request.getClientURL() << "?" << Request::PARAMETER_SERVICE << "=" << FACTORY_KEY << "&amp;" << PARAMETER_REQUEST << "=" << VALUE_REQUEST_DescribeFeatureType << "&amp;" << PARAMETER_TYPENAME << "=";
				bool first(true);
				BOOST_FOREACH(const boost::shared_ptr<WFSType>& type, _types)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					stream << type->getFactoryKey();
				}
				stream << "\" " <<
					">";

				// Features
				BOOST_FOREACH(const boost::shared_ptr<WFSType>& type, _types)
				{
					if(_envelope)
					{
						type->getFeatures(stream, *_envelope, *_srs);
					}
				}

				// Footer
				stream << "</wfs:FeatureCollection>";
			}
			// DescribeFeatureType service
			else if(_request == VALUE_REQUEST_DescribeFeatureType)
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<schema " <<
						"xmlns:synthese=\"https://extranet.rcsmobility.com/projects/synthese\" " <<
						"xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " <<
						"xmlns=\"http://www.w3.org/2001/XMLSchema\" " <<
						"xmlns:gml=\"http://www.opengis.net/gml\" " <<
						"elementFormDefault=\"qualified\" " <<
						"attributeFormDefault=\"unqualified\" " <<
					">" <<
					"<import namespace=\"http://www.opengis.net/gml\" schemaLocation=\"../gml/3.1.1/base/gml.xsd\" />"
				;

				BOOST_FOREACH(const boost::shared_ptr<WFSType>& type, _types)
				{
					type->getSchema(stream);
				}

				stream << "</schema>";
			}
			// GetCapabilities service
			else if(_request == VALUE_REQUEST_GetCapabilities)
			{

				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<wfs:WFS_Capabilites " <<
						"xmlns:ows=\"http://www.opengis.net/ows\" " <<
						"xmlns:ogc=\"http://www.opengis.net/ogc\" " <<
						"xmlns:wfs=\"http://www.opengis.net/wfs\" " <<
						"xmlns:gml=\"http://www.opengis.net/gml\" " <<
						"xmlns:xlink=\"http://www.w3.org/1999/xlink\" " <<
						"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " <<
						"xsi:schemaLocation=\"http://www.opengis.net/wfs ../wfs.xsd\" " <<
						"version=\"" << VALUE_VERSION_1_1_0 << "\" " <<
						"updateSequence=\"0\" " <<
					">" <<

					// Service identification
					"<ows:ServiceIdentification>" <<
						"<ows:ServiceType>WFS</ows:ServiceType>" <<
						"<ows:ServiceTypeVersion>" << VALUE_VERSION_1_1_0 << "</ows:ServiceTypeVersion>" <<
						"<ows:Title>SYNTHESE WFS</ows:Title>" <<
						"<ows:Abstract>Public transportation server</ows:Abstract>" <<
						"<ows:Keywords>" <<
							"<ows:Keyword>RCSmobility</ows:Keyword>" <<
							"<ows:Type>String</ows:Type>" <<
						"</ows:Keywords>"
						"<ows:Fees>None</ows:Fees>" <<
						"<ows:AccessConstraints>None</ows:AccessConstraints>" <<
					"</ows:ServiceIdentification>" <<

					// Service provider
					"<ows:ServiceProvider>" <<
					"<ows:ProviderName>x</ows:ProviderName>" <<
					"<ows:ProviderSite/>" <<
					"<ows:ServiceContact>" <<
					"<ows:IndividualName>x</ows:IndividualName>" <<
					"<ows:PositionName>x</ows:PositionName>" <<
					"<ows:ContactInfo>" <<
					"<ows:Phone>" <<
					"<ows:Voice>x</ows:Voice>" <<
					"<ows:Facsimile>x</ows:Facsimile>" <<
					"</ows:Phone>" <<
					"<ows:Address>" <<
					"<ows:DeliveryPoint>x</ows:DeliveryPoint>" <<
					"<ows:City>x</ows:City>" <<
					"<ows:AdministrativeArea>x</ows:AdministrativeArea>" <<
					"<ows:PostalCode>x</ows:PostalCode>" <<
					"<ows:Country>x</ows:Country>" <<
					"<ows:ElectronicMailAddress>x</ows:ElectronicMailAddress>" <<
					"</ows:Address>" <<
					"<ows:OnlineResource xlink:href=\"x\"/>" <<
					"<ows:HoursOfService>24x7</ows:HoursOfService>" <<
					"<ows:ContactInstructions>x</ows:ContactInstructions>" <<
					"</ows:ContactInfo>" <<
					"<ows:Role>x</ows:Role>" <<
					"</ows:ServiceContact>" <<
					"</ows:ServiceProvider>" <<

					// Operations metadata
					"<ows:OperationsMetadata>" <<
					"<ows:Operation name=\"" << VALUE_REQUEST_GetCapabilities << "\">" <<
					"<ows:DCP>" <<
					"<ows:HTTP>" <<
					"<ows:Get xlink:href=\"" << request.getClientURL() << "\"/>" <<
					"</ows:HTTP>" <<
					"</ows:DCP>" <<
					"<ows:Parameter name=\"AcceptVersions\">" <<
					"<ows:Value>" << VALUE_VERSION_1_1_0 << "</ows:Value>" <<
					"</ows:Parameter>" <<
					"<ows:Parameter name=\"AcceptFormats\">" <<
					"<ows:Value>text/xml</ows:Value>" <<
					"</ows:Parameter>" <<
					"<ows:Parameter name=\"Sections\">" <<
					"<ows:Value>ServiceIdentification</ows:Value>" <<
					"<ows:Value>ServiceProvider</ows:Value>" <<
					"<ows:Value>OperationsMetadata</ows:Value>" <<
					"<ows:Value>FeatureTypeList</ows:Value>" <<
					"<ows:Value>Filter_Capabilities</ows:Value>" <<
					"</ows:Parameter>" <<
					"</ows:Operation>" <<
					"<ows:Operation name=\"" << VALUE_REQUEST_DescribeFeatureType << "\">" <<
					"<ows:DCP>" <<
					"<ows:HTTP>" <<
					"<ows:Get xlink:href=\"" << request.getClientURL() << "?\"/>" <<
					"<ows:Post xlink:href=\"" << request.getClientURL() << "\"/>" <<
					"</ows:HTTP>" <<
					"</ows:DCP>" <<
					"<ows:Parameter name=\"outputFormat\">" <<
					"<ows:Value>" << VALUE_OUTPUTFORMAT_GML_3_1_1 << "</ows:Value>" <<
					"</ows:Parameter>" <<
					"</ows:Operation>" <<
					"<ows:Operation name=\"" << VALUE_REQUEST_GetFeature << "\">" <<
					"<ows:DCP>" <<
					"<ows:HTTP>" <<
					"<ows:Get xlink:href=\"" << request.getClientURL() << "?\"/>" <<
					"<ows:Post xlink:href=\"" << request.getClientURL() << "\"/>" <<
					"</ows:HTTP>" <<
					"</ows:DCP>" <<
					"<ows:Parameter name=\"resultType\">" <<
					"<ows:Value>" << VALUE_RESULTTYPE_RESULTS << "</ows:Value>" <<
//					"<ows:Value>hits</ows:Value>" <<
					"</ows:Parameter>"
					"<ows:Parameter name=\"outputFormat\">" <<
					"<ows:Value>" << VALUE_OUTPUTFORMAT_GML_3_1_1 << "</ows:Value>" <<
					"</ows:Parameter>" <<
					"</ows:Operation>" <<
					"</ows:OperationsMetadata>" <<

					// FeatureTypeList
					"<wfs:FeatureTypeList>";

				BOOST_FOREACH(const boost::shared_ptr<WFSType>& type, _types)
				{
					stream <<
						"<wfs:FeatureType xmlns:synthese=\"https://extranet.rcsmobility.com/projects/synthese\">" <<
						"<wfs:Name>synthese:" << type->getFactoryKey() << "Type</wfs:Name>" <<
						"<wfs:Title>" << type->getFactoryKey() << "</wfs:Title>" <<
						"<wfs:Abstract>" << type->getFactoryKey() << "</wfs:Abstract>" <<
						"<wfs:DefaultSRS>EPSG:" << CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() << "</wfs:DefaultSRS>" <<
						"<wfs:OutputFormats><wfs:Format>" << VALUE_OUTPUTFORMAT_GML_3_1_1 << "</wfs:Format></wfs:OutputFormats>" <<
						"<ows:WGS84BoundingBox>"
						"<ows:LowerCorner>-180 -90</ows:LowerCorner>"
						"<ows:UpperCorner>180 90</ows:UpperCorner>"
						"</ows:WGS84BoundingBox>"
						"<wfs:MetadataURL type=\"FGDC\" format=\"text/xml\">http://www.ogccatservice.com/csw.cgi?service=CSW&amp;version=2.0.0&amp;request=GetRecords&amp;constraintlanguage=CQL&amp;constraint=\"recordid=urn:uuid:4ee8b2d3-9409-4a1d-b26b-6782e4fa3d59\"</wfs:MetadataURL>" <<
						"</wfs:FeatureType>";
				}

				stream <<
					"</wfs:FeatureTypeList>" <<

					// Filter capabilities
					"<ogc:Filter_Capabilities>" <<
					"<ogc:Spatial_Capabilities>" <<
					"<ogc:SpatialOperators>" <<
					"<ogc:SpatialOperator name=\"BBOX\"/>" <<
					"</ogc:SpatialOperators>" <<
					"</ogc:Spatial_Capabilities>" <<
					"</ogc:Filter_Capabilities>" <<
				"</wfs:WFS_Capabilites>";
			}

			return ParametersMap();
		}



		bool WFSService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WFSService::getOutputMimeType() const
		{
			return _outputFormat;
		}
}	}

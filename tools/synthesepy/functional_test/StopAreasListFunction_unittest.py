#    StopAreasListFunction unit tests.
#    @file StopAreasListFunction_unittest.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


import logging
import sys
from xml.dom import minidom

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


log = logging.getLogger(__name__)


class StopAreasListFunctionTest(http_testcase.HTTPTestCase):
    # Uncomment for debugging
    #system_packages = ('routePlanner', 'testData', 'core', 'demo', 'map')

    @classmethod
    def init_project(cls, project):
        # Update labels to test that encoding is done right.
        project.db_backend.query(
            """update t007_connection_places set name = '96''<>"' where id = 1970324837184596;""")

    def test_filter_bbox(self):
        http_api = self.get_http_api()

        output, info = http_api.call_service('StopAreasListFunction', {
            'srid': '900913',
            'bbox': '152102.251737,5408525.019353,159679.072164,5414257.796474',
            'ol': 1,
        })

        expected_xml = """<?xml version="1.0" ?>
        <stopAreas xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:noNamespaceSchemaLocation="http://synthese.rcsmobility.com/include/35_pt/StopAreasListFunction.xsd">
            <stopArea cityId="1688849860511154" cityName="City54"
                city_id="1688849860511154" city_name="City54" directionAlias=""
                id="1970324837184606" name="06" stop_id="1970324837184606"
                stop_name="06" stop_name_13="" stop_name_26=""
                stop_name_for_timetables=""
                x="158106.628029" y="5413891.407412">
                <line creatorId="" id="11821949021891598" image=""
                    lineImage="" lineName="" lineShortName="98"
                    lineStyle="vertclair" line_doc_url=""
                    line_id="11821949021891598" line_image=""
                    line_long_name="" line_map_url="" line_short_name="98"
                    line_style="vertclair" longName="" name="" shortName="98"
                    style="vertclair" line_timetable_id="0"><transportMode article="" co2_emissions="0" energy_consumption="0" id="13792273858822157" name="test" /></line>
                </stopArea>
            <stopArea cityId="1688849860511154" cityName="City54"
                city_id="1688849860511154" city_name="City54" directionAlias=""
                id="1970324837184607" name="07" stop_id="1970324837184607"
                stop_name="07" stop_name_13="" stop_name_26=""
                stop_name_for_timetables=""
                x="153971.888184" y="5413842.009339"/>
            <stopArea cityId="1688849860530938" cityName="City38"
                city_id="1688849860530938" city_name="City38"
                directionAlias="" id="1970324837184596" name="96'&lt;&gt;&quot;"
                stop_id="1970324837184596" stop_name="96'&lt;&gt;&quot;" stop_name_13=""
                stop_name_26="" stop_name_for_timetables=""
                x="155391.434330" y="5410401.567916"/>
        </stopAreas>
        """

        self.assertSameXML(output, expected_xml)

    def assertSameXML(self, xml1, xml2):
        # From http://stackoverflow.com/questions/321795/comparing-xml-in-a-unit-test-in-python
        # by bobince (slightly adapted).

        def isEqualElement(a, b):
            self.assertEquals(a.tagName, b.tagName, 'Tagnames mismatch')

            def getAttrs(node):
                attrs = {}
                for k, v in node.attributes.items():
                    try:
                        fv = float(v)
                    except ValueError:
                        attrs[k] = v
                        continue
                    attrs[k] = round(fv, 1)
                return attrs

            self.assertEquals(getAttrs(a), getAttrs(b),
                'Attributes don\'t match')

            # Ignore text nodes if there are several childs
            if a.childNodes > 1:
                acn = [n for n in a.childNodes if n.nodeType != n.TEXT_NODE]
                bcn = [n for n in b.childNodes if n.nodeType != n.TEXT_NODE]
            else:
                acn = a.childNodes
                bcn = b.childNodes

            self.assertEquals(len(acn), len(bcn), 'Child count mismatch')

            for ac, bc in zip(acn, bcn):
                self.assertEquals(ac.nodeType, bc.nodeType)
                if ac.nodeType == ac.TEXT_NODE:
                    self.assertEquals(ac.data, bc.data)
                if ac.nodeType == ac.ELEMENT_NODE:
                    isEqualElement(ac, bc)

        da, db = minidom.parseString(xml1), minidom.parseString(xml2)
        isEqualElement(da.documentElement, db.documentElement)


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)

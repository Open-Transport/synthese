#    -*- coding: utf-8 -*-
#    OSM import unit tests.
#    @file osm_import_unittest.py
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


import collections
import logging
import os
import pprint
import sys

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
from BeautifulSoup import BeautifulSoup
import mechanize

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


log = logging.getLogger(__name__)


class DBObjectTree(object):
    """
    Class to build a tree of object retrieved from the database.
    It can be used to compare that the db content matches some expectations
    when knowing only about relations between objects and not their specific
    ids.
    """
    def __init__(self, db_backend, tester):
        self.db_backend = db_backend
        self.tester = tester
        self.id_to_obj = {}
        self.root = {}

    def load_objects(self, table_name, parent_id_col=None, parent_key_name=None):
        for obj in self.db_backend.query('select * from %s order by id' % table_name):
            obj['_table'] = table_name
            self.id_to_obj[obj['id']] = obj
            if parent_id_col:
                parent = self.id_to_obj[obj[parent_id_col]]
                parent.setdefault(parent_key_name, []).append(obj)
            else:
                self.root.setdefault('children', []).append(obj)

    def assertEquals(self, other, me=None):
        def update_obj_for_test(obj):
            if 'metric_offset' in obj:
                obj['metric_offset'] = round(obj['metric_offset'], 3)

        if me is None:
            me = self.root

        update_obj_for_test(me)
        update_obj_for_test(other)

        same_keys = set(me) & set(other)
        me_list_keys = set(k for (k, v) in me.iteritems() if type(k) if isinstance(v, list))
        other_list_keys = set(k for (k, v) in other.iteritems() if type(k) if isinstance(v, list))
        list_keys = me_list_keys | other_list_keys
        same_keys -= list_keys
        node_filtered = dict((k, me[k]) for k in me if k in same_keys)
        other_filtered = dict((k, other[k]) for k in other if k in same_keys)

        self.tester.assertEquals(node_filtered, other_filtered)

        for list_key in list_keys:
            self.tester.assertTrue(list_key in me)
            me_children = me[list_key]
            self.tester.assertTrue(list_key in other)
            other_children = other[list_key]
            self.tester.assertEquals(len(me_children), len(other_children))
            for m, o in zip(me_children, other_children):
                self.assertEquals(o, m)

class OSMImportTest(http_testcase.HTTPTestCase):
    # Package override to avoid loading the testData package.
    system_packages = ()
    OSM_SOURCE_ID = 16607027920896002

    @classmethod
    def init_project(cls, project):
        project.db_backend.query(
            "insert into t059_data_sources (id, name, format, icon, charset, srid) "
            "values(?, 'osm', 'OpenStreetMap', '', '', 4326);", [cls.OSM_SOURCE_ID])

        # TODO: Create a dedicated test for Navstreets.
        # TODO: what encoding to use?
        project.db_backend.query(
            "insert into t059_data_sources (id, name, format, icon, charset, srid) "
            "values(16607027920896003, 'Navstreets', 'Navstreets', '', 'CP1252', 27572);")

    def tearDown(self):
        # TODO: this should be handled by an import flag instead of cleaning
        # the db manually and restarting the daemon.
        if not self.no_init:
            self.project.stopdaemon()

            for table in ('t006_cities', 't060_road_places', 't015_roads', 't014_road_chunks'):
                self.project.db_backend.query('delete from %s' % table)

    def check_import(self, file_name, expected_root=None):
        if not self.no_init and not self.project.daemon.is_running():
            log.info('Starting daemon...')
            self.project.rundaemon(False)

        http_api = self.get_http_api()

        if not os.path.isabs(file_name):
            thisdir = os.path.abspath(os.path.dirname(__file__))
            file_name = os.path.join(thisdir, 'test_data', 'osm', file_name)

        http_api.call_service('ImportFunction', {
            'ds': self.OSM_SOURCE_ID,
            'di': '1',
            'pa': file_name,
        })

        tree = DBObjectTree(self.project.db_backend, self)
        tree.load_objects('t006_cities')
        tree.load_objects('t060_road_places', 'city_id', '_road_places')
        tree.load_objects('t015_roads', 'road_place_id', 'roads')
        tree.load_objects('t014_road_chunks', 'road_id', 'road_chunks')

        # Set it to 1 while debugging. It can produce lots of output.
        if 0:
            log.debug("Tree from osm file %s:\n%s",
                file_name, pprint.pformat(tree.root))

        if expected_root:
            tree.assertEquals(expected_root)

        return tree

    def _get_road_journey(self, start_city, start_place, end_city, end_place):
        http_api = self.get_http_api()
        br = http_api.get_admin_browser()
        br.open(http_api.get_admin_url('RoadJourneyPlannerAdmin', {
            'sc': start_city,
            'sp': start_place,
            'ec': end_city,
            'ep': end_place,
        }))

        html = br.response().read()
        soup = BeautifulSoup(html)

        journey_table = soup.find('table', 'adminresults')
        if not journey_table:
            log.debug('No journey result found')
            return []
        steps = []
        for row in journey_table.findAll('tr')[1:]:
            row_content = row.contents
            place_name = row_content[0].string
            def get_dist(elem):
                assert elem.string.endswith('m')
                return int(elem.string[:-1])
            dist = get_dist(row_content[1])
            total_dist = get_dist(row_content[2])
            time = row_content[3].b.string if row_content[3].b else None
            steps.append([place_name, dist])

        log.debug('Journey steps:\n%s', pprint.pformat(steps))
        return steps

    def test_import_one_way(self):
        # TODO: uncomment once it works with MySQL
        if self.backend.name == 'mysql':
            log.warn('OSM road import not yet working with MySQL')
            return

        self.check_import(
            'one_way.osm',
            {'children': [{'_road_places': [{'_table': 't060_road_places',
                                 'name': u'Road0',
                                 'roads': [{'_table': 't015_roads',
                                            'bike_compliance_id': 0,
                                            'handicapped_compliance_id': 0,
                                            'pedestrian_compliance_id': 0,
                                            'road_chunks': [{'_table': 't014_road_chunks',
                                                             #'geometry': <read-write buffer ptr 0x02E4D040, size 96 at 0x02E4D020>,
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 0.0,
                                                             'rank_in_path': 0,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''},
                                                            {'_table': 't014_road_chunks',
                                                             'geometry': None,
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 3181.6999999999998,
                                                             'rank_in_path': 1,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''}],
                                            'road_type': 6}]}],
               '_table': 't006_cities',
               'code': u'0',
               'name': u'CITY0'}]}
        )

    def test_import_three_connected_ways(self):
        # TODO: uncomment once it works with MySQL
        if self.backend.name == 'mysql':
            log.warn('OSM road import not yet working with MySQL')
            return

        self.check_import(
            'three_connected_ways.osm',
            {'children': [{'_road_places': [{'_table': 't060_road_places',
                                 'name': u'Road2',
                                 'roads': [{'_table': 't015_roads',
                                            'bike_compliance_id': 0,
                                            'handicapped_compliance_id': 0,
                                            'pedestrian_compliance_id': 0,
                                            'road_chunks': [{'_table': 't014_road_chunks',
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 0.0,
                                                             'rank_in_path': 0,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''},
                                                            {'_table': 't014_road_chunks',
                                                             'geometry': None,
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 1582.9846300340071,
                                                             'rank_in_path': 1,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''}],
                                            'road_type': 6}]},
                                {'_table': 't060_road_places',
                                 'name': u'Road1',
                                 'roads': [{'_table': 't015_roads',
                                            'bike_compliance_id': 0,
                                            'handicapped_compliance_id': 0,
                                            'pedestrian_compliance_id': 0,
                                            'road_chunks': [{'_table': 't014_road_chunks',
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 0.0,
                                                             'rank_in_path': 0,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''},
                                                            {'_table': 't014_road_chunks',
                                                             'geometry': None,
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 2060.4141143796446,
                                                             'rank_in_path': 1,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''}],
                                            'road_type': 6}]},
                                {'_table': 't060_road_places',
                                 'name': u'Road0',
                                 'roads': [{'_table': 't015_roads',
                                            'bike_compliance_id': 0,
                                            'handicapped_compliance_id': 0,
                                            'pedestrian_compliance_id': 0,
                                            'road_chunks': [{'_table': 't014_road_chunks',
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 0.0,
                                                             'rank_in_path': 0,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''},
                                                            {'_table': 't014_road_chunks',
                                                             'geometry': None,
                                                             'left_end_house_number': u'',
                                                             'left_house_numbering_policy': 65,
                                                             'left_start_house_number': u'',
                                                             'metric_offset': 1885.1549841416399,
                                                             'rank_in_path': 1,
                                                             'right_end_house_number': u'',
                                                             'right_house_numbering_policy': 65,
                                                             'right_start_house_number': u''}],
                                            'road_type': 6}]}],
               '_table': 't006_cities',
               'code': u'0',
               'name': u'CITY0'}]}
        )

        self.assertEquals(
            self._get_road_journey('City0', 'Road0', 'City0', 'Road1'),
            []
        )

        self.assertEquals(
            self._get_road_journey('City0', 'Road0', 'City0', 'Road2'),
            [
                [u'City0 Road0', 0],
                [u'Road1', 2060],
                [u'City0 Road2', 0]
            ]
        )

    def test_import_sample_data(self):
        # TODO: uncomment once it works with MySQL
        if self.backend.name == 'mysql':
            log.warn('OSM road import not yet working with MySQL')
            return

        tree = self.check_import('sample_data.osm.bz2')

        table_counter = collections.defaultdict(int)
        for obj in tree.id_to_obj.itervalues():
            table_counter[obj['_table']] += 1
        self.assertEquals(table_counter, {
            't006_cities': 4,
            't060_road_places': 52,
            't015_roads': 58,
            't014_road_chunks': 154,
        })

        self.assertEquals(
            self._get_road_journey(
                'Courtelevant', 'Rue des Chènevières',
                'Courtelevant', 'Impasse de la Chapperette'
            ),
            [
                [u'Courtelevant Rue des Ch\xe8nevi\xe8res', 0],
                [u'Rue des Grandes Gasses', 32],
                [u'Rue de la Vendeline', 98],
                [u'Rue de la Chapperette', 469],
                [u'Courtelevant Impasse de la Chapperette', 0],
            ]
        )

    # Test import of a large dataset. It can be prepared this way:
    # wget http://download.geofabrik.de/osm/europe/france/franche-comte.osm.bz2
    # To install osmosis, see http://wiki.openstreetmap.org/wiki/Osmosis
    # osmosis --read-xml franche-comte.osm.bz2 --bounding-box top=47.82512 left=6.75646 bottom=47.43333 right=7.14328 --write-xml territoire-belfort.osm.bz2
    def OFF_test_import_territoire_belfort(self):
        osm_file = os.environ.get(
            'OSM_TEST_FILE', 'territoire-belfort.osm.bz2')

        tree = self.check_import(osm_file)

        self.assertEquals(
            self._get_road_journey(
                'Belfort', 'Rue Moppert',
                'Belfort', 'Rue de Colmar'
            ),
            [] # TODO
        )
        self.assertEquals(
            self._get_road_journey(
                'Belfort', 'Place d\'Armes',
                'Belfort', 'Rue Roy'
            ),
            [] # TODO
        )


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)

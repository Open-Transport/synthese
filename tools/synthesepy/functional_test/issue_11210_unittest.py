#    Test for Issue 11210
#    @file issue_11210_unittest.py
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


import hashlib
import itertools
import logging
import multiprocessing.pool
import sys
import urllib2

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


log = logging.getLogger(__name__)

# FIXME: This test is currently disabled due to random failures.
# See https://extranet.rcsmobility.com/issues/13724
if False:
  class Issue11210Test(http_testcase.HTTPTestCase):
    def test_responses_dont_vary(self):
        http_api = self.get_http_api()

        PARAMS1 = {
            "of": "xml",
            "os": "1",
            "ni": "6192449487677434",
            #"og": "wkt",
            "srid": "900913",
            "SERVICE": "LinesListFunction2",
        }
        PARAMS2 = {
            "srid": "900913",
            "roid": "1688849860530938",
            "ol": "1",
            "SERVICE": "StopAreasListFunction",
        }
        URL1 = http_api.get_synthese_url(PARAMS1, True)
        URL2 = http_api.get_synthese_url(PARAMS2, True)

        def get_response_hash(url):
            response = urllib2.urlopen(url).read()
            assert len(response) > 100
            return hashlib.md5(response).hexdigest()

        pool = multiprocessing.pool.ThreadPool(6)
        REQUEST_COUNT = 100
        urls = list(itertools.islice(itertools.cycle([URL1, URL2]), REQUEST_COUNT))
        hashes = pool.map(get_response_hash, urls)
        url_to_hash = {}
        for url, hash in zip(urls, hashes):
            if url in url_to_hash:
                # TODO: activate assertion once issue 11210 is fixed
                log.warn("TODO: fix issue 11210 and enable assertion")
                if 0:
                    self.assertEquals(url_to_hash[url], hash)
            else:
                url_to_hash[url] = hash
        self.assertEquals(len(url_to_hash.keys()), len(set(urls)))


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)

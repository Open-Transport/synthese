#    Synthese HTTP API.
#    @file http_api.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
import urllib
import urllib2

import mechanize

log = logging.getLogger(__name__)


class HTTPApi(object):
    # TODO: get from config
    USERNAME = 'root'
    PASSWORD = 'root'

    def __init__(self, env, hostname=None, debug=False):
        if hostname:
            self.hostname = hostname
        else:
            self.hostname = 'localhost:%s' % env.c.wsgi_proxy_port
        self.sid = None
        self.browser = None

        if debug:
            logging.getLogger('mechanize').setLevel(logging.DEBUG)

    def _get_browser(self):
        br = mechanize.Browser()
        br.set_handle_robots(False)
        return br

    def get_admin_browser(self, logged_in=False):
        if logged_in:
            self._get_sid()

        br = self._get_browser()
        admin_url = 'http://%s/admin' % self.hostname
        br.open(admin_url)
        return br

    def _get_sid(self):
        # TODO: handle expired sid's properly.
        if self.sid:
            return self.sid

        br = self.get_admin_browser()
        br.select_form(name='login')

        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        cookiejar = br._ua_handlers['_cookies'].cookiejar
        # XXX this assumes we have only one sid cookie in the jar.
        self.sid = cookiejar._cookies.values()[0].values()[0]['sid'].value
        return self.sid

    def call_service(self, name, params, send_sid=False):
        SYNTHESE_URL = 'http://%s/synthese3' % self.hostname
        p = {
            'a': name,
        }
        p.update(params)
        if send_sid:
            p['sid'] = self._get_sid()
        post_data = urllib.urlencode(p)
        log.debug('Calling service with url %r, post_data %r', SYNTHESE_URL, post_data)
        response = urllib2.urlopen(SYNTHESE_URL, post_data)

        content = response.read()
        # TODO: detect errors?
        log.debug('Result string: %r, info: %r', content, response.info())
        return (content, response.info())

    def update_web_page_content(self, page_id, content):
        self.call_service('WebPageUpdateAction', {
            'actionParamwp': str(page_id),
            'actionParamc1': unicode(content).encode('utf-8'),
            'actionParamdx': '0',
            'co': '0',
            'nr': '1',
        }, send_sid=True)

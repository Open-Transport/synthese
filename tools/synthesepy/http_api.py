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
import urlparse

import mechanize

log = logging.getLogger(__name__)


class HTTPApi(object):
    def __init__(self, env, hostname=None, username='root', password='root',
        debug=False):
        if hostname:
            self.hostname = hostname
        else:
            self.hostname = 'localhost:%s' % env.c.wsgi_proxy_port
        self.username = username
        self.password = password
        self.debug = debug
        self.sid = None
        self.browser = None
        self.admin_base_url = 'http://{0}/admin/'.format(
            self.hostname)
        self.admin_url = self.admin_base_url + 'synthese'
        self.admin_base_params = {
            'SERVICE': 'admin',
        }

        if self.debug:
            logging.getLogger('mechanize').setLevel(logging.DEBUG)

            h = urllib2.HTTPHandler(debuglevel=1)
            opener = urllib2.build_opener(h)
            urllib2.install_opener(opener)

    def _get_browser(self):
        if self.browser:
            return self.browser
        self.browser = mechanize.Browser()
        self.browser.set_handle_robots(False)
        return self.browser

    def reset_browser(self):
        self.browser = None

    def get_admin_browser(self, logged_in=True):
        if logged_in:
            self._get_sid()

        br = self._get_browser()

        # Base URL should redirect to a Synthese URL with the expected
        # query parameters.
        br.open(self.admin_base_url)
        qs = dict(urlparse.parse_qsl(br.geturl()))
        # This string is only on the login screen.
        if logged_in:
            assert 'Mot de passe : ' not in br.response().read()
        self.admin_base_params['mt'] = qs['mt']
        return br

    def get_admin_url(self, admin_page, params):
        p = self.admin_base_params.copy()
        p['rub'] = admin_page
        p.update(params)
        return self.admin_url + '?' + urllib.urlencode(p)

    def _get_sid(self):
        # TODO: handle expired sid's properly.
        if self.sid:
            return self.sid

        br = self.get_admin_browser(logged_in=False)
        br.select_form(name='login')

        br['actionParamlogin'] = self.username
        br['actionParampwd'] = self.password
        response = br.submit()

        cookiejar = br._ua_handlers['_cookies'].cookiejar
        # XXX this assumes we have only one sid cookie in the jar.
        self.sid = cookiejar._cookies.values()[0].values()[0]['sid'].value
        return self.sid

    def get_synthese_url(self, params={}, use_get=False):
        url = 'http://%s/synthese' % self.hostname
        if use_get:
            url += '?' + urllib.urlencode(params)
        return url

    def call_synthese(self, params, send_sid=False, use_get=False):
        if send_sid:
            params['sid'] = self._get_sid()
        url = self.get_synthese_url(params, use_get)
        if use_get:
            post_data = None
        else:
            post_data = urllib.urlencode(params)
        log.debug('Calling synthese with url %r, post_data %r', url, post_data)

        response = urllib2.urlopen(url, post_data)

        content = response.read()
        # TODO: detect errors?
        log.debug('Result string: %r, info: %r', content, response.info())
        return (content, response.info())

    def call_action(self, action, params, send_sid=False, use_get=False):
        params['a'] = action
        return self.call_synthese(params, send_sid, use_get)

    def call_service(self, service, params, send_sid=False, use_get=False):
        params['SERVICE'] = service
        return self.call_synthese(params, send_sid, use_get)

    # XXX returns a parsed response.
    def get_web_page_content(self, page_id):
        return self.call_service('page', {
            'p': page_id
        })

    def update_web_page_content(self, page_id, content):
        self.call_action('WebPageUpdateAction', {
            'actionParamwp': str(page_id),
            'actionParamc1': unicode(content).encode('utf-8'),
            'actionParamdx': '0',
            'co': '0',
            'nr': '1',
        }, send_sid=True)

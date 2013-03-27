#    Synthese HTTP API.
#    @file http_api.py
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
import urllib
import urllib2
import urlparse

import mechanize
import requests

log = logging.getLogger(__name__)


class HTTPApiException(Exception):
    pass


class HTTPApi(object):
    def __init__(self, env, hostname=None, username='root', password='root',
        debug=False):
        self.env = env
        if hostname:
            self.hostname = hostname
        else:
            self.hostname = 'localhost:%s' % env.c.port
        self.username = username
        self.password = password
        self.debug = debug
        self.sid = None
        self.sid2 = None
        self.browser = None
        self.admin_base_url = 'http://{0}/admin/'.format(
            self.hostname)
        self.base_url = 'http://{0}/'.format(
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

    def get_browser_for_suffix(self, suffix):
        br = self._get_browser()
        br.open(self.base_url + suffix)
        return br

    def get_admin_browser(self, logged_in=True):
        if logged_in:
            self._get_sid()

        br = self._get_browser()

        br.open(self.admin_base_url)
        # Keep this in sync with the :admin_main page in admin package.
        MT_PAGE_ID = 17732923532771328
        self.admin_base_params['mt'] = MT_PAGE_ID
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
        if response.code != 200:
            raise HTTPApiException('call_synthese didn\'t return a 200 status')
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

    # New API using requests module.
    # TODO: migrate above code to use this new API.

    def get_sid2(self):
        # TODO: handle expired sid's properly.
        if self.sid2:
            return self.sid2

        r = self.call_action2('login', {
            'actionParamlogin': self.username,
            'actionParampwd': self.password
        })
        if 'sid' not in r.cookies:
            raise HTTPApiException('Authentication failure')
        self.sid2 = r.cookies['sid']
        return self.sid2

    def call_synthese2(self, params, send_sid=False, use_get=False):
        if send_sid:
            params['sid'] = self.get_sid2()

        # TODO: allow non-localhost?
        url = 'http://localhost:%s/synthese' % self.env.config.port
        log.debug('Calling %s with parameters: %s\n  url: %s', url, params,
            '{0}?{1}'.format(url, urllib.urlencode(params)))

        requests.defaults.max_retries = 1
        common_kwargs = {
            'stream': False,
        }
        if use_get:
            r = requests.get(url, params=params, **common_kwargs)
        else:
            r = requests.post(url, data=params, **common_kwargs)

        if r.status_code != 200:
            raise HTTPApiException('call_synthese didn\'t return a 200 status')

        # Workaround for https://github.com/kennethreitz/requests/issues/434
        # Remove once fixed.
        if not use_get:
            try:
                r.content
            except RuntimeError:
                log.warn('Workaround for '
                    'https://github.com/kennethreitz/requests/issues/434. '
                    'Assuming response is empty')
                r._content = ''

        return r

    def call_action2(self, action, params, send_sid=False, use_get=False):
        params['a'] = action
        params['nr'] = "1"
        return self.call_synthese2(params, send_sid, use_get)

    def call_service2(self, service, params, send_sid=False, use_get=False):
        params['SERVICE'] = service
        params['nr'] = "1"
        return self.call_synthese2(params, send_sid, use_get)

#!/usr/bin/env python
#
#    Simulates a Synthese server that just prints requests made to it.
#    @file dummy_synthese_server.py
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

import BaseHTTPServer
import SimpleHTTPServer

class HandlerWithPost(SimpleHTTPServer.SimpleHTTPRequestHandler):
    def do_GET(self):
        print 'Got GET %r' % (self.path,)
        self._dummy_response()

    def do_POST(self):
        data = self.rfile.read(int(self.headers['content-length']))
        print 'Got POST %r %r' % (self.path, data)
        self._dummy_response()

    def _dummy_response(self):
        self.send_response(200)
        self.end_headers()
        self.wfile.write('Dummy response\n')

def test(HandlerClass = HandlerWithPost,
         ServerClass = BaseHTTPServer.HTTPServer):
    BaseHTTPServer.test(HandlerClass, ServerClass)

if __name__ == '__main__':
    test()

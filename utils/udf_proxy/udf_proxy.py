#!/usr/bin/env python
#
#    Simple HTTP server for proxying requests to Synthese and avoid blocking
#    MySQL.
#    @file udf_proxy.py
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

import BaseHTTPServer
import logging
import logging.handlers
import optparse
import os
from os.path import join
import Queue
import SimpleHTTPServer
import sys
import threading
import urllib2

log = logging.getLogger(__name__)
queue = Queue.Queue()

LISTENING_PORT = None
TARGET_URL = None
TIMEOUT = 10


class Request(object):
    def __init__(self, method, path, data=None):
        self.method = method
        self.path = path
        self.data = data

    def __repr__(self):
        return "<Request %s>" % self.__dict__


class Dispatcher(object):
    def __init__(self):
        threading.Thread(target=self.loop).start()

    def loop(self):
        while True:
            request = queue.get()
            log.info("Dispatching request: %s (%i left)", request, queue.qsize())
            try:
                res = urllib2.urlopen(TARGET_URL + request.path, request.data, TIMEOUT)
            except urllib2.URLError, e:
                log.warn("Exception while dispatching request %s: %s", request, e)
                continue
            if res.code != 200:
                log.warn("Didn't get 200 code reply to request: %s", request)


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        self._handle_request()

    def do_POST(self):
        post_data = self.rfile.read(int(self.headers["content-length"]))
        self._handle_request(post_data)

    def _handle_request(self, post_data=None):
        log.info("Got request %s %s %s (queue size: %i)",
            self.command, self.path, post_data, queue.qsize())
        queue.put(Request(self.command, self.path, post_data))

        self.send_response(200)
        self.end_headers()
        self.wfile.write("Dummy response")

    def log_message(self, format, *args):
        if 1:
            BaseHTTPServer.BaseHTTPRequestHandler.log_message(self, format, *args)


def main():
    thisdir = os.path.abspath(os.path.dirname(__file__))

    log_file = join(thisdir, 'logs.txt')
    with open(log_file, 'wb') as f:
        f.write('')
    handler = logging.handlers.RotatingFileHandler(
        log_file, maxBytes=10 * 1024 * 1024, backupCount=3)
    handler.setFormatter(logging.Formatter(
        '%(asctime)s %(levelname)-10s %(message)s'))
    handler.setLevel(logging.INFO)
    log.addHandler(handler)

    dispatcher = Dispatcher()

    server_address = ('', LISTENING_PORT)
    httpd = BaseHTTPServer.HTTPServer(server_address, RequestHandler)
    httpd.serve_forever()


if __name__ == '__main__':
    usage = 'usage: %prog [options] MODULE_NAME SOURCE_FILENAME'
    parser = optparse.OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-p', '--port', type='int',
         default=9080, help='Proxy listening port')
    parser.add_option('-t', '--target-url',
         default='http://localhost:8080/synthese3',
         help='URL where to forward requests')

    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    LISTENING_PORT = options.port
    TARGET_URL = options.target_url

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))
    main()

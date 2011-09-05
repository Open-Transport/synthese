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
import time
import urllib2

thisdir = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, join(thisdir, 'third_party'))

import daemon

log = logging.getLogger(__name__)

# New requests will be dropped once the queue is full.
MAX_QUEUE_SIZE = 50000
queue = Queue.LifoQueue(MAX_QUEUE_SIZE)

LISTENING_PORT = None
TARGET_URL = None
TIMEOUT = 10
VERBOSE = False
LOG_PATH = None


class Request(object):
    def __init__(self, method, path, data=None):
        self.method = method
        self.path = path
        self.data = data

    def __repr__(self):
        return '<Request %s>' % self.__dict__


class Dispatcher(object):
    def __init__(self):
        self.stop = False
        threading.Thread(target=self.loop).start()

    def loop(self):
        while not self.stop:
            request = queue.get()
            log.info('Dispatching request: %s (%i left)', request, queue.qsize())
            if request == 'stop':
                log.info('Stop request, exiting dispatcher')
                break
            try:
                res = urllib2.urlopen(TARGET_URL + request.path, request.data, TIMEOUT)
            except urllib2.URLError, e:
                log.warn('Exception while dispatching request %s: %s', request, e)
                continue
            if res.code != 200:
                log.warn('Didn\'t get 200 code reply to request: %s', request)


class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(self):
        self._handle_request()

    def do_POST(self):
        post_data = self.rfile.read(int(self.headers['content-length']))
        self._handle_request(post_data)

    def _handle_request(self, post_data=None):
        log.info('Got request %s %s %s (queue size: %i)',
            self.command, self.path, post_data, queue.qsize())

        response = 'Dummy response\n'

        if self.path == '/status':
            response = 'Queue size: %i\n' % queue.qsize()
        else:
            if queue.full():
                log.warn("Queue is full, not saving request")
            else:
                queue.put(Request(self.command, self.path, post_data))

        self.send_response(200)
        self.end_headers()
        self.wfile.write(response)

    def log_message(self, format, *args):
        if VERBOSE:
            BaseHTTPServer.BaseHTTPRequestHandler.log_message(self, format, *args)

class StoppableHTTPServer(BaseHTTPServer.HTTPServer):
  """This is a specialization of of BaseHTTPServer to allow it
  to be exited cleanly (by setting its "stop" member to True).
  
  Stolen from http://src.chromium.org/git/chromium.git/net/tools/testserver/testserver.py
  """

  def serve_forever(self):
    self.stop = False
    while not self.stop:
        self.handle_request()
    self.socket.close()

def main():
    with open(LOG_PATH, 'wb') as f:
        f.write('')
    handler = logging.handlers.RotatingFileHandler(
        LOG_PATH, maxBytes=10 * 1024 * 1024, backupCount=3)
    handler.setFormatter(logging.Formatter(
        '%(asctime)s %(levelname)-10s %(message)s'))
    handler.setLevel(logging.INFO)
    log.addHandler(handler)
    # XXX why is this needed?
    log.setLevel(logging.DEBUG if VERBOSE else logging.INFO)

    dispatcher = Dispatcher()
    server_address = ('', LISTENING_PORT)
    httpd = StoppableHTTPServer(server_address, RequestHandler)
    threading.Thread(target=httpd.serve_forever).start()
    log.info('Dispatcher and http server started on port %i', LISTENING_PORT)
    try:
        while True:
            time.sleep(5)
    except KeyboardInterrupt, e:
        log.info('Keyboard interrupt, terminating...')
        httpd.stop = True
        dispatcher.stop = True
        try:
            urllib2.urlopen('http://localhost:%i' % LISTENING_PORT, None, 2)
        except Exception, e:
            pass
        while not queue.empty():
            queue.get_nowait()
        queue.put('stop')


class UDFProxyDaemon(daemon.Daemon):
    def run(self):
        main()


if __name__ == '__main__':
    usage = 'usage: %prog [options] start|stop|restart'
    parser = optparse.OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-n', '--no-daemon', action='store_true',
         default=False, help='Don\'t daemonize and print logs to stderr')
    parser.add_option('-p', '--port', type='int',
         default=9080, help='Proxy listening port')
    parser.add_option('-t', '--target-url',
         default='http://localhost:8080',
         help='URL where to forward requests')
    parser.add_option('--log-path',
        default=join(thisdir, 'logs.txt'),
        help='Location of log file')

    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.print_help()
        sys.exit(1)

    LISTENING_PORT = options.port
    TARGET_URL = options.target_url
    VERBOSE = options.verbose
    LOG_PATH = options.log_path

    if options.no_daemon:
        logging.basicConfig(level=logging.DEBUG if options.verbose else logging.INFO)
    command = args[0]

    if options.no_daemon or sys.platform == 'win32':
        if command != 'start':
            raise Exception('Without daemon, only start is allowed')
        main()
        sys.exit(0)

    pid_file = join(thisdir, 'udf_proxy.pid')
    daemon = UDFProxyDaemon(pid_file)
    if 'start' == command:
        daemon.start()
    elif 'stop' == command:
        daemon.stop()
    elif 'restart' == command:
        daemon.restart()
    else:
        print 'Unknown command'
        sys.exit(2)
    sys.exit(0)

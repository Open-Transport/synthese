#!/usr/bin/env python
#
#    Script to replay Synthese requests from a log file.
#    @file replay_logs.py
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

import datetime
import logging
import multiprocessing
import optparse
import Queue
import re
import sys
import time
from time import mktime
import urllib2


log = logging.getLogger(__name__)

METHOD_GET, METHOD_POST = range(2)
TYPE_REQU, TYPE_OTHER = range(2)


class Entry(object):
    REQUEST_RE = re.compile(
        'Received request : (.*) \(\d+ bytes\)(?: \+ \d+ bytes of POST data : (.*))?$')

    @staticmethod
    def from_line(line):
        try:
            level, stamp, thread, message = [p.strip() for p in line.split('#', 4)]
        except ValueError:
            # Can happen with the Spatialite message.
            return None

        e = Entry()

        levels = ['trace', 'debug', 'info', 'warn', 'fatal', 'none']
        level_name_to_int = dict(zip(levels, range(-1, len(levels))))
        e.level = level_name_to_int[level.lower()]

        date, time_nano = stamp.split()
        year, month, day = date.split('/')
        time, nano_s = time_nano.split('.')
        hour, minute, second = time.split(':')

        t = datetime.datetime(
            int(year), int(month), int(day),
            int(hour), int(minute), int(second), int(nano_s) / 1000)

        e.time = t
        e.unix_time = mktime(t.timetuple()) + t.microsecond / 1e6
        e.thread = thread
        e.message = message
        e.type = TYPE_OTHER

        m = Entry.REQUEST_RE.match(e.message)
        if m:
            e.type = TYPE_REQU

            e.path = m.group(1)
            e.post_data = m.group(2)
            e.method = METHOD_POST if e.post_data else METHOD_GET

        return e

    def __repr__(self):
        return '<Entry %s>' % self.__dict__


class LogParser(object):
    def __init__(self):
        self.entries = []

    def parse(self, log_path):
        for line in open(log_path):
            line = line.strip()
            e = Entry.from_line(line)
            if e:
                self.entries.append(e)


class ResponseInfo(object):
    def __init__(self, code, duration, size):
        self.code = code
        self.duration = duration
        self.size = size

    def __repr__(self):
        return '<ResponseInfo %s>' % self.__dict__


TIMEOUT = 60


def replay_entry(synthese_host, entry):
    url = 'http://%s%s' % (synthese_host, entry.path)
    start = time.time()
    content = ''
    try:
        response = urllib2.urlopen(url, entry.post_data, timeout=TIMEOUT)
        content = response.read()
        code = response.code
    except urllib2.HTTPError, e:
        code = e.code
    except urllib2.URLError, e:
        code = -1
    return ResponseInfo(code, time.time() - start, len(content))


class Replayer(object):
    def __init__(self, synthese_host, entries, replay_duration, stop_on_error,
        max_entries):
        self.synthese_host = synthese_host
        self.stop_on_error = stop_on_error

        self.entries = [e for e in entries if e.type == TYPE_REQU]
        if max_entries > 0:
            self.entries = self.entries[:max_entries]

        self.start_time = entries[0].unix_time
        self.end_time = entries[-1].unix_time
        self.entries_duration = self.end_time - self.start_time
        log.info('Entries count: %i, duration: %i s', len(self.entries), self.entries_duration)

        if replay_duration >= 0:
            self.replay_duration = replay_duration
        else:
            self.replay_duration = self.entries_duration

    def replay(self):
        now = time.time()
        time_ratio = self.replay_duration / self.entries_duration
        count = 0
        total_duration = 0
        total_size = 0

        # This should be at least the size of the Synthese thread count.
        POOL_SIZE = 10
        pool = multiprocessing.Pool(processes=POOL_SIZE)
        responses_queue = Queue.Queue()

        class Stats(object):
            total_duration = 0
            total_size = 0
            failure_count = 0
        stats = Stats()

        def process_queue():
            while not responses_queue.empty():
                response = responses_queue.get_nowait()
                if response.code != 200:
                    stats.failure_count += 1
                    log.warn('Found error: code=%s on entry: %s',
                        response.code, e)
                    if self.stop_on_error:
                        raise Exception('Found error: code=%s on entry: %s' % (
                            response.code, e))
                log.debug('Duration: %s', response.duration)
                stats.total_duration += response.duration
                stats.total_size += response.size
                return
                total_duration += response.duration
                total_size += response.size

        for e in self.entries:
            count += 1
            log.debug('Replaying entry %i/%i', count, len(self.entries))
            relative_target_time_offset = ((e.unix_time - self.start_time) /
                self.entries_duration)
            target_time_offset = relative_target_time_offset * self.replay_duration
            log.debug('target_time_offset: %s', target_time_offset)
            target_time = now + target_time_offset
            to_wait = target_time - time.time()
            log.debug('to wait: %s', to_wait)
            if to_wait > 0:
                time.sleep(to_wait)

            def done_callback(response_info):
                responses_queue.put(response_info)

            pool.apply_async(
                replay_entry, args=(self.synthese_host, e),
                callback=done_callback)

            process_queue()

        log.info('Finished replaying requests. Waiting for pending responses.')
        pool.close()
        pool.join()
        process_queue()

        log.info('Mean duration: %f ms', stats.total_duration / len(self.entries) * 1000)
        log.info('Mean size: %f kb', stats.total_size / len(self.entries) / 1024)
        log.info('Failure count (%i/%i) = %i%%',
            stats.failure_count / len(self.entries), len(self.entries),
            stats.failure_count / len(self.entries) * 100)


if __name__ == '__main__':
    usage = 'usage: %prog [options] LOG_FILE1 LOG_FILE2 ...'
    parser = optparse.OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-s', '--host', help='Synthese host',
        default='localhost:8080')
    parser.add_option('-i', '--info', action='store_true',
        help='Show requests info only')
    parser.add_option('-d', '--replay-duration',
        type='int', default=-1,
        help='Replay all entries during that time (seconds)')
    parser.add_option('-m', '--max-entries',
        type='int', default=-1,
        help='Max number of entries to replay')
    parser.add_option('-e', '--stop-on-error', action='store_true',
         default=False, help='Stop replay when a HTTP error is detected')

    (options, args) = parser.parse_args()
    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    log_files = args

    parser = LogParser()
    for log_file in log_files:
        parser.parse(log_file)

    replayer = Replayer(options.host, parser.entries, options.replay_duration,
        options.stop_on_error, options.max_entries)

    if options.info:
        sys.exit(0)

    replayer.replay()

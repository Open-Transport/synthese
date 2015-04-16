#    Class for running unit tests.
#    @file main.py
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


import codecs
import logging
import os
import os.path
import subprocess
import sys
import traceback

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest

import synthesepy.build
from synthesepy import db_backends
from synthesepy import utils


log = logging.getLogger(__name__)


class Tester(object):
    instance = None

    def __init__(self, env):
        self.env = env
        self.config = env.config
        Tester.instance = self

    def run_style_tests(self, suite_args):

        # TODO: not yet ready
        return True

        # Code based on http://svn.webkit.org/repository/webkit/trunk/Tools/Scripts/check-webkit-style

        thisdir = os.path.dirname(os.path.abspath(__file__))

        args = suite_args
        if not args:
            args = [os.path.abspath(
                os.path.join(thisdir, os.pardir, os.pardir, os.pardir)
            )]
        _log = log

        sys.path.append(os.path.join(thisdir, os.pardir, 'third_party'))

        from webkitpy.style_references import detect_checkout
        import webkitpy.style.checker as checker
        from webkitpy.style.patchreader import PatchReader
        from webkitpy.style.checker import StyleProcessor
        from webkitpy.style.filereader import TextFileReader
        from webkitpy.style.main import change_directory

        # Change stderr to write with replacement characters so we don't die
        # if we try to print something containing non-ASCII characters.
        stderr = codecs.StreamReaderWriter(sys.stderr,
                                           codecs.getreader('utf8'),
                                           codecs.getwriter('utf8'),
                                           'replace')
        # Setting an "encoding" attribute on the stream is necessary to
        # prevent the logging module from raising an error.  See
        # the checker.configure_logging() function for more information.
        stderr.encoding = "UTF-8"

        # FIXME: Change webkitpy.style so that we do not need to overwrite
        #        the global sys.stderr.  This involves updating the code to
        #        accept a stream parameter where necessary, and not calling
        #        sys.stderr explicitly anywhere.
        sys.stderr = stderr

        #SYNTH args = sys.argv[1:]

        # Checking for the verbose flag before calling check_webkit_style_parser()
        # lets us enable verbose logging earlier.
        is_verbose = "-v" in args or "--verbose" in args

        checker.configure_logging(stream=stderr, is_verbose=is_verbose)
        _log.debug("Verbose logging enabled.")

        parser = checker.check_webkit_style_parser()
        (paths, options) = parser.parse(args)

        checkout = detect_checkout()

        if checkout is None:
            if not paths:
                _log.error("WebKit checkout not found: You must run this script "
                           "from within a WebKit checkout if you are not passing "
                           "specific paths to check.")
                sys.exit(1)

            checkout_root = None
            _log.debug("WebKit checkout not found for current directory.")
        else:
            checkout_root = checkout.root_path()
            _log.debug("WebKit checkout found with root: %s" % checkout_root)

        configuration = checker.check_webkit_style_configuration(options)

        paths = change_directory(checkout_root=checkout_root, paths=paths)

        style_processor = StyleProcessor(configuration)

        file_reader = TextFileReader(style_processor)

        if paths and not options.diff_files:
            file_reader.process_paths(paths)
        else:
            changed_files = paths if options.diff_files else None
            patch = checkout.create_patch(options.git_commit, changed_files=changed_files)
            patch_checker = PatchReader(file_reader)
            patch_checker.check(patch)

        error_count = style_processor.error_count
        file_count = file_reader.file_count
        delete_only_file_count = file_reader.delete_only_file_count

        _log.info("Total errors found: %d in %d files"
                  % (error_count, file_count))
        # We fail when style errors are found or there are no checked files.
        #sys.exit(error_count > 0 or (file_count == 0 and delete_only_file_count == 0))
        return not (error_count > 0 or (file_count == 0 and delete_only_file_count == 0))

    def run_python_tests(self, suite_args):
        # Save data on our instance, for access from the tests.
        self.backends = []
        for conn_string in self.env.c.test_conn_strings:
            self.backends.append(db_backends.create_backend(
                self.env, conn_string))

        sys_argv = sys.argv[0:1]

        test_names = suite_args

        if test_names:
            sys_argv.append('-v')
            sys_argv.extend(test_names)
        else:
            thisdir = os.path.dirname(os.path.abspath(__file__))
            start_dir = os.path.join(thisdir, os.pardir)

            sys_argv.extend([
                'discover',
                '-p', '*_unittest.py',
                '-s', start_dir,
                '-v'
            ])

        test_prog = unittest.main(argv=sys_argv, module=None, exit=False)
        return test_prog.result.wasSuccessful()

    # put tests which cannot be repaired in this list
    KNOWN_FAILURES = set([
    ])

    def update_environment_for_cpp_tests(self):
        self.env.prepare_for_launch()

        # Setup environment variables used by tests.

        try:
            mysql_conn_string = [cs for cs in self.env.c.test_conn_strings if
                cs.startswith('mysql://')][0]
        except IndexError:
            mysql_conn_string = None

        if mysql_conn_string:
            mysql_ci = db_backends.ConnectionInfo(mysql_conn_string)
            db = mysql_ci['db']
            del mysql_ci['db']
            mysql_params = mysql_ci.conn_string.replace('mysql://', '')
            os.environ.update({
                'SYNTHESE_MYSQL_PARAMS': mysql_params,
                'SYNTHESE_MYSQL_DB': db,
            })

    def run_cpp_tests(self, suite_args):
        self.update_environment_for_cpp_tests()

        builder = synthesepy.build.get_builder(self.env)

        args = [builder.get_cmake_tool_path('ctest')]
        if self.env.c.verbose:
            args.append('-V')

        if self.KNOWN_FAILURES:
            failing_tests = '|'.join(['test_{0}_{1}Test'.format(path, test) for
                path, test in self.KNOWN_FAILURES])
            args.extend(['-E', failing_tests])
        utils.call(args, cwd=self.env.env_path)

        return True

    def run_tests(self, suites):
        log.debug('run_tests: %s', suites)

        ALL_SUITES = ['style', 'python', 'cpp']

        if not suites:
            suites = ALL_SUITES

        failed_suites = []

        # Add a debug=1 parameter to the connection strings. It helps detecting
        # issues going undetected in production.
        for i, conn_string in enumerate(self.env.c.test_conn_strings):
            ci = db_backends.ConnectionInfo(conn_string)
            ci['debug'] = '1'
            self.env.c.test_conn_strings[i] = ci.conn_string

        # This makes error messages easier to analyze (they will be at the end
        # of the output).
        STOP_ON_FIRST_FAIL = True

        for suite in suites:
            # Hack to allow passing a suite with a space in it. On Windows,
            # an argument with a space will be split in two separate arguments.
            suite = suite.replace('@', ' ')
            try:
                suite, suite_args = suite.split(':', 1)
            except ValueError:
                suite_args = ''
            suite_args = suite_args.split()
            if suite not in ALL_SUITES:
                raise Exception(
                    'Suite %s doesn\'t exist (available suites: %s)' % (
                    suite, ALL_SUITES))
            log.info('Running suite %s', suite)
            test_method = getattr(self, 'run_%s_tests' % suite)
            successful = False
            try:
                successful = test_method(suite_args)
            except Exception, e:
                log.warn('Suite failed: %s', e)
                traceback.print_exc()
            if not successful:
                failed_suites.append(suite)
                if STOP_ON_FIRST_FAIL:
                    raise Exception('Suite %s failed' % suite)

        if failed_suites:
            log.error('The following suites failed: %s' % failed_suites)
            raise Exception('Tests failed')

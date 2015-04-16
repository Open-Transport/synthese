"""
Unit test for the style fix script.
"""

__author__ = 'Sylvain Pasche <sylvain.pasche@gmail.com>'

import unittest

import fix_style


class TestFixStyle(unittest.TestCase):
    def assert_update(self, lines_before, expected_lines_after):
        lines_after = fix_style.fix_style_from_lines(lines_before)
        self.assertEqual(expected_lines_after, lines_after)

    def test_carriage_return(self):
        self.assert_update([
            'foo\n\r',
            'foo\r',
        ], [
            'foo\n',
            'foo\n',
        ])

    def test_end_of_line(self):
        self.assert_update([
            'int a; \n',
            'int b;    \n',
            'int c; \t \t \n',
        ], [
            'int a;\n',
            'int b;\n',
            'int c;\n',
        ])

    def test_tabs(self):
        # TODO: not implemented yet.
        return
        self.assert_update([
            '  int foo;\n',
            '  double blah;\n',
        ], [
            '\tint foo;\n',
            '\tdouble blah;\n',
        ])

    def test_newline_eol(self):
        self.assert_update([
            'int blah;\n',
            'int foo;',
        ], [
            'int blah;\n',
            'int foo;\n',
        ])

        self.assert_update([
            '',
        ], [
            '\n',
        ])

        self.assert_update([
        ], [
        ])


if __name__ == '__main__':
    unittest.main()



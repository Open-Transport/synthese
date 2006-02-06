#!/usr/bin/env python2.4
# -*- coding:iso8859-1 -*-

import string

conv = ""
filter = string.ascii_letters + string.digits + string.punctuation + ' '
accent = {
	"������": 'A',
	"��": 'C',
	"�����": 'E',
	"����": 'I',
	"�����": 'O',
	"����": 'U'
}

for c in range(256):
	final = '.'
	if chr(c) in filter:
		final = chr(c)
	for a in accent:
		if chr(c) in a:
			final = accent[a]
	conv += string.upper(final)
print conv

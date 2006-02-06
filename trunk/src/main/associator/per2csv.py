#!/usr/bin/env python2.4

import csv,string

output = csv.writer(file('gares.csv','wb'))
output.dialect.delimiter = ':'
#output.dialect.quoting = csv.QUOTE_NONNUMERIC
output.dialect.quotechar = '"'
output.dialect.skipinitialspace = True
output.dialect.lineterminator = '\n'

input = open('gares.per','r')
for line in input.readlines():
	if line[0]=='[':
		id='*'+str(int(line[2:].strip()))
	if line[:2]=='DE':
		fields = []
		for field in line[4:].strip().split('(')[:2]:
			fields.append(field.strip())
		fields.append('')
		fields.append(id)
		output.writerow(fields)

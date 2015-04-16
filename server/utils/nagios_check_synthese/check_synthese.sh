#!/bin/sh

## Nagios plug-in to check SYNTHESE.
## @file check_synthese.sh
## @author Hugues Romain
## @date 2009

##	This file belongs to the SYNTHESE project (public transportation specialized software)
##	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
##
##	This program is free software; you can redistribute it and/or
##	modify it under the terms of the GNU General Public License
##	as published by the Free Software Foundation; either version 2
##	of the License, or (at your option) any later version.
##
##	This program is distributed in the hope that it will be useful,
##	but WITHOUT ANY WARRANTY; without even the implied warranty of
##	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##	GNU General Public License for more details.
##
##	You should have received a copy of the GNU General Public License
##	along with this program; if not, write to the Free Software
##	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

i=1
wget -q -O - $1 | (
while read line
do
	if [[ $i = 2 ]]
	then
		echo $line
	fi
	if [[ $i = 1 ]]
	then
		ret=$line
	fi
	i=$((i+1))
done

if [ $i = 3 ]
then
	exit $ret
else
	echo "SYNTHESE Server Unreachable"
	exit 3
fi
)


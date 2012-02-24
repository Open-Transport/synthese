#    Ineo realtime debug page.
#    @file ineo.py
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
import pprint

import flask
from flask import render_template, request, session, url_for
import werkzeug

from synthesepy import db_backends
from . import manager

def _build_service_list(project, jour, template_args):
    config = project.config
    db = config.ineo_db

    limit = int(request.args.get('limit', 500))
    offset = int(request.args.get('offset', 0))

    services = []
    for service in db.query("""SELECT *, l.nom as ligne_nom from COURSE c
            JOIN LIGNE l on l.ref = c.ligne
            LIMIT {limit}
            OFFSET {offset}
        """.format(**locals())):
        service['link'] = '<a href="{0}">Service {1}</a>'.format(
            url_for('.ineo', service_id=service['ref']), service['ref'])
        services.append(service)
    template_args['services'] = (services, ('ref', 'link', 'sv', 'ligne', 'ligne_nom'))

    template_args['next_page'] = url_for('.ineo', offset=offset + limit)
    template_args['services_count'] = db.query("SELECT count(1) as c from COURSE", one=True)['c']


def _build_ineo_tables(project, jour, service_id, template_args):
    config = project.config
    db = config.ineo_db

    # Ineo data

    deviation = ''
    if config.ineo_ver >= 1.07:
        deviation = 'HORAIRE.deviation,'

    query = """SELECT
        ARRET.nom arretNom,
        ARRET.mnemoc,
        ARRETCHN.pos,
        HORAIRE.hra,
        HORAIRE.hrd,
        HORAIRE.type,
        {deviation}
        LIGNE.mnemo ligneNom,
        COURSE.chainage
    FROM COURSE, ARRETCHN, HORAIRE, ARRET, LIGNE
    WHERE
        COURSE.ref={service_id} AND COURSE.jour='{jour}' AND
        HORAIRE.course=COURSE.ref AND HORAIRE.jour=COURSE.jour AND
        ARRETCHN.chainage=COURSE.chainage AND HORAIRE.arretchn=ARRETCHN.ref AND ARRETCHN.jour=HORAIRE.jour AND
        ARRET.ref=ARRETCHN.arret AND ARRET.jour=ARRETCHN.jour AND
        LIGNE.ref=COURSE.ligne AND LIGNE.jour=COURSE.jour
    ORDER BY
        ARRETCHN.pos
    """.format(**locals())

    result = db.query(query)
    if not result:
        return 'Can\'d find any COURSE with given service_id'
    template_args['line_name'] = result[0]['ligneNom']
    template_args['chainage'] = result[0]['chainage']
    template_args['ineo_result'] = (
        result,
        ('mnemoc', 'arretNom', 'pos', 'hra', 'hrd', 'type', 'deviation'))

    # Synthese data

    services = project.db_backend.query("""SELECT
        *
    FROM t016_scheduled_services
    WHERE
        datasource_links='{config.ineo_realtime_data_source}|{service_id}'
    LIMIT 10
    """.format(**locals()))
    pprint.pprint(services)

    stops_tables = []
    services_tables = []
    for service in services:
        schedules = service['schedules'].split(',')
        line_id = service['path_id']
        stops = project.db_backend.query("""SELECT
            t012_physical_stops.name,
            t010_line_stops.schedule_input,
            t010_line_stops.metric_offset
        FROM t010_line_stops, t012_physical_stops
        WHERE
            line_id={line_id} AND
            t012_physical_stops.id=t010_line_stops.physical_stop_id
        """.format(**locals()))

        i = 0
        for stop in stops:
            if stop['schedule_input']:
                stop['schedule'] = schedules[i]
                i += 1

        stops_tables.append((stops, ('name', 'metric_offset', 'schedule')))

    template_args['stops_tables'] = stops_tables

    services_tables.append((services, ('id', 'path_id', 'schedules')))
    template_args['services_tables'] = services_tables

@manager.route('/ineo/', defaults={'service_id': -1})
@manager.route('/ineo/<service_id>')
def ineo(service_id):
    if not session.get('logged_in'):
        return redirect(url_for('.login', next=url_for('.ineo')))
    project = flask.current_app.project

    config = project.config
    db = config.ineo_db

    template_args = {}

    jour = datetime.date.today().strftime('%Y-%m-%d')
    if 'jour' in request.args:
        jour = request.args['jour']
    template_args['jour'] = jour

    if service_id >= 0:
        _build_ineo_tables(project, jour, service_id, template_args)
    else:
        _build_service_list(project, jour, template_args)

    return render_template('ineo.html', **template_args)

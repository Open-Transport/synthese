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
from functools import wraps
import itertools
import pprint

import flask
from flask import render_template, request, session, url_for
import werkzeug

from synthesepy import db_backends
from . import manager


def login_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if not session.get('logged_in'):
            return redirect(url_for('.login', next=request.url))
        return f(*args, **kwargs)
    return decorated_function


def _get_jour():
    jour = datetime.date.today().strftime('%Y-%m-%d')
    if 'jour' in request.args:
        jour = request.args['jour']
    return jour


@manager.route('/ineo/routes')
@login_required
def ineo_routes():
    project = flask.current_app.project
    config = project.config
    db = config.ineo_db

    jour = _get_jour()

    query = """SELECT
        l.ref as ligne_ref,
        l.mnemo as ligne_mnemo,
        l.nom as ligne_nom,
        d.nom as dest_nom,
        ch.ref as chainage_ref,
        ch.sens as chainage_sens,
        ch.nom as chainage_nom
    FROM CHAINAGE ch
        JOIN LIGNE l ON l.ref = ch.ligne and l.jour = '{jour}'
        JOIN DEST d ON d.ref = ch.dest and d.jour = '{jour}'
    WHERE
        ch.jour = '{jour}' AND ch.type ='C'
    ORDER BY
        l.mnemo, ch.sens, ch.nom
    """.format(**locals())

    result = db.query(query)

    for route in result:
        route['chainage_nom'] = u'<a href={0}>{1}</a>'.format(
            url_for('.ineo_route', route_id=route['chainage_ref'], jour=jour),
            route['chainage_nom'])

    routes = (
        result,
        ('ligne_ref', 'ligne_mnemo', 'ligne_nom', 'dest_nom', 'chainage_ref',
         'chainage_sens', 'chainage_nom'))

    return render_template('ineo/routes.html', **locals())


@manager.route('/ineo/route/<route_id>')
@login_required
def ineo_route(route_id):
    project = flask.current_app.project
    config = project.config
    db = config.ineo_db

    jour = _get_jour()

    result = db.query("""SELECT
        ch.ref,
        ch.nom as chainage_nom,
        l.ref as ligne_ref,
        l.nom as ligne_nom
    FROM
        CHAINAGE ch
        JOIN LIGNE l ON l.ref = ch.ligne and l.jour = '{jour}'
    WHERE
        ch.ref = {route_id} AND ch.jour = '{jour}'
    """.format(**locals()), one=True)
    chainage_nom = result['chainage_nom']
    ligne_nom = result['ligne_nom']

    query = """SELECT
        c.ref as course_ref,
        h.htd, ach.pos,
        a.ref as arret_ref, a.nom as arret_nom
    FROM
        CHAINAGE ch
        JOIN COURSE c ON c.chainage = ch.ref AND c.jour = '{jour}'
        JOIN HORAIRE h ON h.course = c.ref AND h.jour = '{jour}'
        JOIN ARRETCHN ach ON ach.ref = h.arretchn AND ach.jour = '{jour}'
        JOIN ARRET a ON a.ref = ach.arret AND a.jour = '{jour}'
    WHERE
        ch.ref = {route_id} AND ch.jour = '{jour}'
    ORDER BY
        c.ref, ach.pos
    """.format(**locals())


    result = db.query(query)

    def service_grouper(service):
        KEEP_KEYS = set(['course_ref'])
        key = dict((k, v) for (k, v) in service.iteritems() if k in KEEP_KEYS)
        return key
    services = []
    for service, horaires in itertools.groupby(result, service_grouper):
        horaires = list(horaires)
        begin = horaires[0]
        end = horaires[-1]
        service['beg_htd'] = begin['htd']
        service['end_htd'] = end['htd']
        service['course_ref'] = '<a href={0}>{1}</a>'.format(
            url_for('.ineo_service_detail', service_id=service['course_ref'], jour=jour),
            service['course_ref'])

        services.append(service)

    services.sort(key=lambda s: s['beg_htd'])

    services = (
        services,
        ('course_ref', 'beg_htd', 'end_htd'))

    return render_template('ineo/route.html', **locals())


@manager.route('/ineo/service/<service_id>')
@login_required
def ineo_service_detail(service_id):
    project = flask.current_app.project
    config = project.config
    db = config.ineo_db

    jour = _get_jour()

    # Ineo data

    deviation = ''
    if config.ineo_ver >= 1.07:
        deviation = 'h.deviation,'

    query = """SELECT
        a.nom arret_nom,
        a.mnemoc,
        ch.nom as chainage_nom,
        ach.pos,
        h.hta,
        h.htd,
        h.hra,
        h.hrd,
        h.type,
        {deviation}
        l.mnemo ligne_nom,
        c.chainage
    FROM
        COURSE c
        JOIN CHAINAGE ch ON ch.ref = c.chainage AND ch.jour = '{jour}'
        JOIN LIGNE l ON l.ref = c.ligne AND l.jour = '{jour}'
        JOIN HORAIRE h ON h.course = c.ref AND h.jour = '{jour}'
        JOIN ARRETCHN ach ON ach.ref = h.arretchn AND ach.jour = '{jour}'
        JOIN ARRET a ON a.ref = ach.arret AND a.jour = '{jour}'
    WHERE
        c.ref = {service_id} AND c.jour = '{jour}'
    ORDER BY
        ach.pos
    """.format(**locals())

    result = db.query(query)
    if not result:
        flask.abort(404)
    line_name = result[0]['ligne_nom']
    chainage = result[0]['chainage']
    chainage_nom = result[0]['chainage_nom']
    ineo_result = (
        result,
        ('mnemoc', 'arret_nom', 'pos', 'hta', 'htd', 'hra', 'hrd', 'type', 'deviation'))

    # Synthese data

    services = project.db_backend.query("""SELECT
        *
    FROM t016_scheduled_services
    WHERE
        datasource_links='{config.ineo_realtime_data_source}|{service_id}'
    LIMIT 10
    """.format(**locals()))

    stops_tables = []
    services_tables = []
    for service in services:
        schedules = service['schedules'].split(',')
        line_id = service['path_id']
        stops = project.db_backend.query("""SELECT
            t012_physical_stops.name,
            t010_line_stops.schedule_input,
            t010_line_stops.metric_offset
        FROM
            t010_line_stops, t012_physical_stops
        WHERE
            line_id = {line_id} AND
            t012_physical_stops.id = t010_line_stops.physical_stop_id
        """.format(**locals()))

        i = 0
        for stop in stops:
            if stop['schedule_input']:
                stop['schedule'] = schedules[i]
                i += 1

        stops_tables.append((stops, ('name', 'metric_offset', 'schedule')))

    stops_tables = stops_tables

    services_tables.append((services, ('id', 'path_id', 'schedules')))
    services_tables = services_tables

    return render_template('ineo/service_detail.html', **locals())

#!/usr/bin/python2

import re
import sys
import argparse
import pyspatialite.dbapi2 as sqlite3


parser = argparse.ArgumentParser()
parser.add_argument("db_file", help="SQLite db file to minify")
parser.add_argument("uid", nargs='*', help="UID to keep in minified db")
parser.add_argument("-g", "--discard-geometry",
                    action="store_true",
                    help="Remove Spatialite related geometry columns, tables and indexes")
parser.add_argument("-o", "--obfuscate",
                    action="store_true",
                    help="Obfuscate human readable fields")
args = parser.parse_args()

all_uids = {}
db_file = args.db_file
uids = args.uid

conn = sqlite3.connect(db_file)
conn.row_factory = sqlite3.Row


class TableInfo:
    def __init__(self, table_name, single_foreign_key_columns = [], multiple_foreign_keys_columns = [], obfuscation_map = {}):
        self.table_name = table_name
        self.single_foreign_key_columns = single_foreign_key_columns
        self.multiple_foreign_keys_columns = multiple_foreign_keys_columns
        self.obfuscation_map = obfuscation_map
    


def append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map):
    table_id = int(table_name[1:4])
    table_infos[table_id]  = TableInfo(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    

cursor = conn.cursor()

cursor.execute("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;")
table_names = []
for row in cursor.fetchall():
    table_names.append(row['name'])


def collected_table_uids(table_id):
    if table_id not in all_uids.keys():
        all_uids[table_id] = []
    return all_uids[table_id]    
    
    
def decode_table_id(uid):
    table_id = (int(uid) & 0xFFFF000000000000) >> 48
    return table_id
        
def collect_uid(uid):
    table_id = decode_table_id(uid)
    uids = collected_table_uids(table_id)
    if uid in uids:
        return
    print("Collected UID %s from table %s" % (uid, table_infos[table_id].table_name))
    uids.append(uid)    
    collect_foreign_uids(uid)
    if table_id == 9:
        collect_line_stops_for_line(uid)

def collect_all_table_uids(table_id):
    cursor.execute("SELECT id FROM %s" % table_infos[table_id].table_name)
    for row in cursor.fetchall():
        collect_uid(row['id'])

def collect_foreign_uids(uid):
    table_id = decode_table_id(uid)
    if table_id not in table_infos.keys():
        raise Exception("Unhandled table %s" % table_id)
    table_info = table_infos[table_id]
    row = cursor.execute("select * from %s where id = %s" % (table_info.table_name, uid)).fetchone()
    for single_foreign_key_column in table_info.single_foreign_key_columns:
        collect_single_foreign_uid_from_row(row, single_foreign_key_column)
    for multiple_foreign_keys_column in table_info.multiple_foreign_keys_columns:
        collect_multiple_foreign_uids_from_row(row, multiple_foreign_keys_column)

    
def collect_single_foreign_uid_from_row(row, column_name):
    column_value = row[column_name]
    if not column_value:
        return
    collect_uid(column_value)
    
def collect_multiple_foreign_uids_from_row(row, column_name):
    column_value = row[column_name].strip()
    if not column_value:
        return
    foreign_uids = column_value.split(",")
    for foreign_uid in foreign_uids:
        collect_uid(foreign_uid)
        
def collect_line_stops_for_line(line_uid):
    for row in cursor.execute("select id from t010_line_stops where line_id = %s" % line_uid).fetchall():
        collect_uid(row['id'])


def obfuscate_table_col(table_name, table_col, col_value_prefix):
    sql = 'update %s set %s="%s" || (select count(_%s.id) from %s as _%s where _%s.id < %s.id)' % (table_name, table_col, col_value_prefix, table_name, table_name, table_name, table_name, table_name)
    cursor.execute(sql)
        
        
def clear_all_but_collected_uids():
    for table_name in table_names:
        if re.match("^t[0-9]{3}_.*", table_name):
            clear_table_but_collected_uids(table_name)



def obfuscate_all():
    for key, table_info in table_infos.iteritems():
        print("Obfuscating table %s" % table_info.table_name)
        for column_name, obfuscation_prefix in table_info.obfuscation_map.iteritems():
            obfuscate_table_col(table_info.table_name, column_name, obfuscation_prefix)
            
        
def clear_table_but_collected_uids(table_name):
    table_id = int(table_name[1:4])
    uids = collected_table_uids(table_id)
    uids_str = ""
    for uid in uids:
        uids_str += str(uid)
        uids_str += ","
    uids_str = uids_str[:-1]
    sql = "delete from %s where id not in (%s)" % (table_name, uids_str)
    cursor.execute(sql)
    print("Minified table %s" % table_name)


def remove_geometry():
    for table_id in [7, 10, 12, 13, 14, 43, 72, 73, 78]:
        remove_table_geometry(table_infos[table_id].table_name)
    sql = "drop view geom_cols_ref_sys"
    cursor.execute(sql)
    sql = "drop table geometry_columns"
    cursor.execute(sql)
    sql = "drop table geometry_columns_auth"
    cursor.execute(sql)
    sql = "drop table spatial_ref_sys"
    cursor.execute(sql)
    sql = "drop table spatialite_history"
    cursor.execute(sql)
    sql = "drop table views_geometry_columns"
    cursor.execute(sql)
    sql = "drop table virts_geometry_columns"
    cursor.execute(sql)
    
def remove_table_geometry(table_name):
    sql = "select DiscardGeometryColumn('%s', 'geometry')" % table_name
    cursor.execute(sql)
    sql = "update %s set geometry = NULL" % table_name
    cursor.execute(sql)
    sql = "drop table idx_%s_geometry" % table_name
    cursor.execute(sql)
    
    print("Discarded geometry for table %s" % table_name)


def drop_table(table_name):
    sql = "drop table %s" % table_name
    print(sql)
    cursor.execute(sql)

def minify_db(db_file, uids):
    if args.discard_geometry:
        remove_geometry()
    for uid in uids:
        if len(uid) <= 3:
            collect_all_table_uids(int(uid))
        else:
            collect_uid(uid)
    clear_all_but_collected_uids()
    if args.obfuscate:
        obfuscate_all()
    cursor.execute("vacuum")
    cursor.execute("reindex")
    conn.commit()

table_infos = {}

# ===========================================================
table_name = "t001_object_site_links"
table_single_foreign_key_columns = ["object_id", "site_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t003_alarms"
table_single_foreign_key_columns = ["scenario_id", "template_id", "messages_section_id", "calendar_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t006_cities"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name' : 'city_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t007_connection_places"
table_single_foreign_key_columns = ["city_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name' : 'connection_place_', 'timetable_name' : 'timetable_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t008_fares"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t009_lines"
table_single_foreign_key_columns = ["commercial_line_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t010_line_stops"
table_single_foreign_key_columns = ["physical_stop_id", "line_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t011_place_aliases"
table_single_foreign_key_columns = ["aliased_place_id", "city_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t012_physical_stops"
table_single_foreign_key_columns = ["place_id", "projected_road_chunk_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'physical_stop_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t013_public_places"
table_single_foreign_key_columns = ["city_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'public_place_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t014_road_chunks"
table_single_foreign_key_columns = ["road_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t015_roads"
table_single_foreign_key_columns = ["road_place_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t016_scheduled_services"
table_single_foreign_key_columns = ["path_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t017_continuous_services"
table_single_foreign_key_columns = ["path_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t021_reservation_contacts"
table_single_foreign_key_columns = ["path_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t022_transport_networks"
table_single_foreign_key_columns = ["days_calendars_parent_id", "periods_calendars_parent_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t025_sites"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'site_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t026_users"
table_single_foreign_key_columns = ["city_id", "profile_id", "creator_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t027_profiles"
table_single_foreign_key_columns = ["parent"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t036_display_types"
table_single_foreign_key_columns = ["display_main_page_id", "display_row_page_id", "display_destination_page_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'display_type_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t039_scenarios"
table_single_foreign_key_columns = ["folder_id", "template_id"]
table_multiple_foreign_keys_columns = ["messages_section_ids"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t040_alarm_object_links"
table_single_foreign_key_columns = ["object_id", "alarm_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t041_display_screens"
table_single_foreign_key_columns = ["broadcast_point_id", "type_id"]
table_multiple_foreign_keys_columns = ["displayed_places_ids"]
table_obfuscation_map = { 'broadcast_point_comment': 'comment' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t042_commercial_lines"
table_single_foreign_key_columns = ["network_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'commercial_line_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t043_crossings"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t044_reservations"
table_single_foreign_key_columns = ["transaction_id", "line_id", "service_id", "departure_place_id", "arrival_place_id", "reservation_rule_id", "vehicle_id", "vehicle_position_id_at_departure", "vehicle_position_id_at_arrival", "acknowledge_user_id", "cancellation_acknowledge_user_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t045_log_entries"
table_single_foreign_key_columns = ["user_id", "object_id", "object2_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t046_reservation_transactions"
table_single_foreign_key_columns = ["last_reservation_id", "customer_id", "booking_user_id", "cancel_user_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t047_online_reservation_rules"
table_single_foreign_key_columns = ["reservation_rule_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t048_user_favorite_journey"
table_single_foreign_key_columns = ["user_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t049_rolling_stock"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t051_scenario_folder"
table_single_foreign_key_columns = ["parent_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t052_timetables"
table_single_foreign_key_columns = ["book_id", "calendar_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t053_timetable_rows"
table_single_foreign_key_columns = ["timetable_id", "place_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t054_calendar_templates"
table_single_foreign_key_columns = ["parent_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'calendar_template_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t055_calendar_template_elements"
table_single_foreign_key_columns = ["calendar_id", "include_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t056_non_concurrency_rules"
table_single_foreign_key_columns = ["priority_line_id", "hidden_line_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t057_display_monitoring_status"
table_single_foreign_key_columns = ["screen_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t058_display_screen_cpu"
table_single_foreign_key_columns = ["place_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t059_data_sources"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'data_source_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t060_road_places"
table_single_foreign_key_columns = ["city_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t061_pt_use_rules"
table_single_foreign_key_columns = ["default_fare_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t062_rolling_stock_filters"
table_single_foreign_key_columns = ["site_id"]
table_multiple_foreign_keys_columns = ["rolling_stock_ids"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t063_web_pages"
table_single_foreign_key_columns = ["site_id", "up_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'title' : 'title_'}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t065_city_aliases"
table_single_foreign_key_columns = ["aliased_city_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t066_junctions"
table_single_foreign_key_columns = ["start_physical_stop", "end_physical_stop"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t069_vehicles"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = ["allowed_lines"]
table_obfuscation_map = { 'name': 'vehicle_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t070_compositions"
table_single_foreign_key_columns = ["service_id", "vehicle_service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t071_drt_areas"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t072_vehicle_positions"
table_single_foreign_key_columns = ["vehicle_id", "service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t073_depots"
table_single_foreign_key_columns = ["vehicle_id", "service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { 'name': 'depot_' }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t074_destinations"
table_single_foreign_key_columns = ["vehicle_id", "service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t076_service_calendar_links"
table_single_foreign_key_columns = ["service_id", "calendar_id", "calendar2_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t077_vehicle_services"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t078_houses"
table_single_foreign_key_columns = ["road_place_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t080_dead_runs"
table_single_foreign_key_columns = ["network_id", "depot_id", "stop_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t081_driver_services"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t082_free_drt_areas"
table_single_foreign_key_columns = ["commercial_line_id", "transport_mode_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t083_free_drt_time_slots"
table_single_foreign_key_columns = ["area_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t084_public_place_entrances"
table_single_foreign_key_columns = ["public_place_id", "road_chunk_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t085_driver_allocations"
table_single_foreign_key_columns = ["driver_allocation_template_id", "driver_id", "driver_activity_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t086_pt_services_configurations"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t087_driver_activites"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t088_driver_allocation_templates"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = ["driver_service_ids"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t089_service_quotas"
table_single_foreign_key_columns = ["service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t092_tree_folders"
table_single_foreign_key_columns = ["parent_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = {}
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t093_inter_synthese_slaves"
table_single_foreign_key_columns = ["passive_mode_import_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t094_inter_synthese_configs"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t095_inter_synthese_config_items"
table_single_foreign_key_columns = ["config_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t096_inter_synthese_queue"
table_single_foreign_key_columns = ["slave_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t097_vdv_servers"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t098_vdv_server_subscriptions"
table_single_foreign_key_columns = ["vdv_server_id", "stop_area_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t099_vdv_clients"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = ["transport_network_ids"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t101_message_types"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t102_message_alternatives"
table_single_foreign_key_columns = ["alarm_id", "message_type_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t103_website_configs"
table_single_foreign_key_columns = ["website_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t104_message_application_periods"
table_single_foreign_key_columns = ["scenario_calendar_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t105_imports"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t106_custom_broadcast_points"
table_single_foreign_key_columns = ["root_id", "up_id", "message_type_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t107_notification_providers"
table_single_foreign_key_columns = ["begin_message_type_id", "end_message_type_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t108_notification_events"
table_single_foreign_key_columns = ["alarm_id", "notification_provider_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t109_messages_sections"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t110_scenario_calendars"
table_single_foreign_key_columns = ["scenario_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t111_inter_synthese_packages"
table_single_foreign_key_columns = ["import_id", "lock_user_id"]
table_multiple_foreign_keys_columns = ["object_ids"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t115_exports"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t116_operation_units"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = ["allowed_lines"]
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t117_vehicle_service_usages"
table_single_foreign_key_columns = ["vehicle_service_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t118_descents"
table_single_foreign_key_columns = ["service_id", "stop_id", "activation_user_id", "cancellation_user_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t118_descents"
table_single_foreign_key_columns = ["service_id", "stop_id", "activation_user_id", "cancellation_user_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)

# ===========================================================
table_name = "t119_vehicle_calls"
table_single_foreign_key_columns = ["vehicle_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    
# ===========================================================
table_name = "t120_settings"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    
# ===========================================================
table_name = "t122_media_libraries"
table_single_foreign_key_columns = ["website_id"]
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    
# ===========================================================
table_name = "t123_message_tags"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    
# ===========================================================
table_name = "t999_config"
table_single_foreign_key_columns = []
table_multiple_foreign_keys_columns = []
table_obfuscation_map = { }
append_table_info(table_name, table_single_foreign_key_columns, table_multiple_foreign_keys_columns, table_obfuscation_map)
    
minify_db(db_file, uids)


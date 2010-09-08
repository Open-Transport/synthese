BEGIN TRANSACTION;
CREATE TABLE t000_environments (id INTEGER UNIQUE PRIMARY KEY);
INSERT INTO t000_environments VALUES(1);
CREATE TABLE t001_environment_links (id INTEGER UNIQUE PRIMARY KEY, environment_id INTEGER, link_target_id INTEGER);
CREATE TABLE t002_addresses (id INTEGER UNIQUE PRIMARY KEY, place_id INTEGER, road_id INTEGER, metric_offset DOUBLE, x DOUBLE, y DOUBLE);
CREATE TABLE t003_alarms (id INTEGER UNIQUE PRIMARY KEY, message TEXT, period_start TIMESTAMP, period_end TIMESTAMP, level INTEGER);
CREATE TABLE t004_axes (id INTEGER UNIQUE PRIMARY KEY, name TEXT, free BOOLEAN, allowed BOOLEAN);
CREATE TABLE t005_service_dates (id INTEGER UNIQUE PRIMARY KEY, service_id INTEGER, date DATE);
CREATE TABLE t006_cities (id INTEGER UNIQUE PRIMARY KEY, name TEXT);
CREATE TABLE t007_connection_places (id INTEGER UNIQUE PRIMARY KEY, name TEXT, city_id INTEGER, connection_type INTEGER, is_city_main_connection BOOLEAN, default_transfer_delay INTEGER, transfer_delays TEXT, alarm_id INTEGER);
CREATE TABLE t008_fares (id INTEGER UNIQUE PRIMARY KEY, name TEXT, fare_type INTEGER);
CREATE TABLE t009_lines (id INTEGER UNIQUE PRIMARY KEY, transport_network_id INTEGER, axis_id INTEGER, name TEXT, short_name TEXT, long_name TEXT, color TEXT, style TEXT, image TEXT, timetable_name TEXT, direction TEXT, is_walking_line BOOLEAN, use_in_departure_boards BOOLEAN, use_in_timetables BOOLEAN, use_in_routeplanning BOOLEAN, rolling_stock_id INTEGER, fare_id INTEGER, alarm_id INTEGER, bike_compliance_id INTEGER, handicapped_compliance_id INTEGER, pedestrian_compliance_id INTEGER, reservation_rule_id INTEGER);
CREATE TABLE t010_line_stops (id INTEGER UNIQUE PRIMARY KEY, physical_stop_id INTEGER, line_id INTEGER, rank_in_path INTEGER, is_departure BOOLEAN, is_arrival BOOLEAN, metric_offset DOUBLE, via_points TEXT);
CREATE TABLE t011_place_aliases (id INTEGER UNIQUE PRIMARY KEY, name TEXT, aliased_place_id INTEGER, city_id INTEGER, is_city_main_connection BOOLEAN);
CREATE TABLE t012_physical_stops (id INTEGER UNIQUE PRIMARY KEY, name TEXT, place_id INTEGER, x DOUBLE, y DOUBLE);
CREATE TABLE t013_public_places (id INTEGER UNIQUE PRIMARY KEY, name TEXT, city_id INTEGER);
CREATE TABLE t014_road_chunks (id INTEGER UNIQUE PRIMARY KEY, address_id INTEGER, rank_in_path INTEGER, is_departure BOOLEAN, is_arrival BOOLEAN, via_points TEXT);
CREATE TABLE t015_roads (id INTEGER UNIQUE PRIMARY KEY, name TEXT, city_id INTEGER, road_type INTEGER, fare_id INTEGER, alarm_id INTEGER, bike_compliance_id INTEGER, handicapped_compliance_id INTEGER, pedestrian_compliance_id INTEGER, reservation_rule_id INTEGER, via_points TEXT);
CREATE TABLE t016_scheduled_services (id INTEGER UNIQUE PRIMARY KEY, service_number TEXT, schedules TEXT, path_id INTEGER, bike_compliance_id INTEGER, handicapped_compliance_id INTEGER, pedestrian_compliance_id INTEGER, reservation_rule_id INTEGER);
CREATE TABLE t017_continuous_services (id INTEGER UNIQUE PRIMARY KEY, service_number TEXT, schedules TEXT, path_id INTEGER, range INTEGER, max_waiting_time INTEGER, bike_compliance_id INTEGER, handicapped_compliance_id INTEGER, pedestrian_compliance_id INTEGER);
CREATE TABLE t018_pedestrian_compliances (id INTEGER UNIQUE PRIMARY KEY, status INTEGER, capacity INTEGER);
CREATE TABLE t019_handicapped_compliances (id INTEGER UNIQUE PRIMARY KEY, status INTEGER, capacity INTEGER);
CREATE TABLE t020_bike_compliances (id INTEGER UNIQUE PRIMARY KEY, status INTEGER, capacity INTEGER);
CREATE TABLE t021_reservation_rules (id INTEGER UNIQUE PRIMARY KEY, reservation_type INTEGER, online BOOLEAN, origin_is_reference BOOLEAN, min_delay_minutes INTEGER, min_delay_days INTEGER, max_delay_days INTEGER, hour_deadline TIME, phone_exchange_number TEXT, phone_exchange_opening_hours TEXT, description TEXT, web_site_url TEXT);
CREATE TABLE t022_transport_networks (id INTEGER UNIQUE PRIMARY KEY, name TEXT);
CREATE TABLE t023_interface_pages (id INTEGER UNIQUE PRIMARY KEY, interface_id INTEGER, page_code TEXT, content TEXT);
INSERT INTO t023_interface_pages VALUES(135461354,1,'simplelogin','print {Bienvenue sur l''écran de login !}
print {{htmlform login admin rub=home login {}}}
print {{loginhtmlfield}}
print {{passwordhtmlfield}}
print {<input type="submit" value="ok" />}
print </form>
print {Fin de page}');
INSERT INTO t023_interface_pages VALUES(184464073709551516,1,'admin','print {<html><head><title>}

print {</title></head><body>}
print {<table cellSpacing="0" cellPadding="1" border="1" borderColor="#000066">}
print {<TR><TD width="259" bgColor="#000066" colSpan="1" height="21" rowSpan="1"><FONT face="Verdana" size="1"><FONT color="#ffffff">}

print {USERNAMESURNAME}

print {</FONT>}

print {{logouthtmllink {} simplelogin {D&eacute;connecter}}}

print {</TD><TD bgColor="#000066" height="21"><FONT size="2"><FONT face="Verdana" color="gold">}

print {{adminpos}}

print {</FONT></TD>}
print {</TR><TR><TD vAlign="top" width="259">}

print {{admintree}}

print {</TD><TD vAlign="top"><FONT face="Verdana"><FONT size="2">}

admincontent

print {</FONT></TD>}
print {</tr></table></body></html>}
');
INSERT INTO t023_interface_pages VALUES(184464073709551517,1,'redir','print {<html><script type=""text/javascript"">window.location.replace(''}
print {{param 0}}
print {'');</script></html>}');
CREATE TABLE t024_interfaces (id INTEGER UNIQUE PRIMARY KEY);
INSERT INTO t024_interfaces VALUES(1);
CREATE TABLE t025_sites (id INTEGER UNIQUE PRIMARY KEY, name TEXT, interface_id INTEGER, env_id INTEGER, start_date DATE, end_date DATE, online_booking INTEGER, use_old_data INTEGER, client_url TEXT);
INSERT INTO t025_sites VALUES(1,'test',1,1,'2006-11-01','2007-11-01',1,0,'test');
CREATE TABLE t026_users (id INTEGER UNIQUE PRIMARY KEY, name TEXT, surname TEXT, login TEXT, password TEXT, profile_id INTEGER);
INSERT INTO t026_users VALUES(1,'name','surname','test','test',1);
CREATE TABLE t027_profiles (id INTEGER UNIQUE PRIMARY KEY, name TEXT, parent INTEGER, rights TEXT);
INSERT INTO t027_profiles VALUES(1,'admin',0,'*,*,100,100');
CREATE TABLE t999_config (param_name TEXT UNIQUE PRIMARY KEY, param_value TIMESTAMP);
CREATE TRIGGER t001_environment_links_no_update BEFORE UPDATE OF id, environment_id, link_target_id ON t001_environment_links BEGIN SELECT RAISE (ABORT, 'Update of id, environment_id, link_target_id in t001_environment_links is forbidden.') WHERE 1; END;
CREATE TRIGGER t002_addresses_no_remove BEFORE DELETE ON t002_addresses BEGIN SELECT RAISE (ABORT, 'Deletion in t002_addresses is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t002_addresses_no_update BEFORE UPDATE OF place_id, road_id, metric_offset ON t002_addresses BEGIN SELECT RAISE (ABORT, 'Update of place_id, road_id, metric_offset in t002_addresses is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t004_axes_no_remove BEFORE DELETE ON t004_axes BEGIN SELECT RAISE (ABORT, 'Deletion in t004_axes is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t005_service_dates_no_update BEFORE UPDATE OF service_id, date ON t005_service_dates BEGIN SELECT RAISE (ABORT, 'Update of service_id, date in t005_service_dates is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t006_cities_no_remove BEFORE DELETE ON t006_cities BEGIN SELECT RAISE (ABORT, 'Deletion in t006_cities is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t007_connection_places_no_remove BEFORE DELETE ON t007_connection_places BEGIN SELECT RAISE (ABORT, 'Deletion in t007_connection_places is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t007_connection_places_no_update BEFORE UPDATE OF city_id, is_city_main_connection ON t007_connection_places BEGIN SELECT RAISE (ABORT, 'Update of city_id, is_city_main_connection in t007_connection_places is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t008_fares_no_remove BEFORE DELETE ON t008_fares BEGIN SELECT RAISE (ABORT, 'Deletion in t008_fares is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t009_lines_no_remove BEFORE DELETE ON t009_lines BEGIN SELECT RAISE (ABORT, 'Deletion in t009_lines is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t009_lines_no_update BEFORE UPDATE OF transport_network_id, axis_id ON t009_lines BEGIN SELECT RAISE (ABORT, 'Update of transport_network_id, axis_id in t009_lines is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t010_line_stops_no_remove BEFORE DELETE ON t010_line_stops BEGIN SELECT RAISE (ABORT, 'Deletion in t010_line_stops is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t010_line_stops_no_update BEFORE UPDATE OF physical_stop_id, line_id, rank_in_path, is_departure, is_arrival ON t010_line_stops BEGIN SELECT RAISE (ABORT, 'Update of physical_stop_id, line_id, rank_in_path, is_departure, is_arrival in t010_line_stops is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t011_place_aliases_no_remove BEFORE DELETE ON t011_place_aliases BEGIN SELECT RAISE (ABORT, 'Deletion in t011_place_aliases is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t011_place_aliases_no_update BEFORE UPDATE OF aliased_place_id, city_id, is_city_main_connection ON t011_place_aliases BEGIN SELECT RAISE (ABORT, 'Update of aliased_place_id, city_id, is_city_main_connection in t011_place_aliases is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t012_physical_stops_no_remove BEFORE DELETE ON t012_physical_stops BEGIN SELECT RAISE (ABORT, 'Deletion in t012_physical_stops is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t012_physical_stops_no_update BEFORE UPDATE OF place_id ON t012_physical_stops BEGIN SELECT RAISE (ABORT, 'Update of place_id in t012_physical_stops is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t013_public_places_no_remove BEFORE DELETE ON t013_public_places BEGIN SELECT RAISE (ABORT, 'Deletion in t013_public_places is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t013_public_places_no_update BEFORE UPDATE OF city_id ON t013_public_places BEGIN SELECT RAISE (ABORT, 'Update of city_id in t013_public_places is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t014_road_chunks_no_remove BEFORE DELETE ON t014_road_chunks BEGIN SELECT RAISE (ABORT, 'Deletion in t014_road_chunks is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t014_road_chunks_no_update BEFORE UPDATE OF address_id, rank_in_path, is_departure, is_arrival ON t014_road_chunks BEGIN SELECT RAISE (ABORT, 'Update of address_id, rank_in_path, is_departure, is_arrival in t014_road_chunks is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t015_roads_no_remove BEFORE DELETE ON t015_roads BEGIN SELECT RAISE (ABORT, 'Deletion in t015_roads is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t015_roads_no_update BEFORE UPDATE OF city_id ON t015_roads BEGIN SELECT RAISE (ABORT, 'Update of city_id in t015_roads is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t016_scheduled_services_no_remove BEFORE DELETE ON t016_scheduled_services BEGIN SELECT RAISE (ABORT, 'Deletion in t016_scheduled_services is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t016_scheduled_services_no_update BEFORE UPDATE OF path_id ON t016_scheduled_services BEGIN SELECT RAISE (ABORT, 'Update of path_id in t016_scheduled_services is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t017_continuous_services_no_remove BEFORE DELETE ON t017_continuous_services BEGIN SELECT RAISE (ABORT, 'Deletion in t017_continuous_services is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t017_continuous_services_no_update BEFORE UPDATE OF path_id ON t017_continuous_services BEGIN SELECT RAISE (ABORT, 'Update of path_id in t017_continuous_services is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t021_reservation_rules_no_remove BEFORE DELETE ON t021_reservation_rules BEGIN SELECT RAISE (ABORT, 'Deletion in t021_reservation_rules is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t023_interface_pages_no_update BEFORE UPDATE OF id, interface_id, page_code ON t023_interface_pages BEGIN SELECT RAISE (ABORT, 'Update of id, interface_id, page_code in t023_interface_pages is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t024_interfaces_no_update BEFORE UPDATE OF id ON t024_interfaces BEGIN SELECT RAISE (ABORT, 'Update of id in t024_interfaces is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t025_sites_no_update BEFORE UPDATE OF id ON t025_sites BEGIN SELECT RAISE (ABORT, 'Update of id in t025_sites is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t026_users_no_update BEFORE UPDATE OF id ON t026_users BEGIN SELECT RAISE (ABORT, 'Update of id in t026_users is forbidden.') WHERE (SELECT param_value FROM t999_config WHERE param_name=triggers_enabled); END;
CREATE TRIGGER t027_profiles_no_update BEFORE UPDATE OF id ON t027_profiles BEGIN SELECT RAISE (ABORT, 'Update of id in t027_profiles is forbidden.') WHERE 1; END;
CREATE TRIGGER t999_config_no_update BEFORE UPDATE OF param_name ON t999_config BEGIN SELECT RAISE (ABORT, 'Update of param_name in t999_config is forbidden.') WHERE 1; END;
COMMIT;

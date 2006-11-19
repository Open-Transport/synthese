
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::env::LineMarkerInterfacePage>("line_marker");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::env::LogicalStopNameValueInterfaceElement>("stop_name");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::env::CityNameValueInterfaceElement>("city_name");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::env::LogicalStopNameValueInterfaceElement>("stop_name");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::env::CityNameValueInterfaceElement>("city_name");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::EnvironmentTableSync>("15.00.00");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::AlarmTableSync>("15.10.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::FareTableSync>("15.10.02");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::BikeComplianceTableSync>("15.10.03");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::HandicappedComplianceTableSync>("15.10.04");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::PedestrianComplianceTableSync>("15.10.05");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::ReservationRuleTableSync>("15.10.06");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::CityTableSync>("15.20.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::TransportNetworkTableSync>("15.20.02");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::AxisTableSync>("15.20.03");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::LineTableSync>("15.30.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::RoadTableSync>("15.30.02");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::ConnectionPlaceTableSync>("15.40.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::PublicPlaceTableSync>("15.40.02");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::PlaceAliasTableSync>("15.50.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::AddressTableSync>("15.50.02");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::RoadChunkTableSync>("15.60.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::ContinuousServiceTableSync>("15.60.02");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::ScheduledServiceTableSync>("15.60.03");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::ServiceDateTableSync>("15.70.01");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::PhysicalStopTableSync>("15.80.01");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::LineStopTableSync>("15.90.01");


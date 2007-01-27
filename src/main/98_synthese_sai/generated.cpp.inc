
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::db::DBModule>("02_db");



synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmTableSync>("17.10.01");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::env::LineMarkerInterfacePage>("line_marker");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::env::LogicalStopNameValueInterfaceElement>("stop_name");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::env::CityNameValueInterfaceElement>("city_name");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::env::EnvironmentTableSync>("15.00.00");

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



synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::security::SecurityModule>("12_security");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::ProfileTableSync>("12.01 Profile");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::UserTableSync>("12.02 User");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::security::UsersAdmin>("users");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::security::UserAdmin>("user");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::AddUserAction>("sau");

synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::interfaces::InterfaceModule>("11_interfaces");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::ParameterValueInterfaceElement>("param");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::IfThenElseInterfaceElement>("if");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::interfaces::FormattedNumberInterfaceElement>("format");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::CommentInterfaceElement>("//");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::PrintInterfaceElement>("print");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::LineLabelInterfaceElement>("label");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::IncludePageInterfaceElement>("include");
synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::interfaces::GotoInterfaceElement>("goto");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfaceTableSync>("16.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::interfaces::InterfacePageTableSync>("16.02");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::admin::AdminInterfacePage>("admin");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::admin::HomeAdmin>("home");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::admin::AdminRequest>("admin");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagesTreeInterfaceElement>("admintree");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::admin::AdminPagePositionInterfaceElement>("adminpos");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::admin::AdminContentInterfaceElement>("admincontent");


synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::server::ServerModule>("999_server");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::ServerConfigTableSync>("0 Config");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::SiteTableSync>("30.01 Site");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LoginAction>("login");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LogoutAction>("logout");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::server::SimpleLoginInterfacePage>("simplelogin");
synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::server::RedirectInterfacePage>("redir");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::HtmlFormInterfaceElement>("htmlform");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LogoutHTMLLinkInterfaceElement>("logouthtmllink");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LoginHtmlField>("loginhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::PasswordHtmlField>("passwordhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::RequestErrorMessageInterfaceElement>("errormessage");

synthese::util::Factory<synthese::server::Request>::integrate<synthese::server::SimplePageRequest>("page");
synthese::util::Factory<synthese::server::Request>::integrate<synthese::server::ActionOnlyRequest>("action");

synthese::util::Factory<synthese::interfaces::LibraryInterfaceElement>::integrate<synthese::departurestable::DeparturesTableInterfaceElement>("departurestable");

synthese::util::Factory<synthese::interfaces::InterfacePage>::integrate<synthese::departurestable::DepartureTableRowInterfacePage>("departurestablerow");


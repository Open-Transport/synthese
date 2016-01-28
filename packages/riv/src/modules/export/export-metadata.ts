class Parameter {
  constructor(public name: string, public label: string, public type: string = "text") {
  }
}

class ExportMetadata {

  // Parameters description can be found in server/src/61_data_exchange/HafasFileFormat.hpp

  parameters: Parameter[] = [
    new Parameter("debug", "Debug mode (format: 0/1)"),
    new Parameter("network", "Name of the network to export"),
    new Parameter("main_ds", "Name of the data source used to get business codes of the TransportNetwork and RollingStock objects"),
    new Parameter("stops_ds", "Name of the data source used to get business codes of the StopArea objects"),
    // NOTE: bitfield_start_date and bitfield_end_date are set automatically from the year configurations.
    new Parameter("timetable_name", "Name of the exported time table. Last line of the ECKDATEN file"),
    new Parameter("ftp_host", "FTP Hostname"),
    new Parameter("ftp_port", "FTP Port (optional. Default: 21)"),
    new Parameter("ftp_user", "FTP Username"),
    new Parameter("ftp_pass", "FTP Password", "password"),
    new Parameter("ftp_path", "FTP path where the archive should be uploaded"),
  ];
}

let exportMetadata = new ExportMetadata();

export default exportMetadata;

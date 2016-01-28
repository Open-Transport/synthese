import {Injectable} from 'angular2/core';

import {SyntheseService} from 'synthese/base';

const EXPORT_MODULE_NAME = "riv-export";
const CONFIG_NAME = "config";

const PARAMETER_FILE_FORMAT = "ff";
const PARAMETER_NETWORK_NAME = "network";
const PARAMETER_BITFIELD_START_DATE = "bitfield_start_date";
const PARAMETER_BITFIELD_END_DATE = "bitfield_end_date";

@Injectable()
export class ExportService {
  constructor(public syntheseService: SyntheseService) {
  }

  loadConfig() {
    return this
      .syntheseService
      .getSetting(EXPORT_MODULE_NAME, CONFIG_NAME)
      .then(value => {
        try {
          return JSON.parse(value);
        } catch(e) {
          console.log(`Unable to parse config "${value}", using empty configuration: ${e}`);
          return {
            globalParameters: {},
            years: [],
            networkParameters: {}
          };
        }
      });
  }

  saveConfig(config) {
    return this
      .syntheseService
      .setSetting(EXPORT_MODULE_NAME, CONFIG_NAME, JSON.stringify(config));
  }

  _buildExportParameters(config, network, year) {
    let exportParams = {
      "SERVICE": "ExportFunction",
    }
    exportParams[PARAMETER_FILE_FORMAT] = "Hafas";
    exportParams[PARAMETER_NETWORK_NAME] = network;

    for (let name in config.globalParameters) {
      exportParams[name] = config.globalParameters[name];
    }
    let networkParameters = config.networkParameters[network] || {};
    for (let name in networkParameters) {
      exportParams[name] = networkParameters[name];
    }

    let yearConfig = config.years.filter(yearConfig => yearConfig.id == year)[0];

    exportParams[PARAMETER_BITFIELD_START_DATE] = yearConfig.start;
    exportParams[PARAMETER_BITFIELD_END_DATE] = yearConfig.end;

    return exportParams;
  }

  runExport(network, year): Promise<any> {
    return this
      .loadConfig()
      .then(config => this._buildExportParameters(config, network, year))
      .then(exportParams => {

        return this.syntheseService.callService('/export/', exportParams, false)
          .then(exportOutput => {
            return {
              success: true,
              output: exportOutput,
            };
          })
          .catch(e => {
            return {
              success: false,
              output: e.text(),
            };
          });

        });
  }
}

import {Component, Input, OnInit} from 'angular2/core';
import {SyntheseService} from 'synthese/base';
import {ExportService} from './export.service';
import exportMetadata from './export-metadata';


class ExportAdminModel {
  globalParameters: any[] = [];
  years: any[] = [];
  networks: any[] = [];
}


@Component({
  selector: 'parameter-input',
  template: `
    <div class="control-group">
      <label class="control-label">{{parameter.label}}</label>
      <div class="controls">
        <div class="input-append">
          <input name="{{parameter.name}}" type="{{parameter.type}}"
                 [(ngModel)]="parameter.value">
        </div>
      </div>
    </div>
  `
})
class ParameterInputComponent {
  @Input() parameter;
}


@Component({
  templateUrl: '/riv/modules/export/export-admin.component.html',
  directives: [ParameterInputComponent],
  providers: [SyntheseService, ExportService],
})
export class ExportAdminComponent implements OnInit {
  model: ExportAdminModel;
  statusMessage: string;

  constructor(
    public syntheseService: SyntheseService,
    public exportService: ExportService) {
  }

  _buildModelFromConfig(config, networks) {
    let model = new ExportAdminModel();

    model.globalParameters = exportMetadata.parameters.map((parameter) => ({
      name: parameter.name,
      label: parameter.label,
      value: config.globalParameters[parameter.name] || "",
      type: parameter.type,
    }));

    model.years = config.years;
    model.years.sort((a, b) => parseInt(a.id) - parseInt(b.id));

    model.networks = networks.map(network => {
      let networkConfig = config.networkParameters[network] || {};

      return {
        name: network,
        parameters: exportMetadata.parameters.map(parameter => ({
          name: parameter.name,
          label: parameter.label,
          value: networkConfig[parameter.name] || "",
          type: parameter.type,
        }))
      };
    });

    return model;
  }

  _buildConfigFromModel(model) {
    let config = {
      globalParameters: {},
      years: [],
      networkParameters: {},
    };

    model.globalParameters.forEach(globalParameter => {
      config.globalParameters[globalParameter.name] = globalParameter.value;
    });

    config.years = model.years;

    model.networks.forEach(network => {
      let networkConfig = {};
      network.parameters.forEach(parameter => {
        networkConfig[parameter.name] = parameter.value;
      })
      config.networkParameters[network.name] = networkConfig;
    });

    return config;
  }

  ngOnInit() {
    Promise.all([
      this.exportService.loadConfig(),
      this.syntheseService.getNetworks(),
    ])
    .then(([config, networks_]) => {
      let networks = <any[]> networks_;
      networks = networks.map(network => network.name);
      this.model = this._buildModelFromConfig(config, networks);
    });
  }

  onDeleteYear(index) {
    this.model.years.splice(index, 1);
  }

  onAddYear() {
    this.model.years.push({
      id: "", start: "", end: ""
    });
  }

  onSubmit() {
    console.log("Submit");
    let config = this._buildConfigFromModel(this.model);
    console.log("new config:", config);
    this
      .exportService
      .saveConfig(config)
      .then(() => {
        this.statusMessage = "Configuration sauvegardée";
        setTimeout(() => this.statusMessage = "", 3000);
      })
      .catch((e) => this.syntheseService.reportError(e));
  }
}

import {Component, OnInit} from 'angular2/core';
import {SyntheseService} from 'synthese/base';
import {ExportService} from './export.service';

enum State {Configuring, Running, ResultSuccess, ResultFailure};

@Component({
  templateUrl: '/riv/modules/export/export.component.html',
  providers: [SyntheseService, ExportService],
})
export class ExportComponent implements OnInit {
  StateEnum = State;

  state: State = State.Configuring;
  networks;
  selectedNetwork;
  years;
  selectedYear;
  output;

  constructor(
    public syntheseService: SyntheseService,
    public exportService: ExportService) {
  }

  ngOnInit() {
    this.state = State.Configuring;

    Promise.all([
      this.syntheseService.getNetworks(),
      this.exportService.loadConfig()
    ])
    .then(([networks, config]) => {
      this.networks = networks.map(networkInfo => networkInfo.name);
      if (this.networks.length == 1) {
        this.selectedNetwork = this.networks[0];
      }
      this.years = config.years.map(year => year.id);
      this.years.sort((a, b) => parseInt(a) - parseInt(b));
      if (this.years.length) {
        this.selectedYear = this.years[this.years.length - 1];
      }
    })
    .catch(e => this.syntheseService.reportError(e))
  }

  onRunExport() {
    this.state = State.Running;

    this.exportService
      .runExport(this.selectedNetwork, this.selectedYear)
      .then(result => {
        this.state = result.success ? State.ResultSuccess : State.ResultFailure;
        this.output = result.output;
      })
      .catch(e => this.syntheseService.reportError(
        "Echec lors de l'appel du service d'export: " + e));
  }

  onNewExport() {
    this.ngOnInit();
  }
}

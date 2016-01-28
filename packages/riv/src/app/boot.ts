import {provide, Injectable, Component} from 'angular2/core';

import {SiteConfig, bootstrapSite} from 'synthese/base';
import {DemoModule} from 'synthese/modules/demo/demo.module';

import {ExportModule} from 'riv/modules/export/export.module';
//import {PTModule} from '../modules/pt/pt.module';


@Component({
  template: `
    <h2>Bienvenue sur l'application RIV</h2>

    <p>
      Les différentes fonctionalités sont disponibles sur le menu principal.
    </p>
  `,
})
class RIVHomeComponent {
  constructor(public siteConfig: SiteConfig) {
  }
}

@Injectable()
class RIVSiteConfig extends SiteConfig {
  constructor() {
    super("riv", "RIV");
    this.modules.push(new DemoModule());
    this.modules.push(new ExportModule());
    //this.modules.push(new PTModule());
  }
}

bootstrapSite([
  provide("HomeComponent", {useValue: RIVHomeComponent}),
  provide(SiteConfig, {useClass: RIVSiteConfig}),
]);

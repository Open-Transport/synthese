import {Component} from 'angular2/core';
import {SiteConfig} from 'synthese/base/site-config';

@Component({
  template: `
    <div class="page-header">
      <h1>Bienvenue sur {{siteConfig.name}}</h1>
    </div>
  `
})
export class HomeComponent {
  constructor(public siteConfig: SiteConfig) {
  }
}

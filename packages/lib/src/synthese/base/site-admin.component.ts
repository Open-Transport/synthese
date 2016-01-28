import {Component, Type} from 'angular2/core';
import {Router, RouteConfig, RouteDefinition, ROUTER_DIRECTIVES} from 'angular2/router';

import {SiteConfig} from 'synthese/base/site-config';
import {SyntheseModule} from './synthese-module';

@Component({
  template: `
    <h3>Ecran d'administration des modules</h3>
    Veuillez choisir un module à paramétrer.
  `
})
class AdminHome {
}

export function createSiteAdminComponentClass(siteConfig: SiteConfig) {

  let routeConfig: RouteDefinition[] = [{
    path: '/',
    name: 'AdminHome',
    component: AdminHome,
    useAsDefault: true,
  }];

  siteConfig.modules.forEach(syntheseModule => {
    if (syntheseModule.adminComponent) {
      routeConfig.push({
        path: syntheseModule.routerPath,
        name: syntheseModule.routerName,
        component: <Type>syntheseModule.adminComponent,
      });
    }
  });

  @Component({
    templateUrl: '/lib/synthese/base/site-admin.component.html',
    directives: [ROUTER_DIRECTIVES],
  })
  @RouteConfig(routeConfig)
  class SiteAdminComponent {
    adminModules: SyntheseModule[];

    constructor(public siteConfig: SiteConfig, public router: Router) {
      this.adminModules = siteConfig.modules.filter(m => !!m.adminComponent);
    }

    isRouteActive(link) {
      return this.router.isRouteActive(this.router.generate([link]));
    }
  }

  return SiteAdminComponent;
}

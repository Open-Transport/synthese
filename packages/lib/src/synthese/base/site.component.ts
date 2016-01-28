import {Injectable, Inject, Component, OnInit, Type}      from 'angular2/core';
import {RouteConfig, RouteDefinition, ROUTER_DIRECTIVES} from 'angular2/router';
import {Http, HTTP_PROVIDERS} from 'angular2/http';

import {SiteConfig} from './site-config';
import {LoginComponent} from './login.component';
import {SyntheseService} from './synthese.service';
import {SessionService} from './session.service';

export function createSiteComponentClass(
  homeComponent,
  adminComponent,
  siteConfig: SiteConfig): Object {

  let routeConfig: RouteDefinition[] = siteConfig.modules.map((syntheseModule) => {
    return {
      path: syntheseModule.routerPath,
      name: syntheseModule.routerName,
      component: <Type>syntheseModule.component,
    };
  });

  // link to home.
  routeConfig.push(
    {path: '/',   name: 'Home', component: homeComponent, useAsDefault: true},
    {path: '/admin/...',   name: 'Admin', component: adminComponent}
  );

  @Component({
    selector: 'site',
    templateUrl: '/lib/synthese/base/site.component.html',
    // FIXME: Angular ignores absolute CSS urls.
    // Assume that the page is on /application and use a relative link.
    styleUrls: [
      '../lib/synthese/base/site.component.css',
    ],

    directives: [LoginComponent, ROUTER_DIRECTIVES],
    providers: [SyntheseService, SessionService, HTTP_PROVIDERS],
  })
  @RouteConfig(routeConfig)
  class SiteComponent implements OnInit {

    constructor(public session: SessionService, public siteConfig: SiteConfig) {
    }

    ngOnInit() {
      // Try to login with empty credentials, to know if the user is already logged in.
      this.session.login("", "").catch((e) => {
        console.log("Login error", e);
      });
    }
  }

  return SiteComponent;
}

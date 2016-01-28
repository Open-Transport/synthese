import {provide, Injectable, Inject, Injector, Component} from 'angular2/core';
import {bootstrap} from 'angular2/platform/browser';
import {ROUTER_PROVIDERS, LocationStrategy, HashLocationStrategy} from 'angular2/router';

import {SiteConfig} from './site-config';
import {HomeComponent} from './home.component';
import {createSiteComponentClass} from './site.component';
import {createSiteAdminComponentClass} from './site-admin.component';

let BASE_PROVIDERS = [
  provide("HomeComponent", {useValue: HomeComponent}),
];

export function bootstrapSite(providers) {
  var injector = Injector.resolveAndCreate(BASE_PROVIDERS);

  providers.push(
    provide("SiteComponent", {
      useFactory: createSiteComponentClass,
      deps: ["HomeComponent", "SiteAdminComponent", SiteConfig]
    }),
    provide("SiteAdminComponent", {
      useFactory: createSiteAdminComponentClass,
      deps: [SiteConfig],
    })
  );

  var siteInjector = injector.resolveAndCreateChild(providers);
  let SiteComponentClass = siteInjector.get("SiteComponent");

  bootstrap(SiteComponentClass, [
    providers,
    ROUTER_PROVIDERS,
    provide(LocationStrategy,
      {useClass: HashLocationStrategy}),
  ]).catch(e => {
    console.error("Error in application: ", e);
  });
}

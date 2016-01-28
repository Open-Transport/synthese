import {Injectable} from 'angular2/core';
import {SyntheseModule} from './synthese-module';

@Injectable()
export abstract class SiteConfig {
  modules: SyntheseModule[];

  constructor(public id: string, public name: string) {
    this.modules = [];
  }
}

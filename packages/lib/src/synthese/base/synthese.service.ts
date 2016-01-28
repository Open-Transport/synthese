import {Injectable} from 'angular2/core';
import {Http, Headers} from 'angular2/http';

import {SiteConfig} from 'synthese/base/site-config';

@Injectable()
export class SyntheseService {

  constructor(private http: Http, private siteConfig: SiteConfig) {
  }

  callService(path, params = {}, isJSON = true, methodIsPost = false) {

    var serialize = function (data) {
      return Object.keys(data).map(function (keyName) {
        return encodeURIComponent(keyName) + '=' + encodeURIComponent(data[keyName])
      }).join('&');
    };

    if (path[0] != '/') {
      // if path is relative, assume that it is relative to the current site.
      path = '/' + this.siteConfig.id + '/' + path;
    }

    var headers = new Headers();
      headers.append('Content-Type', 'application/x-www-form-urlencoded');

    return new Promise((resolve, reject) => {
      let req;

      if (methodIsPost) {
        req = this.http
          .post(path, serialize(params), {
            headers: headers,
          });
      } else {
        req = this.http
          .get(path + '?' + serialize(params), {
            headers: headers,
          });
      }

      req.subscribe(
        res => resolve(isJSON ? res.json() : res.text()),
        err => reject(err)
      );
    });

  }

  getNetworks(): Promise<any> {
    return this.callService('ajax/networks_list');
  }

  getSetting(module, name, defaultValue = ""): Promise<string> {
    return this.callService('/', {
      "SERVICE": "settings",
      "action": "get",
      module: module,
      name: name,
      value: defaultValue
    }, false);
  }

  setSetting(module, name, value): Promise<{}> {
    return this.callService('/', {
      "SERVICE": "settings",
      "action": "set",
      module: module,
      name: name,
      value: value
    }, false);
  }

  reportError(message) {
    console.error("SyntheseService error reported", message);
    // TODO: show the error in the GUI.
  }

};

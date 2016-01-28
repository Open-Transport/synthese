import {Injectable} from 'angular2/core';

import {SiteConfig} from './site-config';
import {SyntheseService} from './synthese.service';

@Injectable()
export class SessionService {
  loggedIn = false;
  info: Object;

  constructor(
    public syntheseService: SyntheseService,
    public siteConfig: SiteConfig) {
  }

  login(userName, password) {

    var params = {
      a: "login",
      "actionParamlogin": userName,
      "actionParampwd": password,
    };

    // FIXME: angular shows in error in the console when throwing an exception from a Promise handler
    // wrap in another Promise to avoid the issue.

    return new Promise((accept, reject) => {
      this.syntheseService.callService('ajax/login', params, true, true)
        .then((loginResult: any) => {

          if (!loginResult.session_id) {
            this.loggedIn = false;
            return reject("session_id is empty (use not logged in)");
          }

          this.loggedIn = true;
          this.info = loginResult;

          accept(loginResult);
        })
        .catch(reject);
      });
  }

  logout() {
    this.syntheseService.callService('ajax/login', {
      "a": "logout"
    }, true, true).then(() => this.loggedIn = false);
  }

}

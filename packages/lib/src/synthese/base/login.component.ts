import {Component} from 'angular2/core';

import {SiteConfig} from 'synthese/base/site-config';
import {SessionService} from 'synthese/base/session.service';

@Component({
  selector: 'login',
  templateUrl: '/lib/synthese/base/login.component.html',
})
export class LoginComponent {
  userName: string;
  password: string;
  error = "";

  constructor(public session: SessionService, private siteConfig: SiteConfig) {
  }

  onSubmit() {
    this.error = "";

    this
      .session
      .login(this.userName, this.password)
      .then(() => console.debug("successful login"))
      .catch(error => this.error = "Failed to login: " + error);
  }
}

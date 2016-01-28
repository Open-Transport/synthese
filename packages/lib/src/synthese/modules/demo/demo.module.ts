import {SyntheseModule} from 'synthese/base';
import {DemoComponent} from './demo.component';
import {DemoAdminComponent} from './demo-admin.component';

export class DemoModule extends SyntheseModule {
  routerPath = "/demo";
  routerName = "Demo";
  menuLabel = "Module démo";
  component = DemoComponent;
  adminComponent = DemoAdminComponent;
}

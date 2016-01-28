import {SyntheseModule} from 'synthese/base';
import {ExportComponent} from './export.component';
import {ExportAdminComponent} from './export-admin.component';

export class ExportModule extends SyntheseModule {
  routerPath = "/export";
  routerName = "Export";
  menuLabel = "Export";
  component = ExportComponent;
  adminComponent = ExportAdminComponent;
}

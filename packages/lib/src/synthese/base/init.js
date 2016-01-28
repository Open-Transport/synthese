function syntheseAngularInit(dev, site, usingSites) {

  var systemConfig = {};
  if (dev) {
    systemConfig.transpiler = 'typescript';
    systemConfig.typescriptOptions = {
      emitDecoratorMetadata: true
    };
  }

  if (!usingSites) {
    usingSites = [];
  }
  usingSites.push(site);

  systemConfig.map = {};
  systemConfig.packages = {};

  // synthese lib

  systemConfig.map['synthese'] = '/lib/synthese' + (dev ? '' : '/js');
  systemConfig.packages['/lib/synthese'] = {
    defaultExtension: dev ? 'ts' : 'js'
  };

  // public sites

  usingSites.forEach(function(site) {
    systemConfig.map[site + '/modules'] = '/' + site + (dev ? '' : '/js') + '/modules';
    systemConfig.packages['/' + site + (dev ? '/modules' : '/js')] = { defaultExtension: dev ? 'ts' : 'js' };
  });

  // own private site

  systemConfig.map['app'] = '/' + site + (dev ? '' : '/js') + '/app';
  systemConfig.packages['/' + site + (dev ? '/app' : '/js')] = { defaultExtension: dev ? 'ts' : 'js' };


  System.config(systemConfig);

  System.import('app/boot')
    .then(null, console.error.bind(console));
}

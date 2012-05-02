var ConfigManager = {

  mergeArrays: function(defaults, array, insertFirst) {
    function byName(array, name) {
      return _.find(array, function(item) {return item.name == name}) || {};
    }
    var arrayOnlyNames = _.difference(_.pluck(array, 'name'), _.pluck(defaults, 'name'));

    var result = [];

    if (insertFirst) {
      arrayOnlyNames.forEach(function(name) {
        result.push(byName(array, name));
      });
    }

    defaults.forEach(function(defaultItem) {
      var name = defaultItem.name;
      var item = _.defaults(byName(array, name), defaultItem);
      result.push(item);
    });

    if (!insertFirst) {
      arrayOnlyNames.forEach(function(name) {
        result.push(byName(array, name));
      });
    }

    return result;
  },

  buildLinkModels: function(server, commonLinks, serverConfig) {
    var linksConfig = serverConfig.links || [];
    if (!serverConfig.noCommonLinks) {
      linksConfig = this.mergeArrays(commonLinks, linksConfig);
    }

    return linksConfig.map(function(linkConfig) {
      var link = new Link(linkConfig.name, linkConfig.url);
      _.defaults(link, linkConfig);
      link.server = server;
      link.project = server.project;

      if (link.path)
        link.path = _.template(link.path, link);
      // TODO: remove this hack
      link.path_(link.path);

      return link;
    });
  },

  buildServerModels: function(project, defaultsConfig, projectConfig) {

    var serversConfig = this.mergeArrays(defaultsConfig.servers, projectConfig.servers, true);

    if (projectConfig.serversRemove) {
      serversConfig = serversConfig.filter(function(serverConfig) {
        return projectConfig.serversRemove.indexOf(serverConfig.name) == -1;
      });
    }

    var servers = serversConfig.map(function(serverConfig) {
      var server = new Server(serverConfig.name, serverConfig.domain, []);
      server.project = project;
      _.defaults(server, serverConfig);

      var commonLinks = this.mergeArrays(defaultsConfig.commonLinks, projectConfig.commonLinks || [])
      server.links = this.buildLinkModels(server, commonLinks, serverConfig);

      if (server.domain)
        server.domain = _.template(server.domain, server);
      // TODO: remove this hack
      server.domain_(server.domain);

      return server;
    }, this)

    return servers;
  },

  buildProjectModels: function(defaultsConfig, projectsConfig) {

    var projects = projectsConfig.map(function(projectConfig) {
      var project = new Project(projectConfig.name, []);
      _.defaults(project, projectConfig);
      project.servers = this.buildServerModels(project, defaultsConfig, projectConfig);
      return project;
    }, this);

    return projects;
  },

  buildViewModel: function(defaultsConfig, projectsConfig) {
    var viewModel = new ViewModel();
    viewModel.projects = this.buildProjectModels(defaultsConfig, projectsConfig);
    return viewModel;
  }
};

function Link(name, path) {
  this.name = name;
  // TODO: find a better way instead of duplicating path.
  this.path = path;
  this.path_ = ko.observable();
  this.note = null;
  this.typedURL = ko.observable();
  var self = this;

  this.url = ko.computed({
    read: function() {
      var path = self.path_();
      var typedURL = self.typedURL();
      if (!self.server)
        return "N/A";
      if (typedURL)
        return typedURL;
      if (self.path_() && self.path_().search(/^http/) == 0) {
        return self.path_();
      }
      var uri = new Uri().setProtocol(self.server.protocol || "http").setHost(self.server.domain_()).setPath(self.path_());
      return uri.toString();
    },
    // FIXME: not called?
    write: function(url) {
      self.typedURL(url);
    },
  });

  // FIXME: write above isn't called.
  this.onURLChange = function(obj, event) {
    var url = event.target.value;
    self.typedURL(url);
  };
}

function Server(name, domain, links) {
  this.name = name;
  this.domain = domain;
  this.domain_ = ko.observable();

  this.links = links;
  this.links.forEach(function(link) {
    link.server = this;
  }, this);
}

function Project(name, servers) {
  this.name = name;
  this.servers = servers;
  this.servers.forEach(function(server) {
    server.project = this;
  }, this);
}

function ViewModel() {
  var self = this;
  self.projects = [];
  self.activeProject = ko.observable();
  self.activeServer = ko.observable();
  self.activeLink = ko.observable();

  self.activeServers = ko.computed(function() {
    return self.activeProject() ? self.activeProject().servers : [];
  });

  self.activeLinks = ko.computed(function() {
    return self.activeServer() ? self.activeServer().links : [];
  });

  self.linkOptionsChanged = function() {
    console.log("linkOptionsChanged", arguments, self.activeLink());
    this.selectLink(self.activeLink());
  };

  self._loadFrame = function(link) {
    console.log("TODO: load frame", link);
  };

  self.selectLink = function(link) {
    console.log("select link", link);
    if (link) {
      self.activeProject(link.server.project);
      self.activeServer(link.server);
    }
    self.activeLink(link);

    if (link)
      self._loadFrame(link);
  };

  self.removeFrame = function() {
    self.activeLink(null);
  };

  ko.computed(function() {

    var title = 'Synthese Dashboard';
    var link = self.activeLink(), linkPath = '/', state = null;
    if (link) {
      function shortName(obj) {
        return obj.shortName || obj.name;
      }
      title = shortName(link.server.project) + ' ' + shortName(link.server) + ' ' + shortName(link);
      linkPath = '/p/' + link.server.project.name + '/' + link.server.name + '/' + link.name
      state = {
        project: link.server.project.name,
        server: link.server.name,
        link: link.name,
      };
    }

    document.title = title;
    history.pushState(state, title, linkPath);
  });

  // Scroll to project
  ko.computed(function() {
    var project = self.activeProject();
    // dummy call to update scroll when link changes.
    self.activeLink();
    if (!project)
      return;
    console.log("Scrolling to project");
    var topPosition = $(".page-header[name=" + project.name + "]").position().top;
    document.documentElement.scrollTop = topPosition - 40 - 10;

  });

  self.onPopState = function(event) {
    console.log("onPopState", event.state);
    var state = event.state;
    if (state === null) {
      self.activeLink(null);
      return;
    }

    function findBy(kind, array, name) {
      var match = array.filter(function(item) {
        return item.name == name;
      })[0];
      if (!match) {
        throw new Error("Can't find a " + kind + " with name " + name);
      }
      return match;
    }
    var link;
    try {
      var project = findBy('Project', self.projects, state.project);
      var server = findBy('Server', project.servers, state.server);
      link = findBy('Link', server.links, state.link);
    } catch(e) {
      console.warn(e.message);
      return;
    }

    // XXX fails
    self.selectLink(link);
  }
};

var viewModel = ConfigManager.buildViewModel(defaultsConfig, projectsConfig);
window.onpopstate = viewModel.onPopState;

$(function() {

  ko.applyBindings(viewModel);

  if (window.initialState) {
    setTimeout(function() {
      viewModel.onPopState({
        state: window.initialState
      });
    }, 0);
  }
});

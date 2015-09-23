Name:           3rd-synthese
Version:        0.1
Release:        0
BuildArch:      x86_64
Summary:        3rd party librairies for Synthese
License:        Mixed
Source:         http://ci.rcsmobility.com/~build/3rd/3rd-dev-Linux-x86_64.tar.gz

%description

Provides all the necessary 3rd party libraries for Synthese to work with.

%prep
%build

%install

mkdir -p %{buildroot}/opt/rcs
tar xvf $RPM_SOURCE_DIR/3rd-dev-Linux-x86_64.tar.gz -C %{buildroot}/opt/rcs

%post

%files
%defattr(-, root, root, -)
/*
%exclude /opt/rcs/*/include
%exclude /opt/rcs/geos/capi



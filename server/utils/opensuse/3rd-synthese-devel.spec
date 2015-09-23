Name:           3rd-synthese-devel
Version:        0.1
Release:        0
BuildArch:      x86_64
Summary:        3rd party librairies for Synthese. Development version
License:        Mixed
Source:         http://ci.rcsmobility.com/~build/3rd/3rd-dev-Linux-x86_64.tar.gz

%description

Provides all the necessary 3rd party libraries for Synthese to work with and their include files.

%prep
%build

%install

mkdir -p %{buildroot}/opt/rcs
tar xvf $RPM_SOURCE_DIR/3rd-dev-Linux-x86_64.tar.gz -C %{buildroot}/opt/rcs

%post

%files
%defattr(-, root, root, -)
/*



# Spec file for the boost package used by Synthese

%define ver 1.57.0
%define file_version 1_57_0
%define short_version 1_57
%define lib_appendix 1_57_0

Name:           boost-synthese
BuildRequires:  chrpath
BuildRequires:  gcc-c++
BuildRequires:  libbz2-devel
BuildRequires:  libexpat-devel
Url:            http://www.boost.org
Summary:        Boost C++ Libraries for Synthese
License:        BSD-3-Clause
Group:          Development/Libraries/C and C++
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Version:        %{ver}
Release:        0
Source0:        boost_%{file_version}.tar.gz


%description
Boost provides free peer-reviewed portable C++ source libraries. The
emphasis is on libraries that work well with the C++ Standard Library.
One goal is to establish "existing practice" and provide reference
implementations so that the Boost libraries are suitable for eventual
standardization. Some of the libraries have already been proposed for
inclusion in the C++ Standards Committee's upcoming C++ Standard
Library Technical Report.

Although Boost was begun by members of the C++ Standards Committee
Library Working Group, membership has expanded to include nearly two
thousand members of the C++ community at large.

This package provides all the necessary boost libraries for Synthese to work with.


%prep
%setup -q -n boost_%{file_version}

#everything in the tarball has the executable flag set ...
find -type f ! \( -name \*.sh -o -name \*.py -o -name \*.pl \) -exec chmod -x {} +

#stupid build machinery copies .orig files
find . -name \*.orig -exec rm {} +

%build
find . -type f -exec chmod u+w {} +

# I didn't find a way to not produce the headers which are not included in the package
./bootstrap.sh --libdir=%{buildroot}%{_libdir} --includedir=%{buildroot}%{_includedir} --with-libraries=program_options,iostreams,test,date_time,filesystem,system,regex,thread,random
./b2 -j4

%install
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_includedir}/boost

./b2 install -j4

mkdir -p %{buildroot}%{_docdir}

pushd %{buildroot}%{_libdir}

blibs=$(find . -name \*.so.%{version})
echo $blibs | xargs chrpath -d 

popd

%post 
/sbin/ldconfig

%postun
/sbin/ldconfig

# Keep only the versionned libs, so that it doesn't collide with the system
%files
%defattr(-, root, root, -)
%exclude %{_includedir}
%exclude %{_libdir}/*.so
%exclude %{_libdir}/*.a
%{_libdir}/*.%{version}

%changelog


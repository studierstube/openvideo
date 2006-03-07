Summary:	OpenVideo
Name:		openvideo
Version:	1.0.0
Release:	1
Copyright:	GPL
Group:		Development/Libraries
Source:		%{name}-%{version}.tar.bz2
#Source1:	%{name}-scons.tar.gz
#Vendor:		...
#URL:		http://www.grinninglizard.com/tinyxml
Packager:	Institute for Computer Graphics and Vision, Graz University of Technology, Austria
Prefix:		/usr
BuildRoot: 	%{_tmppath}/buildroot-%{name}-%{version}
Requires:	tinyxml
BuildRequires:	tinyxml

%define _prefix %{prefix}

%description
...

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
%setup

%build
scons INSTALLDIR=%{_prefix}

%install
scons INSTALLROOT=$RPM_BUILD_ROOT INSTALLDIR=%{_prefix} install
rm $RPM_BUILD_ROOT/%{_libdir}/.sconsign

%clean
scons -c
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/*
%{_prefix}/include/*
%exclude %{_libdir}/pkgconfig/.sconsign
%exclude %{_prefix}/include/openvideo/.sconsign
%exclude %{_prefix}/include/openvideo/nodes/.sconsign

#%package devel
#Summary:	...
#Group:		Development/Libraries
#Requires:	%{name} = %{version}
#
#%description devel
#This package contains header files and include files that are needed for development using ...
#
#%files devel
#%defattr(-,root,root)
#%{_bindir}/*
#%{_prefix}/include/*
#%{_prefix}/share/aclocal/*
#%{_prefix}/share/Coin/conf/*

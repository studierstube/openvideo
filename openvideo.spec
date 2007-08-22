Summary:	Support for video data
Name:		openvideo
Version:	1.0.0
Release:	%(date '+%Y%m%d')
License:	GPL
Group:		Development/Libraries
Source:		%{name}-%{version}.tar.bz2
URL:		http://studierstube.icg.tugraz.at/openvideo
Vendor:		Institute for Computer Graphics and Vision, Graz University of Technology, Austria
Packager:	Institute for Computer Graphics and Vision, Graz University of Technology, Austria
Prefix:		/usr
BuildRoot: 	%{_tmppath}/buildroot-%{name}-%{version}
Requires:	tinyxmlmod giflib ACE simage avifile
BuildRequires:	ICGBuilder tinyxmlmod-devel giflib-devel ACE-devel simage-devel avifile-devel

%define _prefix %{prefix}

%description
OpenVideo is a general data integration and processing software with special
support for video data. It implements a hardware abstraction layer by
interfacing several different device drivers either directly or through the
functionality of a third party video libraries. OpenVideo is designed to be as
extensible and easily configurable as possible. OpenVideo is currently
implemented on windows and on linux systems.

%prep
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
%setup

%build
scons

%install
scons --cache-disable DESTDIR=$RPM_BUILD_ROOT PREFIX=%{_prefix} LIBDIR=%{_libdir} INCLUDEDIR=%{_includedir} install

%clean
scons -c
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/openvideo
%{_libdir}/*.so*

%package devel
Summary:	OpenVideo headers
Group:		Development/Libraries
Requires:	%{name} = %{version} ICGBuilder tinyxmlmod-devel ACE-devel simage-devel avifile-devel

%description devel
This package contains header files and include files that are needed for development using OpenVideo.

%files devel
%defattr(-,root,root)
%{_libdir}/pkgconfig/*
%{_prefix}/include/*

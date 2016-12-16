%global _hardened_build 1
%define debug_package %{nil}

Summary: ghostwriter: A cross-platform, aesthetic, distraction-free Markdown editor
Name: ghostwriter
Version: 1.4.2
Release: 1%{?dist}
License: GPLv3+
Group: Development/Tools
URL: http://wereturtle.github.io/ghostwriter/
Source0: https://github.com/wereturtle/ghostwriter/archive/v%{version}.tar.gz
BuildRequires: gcc gcc-c++ make
BuildRequires: qt5-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-qtwebkit-devel 
BuildRequires: qt5-qtmultimedia-devel hunspell-devel pkgconfig
Requires: qt5-qtbase qt5-qtwebkit qt5-qtsvg qt5-qtmultimedia hunspell

%description
ghostwriter is a Windows and Linux text editor for Markdown, which is a plain 
text markup format created by John Gruber. For more information about Markdown, 
please visit John Gruber’s website at http://www.daringfireball.net. 

ghostwriter provides a relaxing, distraction-free writing environment, whether 
your masterpiece be that next blog post, your school paper, or your NaNoWriMo 
novel. 

%prep
%autosetup

%build
qmake-qt5 PREFIX=%{_prefix}
make %{?_smp_mflags}

%install
export INSTALL_ROOT=%{buildroot}
make install

%files
%{_bindir}/ghostwriter
%{_datarootdir}/ghostwriter/*
%{_datarootdir}/appdata/ghostwriter.appdata.xml
%{_datarootdir}/applications/ghostwriter.desktop
%{_datarootdir}/icons/hicolor/*/apps/ghostwriter.*
%{_mandir}/man1/*
%{_datarootdir}/pixmaps/*
%doc COPYING CREDITS.md

%changelog
* Thu Dec 15 2016 Arun Babu Neelicattu <arun.neelicattu@gmail.com> - 1.4.2-1
- Initial version v1.4.2


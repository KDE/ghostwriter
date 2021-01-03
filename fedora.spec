%undefine _hardened_build
%define debug_package %{nil}
%global appver 2.0.0-rc1
%global build_timestamp 20210102
%global changelog_date Sat Jan 02 2021
%global tarball %([[ %{appver} == *"-"* ]] && echo master || echo %{appver})


Summary: ghostwriter: A cross-platform, aesthetic, distraction-free Markdown editor
Name: ghostwriter
Version: %(echo %{appver} | tr '-' '~')
Release: 0.%{build_timestamp}%{?dist}
License: GPLv3+
URL: http://wereturtle.github.io/ghostwriter/
Source0: https://github.com/wereturtle/ghostwriter/archive/%{tarball}.tar.gz
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: qt5-devel
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtsvg-devel
BuildRequires: qt5-qtwebengine-devel
BuildRequires: qt5-qtmultimedia-devel
BuildRequires: hunspell-devel
BuildRequires: pkgconfig
Requires: qt5-qtbase
Requires: qt5-qtwebengine
Requires: qt5-qtsvg
Requires: qt5-qtmultimedia
Requires: hunspell
Suggests: pandoc
Suggests: pandoc-pdf
Suggests: tex-live
Suggests: pandoc-citeproc


%description
ghostwriter is a Windows and Linux text editor for Markdown, which is a plain
text markup format created by John Gruber. For more information about Markdown,
please visit John Gruber’s website at http://www.daringfireball.net.

ghostwriter provides a relaxing, distraction-free writing environment, whether
your masterpiece be that next blog post, your school paper, or your NaNoWriMo
novel.


%prep
%autosetup -n %{name}-%{tarball}


%build
qmake-qt5 PREFIX=%{_prefix}
make %{?_smp_mflags}


%install
export INSTALL_ROOT=%{buildroot}
make install


%files
%{_bindir}/ghostwriter
%{_datarootdir}/ghostwriter/*
%{_metainfodir}/ghostwriter.appdata.xml
%{_datarootdir}/applications/ghostwriter.desktop
%{_datarootdir}/icons/hicolor/*/apps/ghostwriter.*
%{_mandir}/man1/*
%doc COPYING CREDITS.md CHANGELOG.md


%changelog
* %{changelog_date} wereturtle <wereturtledev@gmail.com> - %{appver}
- New upstream release.


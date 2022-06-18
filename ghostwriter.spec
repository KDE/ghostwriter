# RPM spec file for Fedora and openSUSE
%undefine _hardened_build
%define debug_package %{nil}

%global appver 2.1.5-staging
%global build_timestamp %(date "+%%Y%%m%%d%%H%%M%%S")
%global changelog_date Sat Jun 18 2022
%global tarball %([[ %{appver} == *"-"* ]] && echo refs/heads/master.tar.gz || echo %{version}/%{name}-%{version}.tar.gz)

Name: ghostwriter
Version: %(echo %{appver} | tr '-' '~')
Release: 0.%{build_timestamp}%{?dist}

License: GPLv3+ and CC-BY and CC-BY-SA and MPLv1.1 and BSD and LGPLv3 and MIT and ISC
Summary: Cross-platform, aesthetic, distraction-free Markdown editor
URL: https://github.com/wereturtle/%{name}
Source0: %{url}/archive/%{tarball}

BuildRequires: git
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: pkgconfig
BuildRequires: pkgconfig(hunspell)
BuildRequires: cmake(Qt5Concurrent)
BuildRequires: cmake(Qt5Core)
BuildRequires: cmake(Qt5DBus)
BuildRequires: cmake(Qt5Gui)
BuildRequires: cmake(Qt5Help)
BuildRequires: cmake(Qt5LinguistTools)
BuildRequires: cmake(Qt5Network)
BuildRequires: cmake(Qt5Svg)
BuildRequires: cmake(Qt5WebEngine)
BuildRequires: cmake(Qt5X11Extras)
BuildRequires: cmake(Qt5Xml)
BuildRequires: cmake(Qt5XmlPatterns)
%if 0%{?suse_version}
BuildRequires: update-desktop-files
%endif

BuildRequires: desktop-file-utils
BuildRequires: gcc-c++
BuildRequires: hunspell-devel
BuildRequires: libappstream-glib
BuildRequires: make

Provides: bundled(cmark-gfm) = 0.29.0.gfm.3
Provides: bundled(fontawesome-fonts) = 5.10.2
Provides: bundled(nodejs-mathjax-full) = 3.1.2
Provides: bundled(nodejs-react) = 17.0.1
Provides: bundled(QtAwesome) = 5

Requires: hicolor-icon-theme

Recommends: cmark%{?_isa}
Recommends: multimarkdown%{?_isa}
Recommends: pandoc%{?_isa}

# Required qt5-qtwebengine is not available on some arches.
ExclusiveArch: %{qt5_qtwebengine_arches}

%description
ghostwriter is a text editor for Markdown, which is a plain text markup
format created by John Gruber. For more information about Markdown, please
visit John Gruber’s website at http://www.daringfireball.net.

ghostwriter provides a relaxing, distraction-free writing environment,
whether your masterpiece be that next blog post, your school paper,
or your novel.

%prep
%autosetup -n %{name}-%{version} -p1
mkdir -p %{_vpath_builddir}
rm -rf 3rdparty/hunspell

%build
pushd %{_vpath_builddir}
    %qmake_qt5 PREFIX=%{_prefix} ..
popd
%make_build -C %{_vpath_builddir}

%check
appstream-util validate-relax --nonet %{buildroot}%{_metainfodir}/%{name}.appdata.xml
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop

%install
%make_install INSTALL_ROOT=%{buildroot} -C %{_vpath_builddir}
%find_lang %{name} --with-qt

%files -f %{name}.lang
%doc CHANGELOG.md CONTRIBUTING.md CREDITS.md README.md
%license COPYING
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1*
%dir %{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.*
%{_datadir}/metainfo/%{name}.appdata.xml

%changelog
* %{changelog_date} wereturtle <wereturtledev@gmail.com> - %{appver}
- Upstream release.

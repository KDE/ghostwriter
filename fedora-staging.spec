%undefine _hardened_build
%define debug_package %{nil}
%define gitcommit %(git rev-parse --short HEAD)
%define latest_tag %(git describe --abbrev=0)
%define app_version %(git describe)
%define delta_from_tag %(git rev-list %{latest_tag}.. --count)
%define version %(echo %{latest_tag} | tr '-' '~')
%define timestamp %(date "+%%Y%%m%%d%%H%%M%%S")
%define changelogdate %(date "+%%a %%h %%d %%Y")

Summary: ghostwriter: A cross-platform, aesthetic, distraction-free Markdown editor
Name: ghostwriter
Version: %{version}
Release: 0.%{delta_from_tag}.%{timestamp}git%{gitcommit}%{?dist}
License: GPLv3+
Group: Development/Tools
URL: http://wereturtle.github.io/ghostwriter/
Source0: https://github.com/wereturtle/ghostwriter/archive/master.tar.gz
BuildRequires: gcc gcc-c++ make git
BuildRequires: qt5-devel qt5-qtbase-devel qt5-qtsvg-devel qt5-qtwebengine-devel
BuildRequires: qt5-qtmultimedia-devel hunspell-devel pkgconfig
Requires: qt5-qtbase qt5-qtwebengine qt5-qtsvg qt5-qtmultimedia hunspell

%description
ghostwriter is a Windows and Linux text editor for Markdown, which is a plain
text markup format created by John Gruber. For more information about Markdown,
please visit John Gruber’s website at http://www.daringfireball.net.

ghostwriter provides a relaxing, distraction-free writing environment, whether
your masterpiece be that next blog post, your school paper, or your NaNoWriMo
novel.

%prep
%autosetup -n %{name}-master

%build
qmake-qt5 PREFIX=%{_prefix} VERSION=%{app_version}
make %{?_smp_mflags}

%install
export INSTALL_ROOT=%{buildroot}
make install

%files
%{_bindir}/ghostwriter
%{_datarootdir}/ghostwriter/*
%{_datarootdir}/metainfo/ghostwriter.appdata.xml
%{_datarootdir}/applications/ghostwriter.desktop
%{_datarootdir}/icons/hicolor/*/apps/ghostwriter.*
%{_mandir}/man1/*
%doc COPYING CREDITS.md CHANGELOG.md

%changelog
* %{changelogdate} wereturtle <wereturtledev@gmail.com> - %{app_version}
- New upstream release.


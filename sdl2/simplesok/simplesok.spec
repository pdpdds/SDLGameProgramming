#
# spec file for package simplesok
#
# Copyright (c) 2014 Mateusz Viste
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name: simplesok
Version: 1.0.1
Release: 1%{?dist}
Summary: Simple Sokoban is a (simple) Sokoban game aimed for playability and portability across systems.

Group: Amusements/Games/Logic

License: GPL-3.0+
URL: http://simplesok.sourceforge.net/
Source0: %{name}-%{version}.tar.gz

%if 0%{?fedora}
BuildRequires: SDL2-devel >= 2.0.1
%else
BuildRequires: libSDL2-devel >= 2.0.1
%endif

%description
Simple Sokoban is a (simple) Sokoban game aimed for playability and portability across systems. It is written in ANSI C89, using SDL for user interactions. I developed it natively under x86_64 Linux, but it should compile just fine on virtually anything that has a C compiler and the SDL library.

%prep
%setup

%build
make

%check

%install
install -D simplesok %buildroot/%{_bindir}/simplesok
mkdir -p %buildroot/usr/share/icons/hicolor/48x48/apps/
install -D simplesok.png %buildroot/usr/share/icons/hicolor/48x48/apps/

%files
%dir /usr/
%dir /usr/share/
%dir /usr/share/icons/
%dir /usr/share/icons/hicolor/
%dir /usr/share/icons/hicolor/48x48/
%dir /usr/share/icons/hicolor/48x48/apps
%attr(644, root, root) %doc simplesok.txt license.txt history.txt
%attr(755, root, root) %{_bindir}/simplesok
%attr(644, root, root) /usr/share/icons/hicolor/48x48/apps/simplesok.png

%changelog
* Mon Apr 15 2014 Mateusz Viste <mateusz@viste-family.net> 1.0
 - First public release

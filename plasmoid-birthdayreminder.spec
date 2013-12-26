#
# spec file for package plasmoid-birthdayreminder (Version 0.9.73)
#
# Copyright (c) 2009 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (c) 2008-2009 Buschmann.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild

BuildRequires:  kdebase4-workspace-devel libkdepimlibs4-devel update-desktop-files
BuildRequires:  licenses

Name:           plasmoid-birthdayreminder
Summary:        Birthday Reminder Plasmoid
Version:        0.9.73
Release:        0.2
License:        GPL
Group:          System/GUI/KDE
Source0:        91641-birthday-plasmoid-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Url:            http://www.kde-look.org/content/show.php/K+Birthday+Reminder?content=91641
Requires:       licenses  
%kde4_runtime_requires
%kde4_pimlibs_requires

%description
Plasmoid reminding you at birthdays and anniversaries of contacts
in the (standard) KDE address book. Inspired by the KDE3 kicker
applet KBirthday from Jan Hambrecht.

Author:
-------
    Meinhard Ritscher

%prep
%setup -n KBirthdayPlasma_0_9_73

%build
  %cmake_kde4 -d builddir
  %make_jobs

%install
  cd builddir
  %makeinstall
  cd ..
  MD5SUM=$(md5sum LICENSE.GPL | sed 's/ .*//')
  if test -f /usr/share/doc/licenses/md5/$MD5SUM ; then  
      ln -sf /usr/share/doc/licenses/md5/$MD5SUM LICENSE.GPL;
  fi
  chmod -x README
  %suse_update_desktop_file -n $RPM_BUILD_ROOT/usr/share/kde4/services/plasma-applet-kbirthday.desktop
  %suse_update_desktop_file -n $RPM_BUILD_ROOT/usr/share/kde4/services/plasma-dataengine-kabc.desktop
  %kde_post_install

%clean
  rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc ChangeLog LICENSE.GPL README THANKS TODO
%_kde4_modules/plasma_applet_kbirthdayapplet.so
%_kde4_modules/plasma_engine_kabc.so
%_kde_share_dir/services/plasma-applet-kbirthday.desktop
%_kde_share_dir/services/plasma-dataengine-kabc.desktop
%_kde_share_dir/apps/desktoptheme/default/widgets/birthdaycake.svg
/usr/share/icons/hicolor/scalable/apps/birthdaycake.svgz
/usr/share/locale/de/LC_MESSAGES/plasma_applet_kbirthdayapplet.mo                               
/usr/share/locale/en_GB/LC_MESSAGES/plasma_applet_kbirthdayapplet.mo
/usr/share/locale/fr/LC_MESSAGES/plasma_applet_kbirthdayapplet.mo

%changelog
* Thu Jan 14 2010 unreachable@gmx.co.uk
- update to version 0.9.73
  * French translation added (from Alain Portal)
  * a couple of misspellings corrected
  * patches applied provided by Alain Portal
* Wed Jun  3 2009 dmueller@suse.de
- don't require kdebase4-workspace - that means *any* version
* Thu Mar  6 2009 unreachable@gmx.co.uk
- update to version 0.9.72
  * I18N files added
  * German translation added
* Tue Mar  3 2009 buschmann23@opensuse.org
- update to version 0.9.71
  * adapted cmake build files for KDE 4.2 final
  * crash when started in hour after midnight - FIXED
  * applied openSUSE patches
- removing obsolete kde42-cmake-changes.patch
- romoving obsolete desktop-file.patch
* Tue Feb  3 2009 buschmann23@opensuse.org
- update to version 0.9.70
  * I18N_ARGUMENT_MISSING FIXED by Patch from Tumsa on
    KDE-Looks org (thanks!)
  * Since the ToolTip interface changed from KDE 4.1 to 4.2 this
    version depends on 4.2
  * Extended panel support - this is not final yet!
- cleaning spec file
- adding kde42-cmake-changes.patch
* Tue Oct 28 2008 buschmann@huessenbergnetz.de
- initial package

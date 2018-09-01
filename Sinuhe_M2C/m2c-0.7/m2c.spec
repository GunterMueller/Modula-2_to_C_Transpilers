# RPM spec file for m2c
#
# Copyright 2014 David Egan Evans, Magna UT 84044 USA
#
# Permission to use, copy, modify, and distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
# OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)

Name: m2c
Summary: A Modula-2 (PIM4) to C translator
Version: 0.7
Release: 1
License: GPLv2+
Packager: %packer
Vendor: %packer <sinuhe@gnu.org>
Group: Development/Languages
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
This is a Modula-2 compiler which translates to C. The translator is
based on the language report in the 3rd and 4th editions of Wirth's book
Programming in Modula-2.

%prep
%setup -n %{name}-%{version}

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure +cc=gcc
%{__make} prefix=/usr libdir=/usr/lib/m2c man1dir=%{_mandir}/man1
strip m2c

%install
%{__mkdir_p} %{buildroot}/usr/{bin,include,lib/m2c,share/{info,man/man1}}
%{__make} install prefix=%{buildroot}/usr \
	libdir=%{buildroot}/usr/lib/m2c \
	man1dir=%{buildroot}%{_mandir}/man1 \
	includedir=%{buildroot}%{_includedir}
makeinfo doc/modula-2.texinfo
cp modula-2.info %{buildroot}%{_infodir}

%post
for i in %{info_files}
do /sbin/install-info %{_infodir}/$i %{_infodir}/dir 2> /dev/null || :
done
ldconfig

%preun
if test "$1" = 0; then
	for i in %{info_files}
	do /sbin/install-info --delete %{_infodir}/$i \
		%{_infodir}/dir 2> /dev/null || :
	done
fi

%clean
rm -Rf %{buildroot}

%files 
%doc COPYING
%{_bindir}/%{name}
%{_libdir}/%{name}
%{_mandir}/*
%{_includedir}/*
%{_infodir}/*

%changelog
* Sun Sep 21 2014  D. E. Evans <sinuhe@gnu.org>
- Here's an initial spec file for x86 Fedora.

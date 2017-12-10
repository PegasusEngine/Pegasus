/****************************************************************************
**
** Copyright (C) 2015 Green Hills Software. All rights reserved.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the qmake spec of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_INTEGRITY_PLATFORMDEFS_H
#define Q_INTEGRITY_PLATFORMDEFS_H

// Get Qt defines/settings

#include "qglobal.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define __STDC_CONSTANT_MACROS

#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
// INTEGRITY doesn't have the System V <sys/shm.h> header. This is not a standard
// POSIX header, it's only documented in the Single UNIX Specification.
// The preferred POSIX compliant way to share memory is to use the functions
// in <sys/mman.h> that comply with the POSIX Real Time Interface (1003.1b).
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif

// for htonl
#include <arpa/inet.h>

#define QT_SNPRINTF ::snprintf
#define QT_VSNPRINTF ::vsnprintf

// INTEGRITY doesn't have getpagesize()
inline int getpagesize()
{
    return sysconf(_SC_PAGESIZE);
}

// geteuid() is only available with multi-process posix, but we do not want
// to rely on it
inline uid_t geteuid(void)
{
    return 0;
}

// getuid() is only available with multi-process posix, but we do not want
// to rely on it
inline uid_t getuid(void)
{
    return 0;
}

#include "../posix/qplatformdefs.h"
#undef QT_OPEN_LARGEFILE
#define QT_OPEN_LARGEFILE 0
#define PATH_MAX 1024

#endif // Q_INTEGRITY_PLATFORMDEFS_H

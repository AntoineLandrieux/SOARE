#ifndef __SOARE_PLATFORM_H__
#define __SOARE_PLATFORM_H__ 0x1

/* #pragma once */

/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <platform.h>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

#if defined(_WIN32)
#define __PLATFORM__ "Windows" // Windows

#elif defined(__ANDROID__)
#define __PLATFORM__ "Android" // Android

#elif defined(__linux__)
#define __PLATFORM__ "Linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other...

#elif defined(__APPLE__) && defined(__MACH__)
#define __PLATFORM__ "MacOS" // Apple iOS

#elif defined(__hpux)
#define __PLATFORM__ "HP-UX" // HP-UX

#elif defined(_AIX)
#define __PLATFORM__ "AIX" // IBM AIX

#elif defined(__sun) && defined(__SVR4)
#define __PLATFORM__ "Solaris" // Oracle Solaris, Open Indiana

#elif defined(__unix__)
#define __PLATFORM__ "Unix" // Unix

#else
#define __PLATFORM__ "Unknown OS" // Other

#endif

#endif /* __SOARE_PLATFORM_H__ */
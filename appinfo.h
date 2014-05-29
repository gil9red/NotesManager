#ifndef APPINFO_H
#define APPINFO_H

#define stringify(v1) #v1
#define quote(v1) stringify(v1)

#define V_MAJOR 0
#define V_MINOR 0
#define V_BUILD 3
#define V_REVISION 0
#define V_ALPHA 0
#define V_RC 0

#define V_VERSION  V_MAJOR.V_MINOR.V_BUILD.V_REVISION
#define V_SVERSION  V_MAJOR.V_MINOR.V_BUILD

#if V_ALPHA>0
#define V_SVERSION_STR quote(V_SVERSION)"-a"
#elif V_RC>0
#define V_SVERSION_STR quote(V_SVERSION)"-rc"quote(V_RC)
#else
#define V_SVERSION_STR quote(V_SVERSION)
#endif

#define VER_FILEVERSION             V_MAJOR,V_MINOR,V_BUILD,V_REVISION
#define VER_FILEVERSION_STR         quote(V_VERSION)

#define VER_PRODUCTVERSION          V_MAJOR,V_MINOR,V_BUILD,V_REVISION
#define VER_PRODUCTVERSION_STR      quote(V_SVERSION)

#define VER_COMPANYNAME_STR         "Ilya Petrash"
#define VER_FILEDESCRIPTION_STR     "NotesManager"
#define VER_INTERNALNAME_STR        "NotesManager"
#define VER_ORIGINALFILENAME_STR    "NotesManager.exe"
#define VER_PRODUCTNAME_STR         "NotesManager"
#define VER_COMMENTS                "NotesManager is a program that will help you store and manage your notes"

/*! \mainpage
 *  \author    Илья Петраш <gil9red@gmail.com>
 *  \version   0.0.3
 *  \date      2013-2014
 */

//! Содержит переменные, описывающие проект
namespace App
{
    static const char version[] = VER_PRODUCTVERSION_STR;
    static const char name[]    = VER_PRODUCTNAME_STR;
}

#endif // APPINFO_H

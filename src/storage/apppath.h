#ifndef APPPATH_H
#define APPPATH_H

#include <QString>

class AppPath
{
public:
    AppPath();

    static QString dataDir();
    static QString databasePath();
    static QString webEngineDir();
    static QString cacheDir();
    static QString backgroundsDir();
    static QString logsDir();

    static bool initialize();

};

#endif // APPPATH_H

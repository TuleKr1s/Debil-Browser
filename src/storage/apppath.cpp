#include "apppath.h"

#include <QStandardPaths>
#include <QDir>

AppPath::AppPath() {}

QString AppPath::dataDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

QString AppPath::databasePath()
{
    return QDir(dataDir()).filePath("Data/browser.db");
}

QString AppPath::webEngineDir()
{
    return QDir(dataDir()).filePath("WebEngine/Defaults");
}

QString AppPath::cacheDir()
{
    return QStandardPaths::writableLocation(
        QStandardPaths::CacheLocation
        );
}

QString AppPath::backgroundsDir()
{
    return QDir(dataDir()).filePath("Backgrounds");
}

QString AppPath::logsDir()
{
    return QDir(dataDir()).filePath("Logs");
}

bool AppPath::initialize()
{
    const QStringList dirs = {
        QDir(dataDir()).filePath("Data"),
        webEngineDir(),
        cacheDir(),
        backgroundsDir(),
        logsDir()
    };

    for (const QString& path : dirs) {
        if (path.isEmpty() || !QDir().mkpath(path)) {
            return false;
        }
    }

    return true;
}

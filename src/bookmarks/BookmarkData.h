#ifndef BOOKMARKDATA_H
#define BOOKMARKDATA_H

#include <optional>
#include <QByteArray>
#include <QString>

struct BookmarkData
{
    qint64 id;
    QString url;
    QString page_name;
    QByteArray page_icon;
    std::optional<qint64> id_parent;
    int serial_num;
};

struct FolderData
{
    qint64 id;
    QString folder_name;
    QByteArray folder_icon;
    std::optional<qint64> id_parent;
    int serial_num;
};

#endif // BOOKMARKDATA_H

#ifndef BOOKMARKDATABASE_H
#define BOOKMARKDATABASE_H

#include <QtSql/QSqlDatabase>
#include <optional>
#include <QByteArray>
#include <QList>

#include "BookmarkData.h"

class BookmarkDataBase
{
public:
    BookmarkDataBase();

    bool openDatabase();
    std::optional<qint64> addBookmark(QUrl url, QString title,
                                      QByteArray icon,
                                      std::optional<qint64> id_parent,
                                      int serial_num);

    std::optional<qint64> addFolder(QString title, QByteArray icon,
                                        std::optional<qint64> parent_folder,
                                        int serial_num);

    std::optional<QList<BookmarkData>> getBookmarks(std::optional<qint64> id_parent);
    std::optional<QList<FolderData>> getFolders(std::optional<qint64> id_parent);

    bool deleteBookmark(qint64 id);
    bool deleteFolder(qint64 id);
    bool updateBookmark(const BookmarkData& bookmark);

private:
    QSqlDatabase m_database;

    bool createTables();
    bool deleteFolderRecursive(qint64 id);

    void queryErrorHandler(QSqlQuery& query);

};

#endif // BOOKMARKDATABASE_H

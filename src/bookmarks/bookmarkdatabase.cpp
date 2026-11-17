#include "bookmarkdatabase.h"

#include "../storage/apppath.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>

BookmarkDataBase::BookmarkDataBase()
{

}

bool BookmarkDataBase::openDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", "bookmarks_connection");
    m_database.setDatabaseName(AppPath::databasePath());

    if (m_database.open())
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Database successfully opened";

        QSqlQuery query(m_database);
        if (!query.exec("PRAGMA foreign_keys = ON"))
        {
            queryErrorHandler(query);
            return false;
        }

        if (this->createTables())
            qDebug() << "[INFO][BOOKMARKS DATABASE] Tables successfully created";
        else
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to create tables";
            return false;
        }

        return true;
    }
    else
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to open database: "
                 << m_database.lastError().text();
        return false;
    }
}

bool BookmarkDataBase::createTables()
{
    QSqlQuery query(m_database);
    if (!query.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS bookmarks_folders (
            id INTEGER PRIMARY KEY NOT NULL,
            folder_name TEXT NOT NULL,
            folder_icon BLOB,
            id_parent INTEGER,
            serial_num INTEGER NOT NULL,
            FOREIGN KEY (id_parent) REFERENCES bookmarks_folders(id)
        );
    )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    if (!query.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS bookmarks (
            id INTEGER PRIMARY KEY NOT NULL,
            url TEXT NOT NULL,
            page_name TEXT NOT NULL,
            page_icon BLOB,
            id_parent INTEGER,
            serial_num INTEGER NOT NULL,
            FOREIGN KEY (id_parent) REFERENCES bookmarks_folders(id)
        );
    )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    return true;
}

std::optional<qint64> BookmarkDataBase::addFolder(QString title, QByteArray icon,
                                std::optional<qint64> parent_folder, int serial_num)
{
    QSqlQuery query(m_database);
    if (!query.prepare(R"SQL(
        INSERT INTO bookmarks_folders (folder_name, folder_icon, id_parent, serial_num)
        VALUES (:folder_name, :folder_icon, :id_parent, :serial_num);
    )SQL"))
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    query.bindValue(":folder_name", title);
    query.bindValue(":folder_icon", icon.isEmpty() ?
                                        QVariant(QMetaType::fromType<QByteArray>())
                                                   : icon);
    query.bindValue(":id_parent", parent_folder ?
                                      QVariant(parent_folder.value())
                                                : QVariant(QMetaType::fromType<qint64>()));
    query.bindValue(":serial_num", serial_num);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    bool ok = false;
    std::optional<qint64> id = query.lastInsertId().toLongLong(&ok);

    if (ok)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Folder was successfully added to table \"bookmarks_folders\"";
        return id;
    }
    else
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed convertating id from LongLong "
                    "while adding the folder to database";
        return std::nullopt;
    }

}

std::optional<qint64> BookmarkDataBase::addBookmark(QUrl url, QString title, QByteArray icon,
                                            std::optional<qint64> id_parent, int serial_num)
{
    QSqlQuery query(m_database);

    if (!query.prepare(R"SQL(
            INSERT INTO bookmarks (url, page_name, page_icon, id_parent, serial_num)
            VALUES (:url, :page_name, :page_icon, :id_parent, :serial_num);
        )SQL"))
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    query.bindValue(":url", url.toString());
    query.bindValue(":page_name", title);
    query.bindValue(":page_icon", icon.isEmpty() ?
                                        QVariant(QMetaType::fromType<QByteArray>())
                                                : icon);
    query.bindValue(":id_parent", id_parent ?
                                      QVariant(id_parent.value())
                                            : QVariant(QMetaType::fromType<qint64>()));
    query.bindValue(":serial_num", serial_num);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    bool ok = false;
    std::optional<qint64> id = query.lastInsertId().toLongLong(&ok);

    if (ok)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Bookmark was successfully added to table \"bookmarks\"";
        return id;
    }
    else
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed convertating id from LongLong "
                    "while adding the bookmark to database";
        return std::nullopt;
    }
}

std::optional<QList<BookmarkData>> BookmarkDataBase::getBookmarks(std::optional<qint64> id_parent)
{
    QSqlQuery query(m_database);
    if (!query.prepare(R"SQL(
            SELECT id, url, page_name, page_icon, serial_num
            FROM bookmarks
            WHERE id_parent IS :id_parent
            ORDER BY serial_num ASC, id ASC
        )SQL"))
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    query.bindValue(":id_parent", id_parent ?
                                       QVariant(id_parent.value())
                                             : QVariant(QMetaType::fromType<qint64>()));

    if (!query.exec())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    QList<BookmarkData> bookmarks_list;
    while(query.next())
    {
        BookmarkData bookmark;

        bookmark.id = query.value("id").toLongLong();
        bookmark.url = query.value("url").toString();
        bookmark.page_name = query.value("page_name").toString();
        bookmark.page_icon = query.value("page_icon").toByteArray();
        bookmark.id_parent = id_parent;
        bookmark.serial_num = query.value("serial_num").toInt();

        bookmarks_list.append(bookmark);
    }

    if (query.lastError().isValid())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    return bookmarks_list;
}

std::optional<QList<FolderData>> BookmarkDataBase::getFolders(std::optional<qint64> id_parent)
{
    QSqlQuery query(m_database);
    if (!query.prepare(R"SQL(
            SELECT id, folder_name, folder_icon, serial_num
            FROM bookmarks_folders
            WHERE id_parent IS :id_parent
            ORDER BY serial_num ASC, id ASC
        )SQL"))
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    query.bindValue(":id_parent", id_parent ?
                                      QVariant(id_parent.value())
                                            : QVariant(QMetaType::fromType<qint64>()));

    if (!query.exec())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    QList<FolderData> folders_list;
    while(query.next())
    {
        FolderData folder;

        folder.id = query.value("id").toLongLong();
        folder.folder_name = query.value("folder_name").toString();
        folder.folder_icon = query.value("folder_icon").toByteArray();
        folder.id_parent = id_parent;
        folder.serial_num = query.value("serial_num").toInt();

        folders_list.append(folder);
    }

    if (query.lastError().isValid())
    {
        queryErrorHandler(query);
        return std::nullopt;
    }

    return folders_list;
}

bool BookmarkDataBase::deleteBookmark(qint64 id)
{
    QSqlQuery query(m_database);

    // getting page_name of deleting bookmark
    if (!query.prepare(R"SQL(
            SELECT page_name
            FROM bookmarks
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);
    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    QString page_name;
    if (query.next())
    {
        page_name = query.value("page_name").toString();
    }
    else
    {
        if (query.lastError().isValid())
        {
            queryErrorHandler(query);
            return false;
        }
    }



    // deleting bookmark by id
    if (!query.prepare(R"SQL(
            DELETE FROM bookmarks
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    if (query.numRowsAffected() == 1)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE]"
                 << "Bookmark DELETE executed, affected rows:"
                 << query.numRowsAffected() << "ID:" << id;
        return true;
    }
    else
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] The bookmark with id: "
                 << id << ", was not found";
        return false;
    }

}

bool BookmarkDataBase::deleteFolder(qint64 id)
{
    if (!m_database.transaction())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to start transaction: "
                 << m_database.lastError().text();
        return false;
    }

    if (!deleteFolderRecursive(id))
    {
        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Rollback failed: "
                     << m_database.lastError().text();
        }
        return false;
    }

    if (!m_database.commit())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Commit failed: "
                 << m_database.lastError().text();

        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Rollback failed: "
                     << m_database.lastError().text();
        }
        return false;
    }

    return true;
}

bool BookmarkDataBase::deleteFolderRecursive(qint64 id)
{
    QSqlQuery query(m_database);

    // getting page_name of deleting folder
    if (!query.prepare(R"SQL(
            SELECT folder_name
            FROM bookmarks_folders
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);
    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    QString folder_name;
    if (query.next())
    {
        folder_name = query.value("folder_name").toString();
    }
    else
    {
        if (query.lastError().isValid())
        {
            queryErrorHandler(query);
            return false;
        }
    }

    // going to last folder in the hierarchy
    if (!query.prepare(R"SQL(
            SELECT id
            FROM bookmarks_folders
            WHERE id_parent = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    while (query.next())
    {
        qint64 child_id = query.value("id").toLongLong();
        if (!deleteFolderRecursive(child_id))
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to delete folder with id: " << child_id;
            return false;
        }
    }

    if (query.lastError().isValid())
    {
        queryErrorHandler(query);
        return false;
    }

    // deleting every the bookmark from the folder
    if(!query.prepare(R"SQL(
            SELECT id
            FROM bookmarks
            WHERE id_parent = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    while (query.next())
    {
        qint64 bookmark_id = query.value("id").toLongLong();
        if (!this->deleteBookmark(bookmark_id))
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to delete bookmark with id: " << bookmark_id;
            return false;
        }

    }

    if (query.lastError().isValid())
    {
        queryErrorHandler(query);
        return false;
    }

    if (!query.prepare(R"SQL(
            DELETE FROM bookmarks_folders
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", id);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    if (query.numRowsAffected() == 1)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE]"
                 << "Folder DELETE executed, affected rows:"
                 << query.numRowsAffected() << "ID:" << id;
        return true;
    }
    else
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] The folder with id: "
                 << id << ", was not found";
        return false;
    }
}

bool BookmarkDataBase::updateBookmark(const BookmarkData& bookmark)
{

    QSqlQuery query(m_database);
    if (!query.prepare(R"SQL(
            UPDATE bookmarks
            SET url = :url,
                page_name = :page_name,
                page_icon = :page_icon
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", bookmark.id);
    query.bindValue(":url", bookmark.url);
    query.bindValue(":page_name", bookmark.page_name);
    query.bindValue(":page_icon", bookmark.page_icon.isEmpty() ?
                                      QVariant(QMetaType::fromType<QByteArray>())
                                                 : bookmark.page_icon);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    if (query.numRowsAffected() > 0)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Data of the bookmark \""
                 << bookmark.page_name << "\" was changed";
        return true;
    }
    else
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Data of the bookmark \""
                 << bookmark.page_name << "\" was NOT changed";
        return false;
    }
}

bool BookmarkDataBase::updateFolder(const FolderData& folder)
{
    QSqlQuery query(m_database);

    if(!query.prepare(R"SQL(
            UPDATE bookmarks_folders
            SET folder_name = :folder_name,
                folder_icon = :folder_icon
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", folder.id);
    query.bindValue(":folder_name", folder.folder_name);
    query.bindValue(":folder_icon", folder.folder_icon.isEmpty() ?
                                        QVariant(QMetaType::fromType<QByteArray>())
                                                                 : folder.folder_icon);

    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }

    if (query.numRowsAffected() > 0)
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Data of the folder \""
                 << folder.folder_name << "\" was changed";
        return true;
    }
    else
    {
        qDebug() << "[INFO][BOOKMARKS DATABASE] Data of the folder \""
                 << folder.folder_name << "\" was NOT changed";
        return false;
    }
}

bool BookmarkDataBase::moveBookmark(qint64 bookmark_id,
                                    std::optional<qint64> new_folder,
                                    int serial_num)
{
    if (serial_num < 0)
        return false;

    if (!m_database.transaction())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to start move transaction:"
                 << m_database.lastError().text();
        return false;
    }

    if (!moveBookmarkInTransaction(bookmark_id, new_folder, serial_num))
    {
        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Move rollback failed:"
                     << m_database.lastError().text();
        }
        return false;
    }

    if (!m_database.commit())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Move commit failed:"
                 << m_database.lastError().text();

        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Move rollback failed:"
                     << m_database.lastError().text();
        }
        return false;
    }

    qDebug() << "[INFO][BOOKMARKS DATABASE] Bookmark moved. ID:" << bookmark_id
             << "Position:" << serial_num;
    return true;
}

bool BookmarkDataBase::moveBookmarkInTransaction(qint64 bookmark_id,
                                                std::optional<qint64> new_folder,
                                                int serial_num)
{
    QSqlQuery query(m_database);

    // Getting the current parent of the bookmark.
    if (!query.prepare(R"SQL(
            SELECT id_parent FROM bookmarks WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }
    query.bindValue(":id", bookmark_id);
    if (!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }
    if (!query.next())
    {
        if (query.lastError().isValid())
            queryErrorHandler(query);
        return false;
    }
    std::optional<qint64> old_folder;
    if (!query.isNull("id_parent"))
        old_folder = query.value("id_parent").toLongLong();
    query.finish();

    // Checking that the destination folder exists. NULL means the root.
    if (new_folder)
    {
        if (!query.prepare(R"SQL(
                SELECT id FROM bookmarks_folders WHERE id = :id
            )SQL"))
        {
            queryErrorHandler(query);
            return false;
        }
        query.bindValue(":id", new_folder.value());
        if (!query.exec())
        {
            queryErrorHandler(query);
            return false;
        }
        if (!query.next())
        {
            if (query.lastError().isValid())
                queryErrorHandler(query);
            return false;
        }
        query.finish();
    }

    // Getting the bookmarks in their saved order.
    std::optional<QList<BookmarkData>> result = getBookmarks(old_folder);
    if (!result)
        return false;
    QList<BookmarkData> old_folder_bookmarks = result.value();

    int old_position = -1;
    for (int i = 0; i < old_folder_bookmarks.size(); ++i)
    {
        if (old_folder_bookmarks[i].id == bookmark_id)
        {
            old_position = i;
            break;
        }
    }
    if (old_position < 0)
        return false;

    // Removing the bookmark from its old place in the list.
    BookmarkData moved_bookmark = old_folder_bookmarks.takeAt(old_position);
    QList<BookmarkData> new_folder_bookmarks;
    if (old_folder == new_folder)
    {
        new_folder_bookmarks = old_folder_bookmarks;
    }
    else
    {
        result = getBookmarks(new_folder);
        if (!result)
            return false;
        new_folder_bookmarks = result.value();
    }

    // The last allowed position is the end of the destination list.
    if (serial_num > new_folder_bookmarks.size())
        return false;
    new_folder_bookmarks.insert(serial_num, moved_bookmark);

    QList<BookmarkData> changed_bookmarks;

    // Closing the gap in the source folder when moving to another folder.
    if (old_folder != new_folder)
    {
        for (int i = 0; i < old_folder_bookmarks.size(); ++i)
        {
            if (old_folder_bookmarks[i].serial_num != i)
            {
                old_folder_bookmarks[i].serial_num = i;
                changed_bookmarks.append(old_folder_bookmarks[i]);
            }
        }
    }

    // Numbering the destination list and remembering only changed bookmarks.
    for (int i = 0; i < new_folder_bookmarks.size(); ++i)
    {
        if (new_folder_bookmarks[i].serial_num != i
            || new_folder_bookmarks[i].id_parent != new_folder)
        {
            new_folder_bookmarks[i].serial_num = i;
            new_folder_bookmarks[i].id_parent = new_folder;
            changed_bookmarks.append(new_folder_bookmarks[i]);
        }
    }

    // Saving the changed positions. The public method commits or rolls back.
    if (!query.prepare(R"SQL(
            UPDATE bookmarks
            SET id_parent = :id_parent, serial_num = :serial_num
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }
    for (int i = 0; i < changed_bookmarks.size(); ++i)
    {
        BookmarkData bookmark = changed_bookmarks[i];
        query.bindValue(":id", bookmark.id);
        query.bindValue(":id_parent", bookmark.id_parent ?
                                          QVariant(bookmark.id_parent.value())
                                                : QVariant(QMetaType::fromType<qint64>()));
        query.bindValue(":serial_num", bookmark.serial_num);
        if (!query.exec())
        {
            queryErrorHandler(query);
            return false;
        }
        if (query.numRowsAffected() != 1)
            return false;
    }
    return true;
}

bool BookmarkDataBase::moveFolder(qint64 folder_id,
                                  std::optional<qint64> new_folder,
                                  int serial_num)
{

    if (serial_num < 0)
        return false;

    if (!m_database.transaction())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Failed to start move transaction:"
                 << m_database.lastError().text();
        return false;
    }

    if (!moveFolderInTransaction(folder_id, new_folder, serial_num))
    {
        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Move rollback failed:"
                     << m_database.lastError().text();
        }
        return false;
    }

    if (!m_database.commit())
    {
        qDebug() << "[ERROR][BOOKMARKS DATABASE] Move commit failed:"
                 << m_database.lastError().text();

        if (!m_database.rollback())
        {
            qDebug() << "[ERROR][BOOKMARKS DATABASE] Move rollback failed:"
                     << m_database.lastError().text();
        }
        return false;
    }

    qDebug() << "[INFO][BOOKMARKS DATABASE] Folder moved. ID:" << folder_id
             << "Position:" << serial_num;
    return true;

}

bool BookmarkDataBase::moveFolderInTransaction(qint64 folder_id,
                                               std::optional<qint64> new_folder,
                                               int serial_num)
{
    QSqlQuery query(m_database);

    std::optional<qint64> current_parent = new_folder;
    QList<qint64> checked_folders;

    while (current_parent)
    {
        if (current_parent.value() == folder_id)
            return false;

        if (checked_folders.contains(current_parent.value()))
            return false;

        checked_folders.append(current_parent.value());

        if (!query.prepare(R"SQL(
            SELECT id_parent
            FROM bookmarks_folders
            WHERE id = :id
        )SQL"))
        {
            queryErrorHandler(query);
            return false;
        }

        query.bindValue(":id", current_parent.value());

        if (!query.exec())
        {
            queryErrorHandler(query);
            return false;
        }

        if (!query.next())
        {
            if (query.lastError().isValid())
                queryErrorHandler(query);

            return false;
        }

        if (query.isNull("id_parent"))
            current_parent = std::nullopt;
        else
            current_parent = query.value("id_parent").toLongLong();

        query.finish();
    }

    if (!query.prepare(R"SQL(
            SELECT id_parent FROM bookmarks_folders WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }

    query.bindValue(":id", folder_id);
    if(!query.exec())
    {
        queryErrorHandler(query);
        return false;
    }
    if (!query.next())
    {
        if (query.lastError().isValid())
            queryErrorHandler(query);
        return false;
    }

    std::optional<qint64> old_folder;
    if (!query.isNull("id_parent"))
        old_folder = query.value("id_parent").toLongLong();
    query.finish();


    std::optional<QList<FolderData>> result = getFolders(old_folder);
    if (!result)
        return false;
    QList<FolderData> old_folder_folders = result.value();

    int old_position = -1;
    for (int i = 0; i < old_folder_folders.size(); ++i)
    {
        if (old_folder_folders[i].id == folder_id)
        {
            old_position = i;
            break;
        }
    }
    if (old_position < 0)
        return false;


    FolderData moved_folder = old_folder_folders.takeAt(old_position);
    QList<FolderData> new_folder_folders;
    if (old_folder == new_folder)
    {
        new_folder_folders = old_folder_folders;
    }
    else
    {
        result = getFolders(new_folder);
        if(!result)
            return false;
        new_folder_folders = result.value();
    }

    if (serial_num > new_folder_folders.size())
        return false;
    new_folder_folders.insert(serial_num, moved_folder);

    QList<FolderData> changed_folders;

    if (old_folder != new_folder)
    {
        for (int i =0; i < old_folder_folders.size(); ++i)
        {
            if (old_folder_folders[i].serial_num != i)
            {
                old_folder_folders[i].serial_num = i;
                changed_folders.append(old_folder_folders[i]);
            }
        }
    }

    for (int i = 0; i < new_folder_folders.size(); ++i)
    {
        if (new_folder_folders[i].serial_num != i
            || new_folder_folders[i].id_parent != new_folder)
        {
            new_folder_folders[i].serial_num = i;
            new_folder_folders[i].id_parent = new_folder;
            changed_folders.append(new_folder_folders[i]);
        }
    }

    if (!query.prepare(R"SQL(
            UPDATE bookmarks_folders
            SET id_parent = :id_parent, serial_num = :serial_num
            WHERE id = :id
        )SQL"))
    {
        queryErrorHandler(query);
        return false;
    }
    for (int i = 0; i < changed_folders.size(); ++i)
    {
        FolderData folder = changed_folders[i];
        query.bindValue(":id", folder.id);
        query.bindValue(":id_parent", folder.id_parent ?
                                          QVariant(folder.id_parent.value())
                                                         : QVariant(QMetaType::fromType<qint64>()));
        query.bindValue(":serial_num", folder.serial_num);
        if (!query.exec())
        {
            queryErrorHandler(query);
            return false;
        }
        if (query.numRowsAffected() != 1)
            return false;
    }

    return true;
}

void BookmarkDataBase::queryErrorHandler(QSqlQuery& query)
{
    qDebug() << "[ERROR][BOOKMARKS DATABASE] Error occurred while processing the query:"
             << query.lastQuery() << query.lastError().text();
}





















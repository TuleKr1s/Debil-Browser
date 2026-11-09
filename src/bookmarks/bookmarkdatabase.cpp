#include "bookmarkdatabase.h"

#include "../storage/apppath.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

BookmarkDataBase::BookmarkDataBase()
{

}

bool BookmarkDataBase::openDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE", "bookmarks_connection");
    m_database.setDatabaseName(AppPath::databasePath());

    if (m_database.open())
    {
        qDebug() << "[INFO] Bookmark database successfully opened";

        QSqlQuery query(m_database);
        if (!query.exec("PRAGMA foreign_keys = ON"))
        {
            queryErrorHandler(query);
            return false;
        }

        if (this->createTables())
            qDebug() << "[INFO] Tables successfully created";
        else
        {
            qDebug() << "[ERROR] Failed to create tables";
            return false;
        }

        return true;
    }
    else
    {
        qDebug() << "[ERROR] Failed to open bookmark database: "
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

void BookmarkDataBase::queryErrorHandler(QSqlQuery& query)
{
    qDebug() << "[ERROR] Error occurred while processing the query:"
             << query.lastQuery() << query.lastError().text();
}





















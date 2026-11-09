#ifndef BOOKMARKDATABASE_H
#define BOOKMARKDATABASE_H

#include <QtSql/QSqlDatabase>


class BookmarkDataBase
{
public:
    BookmarkDataBase();

    bool openDatabase();

private:
    QSqlDatabase m_database;

    bool createTables();

    void queryErrorHandler(QSqlQuery& query);

};

#endif // BOOKMARKDATABASE_H

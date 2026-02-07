#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QItemSelectionModel>
#include "dbfunctions.h"
#include "myrect.h"

//-------------------------------------------------------------------
//
bool bd::dbCreateTable(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

    QSqlQuery query(db);

    QString qstr = "CREATE TABLE " + table + " ( "
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "id_type INTEGER, "
                   "pencolor VARCHAR, "
                   "penstyle INTEGER, "
                   "penwidth INTEGER, "
                   "left INTEGER, "
                   "top INTEGER, "
                   "width INTEGER, "
                   "height INTEGER, "
                   "FOREIGN KEY(id_type) REFERENCES type(id) );";

    bool b = query.exec(qstr);
    if (query.lastError().isValid())
    {
        qDebug()<<query.lastError();
    }
    return b;
}

//-------------------------------------------------------------------
//
bool bd::dbDeleteTable(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

    QStringList list = db.tables();
    if (list.contains(table))
    {
        QString qstr = "DROP TABLE " + table + " ;";

        QSqlQuery query(qstr, db);

        if (query.lastError().isValid())
        {
            qDebug()<<query.lastError();
            return false;
        }
        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//
bool bd::dbInsertInto(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

     QSqlQuery query(db);

     QString qstr = "INSERT INTO " + table + " (pencolor, penstyle) "
                    "VALUES ('#ff0000', 1) ;";

     bool b = query.exec(qstr);
     if (query.lastError().isValid())
     {
         qDebug()<<query.lastError();
     }
     return b;
}

//-------------------------------------------------------------------
//
bool bd::dbInsertIntoOracle(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

     QSqlQuery query(db);

     QString qstr = "INSERT INTO " + table + " (id, type_name) "
                                             "VALUES (:id, :type_name, :width, :height) ;";
     query.prepare(qstr);

     query.bindValue(":left", 10);
     query.bindValue(":top", 100);
     query.bindValue(":width", 50);
     query.bindValue(":height", 500);

     bool b = query.exec();
     if (query.lastError().isValid())
     {
         qDebug()<<query.lastError();
     }
     return b;
}

//-------------------------------------------------------------------
//
bool bd::dbInsertIntoODBC(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

     QSqlQuery query(db);

     QString qstr = "INSERT INTO " + table + " (type_name) "
                                             "VALUES (?) ;";
     query.prepare(qstr);

     QVector<QString> rectVec = {"rectangle", "ellipse"};


     bool b = false;
     for(const auto& val : rectVec)
     {
         query.addBindValue(val);

         b = query.exec ();
         if (query.lastError().isValid())
         {
             qDebug()<<query.lastError();
             break;
         }
     }
     return b;
}

//-------------------------------------------------------------------
//
bool bd::dbInsertIntoODBCbind(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

     QSqlQuery query(db);

     QString qstr = "INSERT INTO " + table + " (id_type, pencolor, penstyle, penwidth, left, top, width, height ) "
                                             "VALUES (?, ?, ?, ?, ?, ?, ?, ?) ;";
     query.prepare(qstr);

     QVector<MyRect> rectVec = {MyRect(QRect(10,100,10,10), QPen(QColor("#f0f010"), 2, Qt::SolidLine)),
                                MyRect(QRect(10,0,200,100), QPen(QColor("#f000ff"), 1, Qt::DashLine)),
                                MyRect(QRect(8,80,11,12),   QPen(QColor("#0fff0a"), 2, Qt::NoPen)),
                                MyRect(QRect(6,0,40,103),   QPen(QColor("#00a0ff"), 1, Qt::DotLine))};


     bool b = false;
     for(const auto& val : rectVec)
     {
         query.bindValue(0, val.type());
         query.bindValue(1, val.color());
         query.bindValue(2, static_cast<uint>(val.penStyle()));
         query.bindValue(3, val.penWidth());
         query.bindValue(4, val.rect().left());
         query.bindValue(5, val.rect().top());
         query.bindValue(6, val.rect().width());
         query.bindValue(7, val.rect().height());

         b = query.exec ();
         if (query.lastError().isValid())
         {
             qDebug()<<query.lastError();
             break;
         }
     }
     return b;
}

//-------------------------------------------------------------------
//
bool bd::dbPrintTable(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

     QSqlQuery query(db);

     QString qstr = "SELECT * FROM " + table + " ;";

     bool b = query.exec(qstr);
     if (query.lastError().isValid())
     {
         qDebug()<<query.lastError();
         return b;
     }

     while (query.next())
     {
         QSqlRecord rec = query.record();
         int id = query.value(rec.indexOf("id")).toInt();
         QVariant color = query.value(rec.indexOf("pencolor"));
         QVariant style = query.value(rec.indexOf("penstyle"));
         int pwidth = query.value(rec.indexOf("penwidth")).toInt();
         int left= query.value(rec.indexOf("left")).toInt();
         int top= query.value(rec.indexOf("top")).toInt();
         int width= query.value(rec.indexOf("width")).toInt();
         int height= query.value(rec.indexOf("height")).toInt();

         qDebug() << id << color.value<QColor>() << style.value<int>() << pwidth << left << top << width << height;
     }
     return b;

}

//-------------------------------------------------------------------
//
bool bd::dbSelectTable(const QString &connectName, const QString &table, QItemSelectionModel *selection)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

    QSqlQuery query(db);

    QString qstr = "SELECT * FROM " + table + " ;";

    bool b = query.exec(qstr);
    if (query.lastError().isValid())
    {
        qDebug()<<query.lastError();
        return b;
    }

    do
    {
        QSqlRecord rec = query.record();
        int id = query.value(rec.indexOf("id")).toInt();
        QModelIndex index = selection->model()->index(id,0);
        QItemSelection sel;
        sel.select(index, index);
        if (index.isValid())
        selection->select(sel, QItemSelectionModel::Rows | QItemSelectionModel::Select);
    }
    while (query.next());

    return b;
}

//-------------------------------------------------------------------
//
bool bd::dbCreateTableRelat(const QString &connectName, const QString &table)
{
    QSqlDatabase db = QSqlDatabase::database(connectName);
    if (!db.isValid()) return false;

    QSqlQuery query(db);

    QString qstr = "CREATE TABLE " + table + " ( "
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "type_name VARCHAR );";

    bool b = query.exec(qstr);
    if (query.lastError().isValid())
    {
        qDebug()<<query.lastError();
    }
    return b;
}

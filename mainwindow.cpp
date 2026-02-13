#include <QtSql>
#include <QSqlDatabase>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbfunctions.h"
#include "mydelegate.h"

//-------------------------------------------------------------------
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    menu_(new QMenu(this)),
    createConnection_(new QAction("Create connection", this)),
    closeConnection_(new QAction("Close connection", this)),
    createTable_(new QAction("Create table", this)),
    insertInto_(new QAction("Insert into", this)),
    insertIntoOracle_(new QAction("Oracle Insert into", this)),
    insertIntoODBC_(new QAction("Insert into SECOND", this)),
    insertIntoODBCbind_(new QAction("Insert into FIRST", this)),
    printTable_(new QAction("Print table", this)),
    dropTable_(new QAction("Drop table", this)),
    initTableModel_(new QAction("Init tablemodel", this)),
    selectTable_(new QAction("Select table", this)),
    insertRow_(new QAction("Insert row", this)),
    remoweRow_(new QAction("Remove row", this)),
    doQuery_(new QAction("Do query", this)),
    connectName_("myConnection"),
    tableNameFirst_("rectangle"),
    tableNameSecond_("type")
{
    ui->setupUi(this);
    this->setMinimumWidth(2000);
    this->setMinimumHeight(1000);

    createmenu();
    connections();
}

//-------------------------------------------------------------------
//
MainWindow::~MainWindow()
{
    closeConnection();
    delete ui;
}

//-------------------------------------------------------------------
//
void MainWindow::createmenu()
{
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setMinimumWidth(1600);

    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_2->horizontalHeader()->setStretchLastSection(true);

    menu_ = menuBar()->addMenu("BD");
    menu_->addAction(createConnection_);
    menu_->addAction(closeConnection_);
    menu_->addAction(createTable_);

    menu_->addAction(insertIntoODBC_);
    menu_->addAction(insertIntoODBCbind_);

    menu_->addAction(dropTable_);

    menu_ = menuBar()->addMenu("Model");
    menu_->addAction(initTableModel_);
}

//-------------------------------------------------------------------
//
void MainWindow::connections()
{
    connect(createConnection_, SIGNAL(triggered()), this, SLOT(createConnection()));
    connect(closeConnection_, SIGNAL(triggered()), this, SLOT(closeConnection()));
    connect(createTable_, SIGNAL(triggered()), this, SLOT(createTable()));
    connect(dropTable_, SIGNAL(triggered()), this, SLOT(deleteTable()));
    connect(insertIntoODBC_, SIGNAL(triggered()), this, SLOT(insertIntoODBC()));
    connect(insertIntoODBCbind_, SIGNAL(triggered()), this, SLOT(insertIntoODBCbind()));
    connect(initTableModel_, SIGNAL(triggered()), this, SLOT(initModel()));
}

//-------------------------------------------------------------------
//
void MainWindow::createConnection()
{
    for (QString strDrv : QSqlDatabase::drivers())
        qDebug()<< strDrv;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectName_);
    db.isValid() ? qDebug()<<"Connected" : qDebug()<<db.lastError().isValid();

    db.setDatabaseName("rectangle_data.sqlite");

    if (!db.open())
        qDebug() << "Error to open";
    return;
}

//-------------------------------------------------------------------
//
void MainWindow::closeConnection()
{
    {                                                                           // { ВАЖНО!
        QSqlDatabase db = QSqlDatabase::database(connectName_);
        if (!db.isOpen()) return;
        db.close();
    }                                                                           // } ВАЖНО!
    QSqlDatabase::removeDatabase(connectName_);
    qDebug() << "Close connection";

}

//-------------------------------------------------------------------
//
void MainWindow::createTable()
{
    bool tmp1 = bd::dbCreateTableRelat(connectName_, tableNameSecond_);
    qDebug() << "Create second table: " << tmp1;

    bool tmp = bd::dbCreateTable(connectName_, tableNameFirst_);
    qDebug() << "Create first table: " << tmp;
}

//-------------------------------------------------------------------
//
void MainWindow::deleteTable()
{
    bool tmp = bd::dbDeleteTable(connectName_, tableNameFirst_);
    qDebug() << "Drop first table: " << tmp;
    bool tmp1 = bd::dbDeleteTable(connectName_, tableNameSecond_);
    qDebug() << "Drop second table: " << tmp1;
}

//-------------------------------------------------------------------
//
void MainWindow::insertIntoODBC()
{
    bool tmp = bd::dbInsertIntoODBC(connectName_, tableNameSecond_);
    qDebug() << "ODBC_addBindValue_InsertInto: " << tmp;

}

//-------------------------------------------------------------------
//
void MainWindow::insertIntoODBCbind()
{
    bool tmp = bd::dbInsertIntoODBCbind(connectName_, tableNameFirst_);
    qDebug() << "ODBC_bindValue_InsertInto: " << tmp;

}

//-------------------------------------------------------------------
//
void MainWindow::initModel()
{
    initModelSecond();
    initModelFirst();

}
//-------------------------------------------------------------------
//
void MainWindow::initModelFirst()
{
    QSqlDatabase db = QSqlDatabase::database(connectName_);
    if (!db.isValid()) return ;

    relatModel_ = new QSqlRelationalTableModel(this, db);

    relatModel_->setTable(tableNameFirst_);
    relatModel_->select();
    relatModel_->setEditStrategy(QSqlTableModel::OnFieldChange);    //OnFieldChange 	0	Изменения модели применяются сразу к БД
                                                                    //OnRowChange 	1 (умолч.)	Изменения строки применяются  при  выборе другой строки  или элемента
                                                                    //OnManualSubmit 	2 	Изменения  кэшируются пока не будет вызвана submitAll()

    ui->tableView->setModel(relatModel_);
    ui->tableView->hideColumn(0);

    QSqlRelation relation(tableNameSecond_, "id", "type_name");
    relatModel_->setRelation(1, relation);
    relatModel_->select();                                         // Важно!!!!!!!!!!!!!!!!!!

    QStringList headerData = {"ID", "Type" ,"Color", "Style", "Pen Width", "Left", "Top", "Width", "Height"};
    for (int i = 0; i < headerData.size(); ++i)
    {
       relatModel_->setHeaderData(i, Qt::Horizontal, headerData[i]);
    }

    MyDelegate *delegate = new MyDelegate(ui->tableView);
    ui->tableView->setItemDelegate(delegate);

    QSqlRelationalDelegate *rd = new QSqlRelationalDelegate(ui->tableView);
    ui->tableView->setItemDelegateForColumn(1, rd);
}

//-------------------------------------------------------------------
//
void MainWindow::initModelSecond()
{
    QSqlDatabase db = QSqlDatabase::database(connectName_);
    if (!db.isValid()) return ;

    model_ = new QSqlTableModel(this, db);
    model_->setTable(tableNameSecond_);
    model_->select();
    model_->setEditStrategy(QSqlTableModel::OnRowChange );  //OnFieldChange 	0	Изменения модели применяются сразу к БД
                                                             //OnRowChange 	1 (умолч.)	Изменения строки применяются  при  выборе другой строки  или элемента
                                                             //OnManualSubmit 	2 	Изменения  кэшируются пока не будет вызвана submitAll()

    ui->tableView_2->setModel(model_);

    QStringList headerData = {"id" ,"Type_Rect"};
    int i = 0;
    for (const auto & header : headerData)
    {
        model_->setHeaderData(i, Qt::Horizontal, header);
        i++;
    }
}

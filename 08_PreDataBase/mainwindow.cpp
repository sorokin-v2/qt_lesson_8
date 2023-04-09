#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */


    dataDb = new DbData(this);
    db = new DataBase(this);
    msg = new QMessageBox(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);


    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    db->AddDataBase(POSTGRE_DRIVER, DB_NAME);


    /*
     * Соединяем сигнал, который передает ответ от БД с методом, который отображает ответ в ПИ
     */
     connect(db, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    /*
     *  Сигнал для подключения к БД
     */
    connect(db, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */

void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */


    if(ui->lb_statusConnect->text() == "Отключено"){

       ui->lb_statusConnect->setText("Подключение");
       ui->lb_statusConnect->setStyleSheet("color : black");


       auto conn = [&]{db->ConnectToDataBase(dataForConnect);};
       QtConcurrent::run(conn);

    }
    else{
        db->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }

}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
         ///Тут должен быть код ДЗ
    if(ui->cb_category->currentIndex() == 0){
        sqlTableModel = new QSqlTableModel(this, *db->getDataBase());
        sqlTableModel->setTable("film");
        ui->tw_tableView->setModel(nullptr);
        sqlTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        sqlTableModel->select();
        sqlTableModel->setHeaderData(1, Qt::Horizontal, tr("Название фильма"));
        sqlTableModel->setHeaderData(2, Qt::Horizontal, tr("Описание фильма"));
        ui->tw_tableView->setModel(sqlTableModel);
        ui->tw_tableView->hideColumn(0);
        ui->tw_tableView->hideColumn(3);
        ui->tw_tableView->hideColumn(4);
        ui->tw_tableView->hideColumn(5);
        ui->tw_tableView->hideColumn(6);
        ui->tw_tableView->hideColumn(7);
        ui->tw_tableView->hideColumn(8);
        ui->tw_tableView->hideColumn(9);
        ui->tw_tableView->hideColumn(10);
        ui->tw_tableView->hideColumn(11);
        ui->tw_tableView->hideColumn(12);
        ui->tw_tableView->hideColumn(13);
        ui->tw_tableView->resizeColumnsToContents();
        ui->tw_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tw_tableView->horizontalHeader()->setStretchLastSection(true);


        ui->tw_tableView->show();
    }
    else{
        sqlQuerymodel = new QSqlQueryModel(this);
        QString request = "SELECT title as \"Название фильма\", description as \"Описание фильма\""
                          " FROM film where film_id in (select film_id from film_category "
                          " where category_id = %1)";
        ui->tw_tableView->setModel(nullptr);
        sqlQuerymodel->setQuery(request.arg(ui->cb_category->currentIndex() + 1), *db->getDataBase());
        ui->tw_tableView->setModel(sqlQuerymodel);
        ui->tw_tableView->resizeColumnsToContents();
        ui->tw_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tw_tableView->horizontalHeader()->setStretchLastSection(true);

        ui->tw_tableView->show();
    }
}

/*!
 * \brief Слот отображает значение в QTableWidget
 * \param widget
 * \param typeRequest
 */
void MainWindow::ScreenDataFromDB(const QTableWidget *widget, int typeRequest)
{

    ///Тут должен быть код ДЗ

}
/*!
 * \brief Метод изменяет стотояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if(status){
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else{
        db->DisconnectFromDataBase(DB_NAME);
        msg->setIcon(QMessageBox::Critical);
        msg->setText(db->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }

}

void MainWindow::on_pb_clear_clicked()
{
    ui->tw_tableView->setModel(nullptr);
}


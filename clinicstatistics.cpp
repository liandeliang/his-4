#include "clinicstatistics.h"
#include "connectsql.h"
#include "cliniccharge.h"
#include "connectdb.h"
extern ConnectSql sql;
ClinicStatistics::ClinicStatistics(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	listView = new QListView(this);
	model = new QStringListModel(this);
	listView->setWindowFlags(Qt::ToolTip);

	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); 
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.tableWidget_2->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget_2->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget_2->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.tableWidget_5->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget_5->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget_5->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	initUI();
	connect(ui.tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(getItem(int,int)));
	connect(ui.patientEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setCompleter(const QString &)));
}
void ClinicStatistics::initUI()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	ui.dateTimeEdit->setDateTime(current_date_time);
	ui.dateTimeEdit_2->setDateTime(current_date_time);

	ui.dateTimeEdit_3->setDateTime(current_date_time);
	QTime time= QTime::fromString("00:00:00", "hh:mm:ss");
	ui.dateTimeEdit_3->setTime(time);
	ui.dateTimeEdit_4->setDateTime(current_date_time);
	time= QTime::fromString("23:59:59", "hh:mm:ss");
	ui.dateTimeEdit_4->setTime(time);

	ui.dateTimeEdit_5->setDateTime(current_date_time);
	ui.dateTimeEdit_6->setDateTime(current_date_time);

	QSqlQuery query(*sql.db);		
	query.exec("select * from sys_department");
	QStringList list;
	while(query.next())
	{
		QString str = query.value(1).toString();
		list.append(str);
	}
//	ui.departmentcomboBox->addItems(list);

	query.exec("select * from sys_personnel");
	list.clear();
	while(query.next())
	{
		QString str = query.value(2).toString();
		list.append(str);
	}
//	ui.doctorcomboBox->addItems(list);

	query.exec("select * from sys_users");
	list.clear();
	while(query.next())
	{
		QString str = query.value(1).toString();
		list.append(str);
	}
//	ui.comboBox_2->addItems(list);

}
void ClinicStatistics::on_radioButton_clicked()
{
	if(!ui.radioButton->isChecked())
	{
		ui.dateTimeEdit_3->setEnabled(false);
		ui.dateTimeEdit_4->setEnabled(false);
	}
	if(ui.radioButton->isChecked())
	{
		ui.dateTimeEdit_3->setEnabled(true);
		ui.dateTimeEdit_4->setEnabled(true);
	}

}
void ClinicStatistics::on_queryButton_clicked()
{

	ui.tableWidget->setRowCount(0);
	QString startDate = ui.dateTimeEdit_3->dateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString endDate = ui.dateTimeEdit_4->dateTime().toString("yyyy-MM-dd hh:mm:ss");

	QString strPatientName = ui.patientEdit->text();
	QSqlQuery query(*sql.db);	
	if (ui.patientEdit->text()!=""&&ui.patientEdit->text()!=NULL)
	{
		if (ui.radioButton->isChecked())
		{
			query.exec("select * from mz_chargesheet where   date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"' and  patientname='"+strPatientName+"'");
		}
		else
		{
			query.exec("select * from mz_chargesheet where  patientname='"+strPatientName+"'");
		}
	}
	else
	{
		if (ui.radioButton->isChecked())
		{
			query.exec("select * from mz_chargesheet where   date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'");
		}
		else
		{
			query.exec("select * from mz_chargesheet");
		}
	}

	while(query.next())
	{	
		QString strsheetNo = query.value(1).toString();
		QSqlQuery query2(*sql.db);	
		query2.exec("select * from mz_chargedetail where sheetno='"+strsheetNo+"'");
		int row= 0;
		while(query2.next())
		{
			ui.tableWidget->insertRow(row);
			ui.tableWidget->setItem(row,0,new QTableWidgetItem(query2.value(3).toString()));
			ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(2).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));

			ui.tableWidget->setItem(row,2,new QTableWidgetItem(query2.value(4).toString()));
			ui.tableWidget->setItem(row,3,new QTableWidgetItem(query2.value(6).toString()));
			ui.tableWidget->setItem(row,4,new QTableWidgetItem(query2.value(5).toString()));
			ui.tableWidget->setItem(row,5,new QTableWidgetItem(query2.value(7).toString()));
			ui.tableWidget->setItem(row,6,new QTableWidgetItem(query2.value(8).toString()));

			ui.tableWidget->setItem(row,7,new QTableWidgetItem(query.value(5).toString()));
			ui.tableWidget->setItem(row,8,new QTableWidgetItem(query.value(8).toString()));

			ui.tableWidget->setItem(row,9,new QTableWidgetItem(query2.value(2).toString()));
			ui.tableWidget->setItem(row,10,new QTableWidgetItem(query.value(1).toString()));
			row++;
		}
	}
}
void ClinicStatistics::on_clearButton_clicked()
{
	ui.tableWidget->setRowCount(0);
	ui.patientEdit->clear();
	ui.lineEdit->clear();
	ui.lineEdit_2->clear();
}
void ClinicStatistics::on_radioButton_2_clicked()
{
	if(!ui.radioButton_2->isChecked())
	{
		ui.dateTimeEdit->setEnabled(false);
		ui.dateTimeEdit_2->setEnabled(false);
//		ui.comboBox_2->setEnabled(true);
	}
	if(ui.radioButton_2->isChecked())
	{
		ui.dateTimeEdit->setEnabled(true);
		ui.dateTimeEdit_2->setEnabled(true);
//		ui.comboBox_2->setEnabled(false);
	}

}
void ClinicStatistics::on_radioButton_3_clicked()
{
	//if(!ui.radioButton_3->isChecked())
	//{
	//	ui.dateTimeEdit->setEnabled(true);
	//	ui.dateTimeEdit_2->setEnabled(true);
	//	ui.comboBox_2->setEnabled(false);
	//}
	//if(ui.radioButton_3->isChecked())
	//{
	//	ui.comboBox_2->setEnabled(true);
	//	ui.dateTimeEdit->setEnabled(false);
	//	ui.dateTimeEdit_2->setEnabled(false);
	//}

}
void ClinicStatistics::on_queryButton_2_clicked()//�����ս��ѯ
{
	ui.tableWidget_2->setRowCount(0);
	QString startDate = ui.dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString endDate = ui.dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss");

	QSqlQuery query(*sql.db);		
	if(!ui.radioButton_2->isChecked())
	{
		query.exec("select * from mz_dailysummary ");
	}
	if(ui.radioButton_2->isChecked())
	{
		query.exec("select * from mz_dailysummary where makedate between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'");
	}
	int row= 0;
	while(query.next())
	{	
		ui.tableWidget_2->insertRow(row);
		ui.tableWidget_2->setItem(row,0,new QTableWidgetItem(query.value(1).toString()));
		ui.tableWidget_2->setItem(row,1,new QTableWidgetItem(query.value(2).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
		ui.tableWidget_2->setItem(row,2,new QTableWidgetItem(query.value(7).toString()));
		ui.tableWidget_2->setItem(row,3,new QTableWidgetItem(query.value(11).toString()));
		ui.tableWidget_2->setItem(row,4,new QTableWidgetItem(query.value(12).toString()));
		row++;
	}
}
void ClinicStatistics::on_clearButton_2_clicked()
{
	ui.tableWidget_2->setRowCount(0);
}
void ClinicStatistics::on_radioButton_4_clicked()
{
	if(!ui.radioButton_4->isChecked())
	{
		ui.dateTimeEdit_5->setEnabled(false);
		ui.dateTimeEdit_6->setEnabled(false);
	}
	if(ui.radioButton_4->isChecked())
	{
		ui.dateTimeEdit_5->setEnabled(true);
		ui.dateTimeEdit_6->setEnabled(true);
	}

}
void ClinicStatistics::on_queryButton_3_clicked()//����ʱ��Լ�� ��Ŀ��ѯ
{
	ui.tableWidget_5->setRowCount(0);
	ui.tableWidget_5->clear();
	QString startDate = ui.dateTimeEdit_5->dateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString endDate = ui.dateTimeEdit_6->dateTime().toString("yyyy-MM-dd hh:mm:ss");


	QDateTime current_date_time = QDateTime::currentDateTime();
	QString strtime = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

	QStringList typelist;
	typelist.append(QString::fromLocal8Bit("����"));
	QSqlQuery query(*sql.db);
	query.exec("select * from mz_receipt");
	while(query.next())
	{
		typelist.append(query.value(1).toString()+"-"+query.value(2).toString());
	}
	ui.tableWidget_5->setColumnCount(typelist.count());
	ui.tableWidget_5->setHorizontalHeaderLabels(typelist);

	if (ui.comboBox_9->currentIndex()==0)
	{
		ui.tableWidget_5->insertRow(0);
		ui.tableWidget_5->setItem(0,0,new QTableWidgetItem(QString::fromLocal8Bit("�����վ�")));
		for (int i = 1;i<typelist.count();i++)
		{
			QString strType = typelist.at(i);
			QString strsql;
			if (ui.radioButton_4->isChecked())
			{
				strsql ="select * from mz_chargedetail where itemtype='"+strType+"'and date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
			}
			else
			{
				strsql ="select * from mz_chargedetail where itemtype='"+strType+"' ";
			}
			QSqlQuery query(*sql.db);
			query.exec(strsql);
			double totalfee=0.0;
			while(query.next())
			{
				totalfee+=query.value(8).toDouble();	
			}
			ui.tableWidget_5->setItem(0,i,new QTableWidgetItem(QString::number(totalfee)));
		}
	}
	if (ui.comboBox_9->currentIndex()==1)
	{
		QSqlQuery query0(*sql.db);
		query0.exec("select * from sys_department");
		int row = 0;
		QString strsql;
		while(query0.next())
		{	
			ui.tableWidget_5->insertRow(row);
			QString strdepartment = query0.value(1).toString();
			ui.tableWidget_5->setItem(row,0,new QTableWidgetItem(strdepartment));
			QString strType = QString::fromLocal8Bit("01-��ҩ��");

			for (int i = 1;i<typelist.count();i++)
			{
				QString strType = typelist.at(i);
				if (ui.radioButton_4->isChecked())
				{
					strsql ="select * from mz_chargedetail where itemtype='"+strType+"'and date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
				}
				else
				{
					strsql ="select * from mz_chargedetail where itemtype='"+strType+"' ";
				}
				QSqlQuery query(*sql.db);
				query.exec(strsql);
				double totalfee=0.0;
				while(query.next())
				{
					QSqlQuery query2(*sql.db);
					QString strsheetno = query.value(1).toString();
					query2.exec("select * from mz_chargesheet where sheetno ='"+strsheetno+"' and department ='"+strdepartment+"'");
					if (query2.next())
					{
						totalfee+=query.value(8).toDouble();
					}		
				}
				ui.tableWidget_5->setItem(row,i,new QTableWidgetItem(QString::number(totalfee)));
			}
			row++;
		}
	}
	if (ui.comboBox_9->currentIndex()==2)
	{
		QSqlQuery query0(*sql.db);
		query0.exec("select * from sys_personnel");
		int row = 0;
		QString strsql;
		while(query0.next())
		{	
			ui.tableWidget_5->insertRow(row);
			QString strpersonnel= query0.value(2).toString();
			ui.tableWidget_5->setItem(row,0,new QTableWidgetItem(strpersonnel));
			QString strType = QString::fromLocal8Bit("01-��ҩ��");
			for (int i = 1;i<typelist.count();i++)
			{
				QString strType = typelist.at(i);
				if (ui.radioButton_4->isChecked())
				{
					strsql ="select * from mz_chargedetail where itemtype='"+strType+"'and date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
				}
				else
				{
					strsql ="select * from mz_chargedetail where itemtype='"+strType+"' ";
				}
				QSqlQuery query(*sql.db);
				query.exec(strsql);
				double totalfee=0.0;
				while(query.next())
				{
					QSqlQuery query2(*sql.db);
					QString strsheetno = query.value(1).toString();
					query2.exec("select * from mz_chargesheet where sheetno ='"+strsheetno+"' and doctor ='"+strpersonnel+"'");
					if (query2.next())
					{
						totalfee+=query.value(8).toDouble();
					}		
				}
				ui.tableWidget_5->setItem(row,i,new QTableWidgetItem(QString::number(totalfee)));
			}
			row++;
		}
	}
	if (ui.comboBox_9->currentIndex()==3)
	{
		typelist.clear();
		QSqlQuery query0(*sql.db);
		ui.tableWidget_5->clear();
		typelist.append(QString::fromLocal8Bit("����"));
		query0.exec("select * from mz_classification");
		while(query0.next())
		{
			typelist.append(query0.value(1).toString()+"-"+query0.value(2).toString());
		}
		ui.tableWidget_5->setColumnCount(typelist.count());
		ui.tableWidget_5->setHorizontalHeaderLabels(typelist);
		query0.exec("select * from sys_users");
		int row = 0;
		QString strsql;
		while(query0.next())
		{	
			ui.tableWidget_5->insertRow(row);
			QString strpersonnel= query0.value(1).toString();
			ui.tableWidget_5->setItem(row,0,new QTableWidgetItem(strpersonnel));
			QString strType = QString::fromLocal8Bit("01-��ҩ��");
			for (int i = 1;i<typelist.count();i++)
			{
				QString strType = typelist.at(i);
				if (ui.radioButton_4->isChecked())
				{
					strsql ="select * from mz_chargedetail where itemclass='"+strType+"'and date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
				}
				else
				{
					strsql ="select * from mz_chargedetail where itemclass='"+strType+"' ";
				}
				QSqlQuery query(*sql.db);
				query.exec(strsql);
				double totalfee=0.0;
				while(query.next())
				{
					QSqlQuery query2(*sql.db);
					QString strsheetno = query.value(1).toString();
					query2.exec("select * from mz_chargesheet where sheetno ='"+strsheetno+"' and sheetmaker ='"+strpersonnel+"'");
					if (query2.next())
					{
						totalfee+=query.value(8).toDouble();
					}		
				}
				ui.tableWidget_5->setItem(row,i,new QTableWidgetItem(QString::number(totalfee)));
			}
			row++;
		}
	}
}
void ClinicStatistics::on_clearButton_3_clicked()
{
	ui.tableWidget_5->setRowCount(0);
}
void ClinicStatistics::getItem(int row,int column)
{
	ClinicCharge *charge = new ClinicCharge();
	QString strNo= ui.tableWidget->item(row,10)->text();
	charge->resize(943,575);
	charge->show();
	charge->edit(strNo);
}
void ClinicStatistics::completeText(const QModelIndex &index) {
	QString strName = index.data().toString();
	ui.patientEdit->setText(strName);
	listView->hide();
}
void ClinicStatistics::setCompleter(const QString &text) {
	ui.lineEdit->clear();
	ui.lineEdit_2->clear();
	QSqlQuery query(*sql.db);	
	QString strsql= "select * from mz_chargesheet where patientname='"+text+"'";
	query.exec(strsql);
	while(query.next())
	{
		ui.lineEdit->setText(query.value(8).toString());
		ui.lineEdit_2->setText(query.value(9).toString());
	}
}
void ClinicStatistics::keyPressEvent(QKeyEvent *e) {
	if (!listView->isHidden()) {
		int key = e->key();
		int count = listView->model()->rowCount();
		QModelIndex currentIndex = listView->currentIndex();

		if (Qt::Key_Down == key) {
			// �����·����ʱ���ƶ����ѡ����һ������б��е���
			int row = currentIndex.row() + 1;
			if (row >= count) {
				row = 0;
			}

			QModelIndex index = listView->model()->index(row, 0);
			listView->setCurrentIndex(index);
		} else if (Qt::Key_Up == key) {
			// �����·����ʱ���ƶ����ѡ����һ������б��е���
			int row = currentIndex.row() - 1;
			if (row < 0) {
				row = count - 1;
			}

			QModelIndex index = listView->model()->index(row, 0);
			listView->setCurrentIndex(index);
		} else if (Qt::Key_Escape == key) {
			// ����Esc��ʱ����������б�
			listView->hide();
		} else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
			// ���»س���ʱ��ʹ������б���ѡ�е������������б�
			if (currentIndex.isValid()) {
				QString text = listView->currentIndex().data().toString();
				ui.patientEdit->setText(text);
			}

			listView->hide();
		} else {
			// �����������������б�����ʹ��QLineEdit�ļ��̰����¼�
			listView->hide();
			//QLineEdit::keyPressEvent(e);
		}
	} else {
		//QLineEdit::keyPressEvent(e);
	}
}
ClinicStatistics::~ClinicStatistics()
{

}
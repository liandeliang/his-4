#include "drugpricing.h"
#include <QtGui>
#include "connectsql.h"
#include "clinicdrugfilter.h"
#include "package.h"

static int icount;
extern ConnectSql sql;

DrugPricing::DrugPricing(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUI();
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.tableWidget->installEventFilter(this);//ע���¼�������
	
	list_widget = new QListWidget(this);
	list_widget->setWindowFlags(Qt::FramelessWindowHint);
	list_widget->close();
	installEventFilter(list_widget);

	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(getItem(int,int)));
//	connect(ui.lineEdit_newprice,SIGNAL(textChanged(const QString &)),this,SLOT(pricing(const QString &)));
}

DrugPricing::~DrugPricing()
{

}
void DrugPricing::on_deleteButton_clicked()//��������
{
	int i=0,amount;
	int rows = ui.tableWidget->model()->rowCount();   //������
		
	int ok = QMessageBox::warning(this,QString::fromLocal8Bit("����"),QString::fromLocal8Bit("ȷ��ɾ��������"),QMessageBox::Yes,QMessageBox::No);
	if(ok == QMessageBox::Yes)
	{
		QSqlQuery query(*sql.db);		
		QString strSheetNo=ui.lineEdit_No->text();
		QString strsql = "delete from yk_pricing where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		strsql = "delete from yk_pricing_public_info where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		for (i=0;i< rows ;i++)
		{
			QString strName = ui.tableWidget->item(i,1)->text();	
			double oldprice = ui.tableWidget->item(i,7)->text().toDouble();

			strsql= "select * from yk_instorage where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
			query.exec(strsql);
			if(query.next())
			{
				query.prepare("UPDATE yk_instorage SET unitprice= ? WHERE name = '"+strName+"'");
				query.bindValue(0,oldprice);
				query.exec();
			}
			strsql= "select * from yf_receipt where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
			query.exec(strsql);
			if(query.next())
			{
				query.prepare("UPDATE yf_receipt SET unitprice= ? WHERE name = '"+strName+"'");
				query.bindValue(0,oldprice);
				query.exec();
			}
			strsql= "select * from sys_drugdictionary where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
			query.exec(strsql);
			if(query.next())
			{
				query.prepare("UPDATE sys_drugdictionary SET outpatientunitprice= ? WHERE name = '"+strName+"'");
				query.bindValue(0,oldprice);
				query.exec();
			}
		}
		//QSqlQuery query(*sql.db);		
		//QString strSheetNo=ui.lineEdit_No->text();
		//QString strsql = "delete from yk_pricing where sheetno ='"+strSheetNo+"'"; //where ID > 1
		//query.exec(strsql);

		//strsql = "delete from yk_pricing_public_info where sheetno ='"+strSheetNo+"'"; //where ID > 1
		//query.exec(strsql);
		ui.lineEdit_No->setText("");
		ui.lineEdit_profit->setText("0");
		ui.tableWidget->clearContents();
			ui.tableWidget->setRowCount(0);
		ui.tableWidget->clearContents();
		ui.tableWidget->insertRow(0);
	}
}
double DrugPricing::TotalPrice()
{
	int i;
	int rows = ui.tableWidget->model()->rowCount();
	double icurrentamount = 0.0;

	for(i=0;i<rows;i++)
	{
		if(ui.tableWidget->item(i,7)==NULL||(ui.tableWidget->item(i,7)&& ui.tableWidget->item(i,7)->text()==tr("")))
			continue;
		//int icount = ui.tableWidget->item(i,6)->text().toInt();
		//double unitprice = ui.tableWidget->item(i,6+2)->text().toDouble();
		//double amount = icount*unitprice;
		//QString stramount = QString::number(amount);
		//if(ui.radioButton_Add->isChecked() && !ui.radioButton_Minus->isChecked())
		double amount = ui.tableWidget->item(i,9)->text().toDouble();
			icurrentamount+=amount;
		//if(!ui.radioButton_Add->isChecked() && ui.radioButton_Minus->isChecked())
			//icurrentamount-=amount;
	}
	return icurrentamount;
}

void DrugPricing::on_addRowButton_clicked()
{
	//��������һ��
	int rows = ui.tableWidget->model()->rowCount();   //������
	ui.tableWidget->insertRow(rows);
	iRow=rows;
}
void DrugPricing::on_deleteRowButton_clicked()
{
	int rows = ui.tableWidget->currentRow(); //������
	if(ui.tableWidget->item(rows,8)==NULL||(ui.tableWidget->item(rows,8)&& ui.tableWidget->item(rows,8)->text()==tr("")))
	{	
		ui.tableWidget->removeRow(rows);
		iRow--;
		return;
	}

	ui.tableWidget->removeRow(rows);
	iRow--;

	double CurProfit = TotalPrice();
//		CurProfit = CurProfit + profit;
	QString strprofit = QString::number(CurProfit);
	ui.lineEdit_profit->setText(strprofit);
		


}
void DrugPricing::on_printButton_clicked()
{
	QPrinter       printer( QPrinter::HighResolution );
	QPrintDialog   dialog( &printer, this );
	if ( dialog.exec() == QDialog::Accepted ) print( &printer );
}
void DrugPricing::on_previewButton_clicked()
{
	filePrintPreview();
}
void DrugPricing::filePrintPreview()
{
	// ��ӡԤ���Ի���
	QPrinter             printer( QPrinter::HighResolution );
	QPrintPreviewDialog  preview( &printer, this );
	preview.setWindowTitle("preview");
	preview.setMinimumSize(1000,900);
	connect( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)) );
	preview.exec();
}
void DrugPricing::initUI()
{
	
	iRow=0;
	QSqlQuery query(*sql.db);		
	QDateTime current_date_time = QDateTime::currentDateTime();
	ui.dateTimeEdit->setDateTime(current_date_time);
	ui.saveButton->setEnabled(false);
	ui.discardButton->setEnabled(false);
	ui.editButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);
	ui.addRowButton->setEnabled(false);
	ui.deleteRowButton->setEnabled(false);
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
//	item(i, colunm)->setFlags(Qt::NoItemFlags);
	//ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ�еķ�ʽ
	ui.lineEdit_profit->setText("0");

	ui.tableWidget->setRowCount(0);
	ui.tableWidget->clearContents();
	ui.tableWidget->insertRow(0);

}

void DrugPricing::on_editButton_clicked()
{
	SetEdit(true);
	ui.deleteButton->setEnabled(true);
	ui.saveButton->setEnabled(true);
	ui.discardButton->setEnabled(true);
	ui.addRowButton->setEnabled(true);
	ui.deleteRowButton->setEnabled(true);
}

void DrugPricing::on_addButton_clicked()
{
	SetEdit(true);
	ui.saveButton->setEnabled(true);
	ui.discardButton->setEnabled(true);
	ui.editButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);
	ui.addRowButton->setEnabled(true);
	ui.deleteRowButton->setEnabled(true);
	ui.tableWidget->setRowCount(0);
	ui.tableWidget->clearContents();
	ui.tableWidget->insertRow(0);
	iRow = 0;
	icount = 0;
	
	int isheetcount=PublicInfoSheetNo();
	isheetcount++;
	QString strSheetNo= "YPGJ"+QString::number(isheetcount, 10);
	ui.lineEdit_No->setText(strSheetNo);
	ui.lineEdit_profit->setText("0");

}

int  DrugPricing::PublicInfoSheetNo()
{
	QSqlQuery query(*sql.db);		
	query.exec("select * from yk_pricing_public_info");
	int isheetcount=0;
	while(query.next())
	{ 
		isheetcount++;
	}
	return isheetcount;
}
void DrugPricing::getItem(int row,int column)//�������
{
	QString str2;
	QString str = str.fromLocal8Bit("����");
	if (column==8)
	{
		if (ui.tableWidget->item(row,column)->text()==NULL) return;
	//	if (ui.tableWidget->item(row,column+1)->text()==NULL) return;

		double oriprice = ui.tableWidget->item(row,column-1)->text().toDouble();
		double newprice = ui.tableWidget->item(row,column)->text().toDouble();
		int icount = ui.tableWidget->item(row,column-2)->text().toInt();
		if(newprice<0)
		{
			str2 = str.fromLocal8Bit("�¼۲���Ϊ������˶ԣ�");
			QMessageBox::warning(this,str,str2,QMessageBox::Ok);
			QString stramount = QString::number(0);
			ui.tableWidget->setItem(row,column,new QTableWidgetItem(stramount));
			return;
		}
		double profit = (newprice-oriprice)*icount;
		QString strprofit = QString::number(profit);
		ui.tableWidget->setItem(row,column+1,new QTableWidgetItem(strprofit));

		double CurProfit = TotalPrice();

		strprofit = QString::number(CurProfit);
		ui.lineEdit_profit->setText(strprofit);
	}
	if(column==0)
	{
		list_widget->close();
		QString strText;
		if(ui.tableWidget->item(row,0)==NULL)  return;

		strText =  ui.tableWidget->item(row,0)->text();
		if(strText.at(0)== QChar('1')) return;

		list_widget->setGeometry(103, 160+row*30, 150, 280);
		list_widget->show();
		QSqlQuery query(*sql.db);	
		strText =  ui.tableWidget->item(row,0)->text();
		QString strsql= "select * from sys_drugdictionary where abbr = '"+strText+"'";//;//where AbbrName = '"+strName+"'

		query.exec(strsql);
		QStringList list;
		list_widget->clear();
		while(query.next())
		{
			QString str = query.value(1).toString();
			list.append(str);
		}
		list_widget->addItems(list);
	}
}
int  DrugPricing::SheetNo()
{
	QSqlQuery query(*sql.db);		
	query.exec("select * from yk_pricing");
	int isheetcount=0;
	while(query.next())
	{ 
		isheetcount++;
	}
	return isheetcount;
}

void DrugPricing::SetEdit(bool IsEdit)
{
	if (IsEdit==true)
	{
		ui.dateTimeEdit->setEnabled(true);
		ui.tableWidget->setEnabled(true);
		ui.lineEdit_No->setEnabled(true);

		ui.lineEdit_profit->setEnabled(true);
		ui.lineEdit_remark->setEnabled(true);
	}
	else
	{
		ui.dateTimeEdit->setEnabled(false);
		ui.tableWidget->setEnabled(false);
		ui.lineEdit_No->setEnabled(false);
		ui.lineEdit_profit->setEnabled(false);
		ui.lineEdit_remark->setEnabled(false);
	}
}
void DrugPricing::edit(QString strNo)
{
	ui.tableWidget->installEventFilter(this);
	SetEdit(false);
	ui.addButton->setEnabled(false);
	ui.editButton->setEnabled(true);
	QSqlQuery query(*sql.db);	
	query.exec("select * from yk_pricing where sheetno='"+strNo+"'");
	while(query.next())
	{
		ui.lineEdit_No->setText(query.value(1).toString());
		ui.dateTimeEdit->setDateTime(query.value(2).toDateTime());
		
		ui.lineEdit_remark->setText(query.value(4).toString());		
	}
	ui.tableWidget->setRowCount(0);
	QSqlQuery query2(*sql.db);	
	query2.exec("select * from yk_pricing where sheetno='"+strNo+"'");
	int row= 0;
	while(query2.next())
	{
		int Num;
		ui.tableWidget->insertRow(row);
		//ui.tableWidget->setItem(row,1,new QTableWidgetItem(query2.value(4).toString()));
		QString strName=query2.value(5).toString();
		QString strsql= "select * from sys_drugdictionary where name='"+strName+"'";
		query.exec(strsql);
		while(query.next())
		{
			ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
			ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(4).toString()));
			ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(5).toString()));
			ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(6).toString()));
			
			//ui.tableWidget->setItem(row,6,new QTableWidgetItem(query.value(12).toString()));//
			//Num = query.value(10).toString().toInt();
		}
		ui.tableWidget->setItem(row,6,new QTableWidgetItem(query2.value(12).toString()));
		ui.tableWidget->setItem(row,7,new QTableWidgetItem(query2.value(10).toString()));//
		ui.tableWidget->setItem(row,8,new QTableWidgetItem(query2.value(11).toString()));//
		//strsql= "select * from yk_inventory where name='"+strName+"'";
		//query.exec(strsql);
		//if(query.next())
		//{
		//	int kucun1 = query2.value(7).toString().toInt();
		//	kucun1 = kucun1 - Num;
		//	QString strTemp = QString::number(kucun1);
		//	ui.tableWidget->setItem(row,7,new QTableWidgetItem(strTemp));  //������
		//}
		//ui.tableWidget->setItem(row,6,new QTableWidgetItem(query2.value(13).toString()));
		row++;
	}
}
void DrugPricing::on_saveButton_clicked()
{
	static int iFlag;
	int i=0;
	int amount;
	double unitprice;
	int rows = ui.tableWidget->model()->rowCount();   //������

	char strtemp[255]={0};
	//���ӵ��շѵ�������Ϣ����
	QSqlQuery query(*sql.db);

	
	QString str2;
	QString str = str.fromLocal8Bit("����");
	if(!ui.tableWidget->isEnabled())
	{	
		str2 = str.fromLocal8Bit("�ѱ������");		
		QMessageBox::warning(this,str,str2,QMessageBox::Ok);
		return;
	}
	
	QString strSheetNo=ui.lineEdit_No->text();
	QString strsql = "select * from yk_pricing where sheetno ='"+strSheetNo+"'"; //where ID > 1
	query.exec(strsql);
	if(query.next())
	{
		strsql = "delete from yk_pricing where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		strsql = "delete from yk_pricing_public_info where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
	}

	int isheetcount=PublicInfoSheetNo();
		isheetcount++;
	query.prepare("INSERT INTO yk_pricing_public_info VALUES(?, ?, ?, ?)");
	query.bindValue(0, isheetcount);
	query.bindValue(1, ui.lineEdit_No->text());
	query.bindValue(2, ui.dateTimeEdit->dateTime());	

	query.bindValue(3, ui.lineEdit_remark->text());
	iFlag = query.exec();


	for (i=0;i < rows ;i++)
	{
		isheetcount=SheetNo();
		isheetcount++;
		if(ui.tableWidget->item(i,8)==NULL||(ui.tableWidget->item(i,8)&& ui.tableWidget->item(i,8)->text()==tr("")))
		{
			sprintf(strtemp,"�¼�Ϊ�գ���˶ԣ�",i+1);
			str2 = str.fromLocal8Bit(strtemp);		
			QMessageBox::warning(this,str,str2,QMessageBox::Ok);
			return;
		}

		query.prepare("INSERT INTO yk_pricing VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		query.bindValue(0, isheetcount);
		query.bindValue(1, ui.lineEdit_No->text());
		query.bindValue(2, ui.dateTimeEdit->dateTime());
		query.bindValue(3, ui.lineEdit_remark->text());
		query.bindValue(4, ui.tableWidget->item(i,0)->text());
		query.bindValue(5, ui.tableWidget->item(i,1)->text());
		query.bindValue(6, ui.tableWidget->item(i,2)->text());
		query.bindValue(7, ui.tableWidget->item(i,3)->text());
		query.bindValue(8, ui.tableWidget->item(i,5)->text());
		query.bindValue(9, ui.tableWidget->item(i,6)->text().toDouble());
		query.bindValue(10, ui.tableWidget->item(i,7)->text().toDouble());
		query.bindValue(11, ui.tableWidget->item(i,8)->text().toInt());
		query.bindValue(12, ui.lineEdit_profit->text().toDouble());
		
		iFlag = query.exec();

	
		double newprice = ui.tableWidget->item(i,8)->text().toDouble();
		QString strName = ui.tableWidget->item(i,1)->text();
		QString strsql= "select * from yk_instorage where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
		query.exec(strsql);

		if(query.next())
		{
			query.prepare("UPDATE yk_instorage SET unitprice= ? WHERE name = '"+strName+"'");
			query.bindValue(0,newprice);
			iFlag = query.exec();
		}

		strsql= "select * from yf_receipt where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
		query.exec(strsql);

		if(query.next())
		{
			query.prepare("UPDATE yf_receipt SET unitprice= ? WHERE name = '"+strName+"'");
			query.bindValue(0,newprice);
			iFlag = query.exec();
		}

		strsql= "select * from sys_drugdictionary where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
		query.exec(strsql);

		if(query.next())
		{
			query.prepare("UPDATE sys_drugdictionary SET outpatientunitprice= ? WHERE name = '"+strName+"'");
			query.bindValue(0,newprice);
			iFlag = query.exec();
		}


	}
	if(iFlag)
	{
		QString str = str.fromLocal8Bit("��ʾ");
		QString str2 = str.fromLocal8Bit("����ɹ���");
	//	QMessageBox::information(this,str,str2);
//		ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
		SetEdit(false);
		ui.saveButton->setEnabled(false);
		ui.discardButton->setEnabled(false);
		ui.editButton->setEnabled(true);
		ui.deleteButton->setEnabled(true);
		ui.addRowButton->setEnabled(false);
		ui.deleteRowButton->setEnabled(false);
	}
	else
	{
		QString str = str.fromLocal8Bit("��ʾ");
		QString str2 = str.fromLocal8Bit("����ʧ�ܣ�");
		QMessageBox::information(this,str,str2);
	}
}

void DrugPricing::on_discardButton_clicked()
{
	ui.lineEdit_No->setText("");
	ui.lineEdit_profit->setText("");

	ui.tableWidget->setRowCount(0);
	ui.tableWidget->clearContents();
	ui.tableWidget->insertRow(0);
		ui.addRowButton->setEnabled(false);
	ui.deleteRowButton->setEnabled(false);

}
void DrugPricing::print( QPrinter* printer )
{
	//// ������ӡҳ��Ļ��ƶ���
	QPainter painter( printer );
	int      w = printer->pageRect().width();
	int      h = printer->pageRect().height();
	QRect    page( w/5, h/15, w, h );
	QRect    page2( w/4, h/8, w, h );
	QRect    page3( w/4, h/6, w, h );
	QRect    page4( 0, h/10, w, h );
	// ����ҳ���С�����ʵ��������С
	QFont    font = painter.font();
	font.setPixelSize( (w+h) / 100 );
	painter.setFont( font );

	// ��ҳ�������Ʊ�ǩ
	painter.drawText( page, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("                  �������ོ������Ժ") );
	/*painter.drawText( page2, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("���ľ�") );
	painter.drawText( page3, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("һ�����Ʒ�     ��   1  10.00") );
	painter.drawText( page4, Qt::AlignTop | Qt::AlignLeft,  QString::fromLocal8Bit("��ʮ��Ԫ�Ž���"));
	painter.drawText( page, Qt::AlignBottom | Qt::AlignLeft,
		QDateTime::currentDateTime().toString( Qt::DefaultLocaleShortDate ) );*/

	// ����ģ������
	page.adjust( w/20, h/20, -w/20, -h/20 );

	//m_scene->render( &painter, page );
	//����
	/*	QWidget *myForm=new QWidget(this);
	myForm->setObjectName(QString::fromUtf8("Form"));
	myForm->resize(500, 500);
	QTableWidget *tableWidget;
	tableWidget = new QTableWidget(myForm);
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(4);
	tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
	tableWidget->setGeometry(QRect(0, 0,500, 500));    
	tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
	"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
	"QHeaderView::section:checked{background-color: red;}");    */       
	QPixmap image;
	image=image.grabWidget(ui.tableWidget,-35,0,900, 1000);
	painter.drawPixmap(page4,image);

	//printTableWidget(ui.tableWidget,"preview",printer);
}
void DrugPricing::keyPressEvent(QKeyEvent *e) {
	if (!list_widget->isHidden()) {
		int key = e->key();
		int count = list_widget->model()->rowCount();
		QModelIndex currentIndex = list_widget->currentIndex();

		if (Qt::Key_Down == key) {
			int row = currentIndex.row() + 1;
			if (row >= count) {
				row = 0;
			}

			QModelIndex index = list_widget->model()->index(row, 0);
			list_widget->setCurrentIndex(index);
		} else if (Qt::Key_Up == key) {
			int row = currentIndex.row() - 1;
			if (row < 0) {
				row = count - 1;
			}

			QModelIndex index = list_widget->model()->index(row, 0);
			list_widget->setCurrentIndex(index);
		} else if (Qt::Key_Escape == key) {
			list_widget->hide();
		} else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
			if (currentIndex.isValid()) {
				QString strName = list_widget->currentIndex().data().toString();

				int row = ui.tableWidget->currentRow();
				QSqlQuery query(*sql.db);	
				QString strsql= "select * from sys_drugdictionary where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
				query.exec(strsql);
				while(query.next())
				{
					//ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(2).toString()));
					ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString())); //����
					ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(4).toString())); //���
					ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(5).toString())); //��������
					ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(6).toString())); //��λ
					ui.tableWidget->setItem(row,7,new QTableWidgetItem(query.value(15).toString())); //��λ
				}
				QString strsql1= "select * from yk_inventory where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
				query.exec(strsql1);

				if(query.next())
				{
					ui.tableWidget->setItem(row,6,new QTableWidgetItem(query.value(7).toString()));
				}
				else 
				{
					QString strTemp = QString::number(0);
					ui.tableWidget->setItem(row,6,new QTableWidgetItem(strTemp));
				}
			}

			list_widget->hide();
		} else {
			list_widget->hide();
			//QLineEdit::keyPressEvent(e);
		}
	} else {
		//QLineEdit::keyPressEvent(e);
	}
}
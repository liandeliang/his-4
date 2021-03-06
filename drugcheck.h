#ifndef DRUGCHECK_H
#define DRUGCHECK_H
#include <QtGui>
#include <QWidget>
#include "ui_drugcheck.h"

class DrugCheck : public QWidget
{
	Q_OBJECT

public:
	DrugCheck(QWidget *parent = 0);
	~DrugCheck();
	void initUI();
	int SheetNo();
	double TotalPrice();
	int PublicInfoSheetNo();
	int InventorySheetNo();
	//void printTableWidget(const QTableWidget* view,QString stitile,QPrinter *printer);
	void SetEdit(bool IsEdit);
	int iRow;
	QListWidget*list_widget ;
	void keyPressEvent(QKeyEvent *e);
private:
	Ui::DrugCheck ui;
public slots:
	void on_printButton_clicked();
	void on_saveButton_clicked();
	void on_previewButton_clicked();
	void on_addRowButton_clicked();
	void on_deleteRowButton_clicked();
	void on_addButton_clicked();
	void on_editButton_clicked();
	void on_deleteButton_clicked();
	void on_discardButton_clicked();
	//void on_checkButton_clicked();
	void getItem(int row,int column);

	void filePrintPreview();
	void print(QPrinter* printer);
	void edit(QString strNo);
};

#endif // DRUGCHECK_H

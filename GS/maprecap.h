#ifndef MAPRECAP_H
#define MAPRECAP_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QWebFrame>
#include <QString>
#include <QApplication>
#include <QDoubleValidator>

#include "tablemodel.h"
#include "popwindowmp.h"
#include "qcomboboxdelegate.h"
#include "popwindowmp.h"
//#include "mainwindow.h"
#include "mapexecution.h"
#include "messagebox.h"

#include "ui_maprecap.h"

namespace Ui {
    class MapRecap;
}

class MapRecap : public QDialog {
    Q_OBJECT

public:
    explicit MapRecap(QWidget *parent = 0);
    ~MapRecap();
    void updateMap();
    void plotPosition(double lat, double lng);
    void addPoint(QString string);
public slots:
    void addNewMap();
    void addPointToTable(double lat, double lng);
    //addPointToTable - used to add an entry with latitude lat and longitude lng to the table.
    //  lat - the latitude value (usually from the JavaScript program).
    //  lng - the longitude value (usually from the JavaScript program).

private slots:
    void on_backButton_clicked();
    void addClickListener();
    void closeWindow();

private:
    Ui::MapRecap *ui;
    TableModel *model;
};

#endif // MAPRECAP_H

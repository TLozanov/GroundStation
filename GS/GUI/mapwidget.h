#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QQuickWidget>
#include "mission.h"
#include "QVector3D"
#include "QVariant"
#include <QGeoCoordinate>

#include <QTimer>
#include <QEventLoop>

class MapWidget : public QQuickWidget
{
    Q_OBJECT

private:
    template <typename T>
    QVariantList toQVariantList(const QList<T> * list ) {
        QVariantList newList;
        for(const T item: (*list)) {
            newList << item;
        }
        return newList;
    }
    QString headingToCompass(int heading);
    bool updateCenterConstant = true;
    bool updateUAVConstant = true;
    bool blinkUAV = false;
    double uav_latitude = 0;
    double uav_longitude = 0;
    int uav_heading = 0;
    QTimer * timer;
    int timeoutMS = 3000;

public:
    explicit MapWidget(QWidget * parent = nullptr);
    QObject * map;
    QList<QVector3D> * uavPath;

    void addWaypoint(QVector3D point, int wpNum, QColor color, int radius);
    void addMissionPath(QList<QVector3D> *missionPath);

    void drawPoint(QVector2D point, QString label, QColor color, int radius = 20);
    void drawPolyline(QVariantList points, QColor color);
    void drawPolyline(QList<QVector3D> * points, QColor color);
    void drawPolygonF(QPolygonF points, QColor color, QString label);
    void drawPolygon(QVariantList points, QColor color, QString label);
    void drawWaypoints(QList<QVector2D> * wps);
    void clearMap();
    void drawMission(Mission *selectedMission);
    void drawUAV(double lat, double lon, double heading);
    void updateCenter(double lat, double lon);
    void updateUAV();
    void updateArmState(bool state);
    void timeout();
    inline void toggleUpdateCenterConstant() {
        updateCenterConstant = updateCenterConstant ? false : true;
    }
    inline void toggleUpdateUAVConstant() {
        updateUAVConstant = updateUAVConstant ? false : true;
    }
    inline void toggleBlinkUAV() {
        blinkUAV = blinkUAV ? false : true;
    }

    void testRemoveUAV();

//signals:

public slots:
    void updateUAVPosition(mavlink_gps_raw_int_t gps);
    void updateUAVHeading(mavlink_vfr_hud_t vfr);
    void selectWaypoint(int wpNum);
    void moveWaypoint(int wpNum, QVector3D coords);
    void removeWaypoint(int wpNum, QVector3D coords);
    void changeEditMode(bool editing);
};

#endif // MAPWIDGET_H

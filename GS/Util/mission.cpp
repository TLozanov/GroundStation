#include "mission.h"
#include "dbmanager.h"
#include <QDebug>

Mission::Mission(QObject *parent) : QObject(parent){

}

Mission::Mission(QJsonObject obj) {
    id = obj["id"].toInt();
    active = obj["active"].toBool();
    home_pos = posToPoint(obj["home_pos"].toObject());
    air_drop_pos = posToPoint(obj["air_drop_pos"].toObject());
    off_axis_odlc_pos = posToPoint(obj["off_axis_odlc_pos"].toObject());
    emergent_last_known_pos = posToPoint(obj["emergent_last_known_pos"].toObject());
    mission_waypoints = setMissionWaypoints(obj["mission_waypoints"].toArray());
    search_grid_points = set3DPoints(obj["search_grid_points"].toArray());
    fly_zones = setFlyZones(obj["fly_zones"].toArray());
}

QList<FlyZone> * Mission::setFlyZones(QJsonArray flyZoneArray) {
    QList<FlyZone> * fly_zones = new QList<FlyZone>();
    for (int i = 0; i < flyZoneArray.size(); i++) {
        FlyZone fly_zone = FlyZone();
        fly_zone.max_alt = flyZoneArray[i].toObject()["altitude_msl_max"].toDouble();
        fly_zone.min_alt = flyZoneArray[i].toObject()["altitude_msl_max"].toDouble();
        fly_zone.boundary_points = setPoints(flyZoneArray[i].toObject()["boundary_pts"].toArray());
        fly_zones->append(fly_zone);
    }
    return fly_zones;
}



MissionWaypoints Mission::setMissionWaypoints(QJsonArray pointArray) {
    MissionWaypoints missionWaypoints = MissionWaypoints();
    missionWaypoints.waypoints = set3DPoints(pointArray);
    missionWaypoints.actions = new QList<int>();


    for (int i = 0; i < missionWaypoints.waypoints->size();i++) {
        if (i == 0) {
            missionWaypoints.actions->append(MAV_CMD_NAV_TAKEOFF);
        }
        else if (i == missionWaypoints.waypoints->size() -1) {
            missionWaypoints.actions->append(MAV_CMD_NAV_LAND);
        }
        else {
            missionWaypoints.actions->append(MAV_CMD_NAV_WAYPOINT);
        }
    }
    return missionWaypoints;
}


QList<QVector2D> * Mission::setPoints(QJsonArray pointArray) {
    QList<QVector2D> * points = new QList<QVector2D>();
    for(int i = 0; i < pointArray.size(); ++i){
       points->insert(pointArray[i].toObject()["order"].toInt()-1,
                                  posToPoint(pointArray[i].toObject()));
    }
    return points;
}

QList<QVector3D> * Mission::set3DPoints(QJsonArray pointArray) {
    QList<QVector3D> * points = new QList<QVector3D>();
    for(int i = 0; i < pointArray.size(); ++i){
       points->insert(pointArray[i].toObject()["order"].toInt()-1,
                                  posTo3DPoint(pointArray[i].toObject()));
    }
    return points;
}

QVector3D Mission::posTo3DPoint(QJsonObject obj) {
    return QVector3D(posToPoint(obj), obj["altitude"].toDouble());
}

QVector2D Mission::posToPoint(QJsonObject obj) {
    return QVector2D(obj["latitude"].toDouble(), obj["longitude"].toDouble());
}


void Mission::loadWaypoint(mavlink_mission_request_t mrequest) {
    float params[7] = {0, 0, 0, 0, -35.3609161377, 149.1624603271, 40.40};
    int cmd = 16;
    if (mrequest.seq == 0) {
        cmd = 22;
    }
    cmd =22;
    emit (loadToUAV(mrequest.seq, cmd, params));
}


Point Mission::toECEF(double lat, double lon, double alt) {
    // WGS84 ellipsoid constants:
    double a = 6378.137;
    double e = 8.1819190842622e-2;
    // prime vertical radius of curvature
    double N = a / sqrt(1 - pow(e, 2) * pow(sin(lat), 2));

    double x = (N+alt) * cos(lat) * cos(lon);
    double y = (N+alt) * cos(lat) * sin(lon);
    double z = ((1-pow(e,2)) * N + alt) * sin(lat);
    return Point{x, y, z};
}

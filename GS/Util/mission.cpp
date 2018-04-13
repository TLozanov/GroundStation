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
    interopPath = setMissionPath(obj["mission_waypoints"].toArray());
    generatedPath = interopPath;
    search_grid_points = set3DPoints(obj["search_grid_points"].toArray());
    fly_zones = setFlyZones(obj["fly_zones"].toArray());
    /* Default takeoff+landing */
    takeoff.setDefaultTakeoff(20, 45);
    QList<QVector3D> landingPath({QVector3D(33.770980, -117.694640, 5)});
    landing.setDefaultLanding(landingPath, QVector2D(33.771031, -117.694890), 15);
}

Mission::Mission(QJsonObject mission_obj, QJsonDocument obstacles_doc) {
    id = mission_obj["id"].toInt();
    active = mission_obj["active"].toBool();
    home_pos = posToPoint(mission_obj["home_pos"].toObject());
    air_drop_pos = posToPoint(mission_obj["air_drop_pos"].toObject());
    off_axis_odlc_pos = posToPoint(mission_obj["off_axis_odlc_pos"].toObject());
    emergent_last_known_pos = posToPoint(mission_obj["emergent_last_known_pos"].toObject());
    interopPath = setMissionPath(mission_obj["mission_waypoints"].toArray());
    generatedPath = interopPath;
    search_grid_points = set3DPoints(mission_obj["search_grid_points"].toArray());
    fly_zones = setFlyZones(mission_obj["fly_zones"].toArray());
    obstacles = Obstacles(obstacles_doc);
    /* Default takeoff+landing */
    takeoff.setDefaultTakeoff(20, 45);
    QList<QVector3D> landingPath({QVector3D(33.770980, -117.694640, 5)});
    landing.setDefaultLanding(landingPath, QVector2D(33.771031, -117.694890), 15);
}

QList<FlyZone> Mission::setFlyZones(QJsonArray flyZoneArray) {
    QList<FlyZone> fly_zones;
    for (int i = 0; i < flyZoneArray.size(); i++) {
        FlyZone fly_zone;
        fly_zone.max_alt = flyZoneArray[i].toObject()["altitude_msl_max"].toDouble();
        fly_zone.min_alt = flyZoneArray[i].toObject()["altitude_msl_max"].toDouble();
        fly_zone.boundary_points = setPoints(flyZoneArray[i].toObject()["boundary_pts"].toArray());
        fly_zones.append(fly_zone);
    }
    return fly_zones;
}

MissionPath Mission::setMissionPath(QJsonArray pointArray) {
    MissionPath missionPath = MissionPath();
    for(int i = 0; i < pointArray.size(); ++i){
        QVector3D coords = posTo3DPoint(pointArray[i].toObject());
        Waypt waypt = Waypt(coords);
        int order = pointArray[i].toObject()["order"].toInt()-1;
        missionPath.addWaypoint(waypt, order);
    }
    return missionPath;
}

QList<QVector2D> Mission::setPoints(QJsonArray pointArray) {
    QList<QVector2D> points;
    for(int i = 0; i < pointArray.size(); ++i){
       points.insert(pointArray[i].toObject()["order"].toInt()-1,
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
    return QVector3D(posToPoint(obj), obj["altitude_msl"].toDouble());
}

QVector2D Mission::posToPoint(QJsonObject obj) {
    return QVector2D(obj["latitude"].toDouble(), obj["longitude"].toDouble());
}

QJsonValue Mission::pointToPos(QVector2D point) {
    QJsonObject temp;
    temp["latitude"] = point.x();
    temp["longitude"] = point.y();
    return QJsonValue(temp);
}

QVector<Waypoint::WP> Mission::constructWaypoints() {
    uint16_t len = generatedPath.length();
    QVector<Waypoint::WP> waypoints;

    waypoints.append(missionPrologue());
    waypoints.append(takeoff.generateWP(1));
    waypoints.append(generatedPath.generateWaypoints(2));
    waypoints.append(landing.generateWaypoints(len + 2));

    return waypoints;
}

uint16_t Mission::completeMissionLength() {
    int missionPrologue = 1;
    int takeoffLen = 1;
    int waypoints = generatedPath.length();
    int landingLen = landing.length();
    return missionPrologue + takeoffLen + waypoints + landingLen;
}

Obstacles Mission::getObstacles() {
    return obstacles;
}

QJsonDocument Mission::toJson() {
    QJsonObject obj;
    obj["id"] = QJsonValue(id);
    obj["active"] = QJsonValue(active);
    obj["home_pos"] = pointToPos(home_pos);
    obj["air_drop_pos"] = pointToPos(air_drop_pos);
    obj["off_axis_odlc_pos"] = pointToPos(off_axis_odlc_pos);
    obj["emergent_last_known_pos"] = pointToPos(emergent_last_known_pos);
    QList<Waypt> *waypoints = &(interopPath.waypoints);
    QJsonArray meow;
    for (int i = 0; i < waypoints->length(); i++) {
        QJsonObject temp;
        QVector3D coord = waypoints->at(i).coords;
        temp["latitude"] = QJsonValue(coord.x());
        temp["longitude"] = QJsonValue(coord.y());
        temp["altitude_msl"] = QJsonValue(coord.z());
        temp["order"] = QJsonValue(i+1);
        meow.append(temp);
    }
    obj["mission_waypoints"] = QJsonValue(meow);
//    obj["search_grid_points"] = QJsonValue(search_grid_points);
    QJsonArray meow2;
    for (int i = 0; i<fly_zones.length(); i++) {
        QJsonObject temp;
        FlyZone *zone = &fly_zones[i];
        temp["altitude_msl_max"]=zone->max_alt;
        temp["altitude_msl_min"]=zone->min_alt;
        QJsonArray temp2;
        for (int j=0; j<zone->boundary_points.length(); j++) {
            QJsonObject temp3;
            temp3["latitude"] = QJsonValue(zone->boundary_points[j].x());
            temp3["longitude"] = QJsonValue(zone->boundary_points[j].y());
            temp3["order"] = QJsonValue(j+1);
            temp2.append(temp3);
        }
    }
    QJsonDocument doc;
    doc.setObject(obj);
    return doc;
}

QList<QPolygonF> Mission::get_obstacles() {
    QList<QPolygonF> polys;
    for (QJsonValueRef o : obstacles.get_stationary_obstacles()) {
        QJsonObject obstacle = o.toObject();
        double obs_lat = obstacle["latitude"].toDouble();
        double obs_lon = obstacle["longitude"].toDouble();
        double radius = obstacle["cylinder_radius"].toDouble();
        double delta = meters_to_deg(radius, obs_lat);

        QVector<QPointF> obstacle_footprint_points;
        for (double theta = 0; theta < 2*M_PI; theta += M_PI/360) {
            obstacle_footprint_points << QPointF(obs_lat + (delta * cos(theta)), obs_lon + (delta * sin(theta)));
        }
        polys.append(QPolygonF(obstacle_footprint_points));
    }
    return polys;
}

Waypoint::WP Mission::missionPrologue() {
    Waypoint::WP wp = {0,0,16,0,1,0,0,0,0,0,0,0};
    return wp;
}

QList<QVector3D> *Mission::toList() {
    QList<QVector3D> *list = new QList<QVector3D>();
    list->append(QVector3D(home_pos)); // Takeoff
    foreach (Waypt wp, generatedPath.waypoints)
        list->append(wp.coords);
    foreach(Waypt l, landing.waypoints)
        list->append(l.coords);
    return list;
}

QVector3D Mission::moveWaypoint(int wpNum, int key) {
    if (wpNum == 1) // Takeoff unmodifiable
        return QVector3D(home_pos);
    if (wpNum > generatedPath.length() + 1) { // Landing
        int index = wpNum - generatedPath.length() - 2;
        QVector3D l = landing.waypoints.at(index).coords;
        switch (key) {
            case Qt::Key_Up:
                l.setX(l.x()+0.00001);
            break;
            case Qt::Key_Down:
                l.setX(l.x()-0.00001);
            break;
            case Qt::Key_Left:
                l.setY(l.y()-0.00001);
            break;
            case Qt::Key_Right:
                l.setY(l.y()+0.00001);
            break;
        }
        landing.waypoints[index].coords = l;
        return l;
    }
    else { // Waypoints
        int index = wpNum - 2;
        QVector3D wp = generatedPath.waypoints.at(index).coords;
        switch (key) {
            case Qt::Key_Up:
                wp.setX(wp.x()+0.00001);
            break;
            case Qt::Key_Down:
                wp.setX(wp.x()-0.00001);
            break;
            case Qt::Key_Left:
                wp.setY(wp.y()-0.00001);
            break;
            case Qt::Key_Right:
                wp.setY(wp.y()+0.00001);
            break;
        }
        generatedPath.waypoints[index].coords = wp;
        return wp;
    }
}

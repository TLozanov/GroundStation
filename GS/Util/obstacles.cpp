#include "obstacles.h"

Obstacles::Obstacles()
{

}

Obstacles::Obstacles(QJsonDocument document):
    jsonDoc(document)
{

    QJsonObject obj = jsonDoc.object();
    moving_obstacles = obj["moving_obstacles"].toArray();
    stationary_obstacles = obj["stationary_obstacles"].toArray();
}


QJsonArray Obstacles::get_moving_obstacles(){
    return moving_obstacles;
}

QJsonArray Obstacles::get_stationary_obstacles(){
    return stationary_obstacles;
}

void Obstacles::loadStationaryObjects(QQuickWidget * mapWidget){


    //-----------------------------------------------------Stationary Objects------------------------------------------------
    for(int i=0; i<stationary_obstacles.size();++i){

        QMetaObject::invokeMethod(mapWidget->rootObject()->childItems().back(), "addMarker",
                Q_ARG(QVariant, ""),
                Q_ARG(QVariant, "images/red_circle"),
                Q_ARG(QVariant, stationary_obstacles[i].toObject()["latitude"].toVariant()),
                Q_ARG(QVariant, stationary_obstacles[i].toObject()["longitude"].toVariant()),
                Q_ARG(QVariant, "/2"),
                Q_ARG(QVariant, "/2"),
                Q_ARG(QVariant, "0"));
    }
}

void Obstacles::updateMovingObjects(QQuickWidget * mapWidget){

    //-----------------------------------------------------Moving Objects------------------------------------------------
    for(int i=0; i<moving_obstacles.size();++i){

        QMetaObject::invokeMethod(mapWidget->rootObject()->childItems().back(), "addMarker",
                Q_ARG(QVariant, ""),
                Q_ARG(QVariant, "images/red_circle"),
                Q_ARG(QVariant, moving_obstacles[i].toObject()["latitude"].toVariant()),
                Q_ARG(QVariant, moving_obstacles[i].toObject()["longitude"].toVariant()),
                Q_ARG(QVariant, "/2"),
                Q_ARG(QVariant, "/2"),
                Q_ARG(QVariant, "0"));
    }
}

// https://math.stackexchange.com/a/980131
bool Obstacles::segmentIntersectsObstacles(Point a, Point b) {
    for (QJsonValueRef o : stationary_obstacles) {
        QJsonObject obstacle = o.toObject();
        Point cyl_center = Point::fromGeodetic(obstacle["latitude"].toDouble(), obstacle["longitude"].toDouble(), 0);
        qInfo() << "obstacle: " << cyl_center;
        double radius = obstacle["cylinder_radius"].toDouble();
        // ignore the z-axis for now
        double x_min = fmin(a.x, b.x);
        double x_max = fmax(a.x, b.x);
        double y_min = fmin(a.y, b.y);
        double y_max = fmax(a.y, b.y);
        qInfo() << x_min << " " << x_max;
        qInfo() << y_min << " " << y_max;
        for (double x = x_min; x < x_max; x += 0.01) {
            for (double y = y_min; y < y_max; y += 0.01) {
                if (pow(x - cyl_center.x, 2) + pow(y - cyl_center.y, 2) < pow(radius, 2)) {
                    // should check z and cylinder height
                    return true;
                }
            }
        }

//        if (pow(cyl_center.x, 2) + pow(slope * cyl_center.x + offset, 2) == pow(radius, 2)) {
//            // treating cyllinder as infinite height, we should be checking z here
//            return true;
//        }
    }

    return false;
}


#ifndef SPHERICALCAMERA_H
#define SPHERICALCAMERA_H

#include <GL/gl.h>
#include "vec_mat.h"
#include <math.h>

#include <btBulletDynamicsCommon.h>

#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296
#define deg PI_DIV_180

class SphericalCamera {
public:
    SphericalCamera() {
        r = 1;
        theta = 0;
        psy = 0;
    }

    void setPosition(float R, float Theta, float Psy) {
        r = R;
        theta = Theta;
        psy = Psy;
    }

    void setPosition(vec3 pos) {
        r = pos.x;
        theta = pos.y;
        psy = pos.z;
    }

    mat4 lookAt(vec3 target) {
        return LookAt(r*cos(psy*deg)*cos(theta*deg) + target.x,
                      r*sin(psy*deg) + target.y,
                      r*cos(psy*deg)*sin(theta*deg) + target.z,
                      target.x, target.y, target.z,
                      0.0, 1.0, 0.0);
    }

    void addR(float num) {
        r += num;
        if (r < 0.000001f) r = 0.000001f;
    }

    void addTheta(float num) {
        theta += num;
        if (theta > 360) theta -= 360;
        if (theta < 0) theta += 360;
    }

    void addPsy(float num) {
        psy += num;
        if (psy > 90) psy = 90;
        if (psy < -90) psy = -90;
    }

    void setR(float num) {
        r = num;
        if (r < 0.000001f) r = 0.000001f;
    }

    void setTheta(float num) {
        theta = num;
        while (theta > 360) theta -= 360;
        while (theta < 0) theta += 360;
    }

    void setPsy(float num) {
        psy = num;
        if (psy > 90) psy = 90;
        if (psy < -90) psy = -90;
    }

    float getR() {
        return r;
    }

    float getTheta() {
        return theta;
    }

    float getPsy() {
        return psy;
    }

    vec3 getPosition(vec3 target) {
        vec3 retpos;
        retpos.x = r*cos(psy*deg)*cos(theta*deg) + target.x;
        retpos.y = r*sin(psy*deg) + target.y;
        retpos.z = r*cos(psy*deg)*sin(theta*deg) + target.z;
        return retpos;
    }

private:
    float r, theta, psy;

};

#endif

#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include "vec_mat.h"

typedef struct Light {
    bool on;
    vec4 position;
    vec3 spotDirection;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float spotCutoff;
    float spotExponent;
    vec3 attenuation;  //  [0] = constant, [1] = linear, [2] = quadratic
} Light;

Light defaultLight =
{
    true,
    {1.0, 1.0, 1.0, 1.0},
    {-1.0, -1.0, -1.0},
    {0.0, 0.0, 0.0, 1.0 },
    {1.0, 1.0, 1.0, 1.0 },
    {1.0, 1.0, 1.0, 1.0 },
    180.0, 0.0,
    {1.0, 0.0, 0.0}
};

#endif // LIGHT_H_INCLUDED

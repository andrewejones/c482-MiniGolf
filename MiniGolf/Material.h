#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "vec_mat.h"

typedef struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;

} Material;

Material greensMaterial =
{
    {0.1, 0.4, 0.2, 1.0},
    {0.0, 0.2, 0.0, 1.0},
    {0.0, 0.1, 0.1, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    5.0
};

Material cupMaterial =
{
    {0.5, 0.5, 0.5, 1.0},
    {.5, 0.5, 0.5, 1.0},
    {0.1, 0.1, 0.1, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.5
};

Material ballMaterial =
{
    {0.7, 0.7, 0.7, 1.0},
    {.5, 0.5, 0.5, 1.0},
    {0.2, 0.2, 0.2, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    2.0
};

Material groundMaterial =
{
    {0.1, 0.4, 0.2, 1.0},
    {0.0, 0.1, 0.1, 1.0},
    {0.0, 0.1, 0.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.5
};

Material wallsMaterial =
{
    {0.1, 0.1, 0.1, 1},
    {0.2, 0.2, 0.2, 1},
    {0.1, 0.1, 0.1, 1},
    {0.0, 0.0, 0.0, 1.0},
    0.5
};

Material grassMaterial =
{
    {0.1, 0.3, 0.1, 1.0},
    {0.1, 0.2, 0.1, 1.0},
    {0.1, 0.2, 0.1, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    1.0
};

Material skyMaterial =
{
    {0.1, 0.1, 0.2, 1.0},
    {0.0, 0.0, 0.3, 1.0},
    {0.2, 0.2, 0.2, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    2.0
};

Material treeMaterial =
{
    {0.37, 0.15, 0.02, 1.0},
    {0.18, 0.07, 0.01, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    0.5
};

Material redDull =
{
    {0.2, 0.1, 0.1, 1.0},
    {0.3, 0.0, 0.0, 1.0},
    {0.2, 0.2, 0.2, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    1.0
};

Material greenDull =
{
    {0.1, 0.2, 0.1, 1.0},
    {0.0, 0.3, 0.0, 1.0},
    {0.2, 0.2, 0.2, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    2.0
};

Material blueDull =
{
    {0.1, 0.1, 0.2, 1.0},
    {0.0, 0.0, 0.3, 1.0},
    {0.2, 0.2, 0.2, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    2.0
};

Material redPlastic =
{
    {0.3, 0.0, 0.0, 1.0},
    {0.6, 0.0, 0.0, 1.0},
    {0.8, 0.6, 0.6, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    32.0
};

Material greenPlastic =
{
    {0.0, 0.3, 0.0, 1.0},
    {0.0, 0.6, 0.0, 1.0},
    {0.6, 0.8, 0.6, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    32.0
};

Material bluePlastic =
{
    {0.0, 0.0, 0.3, 1.0},
    {0.0, 0.0, 0.6, 1.0},
    {0.6, 0.6, 0.8, 1.0},
    {0.0, 0.0, 0.0, 1.0},
    32.0
};

Material brass =
{
    {0.329412, 0.223529, 0.027451, 1},
    {0.780392, 0.568627, 0.113725, 1},
    {0.992157, 0.941176, 0.807843, 1},
    {0.0, 0.0, 0.0, 1.0},
    27.8974
};

Material bronze =
{
    {0.2125, 0.1275, 0.054, 1},
    {0.714, 0.4284, 0.18144, 1},
    {0.393548, 0.271906, 0.166721, 1},
    {0.0, 0.0, 0.0, 1.0},
    25.6
};

Material polishedbronze =
{
    {0.25, 0.148, 0.06475, 1},
    {0.4, 0.2368, 0.1036, 1},
    {0.774597, 0.458561, 0.200621, 1},
    {0.0, 0.0, 0.0, 1.0},
    76.8
};

Material chrome =
{
    {0.25, 0.25, 0.25, 1},
    {0.4, 0.4, 0.4, 1},
    {0.774597, 0.774597, 0.774597, 1},
    {0.0, 0.0, 0.0, 1.0},
    76.8
};

Material copper =
{
    {0.19125, 0.0735, 0.0225, 1},
    {0.7038, 0.27048, 0.0828, 1},
    {0.256777, 0.137622, 0.086014, 1},
    {0.0, 0.0, 0.0, 1.0},
    12.8
};

Material polishedcopper =
{
    {0.2295, 0.08825, 0.0275, 1},
    {0.5508, 0.2118, 0.066, 1},
    {0.580594, 0.223257, 0.0695701, 1},
    {0.0, 0.0, 0.0, 1.0},
    51.2
};

Material gold =
{
    {0.24725, 0.1995, 0.0745, 1},
    {0.75164, 0.60648, 0.22648, 1},
    {0.628281, 0.555802, 0.366065, 1},
    {0.0, 0.0, 0.0, 1.0},
    51.2
};

Material polishedgold =
{
    {0.24725, 0.2245, 0.0645, 1},
    {0.34615, 0.3143, 0.0903, 1},
    {0.797357, 0.723991, 0.208006, 1},
    {0.0, 0.0, 0.0, 1.0},
    83.2
};

Material pewter =
{
    {0.105882, 0.058824, 0.113725, 1},
    {0.427451, 0.470588, 0.541176, 1},
    {0.333333, 0.333333, 0.521569, 1},
    {0.0, 0.0, 0.0, 1.0},
    9.84615
};

Material silver =
{
    {0.19225, 0.19225, 0.19225, 1},
    {0.50754, 0.50754, 0.50754, 1},
    {0.508273, 0.508273, 0.508273, 1},
    {0.0, 0.0, 0.0, 1.0},
    51.2
};

Material polishedsilver =
{
    {0.23125, 0.23125, 0.23125, 1},
    {0.2775, 0.2775, 0.2775, 1},
    {0.773911, 0.773911, 0.773911, 1},
    {0.0, 0.0, 0.0, 1.0},
    89.6
};

Material emerald =
{
    {0.0215, 0.1745, 0.0215, 0.55},
    {0.07568, 0.61424, 0.07568, 0.55},
    {0.633, 0.727811, 0.633, 0.55},
    {0.0, 0.0, 0.0, 1.0},
    76.8
};

Material jade =
{
    {0.135, 0.2225, 0.1575, 0.95},
    {0.54, 0.89, 0.63, 0.95},
    {0.316228, 0.316228, 0.316228, 0.95},
    {0.0, 0.0, 0.0, 1.0},
    12.8
};

Material obsidian =
{
    {0.05375, 0.05, 0.06625, 0.82},
    {0.18275, 0.17, 0.22525, 0.82},
    {0.332741, 0.328634, 0.346435, 0.82},
    {0.0, 0.0, 0.0, 1.0},
    38.4
};

Material pearl =
{
    {0.25, 0.20725, 0.20725, 0.922},
    {1, 0.829, 0.829, 0.922},
    {0.296648, 0.296648, 0.296648, 0.922},
    {0.0, 0.0, 0.0, 1.0},
    11.264
};

Material ruby =
{
    {0.1745, 0.01175, 0.01175, 0.55},
    {0.61424, 0.04136, 0.04136, 0.55},
    {0.727811, 0.626959, 0.626959, 0.55},
    {0.0, 0.0, 0.0, 1.0},
    76.8
};

Material turquoise =
{
    {0.1, 0.18725, 0.1745, 0.8},
    {0.396, 0.74151, 0.69102, 0.8},
    {0.297254, 0.30829, 0.306678, 0.8},
    {0.0, 0.0, 0.0, 1.0},
    12.8
};

#endif // MATERIAL_H_INCLUDED
#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

#define BUFFER_OFFSET(x) ((const void*) (x))

using namespace std;

class Model {
public:
    vector<GLfloat> vertices, colors, normals, textures;
    vector<GLushort> vertexIndices;
    GLint vPosition = 0, vColor = 1, vNormal = 2, vTexture = 3;
    GLuint vboptr, bufptr, eboptr;
    GLfloat red = 1, green = 0, blue = 0;

// Methods

    Model() {}


    ~Model() { }


    void loadOBJ(const char* fileName) {

        ifstream in(fileName, ios::in);
        if (!in) {
            cerr << "ERROR: Cannot open file '" << fileName << "'" << endl;
            exit(1);
        }

        GLfloat x, y, z;
        GLushort index;
        vector<vec2> temptexs;
        vector<vec3> tempnorms;
        vector<GLushort> normalIndices, textureIndices;

        string line;
        while (getline(in, line)) {
            if (line.substr(0,2) == "v ") { // vertices
                istringstream s(line.substr(2));
                s >> x; s >> y; s >> z;
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            } else if (line.substr(0,3) == "vn ") { // normals
                istringstream s(line.substr(3));
                s >> x; s >> y; s >> z;
                tempnorms.push_back(vec3(x, y, z));
            }  else if (line.substr(0,2) == "f ") { // indices
                istringstream s(line.substr(2));
                if (line.find("//") != string::npos) { // vertices and normals
                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                } else if (line.find('/') != string::npos) { // vertices, normals and textures
                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    textureIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    textureIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    textureIndices.push_back(index);
                    s.ignore(256,'/');
                    s >> index; index--;
                    normalIndices.push_back(index);
                } else  { // vertices
                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                    s.ignore(256,' ');

                    s >> index; index--;
                    vertexIndices.push_back(index);
                }
            } else if (line.substr(0,3) == "vt ") { // texture coordinates
                istringstream s(line.substr(3));
                s >> x; s >> y;
                temptexs.push_back(vec2(x,y));
            } else {  } // ignore everything else
        }

        normals.resize(vertices.size());
        for (int i = 0; i < vertexIndices.size(); i++) {
            normals[(3*vertexIndices[i])+0] = tempnorms[normalIndices[i]][0];
            normals[(3*vertexIndices[i])+1] = tempnorms[normalIndices[i]][1];
            normals[(3*vertexIndices[i])+2] = tempnorms[normalIndices[i]][2];
        }
        if (temptexs.size() > 0) {
            textures.resize(vertices.size()/3*2);
            for (int i = 0; i < vertexIndices.size(); i++) {
                textures[(2*vertexIndices[i])+0] = temptexs[textureIndices[i]][0];
                textures[(2*vertexIndices[i])+1] = temptexs[textureIndices[i]][1];
            }
        }

    }


    void load(GLint vPos, GLint vCol, GLint vNorm, GLint vTex) {

        vPosition = vPos, vColor = vCol, vNormal = vNorm, vTexture = vTex;
        colors.clear();

        int num = vertices.size();
        for (int i = 0; i < num; i++) {
            colors.push_back(red);
            colors.push_back(green);
            colors.push_back(blue);
        }

        glGenVertexArrays(1, &vboptr);
        glBindVertexArray(vboptr);

        glGenBuffers(1, &eboptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboptr);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * vertexIndices.size(), &vertexIndices[0], GL_DYNAMIC_DRAW);

        glGenBuffers(1, &bufptr);
        glBindBuffer(GL_ARRAY_BUFFER, bufptr);
        glBufferData(GL_ARRAY_BUFFER,
                    (sizeof(GLfloat)*vertices.size()) + (sizeof(GLfloat)*colors.size()) + (sizeof(GLfloat)*normals.size()) + (sizeof(GLfloat)*textures.size()),
                     NULL,
                     GL_DYNAMIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        (sizeof(GLfloat)*vertices.size()),
                        &vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        (sizeof(GLfloat)*vertices.size()),
                        (sizeof(GLfloat)*colors.size()),
                        &colors[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        (sizeof(GLfloat)*vertices.size()) + (sizeof(GLfloat)*colors.size()),
                        (sizeof(GLfloat)*normals.size()),
                        &normals[0]);
        glBufferSubData(GL_ARRAY_BUFFER,
                        (sizeof(GLfloat)*vertices.size()) + (sizeof(GLfloat)*colors.size()) + (sizeof(GLfloat)*normals.size()),
                        (sizeof(GLfloat)*textures.size()),
                        &textures[0]);

        glVertexAttribPointer(vPosition,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              BUFFER_OFFSET(0));
        glVertexAttribPointer(vColor,
                              3,
                              GL_FLOAT,
                              GL_TRUE,
                              0,
                              BUFFER_OFFSET(sizeof(GLfloat)*vertices.size()));
        glVertexAttribPointer(vNormal,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              BUFFER_OFFSET((sizeof(GLfloat)*vertices.size()) + (sizeof(GLfloat)*colors.size())));
        glVertexAttribPointer(vTexture,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              BUFFER_OFFSET((sizeof(GLfloat)*vertices.size()) + (sizeof(GLfloat)*colors.size())+ (sizeof(GLfloat)*normals.size())));

        glEnableVertexAttribArray(vPosition);
        glEnableVertexAttribArray(vColor);
        glEnableVertexAttribArray(vNormal);
        glEnableVertexAttribArray(vTexture);

    }


    void draw() {

        glBindVertexArray(vboptr);
        glDrawElements(GL_TRIANGLES, vertexIndices.size(), GL_UNSIGNED_SHORT, NULL);

    }


};

#endif // __MODEL_H__

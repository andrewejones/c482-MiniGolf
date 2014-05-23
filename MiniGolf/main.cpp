#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SFML/Audio.hpp>
#include <btBulletDynamicsCommon.h>
#include <sstream>
#include "vec_mat.h"
#include "Material.h"
#include "Light.h"
#include "Image_File_IO.h"
#include "LoadShaders.h"
#include "SphericalCamera.h"
#include "Model.h"

using namespace std;

GLuint program, tex1_uniform_loc, ballTexID;
GLint winWidth = 800, winHeight = 600;
GLint vPosition = 0, vColor = 1, vNormal = 2, vTexture = 3;

bool needReset = false, needReplay = false, playedClap = false, clapLoaded = false,
     playedAww = false, awwLoaded = false, puttLoaded = false;
int currStrokeNum = 0, totalStrokeNum = 0, powerScale = 20;
unsigned char holeNum = '1';
float timer = 0.0;
GLint mouseDown = 0, startX, startY;

SphericalCamera camera;
mat4 proj, view, trans;

int numLights = 1;
Light Lts[10];

sf::SoundBuffer clapBuffer, awwBuffer, natureBuffer, puttBuffer;
sf::Sound clapSound, awwSound, natureSound, puttSound;

struct CourseObj {
    string name;
    Model *model;
    Material material;
    GLuint textureID;

    btCollisionShape *collisionShape;
    btDefaultMotionState *motionState;
    btRigidBody *rigidBody;

    int *triIndexBase;
    btScalar *vertBase;
    btTriangleIndexVertexArray *tIVA;
};

// object models
Model *ballModel;

Model *treeModel;
Model *leafModel;
const int treeNum = 50;
const float treeMinRad = 250;
const float treeMaxRad = 2000;
mat4 treeMats[treeNum];

// object physics
btCollisionShape *ballCollisionShape;
btRigidBody *ballRigidBody;
vector<CourseObj*> courseObjects;

// world physics
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfiguration;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;


float rngf(float fmin, float fmax) {
    if (fmin == fmax || fmin > fmax)
        return fmax;
    return (((((float) rand()) / (float) RAND_MAX) * (fmax - fmin)) + fmin);
}


void LoadLights(Light Lt[], int num) {
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        string locID = arraypos;
        locID.append(".on");
        glUniform1i(glGetUniformLocation(program, locID.c_str()), Lt[i].on);

        locID = arraypos;
        locID.append(".position");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].position);

        locID = arraypos;
        locID.append(".ambient");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].ambient);

        locID = arraypos;
        locID.append(".diffuse");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].diffuse);

        locID = arraypos;
        locID.append(".specular");
        glUniform4fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].specular);

        locID = arraypos;
        locID.append(".spotDirection");
        glUniform3fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].spotDirection);

        locID = arraypos;
        locID.append(".attenuation");
        glUniform3fv(glGetUniformLocation(program, locID.c_str()), 1, Lt[i].attenuation);

        locID = arraypos;
        locID.append(".spotCutoff");
        glUniform1f(glGetUniformLocation(program, locID.c_str()), Lt[i].spotCutoff);

        locID = arraypos;
        locID.append(".spotExponent");
        glUniform1f(glGetUniformLocation(program, locID.c_str()), Lt[i].spotExponent);
    }
}


void LoadLight(Light Lt) {
    glUniform1i(glGetUniformLocation(program, "Lt.on"), Lt.on);
    glUniform4fv(glGetUniformLocation(program, "Lt.position"), 1, Lt.position);
    glUniform4fv(glGetUniformLocation(program, "Lt.ambient"), 1, Lt.ambient);
    glUniform4fv(glGetUniformLocation(program, "Lt.diffuse"), 1, Lt.diffuse);
    glUniform4fv(glGetUniformLocation(program, "Lt.specular"), 1, Lt.specular);
    glUniform3fv(glGetUniformLocation(program, "Lt.spotDirection"), 1, Lt.spotDirection);
    glUniform3fv(glGetUniformLocation(program, "Lt.attenuation"), 1, Lt.attenuation);
    glUniform1f(glGetUniformLocation(program, "Lt.spotCutoff"), Lt.spotCutoff);
    glUniform1f(glGetUniformLocation(program, "Lt.spotExponent"), Lt.spotExponent);
}


void TurnLightOn(Light Lt) {
    Lt.on = true;
    glUniform1i(glGetUniformLocation(program, "Lt.on"), Lt.on);
}


void TurnLightOff(Light Lt) {
    Lt.on = false;
    glUniform1i(glGetUniformLocation(program, "Lt.on"), Lt.on);
}


void TurnLightsOn(Light Lt[], int num) {
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        Lt[i].on = true;
        glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[i].on);
    }
}


void TurnLightsOff(Light Lt[], int num) {
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    for (int i = 0; i < num; i++)
    {
        char n[10];
        itoa(i, n, 10);
        string arraypos = arrayname;
        arraypos += "[";
        arraypos += n;
        arraypos += "]";

        Lt[i].on = false;
        glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[i].on);
    }
}


void TurnOnALight(Light Lt[], int num) {
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    char n[10];
    itoa(num, n, 10);
    string arraypos = arrayname;
    arraypos += "[";
    arraypos += n;
    arraypos += "]";

    Lt[num].on = true;
    glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[num].on);
}


void TurnOffALight(Light Lt[], int num) {
    using namespace std;
    string arrayname = "Lt";  // array name in the shader.

    char n[10];
    itoa(num, n, 10);
    string arraypos = arrayname;
    arraypos += "[";
    arraypos += n;
    arraypos += "]";

    Lt[num].on = false;
    glUniform1i(glGetUniformLocation(program, arraypos.append(".on").c_str()), Lt[num].on);
}


void LoadMaterial(Material Mat) {
    glUniform4fv(glGetUniformLocation(program, "Mat.ambient"), 1, Mat.ambient);
    glUniform4fv(glGetUniformLocation(program, "Mat.diffuse"), 1, Mat.diffuse);
    glUniform4fv(glGetUniformLocation(program, "Mat.specular"), 1, Mat.specular);
    glUniform4fv(glGetUniformLocation(program, "Mat.emission"), 1, Mat.emission);
    glUniform1f(glGetUniformLocation(program, "Mat.shininess"), Mat.shininess);
}


GLuint loadTextureBMP(const char* filename) {
    GLint imgwidth;
    GLint imgheight;
    GLuint texID;

    GLbyte* image1 = LoadBMP(filename, &imgwidth, &imgheight);
    if (!image1) return 0;

    glGenTextures(1, &texID);
    if (texID == 0) return 0;

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgwidth, imgheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(image1);
    return texID;
}


void initPhysics() {

	// initialize physics world
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-10,0));

}


void exitPhysics() {

    // delete ball
    if (ballModel) {
        if (ballRigidBody && ballRigidBody->getMotionState())
            delete ballRigidBody->getMotionState();
        dynamicsWorld->removeCollisionObject(ballRigidBody);
        if (ballRigidBody) delete ballRigidBody;
        if (ballCollisionShape) delete ballCollisionShape;
        if (ballModel) delete ballModel;
    }

    // delete other objects
    for(int i = courseObjects.size() - 1; i >= 0; i--) {
        if (courseObjects[i]->rigidBody && courseObjects[i]->rigidBody->getMotionState())
            delete courseObjects[i]->rigidBody->getMotionState();
        dynamicsWorld->removeCollisionObject(courseObjects[i]->rigidBody);
        if (courseObjects[i]->rigidBody) delete courseObjects[i]->rigidBody;
        if (courseObjects[i]->collisionShape) delete courseObjects[i]->collisionShape;
        if (courseObjects[i]->model) delete courseObjects[i]->model;
        if (courseObjects[i]->triIndexBase) delete courseObjects[i]->triIndexBase;
        if (courseObjects[i]->vertBase) delete courseObjects[i]->vertBase;
        if (courseObjects[i]->tIVA) delete courseObjects[i]->tIVA;
        courseObjects.pop_back();
    }

    // delete world
    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;

}


void loadBall(vec3 pos) {

    // load model
    Model *model = new Model();
    model->loadOBJ("Courses\\Shared\\OBJs\\ball.obj");
    model->red = 1; model->green = 1; model->blue = 1;
    model->load(vPosition, vColor, vNormal, vTexture);
    ballModel = model;

    glActiveTexture(GL_TEXTURE0);
    ballTexID = loadTextureBMP("Courses\\Shared\\Textures\\ball.bmp");
    if (ballTexID == 0)
        std::cout << "ERROR: Couldn't load texture: '" << "Courses\\Shared\\Textures\\ball.bmp" << "'" << endl;

    // load physics
    ballCollisionShape = new btSphereShape(1);

    btScalar mass = 1;
	btVector3 inertia(0, 0, 0);
    ballCollisionShape->calculateLocalInertia(mass, inertia);

    btTransform initTrans;
    initTrans.setIdentity();
    initTrans.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btDefaultMotionState *motionState = new btDefaultMotionState(initTrans);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(mass, motionState, ballCollisionShape, inertia);
    ballRigidBody = new btRigidBody(rigidBodyConstructionInfo);

    ballRigidBody->setRestitution(.4);
    ballRigidBody->setFriction(.7);
    ballRigidBody->setRollingFriction(0.4);
    ballRigidBody->setAnisotropicFriction(ballCollisionShape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

    dynamicsWorld->addRigidBody(ballRigidBody);

}


void loadTrees() {

    // load tree
    Model *model = new Model();
    model->loadOBJ("Courses\\Shared\\OBJs\\tree.obj");
    model->red = .5; model->green = .5; model->blue = .5;
    model->load(vPosition, vColor, vNormal, vTexture);
    treeModel = model;

    // load leaves
    model = new Model();
    model->loadOBJ("Courses\\Shared\\OBJs\\leaves.obj");
    model->red = .5; model->green = .5; model->blue = .5;
    model->load(vPosition, vColor, vNormal, vTexture);
    leafModel = model;

    int num = treeNum;
    for (int i = 0; i < num; i++)
        treeMats[i] = Translate(rngf(treeMinRad, treeMaxRad)*cos(2.0*PI/((float)treeNum)*i), 0, rngf(treeMinRad, treeMaxRad)*sin(2.0*PI/((float)treeNum)*i)) * Scale(15);

}


void loadCourse(vec3 ball, vec3 cam, vector<string> models, vector<string> texs, vector<Material> mats) {

    // load course
    loadBall(ball);
    loadTrees();
    camera.setPosition(cam);

    CourseObj *courseObj;
    Model *model;
    btCollisionShape *collisionShape;
    btDefaultMotionState *motionState;
    btRigidBody *rigidBody;

    int numInd, numVert, *triIndexBase;
    btScalar *vertBase;
    btTriangleIndexVertexArray *tIVA;
    btTransform initTrans;

    //-------------------------------------------------------------------------

    int texID;
    string str;
    for (int i = 0; i < models.size(); i++) {
        model = new Model();
        str = models[i];
        model->loadOBJ(str.c_str());
        model->red = 0; model->green = .7; model->blue = .2;
        model->load(vPosition, vColor, vNormal, vTexture);

        str = texs[i];
        glActiveTexture(GL_TEXTURE0+i+1);
        texID = loadTextureBMP(str.c_str());
        if (texID == 0)
            std::cout << "ERROR: Couldn't load texture: '" << str << "'" << endl;

        numInd = model->vertexIndices.size();
        numVert = model->vertices.size();

        triIndexBase = new int[numInd];
        for(int i = 0; i < numInd; i++)
            triIndexBase[i] = model->vertexIndices[i];

        vertBase = new btScalar[numVert];
        for(int i = 0; i < numVert; i++)
            vertBase[i] = model->vertices[i];

        tIVA = new btTriangleIndexVertexArray(numInd/3,
                                              triIndexBase,
                                              3*(sizeof(int)),
                                              numVert/3,
                                              vertBase,
                                              3*(sizeof(btScalar)));

        collisionShape = new btBvhTriangleMeshShape(tIVA, false);

        initTrans.setIdentity();
        initTrans.setOrigin(btVector3(0, -1, 0));

        motionState = new btDefaultMotionState(initTrans);

        rigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, motionState, collisionShape, btVector3(0,0,0)));
        rigidBody = rigidBody;

        rigidBody->setRestitution(1);
        rigidBody->setFriction(1);
        rigidBody->setRollingFriction(1);

        dynamicsWorld->addRigidBody(rigidBody);

        courseObj = new CourseObj;
        courseObj->model = model;
        courseObj->name = models[i];
        courseObj->material = mats[i];
        courseObj->textureID = texID;
        courseObj->collisionShape = collisionShape;
        courseObj->motionState = motionState;
        courseObj->rigidBody = rigidBody;
        courseObj->tIVA = tIVA;
        courseObj->triIndexBase = triIndexBase;
        courseObj->vertBase = vertBase;
        courseObjects.push_back(courseObj);
    }

}


void keyboard(unsigned char key, int x, int y) {

    vec3 v3;
    btVector3 btv3, vel;
    btTransform transf;

    vector<string> models;
    vector<string> texs;
    vector<Material> mats;

    switch(key) {

        case '1':
            exitPhysics();
            initPhysics();
            if (!needReset)
                currStrokeNum = 0;
            if (!needReplay && !needReset)
                totalStrokeNum = 0;
            needReplay = false;
            needReset = false;
            playedClap = false;

            holeNum = key;

            models.push_back("Courses\\Shared\\OBJs\\ground.obj");
            models.push_back("Courses\\Shared\\OBJs\\sky.obj");
            models.push_back("Courses\\Shared\\OBJs\\cup.obj");
            models.push_back("Courses\\1\\OBJs\\greens.obj");
            models.push_back("Courses\\1\\OBJs\\walls.obj");

            texs.push_back("Courses\\Shared\\Textures\\ground.bmp");
            texs.push_back("Courses\\Shared\\Textures\\sky.bmp");
            texs.push_back("Courses\\Shared\\Textures\\cup.bmp");
            texs.push_back("Courses\\Shared\\Textures\\greens.bmp");
            texs.push_back("Courses\\Shared\\Textures\\walls.bmp");

            mats.push_back(groundMaterial);
            mats.push_back(skyMaterial);
            mats.push_back(cupMaterial);
            mats.push_back(greensMaterial);
            mats.push_back(wallsMaterial);

            loadCourse(vec3(0, 10, 175), vec3(30, 90, 30), models, texs, mats);
            break;

        case '2':
            exitPhysics();
            initPhysics();
            if (!needReset)
                currStrokeNum = 0;
            if (!needReplay && !needReset)
                totalStrokeNum = 0;
            needReplay = false;
            needReset = false;
            playedClap = false;

            holeNum = key;

            models.push_back("Courses\\Shared\\OBJs\\ground.obj");
            models.push_back("Courses\\Shared\\OBJs\\sky.obj");
            models.push_back("Courses\\Shared\\OBJs\\cup.obj");
            models.push_back("Courses\\2\\OBJs\\greens.obj");
            models.push_back("Courses\\2\\OBJs\\walls.obj");

            texs.push_back("Courses\\Shared\\Textures\\ground.bmp");
            texs.push_back("Courses\\Shared\\Textures\\sky.bmp");
            texs.push_back("Courses\\Shared\\Textures\\cup.bmp");
            texs.push_back("Courses\\Shared\\Textures\\greens.bmp");
            texs.push_back("Courses\\Shared\\Textures\\walls.bmp");

            mats.push_back(groundMaterial);
            mats.push_back(skyMaterial);
            mats.push_back(cupMaterial);
            mats.push_back(greensMaterial);
            mats.push_back(wallsMaterial);

            loadCourse(vec3(-135, 10, 120), vec3(30, 90, 30), models, texs, mats);
            break;

        case '3':
            exitPhysics();
            initPhysics();
            if (!needReset)
                currStrokeNum = 0;
            if (!needReplay && !needReset)
                totalStrokeNum = 0;
            needReplay = false;
            needReset = false;
            playedClap = false;

            holeNum = key;

            models.push_back("Courses\\Shared\\OBJs\\ground.obj");
            models.push_back("Courses\\Shared\\OBJs\\sky.obj");
            models.push_back("Courses\\Shared\\OBJs\\cup.obj");
            models.push_back("Courses\\3\\OBJs\\greens.obj");
            models.push_back("Courses\\3\\OBJs\\walls.obj");

            texs.push_back("Courses\\Shared\\Textures\\ground.bmp");
            texs.push_back("Courses\\Shared\\Textures\\sky.bmp");
            texs.push_back("Courses\\Shared\\Textures\\cup.bmp");
            texs.push_back("Courses\\Shared\\Textures\\greens.bmp");
            texs.push_back("Courses\\Shared\\Textures\\walls.bmp");

            mats.push_back(groundMaterial);
            mats.push_back(skyMaterial);
            mats.push_back(cupMaterial);
            mats.push_back(greensMaterial);
            mats.push_back(wallsMaterial);

            loadCourse(vec3(0, 10, 200), vec3(30, 90, 30), models, texs, mats);
            break;

        case '4':
            exitPhysics();
            initPhysics();
            if (!needReset)
                currStrokeNum = 0;
            if (!needReplay && !needReset)
                totalStrokeNum = 0;
            needReplay = false;
            needReset = false;
            playedClap = false;

            holeNum = key;

            models.push_back("Courses\\Shared\\OBJs\\ground.obj");
            models.push_back("Courses\\Shared\\OBJs\\sky.obj");
            models.push_back("Courses\\Shared\\OBJs\\cup.obj");
            models.push_back("Courses\\4\\OBJs\\greens.obj");
            models.push_back("Courses\\4\\OBJs\\walls.obj");

            texs.push_back("Courses\\Shared\\Textures\\ground.bmp");
            texs.push_back("Courses\\Shared\\Textures\\sky.bmp");
            texs.push_back("Courses\\Shared\\Textures\\cup.bmp");
            texs.push_back("Courses\\Shared\\Textures\\greens.bmp");
            texs.push_back("Courses\\Shared\\Textures\\walls.bmp");

            mats.push_back(groundMaterial);
            mats.push_back(skyMaterial);
            mats.push_back(cupMaterial);
            mats.push_back(greensMaterial);
            mats.push_back(wallsMaterial);

            loadCourse(vec3(187.5, 40, -50), vec3(50, 270, 30), models, texs, mats);
            break;

        case ' ': // putt ball
            if (!needReset && !needReplay) {
                btv3 = ballRigidBody->getCenterOfMassPosition();
                v3 = camera.getPosition(vec3(btv3[0], btv3[1], btv3[2]));
                v3 = normalize(vec3(v3.x-btv3[0], 0, v3.z-btv3[2]));

                vel = ballRigidBody->getLinearVelocity();
                if (vel[0] == 0 && vel[1] == 0 && vel[2] == 0) { // checks if ball not moving
                    ballRigidBody->activate(true);
                    ballRigidBody->setLinearVelocity(-powerScale * btVector3(v3.x, 0, v3.z));
                    currStrokeNum++;
                    totalStrokeNum++;
                    if (puttLoaded)
                        puttSound.play();
                }
            }
            break;

        case 'n': // next course
            if (needReplay)
                keyboard(holeNum+1, 0, 0);
            break;

        case 'z': // reset ball
            if (needReset)
                keyboard(holeNum, 0, 0);
            break;

        case 'w': // increase power
            powerScale += 5;
            if (powerScale > 100) powerScale = 100;
            break;

        case 's': // subtract power
            powerScale -= 5;
            if (powerScale < 5) powerScale = 5;
            break;

    }

    glutPostRedisplay();

}


void init() {

    glEnable(GL_DEPTH_TEST);
    ShaderInfo shaders[] =
    {
        {GL_VERTEX_SHADER, "Texture.vert"},
        {GL_FRAGMENT_SHADER, "Texture.frag"},
        {GL_NONE, NULL}
    };

    program = LoadShaders(shaders);
    glUseProgram(program);

    glUniform4fv(glGetUniformLocation(program, "GlobalAmbient"), 1, vec4(0.2, 0.2, 0.2, 1));

    tex1_uniform_loc = glGetUniformLocation(program, "tex1");
    glUniform1i(tex1_uniform_loc, 0);

    // 400 is my radius...
    numLights = 3;
    for (int i = 0; i < numLights; i++) {
        Lts[i] = defaultLight;
        Lts[i].position = vec4(400*cos(2.0*PI/((float)numLights)*i), 200, 400*sin(2.0*PI/((float)numLights)*i), 1);
    }

    LoadLights(Lts, numLights);
    glUniform1i(glGetUniformLocation(program, "numLights"), numLights);

    // sound set up
    if (clapBuffer.loadFromFile("Courses\\Shared\\Sounds\\clap.wav")) {
        clapLoaded = true;
        clapSound.setBuffer(clapBuffer);
    }
    if (awwBuffer.loadFromFile("Courses\\Shared\\Sounds\\aww.wav")) {
        awwLoaded = true;
        awwSound.setBuffer(awwBuffer);
    }
    if (puttBuffer.loadFromFile("Courses\\Shared\\Sounds\\putt.wav")) {
        puttLoaded = true;
        puttSound.setBuffer(puttBuffer);
    }
    if (natureBuffer.loadFromFile("Courses\\Shared\\Sounds\\nature.wav")) {
        natureSound.setBuffer(natureBuffer);
        natureSound.setLoop(true);
        natureSound.play();
    }

    // default course setup
    keyboard('1', 0, 0);
}


void printOpenGLErrors() {

    GLenum errCode;
    const GLubyte *errString;

    while ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        cout << "OpenGL Error: " << errString << endl;
    }

}


void special(int key, int x, int y) {

    switch(key) {

        case GLUT_KEY_UP: // increase power
            powerScale += 5;
            if (powerScale > 100) powerScale = 100;
            break;

        case GLUT_KEY_DOWN: // decrease power
            powerScale -= 5;
            if (powerScale < 5) powerScale = 5;
            break;

    }

    glutPostRedisplay();

}


void mouse(int button, int state, int x, int y) {

    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
        mouseDown = 1;
        startX = x;
        startY = y;
    } else
        mouseDown = 0;

}


void motion(int x, int y) {

    if (mouseDown == 1) {
        camera.addTheta((x - startX) / 2.0);
        startX = x;

        camera.addPsy((y - startY) / 2.0);
        startY = y;

        glutPostRedisplay();
    }

}


void wheel(int wheel, int direction, int x, int y) {

    GLint shiftDown = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    if (shiftDown) {
        if (direction < 0) {
            powerScale -= 1;
            if (powerScale < 5) powerScale = 5;
        } else {
            powerScale += 1;
            if (powerScale > 100) powerScale = 100;
        }
    } else {
        if (direction < 0) {
            camera.addR(1.5);
        } else {
            camera.addR(-1.5);
        }
    }

}


void reshape(GLint widthIn, GLint heightIn) {

    winWidth = widthIn;
    winHeight = heightIn;

    glViewport(0, 0, widthIn, heightIn);
    proj = Perspective(60.0f, (float)widthIn / (float)heightIn, 0.0001f, 100.0f);

    glutPostRedisplay();

}


mat4 btTransToMat4(btTransform transIn) {
    btScalar matrix[16];
    transIn.getOpenGLMatrix(matrix);
    return mat4(matrix[0], matrix[1], matrix[2], matrix[3],
                matrix[4], matrix[5], matrix[6], matrix[7],
                matrix[8], matrix[9], matrix[10], matrix[11],
                matrix[12], matrix[13], matrix[14], matrix[15]);
}


void writeString(string str, int x, int y, void* font) {

    // set ortho to draw text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glScalef(1, -1, 1);
    glTranslatef(0, -winHeight, 0);
    glMatrixMode(GL_MODELVIEW);

    // draw text
    glRasterPos2i(x, y);
    glColor3f(0.0, 0.0, 0.0);

    int len = str.length();
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, str[i]);
    }

    // reset perspective
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

}


void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // get ball position first, use for camera
    btTransform worldTrans;
    ballRigidBody->getMotionState()->getWorldTransform(worldTrans);
    trans = btTransToMat4(worldTrans);

    // lookat
    btVector3 ballOrigin = worldTrans.getOrigin();
    view = camera.lookAt(vec3(ballOrigin[0],ballOrigin[1],ballOrigin[2]));

    // eye pos for lighting
    glUniform3fv(glGetUniformLocation(program, "eye"), 1, camera.getPosition(vec3(ballOrigin[0],ballOrigin[1],ballOrigin[2])));

    // draw ball
    if (ballTexID != 0) {
        glUniform1i(glGetUniformLocation(program, "useTexture"), 1);
        glUniform1i(tex1_uniform_loc, 0);
    }
    LoadMaterial(ballMaterial);
    glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"), 1, GL_TRUE, proj * view * trans);
    glUniformMatrix4fv(glGetUniformLocation(program, "ModelView"), 1, GL_TRUE, trans);
    mat3 normal = Adjoint(trans);
    glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE, normal);
    ballModel->draw();
    glUniform1i(glGetUniformLocation(program, "useTexture"), 0);


    // draw trees
    LoadMaterial(treeMaterial);
    for (int i = 0; i < treeNum; i++) {
        trans = treeMats[i];
        glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"), 1, GL_TRUE, proj * view * trans);
        glUniformMatrix4fv(glGetUniformLocation(program, "ModelView"), 1, GL_TRUE, trans);
        mat3 normal = Adjoint(trans);
        glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE, normal);
        treeModel->draw();
    }
    // draw leaves
    LoadMaterial(grassMaterial);
    for (int i = 0; i < treeNum; i++) {
        trans = treeMats[i];
        glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"), 1, GL_TRUE, proj * view * trans);
        glUniformMatrix4fv(glGetUniformLocation(program, "ModelView"), 1, GL_TRUE, trans);
        mat3 normal = Adjoint(trans);
        glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE, normal);
        leafModel->draw();
    }


    // draw course stuff
    for (int i = 0; i < courseObjects.size(); i++) {
        if (courseObjects[i]->textureID != 0) {
            glUniform1i(glGetUniformLocation(program, "useTexture"), 1);
            glUniform1i(tex1_uniform_loc, i+1);
        }

        LoadMaterial(courseObjects[i]->material);
        courseObjects[i]->motionState->getWorldTransform(worldTrans);
        trans = btTransToMat4(worldTrans);
        glUniformMatrix4fv(glGetUniformLocation(program, "ModelView"), 1, GL_TRUE, trans);
        glUniformMatrix4fv(glGetUniformLocation(program, "ProjModelView"), 1, GL_TRUE, proj * view * trans);
        normal = Adjoint(trans);
        glUniformMatrix3fv(glGetUniformLocation(program, "NormalMatrix"), 1, GL_FALSE, normal);
        courseObjects[i]->model->draw();

        glUniform1i(glGetUniformLocation(program, "useTexture"), 0);
    }

    // check if ball stationary
    btVector3 vel = ballRigidBody->getLinearVelocity();
    if (vel[0] == 0 && vel[1] == 0 && vel[2] == 0) {
        vel = ballRigidBody->getCenterOfMassPosition();
            if (-2.75 < vel[1] && vel[1] < -2.25) { // check if in hole (-2.75 < y < -2.25), ie won
                if (!playedClap && clapLoaded) {
                    playedClap = true;
                    clapSound.play();
                }
                if (holeNum != '4') {
                    needReplay = true;
                    writeString("It's in the Hole!! Hit 'n' for the Next Hole!", winWidth / 2 - 205, 30, GLUT_BITMAP_TIMES_ROMAN_24);
                } else {
                    writeString("It's in the Hole!! You've Completed the Course!", winWidth / 2 - 215, 30, GLUT_BITMAP_TIMES_ROMAN_24);
                }
            } else if (vel[1] < -2.75) { // check if out of bounds (y <= -2.75)
                needReset = true;
                if (!playedAww && awwLoaded) {
                    playedAww = true;
                    awwSound.play();
                }
                writeString("Out of Bounds!! Hit 'z' to Reset Your Ball!", winWidth / 2 - 210, 30, GLUT_BITMAP_TIMES_ROMAN_24);
            } else { // if neither, print "take your shot"
                writeString("Take Your Best Shot!", winWidth / 2 - 105, 30, GLUT_BITMAP_TIMES_ROMAN_24);
            }
    }

    // print info to screen for user
    stringstream ss;
    ss << "Strokes: ";
    ss << currStrokeNum;
    ss << ", Total: ";
    ss << totalStrokeNum;
    writeString(ss.str(), 10, winHeight - 10, GLUT_BITMAP_TIMES_ROMAN_24);

    ss.str("");
    ss << "Power: ";
    ss << powerScale;
    ss << " / 100";
    writeString(ss.str(), 10, winHeight - 40, GLUT_BITMAP_TIMES_ROMAN_24);

    ss.str("");
    ss << "Hole: ";
    ss << holeNum;
    ss << " of 4";
    writeString(ss.str(), 10, winHeight - 70, GLUT_BITMAP_TIMES_ROMAN_24);

    glutSwapBuffers();
    printOpenGLErrors();

}


void idle() {
    if (camera.getR() < 5) camera.setR(5);
    if (camera.getR() > 500) camera.setR(500);
    if (camera.getPsy() < 10) camera.setPsy(10);
    if (camera.getPsy() > 85) camera.setPsy(85);

    float dtime = timer;
    timer = glutGet(GLUT_ELAPSED_TIME) / 500.0;
    dtime = timer - dtime;

    if (dynamicsWorld) {
        dynamicsWorld->stepSimulation(dtime, 10);
    }

    glutPostRedisplay();

}


int main (int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Mini Golf");

    if (glewInit()) {
        std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
        exit(EXIT_FAILURE);
    }

	init();

    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMouseWheelFunc(wheel);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;

}


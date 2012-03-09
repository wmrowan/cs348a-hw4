#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <limits.h>

#define MAX(X,Y) (X > Y ? X : Y)
#define MIN(X,Y) (X < Y ? X : Y)

struct Point {
        GLfloat vs[0];
        GLfloat x;
        GLfloat y;
        GLfloat z;
};

class Terrain {
private:
    struct Triangle {
        GLfloat vs[0];
        Point v1;
        Point v2;
        Point v3;

        GLfloat max() {
            return MAX(MAX(v1.z, v2.z), v3.z);
        }

        GLfloat min() {
            return MIN(MIN(v1.z, v2.z), v3.z);
        }
    };

    int n_triangles;
    GLfloat max_elevation;
    GLfloat min_elevation;
    Triangle *triangles;

public:
    Terrain() : n_triangles(0), triangles(NULL) {}

    bool init(char *file) {

        // Free any existing state from a previous initialization
        if(triangles) free(triangles);

        std::ifstream in;
        in.open(file);

        // The total number of triangles will be the on the first line of the file
        in >> n_triangles;
        triangles = (Triangle*)malloc(sizeof(Triangle) * n_triangles);

        for(int i = 0; i < n_triangles && in.good(); ++i) {
            in >> triangles[i].v1.x >> triangles[i].v1.y >> triangles[i].v1.z;
            in >> triangles[i].v2.x >> triangles[i].v2.y >> triangles[i].v2.z;
            in >> triangles[i].v3.x >> triangles[i].v3.y >> triangles[i].v3.z;
        }

        if(in.fail()) {
            return false;
        }

        // Now compute the max and min elevations which we'll need for our terrain shading
        max_elevation = INT_MIN;
        min_elevation = INT_MAX;

        for(int i = 0; i < n_triangles; ++i) {
            if(triangles[i].max() > max_elevation)
                max_elevation = triangles[i].max();
            if(triangles[i].min() < min_elevation)
                min_elevation = triangles[i].min();
        }

        return true;
    }

    void draw() {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < n_triangles; ++i) {
            GLfloat elevation = triangles[i].max();
            setElevationColor(elevation);
            glVertex3fv((GLfloat *)&triangles[i].v1.vs);
            glVertex3fv((GLfloat *)&triangles[i].v2.vs);
            glVertex3fv((GLfloat *)&triangles[i].v3.vs);
        }
        glEnd();
        glPopMatrix();
    }

    void setElevationColor(GLfloat elevation) {
        GLfloat relative_height = (elevation - min_elevation) / (max_elevation - min_elevation);
        GLfloat scale = 0.8 - (0.4*relative_height);
        glColor3f(scale,0.8,scale);
    }
};

class Camera {
private:
    Point pos;

public:
    Camera() : pos() {
        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective(45.0, 1, 0.1, 10); 
    }

    void draw() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos.x, pos.y, pos.z,
                  0,0,0,
                  0,1,0);
    }

    void moveTo(GLfloat x, GLfloat y, GLfloat z) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
};

Terrain terrain;
Camera camera;

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.draw();
    terrain.draw();

    glutSwapBuffers();
}

void key(unsigned char k, int x, int y) {
    if(k = 'q') {
        exit(0);
    }
}

void glInit(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(400,400);
    glutInitWindowPosition(40,40);
    glutCreateWindow("Terrain");
	glClearColor(0.795, 0.795, 0.795, 0.0);
    glutDisplayFunc(draw);
    glutKeyboardFunc(key);

    

    // TODO find real values for this
    camera.moveTo(0,0,1);
}

void usage() {
    std::cout << "Usage ./tour terrain_data.tri" << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    glInit(&argc, argv);
    if(argc < 2 || !terrain.init(argv[1])) usage();

    glutMainLoop();
    return 0;
}

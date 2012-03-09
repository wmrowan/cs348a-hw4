#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>

class Terrain {
private:
    struct Point {
        GLfloat vs[0];
        GLfloat v1;
        GLfloat v2;
        GLfloat v3;
    };

    struct Triangle {
        GLfloat ps[0];
        Point p1;
        Point p2;
        Point p3;
    };

    int n_triangles;
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
            in >> triangles[i].p1.v1 >> triangles[i].p1.v2 >> triangles[i].p1.v3;
            in >> triangles[i].p2.v1 >> triangles[i].p2.v2 >> triangles[i].p2.v3;
            in >> triangles[i].p3.v1 >> triangles[i].p3.v2 >> triangles[i].p3.v3;
        }

        return !in.fail();
    }

    void draw() {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < n_triangles; ++i) {
            glColor3f(0.4,0.7,0.9);
            glVertex3fv((GLfloat *)&triangles[i].p1.vs);
            glVertex3fv((GLfloat *)&triangles[i].p2.vs);
            glVertex3fv((GLfloat *)&triangles[i].p3.vs);
        }
        glEnd();
        glPopMatrix();
    }
};

Terrain terrain;

void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    terrain.draw();

    glutSwapBuffers();
}

void glInit(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(400,400);
    glutInitWindowPosition(40,40);
    glutCreateWindow("Terrain");
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glutDisplayFunc(draw);

    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadIdentity();

    // TODO find real values for this
    gluPerspective(45.0, 1, 0.1, 100);
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

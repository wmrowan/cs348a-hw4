#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>

class Terrain {
private:
    struct Triangle {
        float v1;
        float v2;
        float v3;
    };

    int numTriangles;
    Triangle *triangles;

public:
    Terrain() : numTriangles(0), triangles(NULL) {}

    bool init(char *file) {

        // Free any existing state from a previous initialization
        if(triangles) free(triangles);

        std::ifstream in;
        in.open(file);

        // The total number of triangles will be the on the first line of the file
        in >> numTriangles;
        triangles = (Triangle*)malloc(sizeof(Triangle) * numTriangles);

        for(int i = 0; i < numTriangles && in.good(); ++i) {
            in >> triangles[i].v1 >> triangles[i].v2 >> triangles[i].v3;
        }

        return !in.fail();
    }

    void draw() {
        glBegin(GL_TRIANGLES);
        glEnd();
    }
};

void usage() {
    std::cout << "Usage ./tour terrain_data.tri" << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(400,400);
    glutInitWindowPosition(40,40);
    glutCreateWindow("Terrain");

    if(argc < 2) usage();

    Terrain terrain;
    if(!terrain.init(argv[1])) usage();
}

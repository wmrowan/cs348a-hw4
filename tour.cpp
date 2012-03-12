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

        
        GLfloat maxX() {
            return MAX(MAX(v1.x, v2.x), v3.x);
        }

        GLfloat minX() {
            return MIN(MIN(v1.x, v2.x), v3.x);
        }

        GLfloat maxY() {
            return MAX(MAX(v1.y, v2.y), v3.y);
        }

        GLfloat minY() {
            return MIN(MIN(v1.y, v2.y), v3.y);
        }
        
        GLfloat maxZ() {
            return MAX(MAX(v1.z, v2.z), v3.z);
        }

        GLfloat minZ() {
            return MIN(MIN(v1.z, v2.z), v3.z);
        }
    };

    int n_triangles;
    Point max_coords;
    Point min_coords;
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
        max_coords.x = INT_MIN;
	max_coords.y = INT_MIN;
	max_coords.z = INT_MIN;
        min_coords.x = INT_MAX;
	min_coords.y = INT_MAX;
	min_coords.z = INT_MAX;

        for(int i = 0; i < n_triangles; ++i) {
	    if(triangles[i].maxX() > max_coords.x)
	        max_coords.x = triangles[i].maxX();
	    if(triangles[i].minX() < min_coords.x)
	        min_coords.x = triangles[i].minX();
	    if(triangles[i].maxY() > max_coords.y)
	        max_coords.y = triangles[i].maxY();
	    if(triangles[i].minY() < min_coords.y)
	        min_coords.y = triangles[i].minY();
            if(triangles[i].maxZ() > max_coords.z)
	        max_coords.z = triangles[i].maxZ();
            if(triangles[i].minZ() < min_coords.z)
	        min_coords.z = triangles[i].minZ();
	}

	Point minCoords = getMinCoords();
	Point maxCoords = getMaxCoords();

        return true;
    }

    void draw() {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < n_triangles; ++i) {
            GLfloat elevation = triangles[i].maxZ();
            setElevationColor(elevation);
            glVertex3fv((GLfloat *)&triangles[i].v1.vs);
            glVertex3fv((GLfloat *)&triangles[i].v2.vs);
            glVertex3fv((GLfloat *)&triangles[i].v3.vs);
        }
        glEnd();
        glPopMatrix();
    }

    void setElevationColor(GLfloat elevation) {
        GLfloat relative_height = (elevation - min_coords.z) / (max_coords.z - min_coords.z);
        GLfloat scale = 0.8 - (0.4*relative_height);
        glColor3f(scale,0.8,scale);
    }

    Point getMaxCoords() {
        return max_coords;
    }
  
    Point getMinCoords() {
        return min_coords; 
    }
};


class Camera {
private:
    Point pos;
    Point look;
    GLfloat close;
    GLfloat far;
public:
    Camera() : pos() {
        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
    }

    void draw() {
        gluPerspective(45.0, 1, close,far ); 
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos.x, pos.y, pos.z,
                  look.x,look.y,look.z,
                  0,1,0);
    }

    void moveTo(GLfloat x, GLfloat y, GLfloat z) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    
    void lookAt(GLfloat x, GLfloat y, GLfloat z) {
      look.x = x;
      look.y = y;
      look.z = z;
    }

    void setClip(GLfloat min, GLfloat max) {
      close = min;
      far = max;
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

GLfloat AVG(GLfloat x, GLfloat y) {
  return (x+y)/2.0;
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

    
    Point max = terrain.getMaxCoords();
    Point min = terrain.getMinCoords();
    // TODO find real values for this
    //attempting to adjust camera
    camera.moveTo(AVG(max.x,min.x),AVG(max.y,min.y),min.z+1);
    camera.lookAt(AVG(max.x,min.x),AVG(max.y,min.y),AVG(max.z,min.z));
    camera.setClip(min.z, max.z+1);
    printf("%f %f %f\n", min.x, min.y, min.z);
    printf("%f %f %f\n",AVG(max.x,min.x),AVG(max.y,min.y),AVG(max.z,min.z));
    printf("%f %f %f\n",max.x, max.y, max.z);
}

void usage() {
    std::cout << "Usage ./tour terrain_data.tri" << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    if(argc < 2 || !terrain.init(argv[1])) usage();
    glInit(&argc, argv);

    glutMainLoop();
    return 0;
}

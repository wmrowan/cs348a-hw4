#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <limits.h>
#include <assert.h>
#include <list>
#include <math.h>

#define INITIAL_WINDOW_SIZE (400)

#define MAX(X,Y) (X > Y ? X : Y)
#define MIN(X,Y) (X < Y ? X : Y)
#define AVG(X,Y) ((X + Y) / 2.0)

void checkError() {
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        std::cerr << gluErrorString(err) << std::endl;
    }
}

struct Point {
    GLfloat vs[0];
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLfloat dist(Point &other) {
        return sqrt(pow(other.x-x,2)+pow(other.y-y,2)+pow(other.z-z,2));
    }
};

struct Vector {
    GLfloat vs[0];
    GLfloat x;
    GLfloat y;
    GLfloat z;

    void normalize() {
        GLfloat length = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
        x /= length;
        y /= length;
        z /= length;
    }

    bool operator==(Vector &other) {
        // Assumes vectors are normalized
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
};

struct Line {
    Point p;
    Vector v;

    Line(Point p_, Vector v_) : p(p_), v(v_) {}
};

Vector operator-(Point &one, Point &two) {
    Vector r;
    r.x = one.x - two.x;
    r.y = one.y - two.y;
    r.z = one.z - two.z;
    r.normalize();

    return r;
}

Vector cross(Vector &one, Vector &two) {
    Vector r;
    r.x = one.y * two.z - one.z * two.y;
    r.y = one.z * two.x - one.x * two.z;
    r.z = one.x * two.y - one.y * two.x;
    r.normalize();

    return r;
}

Point intersect(Line &one, Line &two) {
    // TODO actually compute intersection
    Point pt;
    pt.x = 1.0;
    pt.y = 1.0;
    pt.z = 1.0;
    return pt;
}

class BezierCurve {
private:
    Point ctrlpts[4];
    GLfloat step_size;

public:
    BezierCurve() : step_size(0.01) {
        Point p;
        p.x = 0.0;
        p.y = 0.0;
        p.z = 0.0;

        setCtrlPoint(0, p);
        p.y = 1.0;
        setCtrlPoint(1, p);
        p.x = 1.0;
        setCtrlPoint(2, p);
        p.y = 0.0;
        setCtrlPoint(3, p);
    }

    void setCtrlPoint(int index, Point val) {
        ctrlpts[index] = val;
    }

    Point evaluate(GLfloat t) {
        assert(t >= 0.0 && t <= 1.0);

        Point seg1a = lerp(ctrlpts[0], ctrlpts[1], t);
        Point seg1b = lerp(ctrlpts[1], ctrlpts[2], t);
        Point seg1c = lerp(ctrlpts[2], ctrlpts[3], t);

        Point seg2a = lerp(seg1a, seg1b, t);
        Point seg2b = lerp(seg1b, seg1c, t);

        Point seg3a = lerp(seg2a, seg2b, t);

        return seg3a;
    }

    void draw() {
        glBegin(GL_LINE_STRIP);
        glColor3f(0.0, 0.0, 0.0);
        for(GLfloat t = 0.0; t < 1.0; t += step_size) {
            glVertex3fv(evaluate(t).vs);
        }
        glEnd();
    }

    GLfloat length() {
        Point last = evaluate(0.0);
        GLfloat l = 0.0;
        for(GLfloat t = step_size; t < 1.0; t += step_size) {
            Point cur = evaluate(t);
            l += cur.dist(last);
            last = cur;
        }

        return l;
    }

    GLfloat maxCurvature() {
        GLfloat max_curvature = 0.0;

        Point one = evaluate(0.0);
        Point two = evaluate(step_size);
        for(GLfloat t = 2*step_size; t < 1.0; t += step_size) {
            Point three = evaluate(t);
             
            GLfloat curvature = findCurvature(one, two, three);
            max_curvature = MAX(max_curvature, curvature);

            one = two;
            two = three;
        }
    }

private:
    
    Point lerp(Point &one, Point &two, GLfloat t) {
        assert(t >= 0.0 && t <= 1.0);

        Point pt;

        pt.x = lerp(one.x, two.x, t);
        pt.y = lerp(one.y, two.y, t);
        pt.z = lerp(one.z, two.z, t);

        return pt;
    }

    GLfloat lerp(GLfloat one, GLfloat two, GLfloat t) {
        assert(t >= 0.0 && t <= 1.0);
        return (t*one + (1.0-t)*two);
    }

    GLfloat findCurvature(Point &one, Point &two, Point &three) {
        Point mpt1 = lerp(one, two, 0.5);
        Vector a = two - mpt1;
        Vector b = three - mpt1;
        Vector vert = cross(a,b);
        Vector ortho1 = cross(vert, a);

        Point mpt2 = lerp(two, three, 0.5);
        a = two - mpt2;
        b = three - mpt2;
        vert = cross(a,b);
        Vector ortho2 = cross(vert, a);

        return 0.0;
    }

};

class Spline {
private:
    std::list<BezierCurve> list;
    Point last;

public:
    Spline() : list() { }

    void addPoint(Point &pt) {
        if(!list.empty()) {
            BezierCurve bc;
            bc.setCtrlPoint(0, last);
            bc.setCtrlPoint(1, last);
            bc.setCtrlPoint(2, pt);
            bc.setCtrlPoint(3, pt);

            list.push_back(bc);
        }

        last = pt;
    }

    GLfloat length() {
        GLfloat l = 0.0;
        for(std::list<BezierCurve>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            l += iter->length();
        }

        return l;
    }

    GLfloat maxCurvature() {
        GLfloat max_curvature = 0.0;
        for(std::list<BezierCurve>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            max_curvature = MAX(max_curvature, iter->maxCurvature());
        }

        return max_curvature;
    }

    void draw() {
        for(std::list<BezierCurve>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            iter->draw();
        }
    }
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
        GLfloat scale = 0.6 - (0.6*relative_height);
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
    int window_width;
    int window_height;
    GLfloat aspect_ratio;

public:
    Camera() :  pos(), look(), close(0.01), far(100),
                window_width(INITIAL_WINDOW_SIZE),
                window_height(INITIAL_WINDOW_SIZE),
                aspect_ratio((GLfloat)window_width / (GLfloat)window_height)
    {
        update();
    }

    void draw() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos.x, pos.y, pos.z,
                  look.x,look.y,look.z,
                  0,1,0);

        //glTranslatef(0, 0, -60000);
        checkError();
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

    void reshape(int new_width, int new_height) {
        window_width = new_width;
        window_height = new_height;
        update();
    }

private:
    void update() {
        glViewport(0,0, window_width, window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspect_ratio, close, far);
        checkError();
    }
};

Terrain terrain;
Camera camera;
BezierCurve curve;

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

void reshape(int width, int height) {
    camera.reshape(width, height);
}

void glInit(int *argc, char **argv) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(INITIAL_WINDOW_SIZE,INITIAL_WINDOW_SIZE);
    glutInitWindowPosition(40,40);
    glutCreateWindow("Terrain");
    glClearColor(0.795, 0.795, 0.795, 0.0);
    glutDisplayFunc(draw);
    glutKeyboardFunc(key);
    glutReshapeFunc(reshape);

    Point max = terrain.getMaxCoords();
    Point min = terrain.getMinCoords();

    // TODO find real values for this
    //attempting to adjust camera
    //camera.moveTo(AVG(max.x,min.x),AVG(max.y,min.y),min.z+1);
    //camera.lookAt(AVG(max.x,min.x),AVG(max.y,min.y),AVG(max.z,min.z));
    //camera.setClip(0.8*min.z, 1.2*max.z);
    //printf("%f %f %f\n", min.x, min.y, min.z);
    //printf("%f %f %f\n",AVG(max.x,min.x),AVG(max.y,min.y),AVG(max.z,min.z));
    //printf("%f %f %f\n",max.x, max.y, max.z);
}

void usage() {
    std::cout << "Usage ./tour terrain_data.tri" << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    if(argc < 2 || !terrain.init(argv[1])) usage();
    glInit(&argc, argv);

    camera.moveTo(0, 0, 60000.0);
    camera.lookAt(0.0, 0.0, 0.0);
    camera.setClip(1, 10000000);

    glutMainLoop();
    return 0;
}

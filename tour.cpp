#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <limits.h>
#include <assert.h>
#include <list>
#include <math.h>
#include <vector>

#define INITIAL_WINDOW_SIZE (800)

#define MAX(X,Y) (X > Y ? X : Y)
#define MIN(X,Y) (X < Y ? X : Y)
#define AVG(X,Y) ((X + Y) / 2.0)
#define NUM_GRADIENTS 8

using namespace std;

#define checkError() (errFunc(__FILE__,__LINE__))

void errFunc(const char *file, int line) {
    return;
    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        std::cerr << file << ":" << line << " "
                  << gluErrorString(err) << std::endl;
    }
}

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

struct Point {
    GLfloat vs[0];
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLfloat dist(Point &other) {
        return sqrt(pow(other.x-x,2)+pow(other.y-y,2)+pow(other.z-z,2));
    }

    Vector subtract(Point &other) {
        Vector vect;
        vect.x = x - other.z;
        vect.y = y - other.y;
        vect.z = z - other.z;
    }
};

struct Site {
    Point p;
    bool locked;
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

GLfloat lerp(GLfloat one, GLfloat two, GLfloat t) {
    assert(t >= 0.0 && t <= 1.0);
    return (t*one + (1.0-t)*two);
}

Point lerp(Point &one, Point &two, GLfloat t) {
    assert(t >= 0.0 && t <= 1.0);

    Point pt;

    pt.x = lerp(one.x, two.x, t);
    pt.y = lerp(one.y, two.y, t);
    pt.z = lerp(one.z, two.z, t);

    return pt;
}

class Parabola {
private:
    Point ctrlpts[3];
    GLfloat step_size;

public:
    Parabola() : step_size(0.01) {
        Point p;
        p.x = 0.0;
        p.y = 0.0;
        p.z = 0.0;

        setCtrlPoint(0, p);
        p.y = 0.5;
        p.x = 0.5;
        setCtrlPoint(1, p);
        p.x = 1.0;
        p.y = 0;
        setCtrlPoint(2, p);
    }

    void setCtrlPoint(int index, Point val) {
        ctrlpts[index] = val;
    }

    Point evaluate(GLfloat t) {
        assert(t >= 0.0 && t <= 1.0);

        Point seg1a = lerp(ctrlpts[0], ctrlpts[1], t);
        Point seg1b = lerp(ctrlpts[1], ctrlpts[2], t);

        Point seg2a = lerp(seg1a, seg1b, t);

        return seg2a;
    }

    void draw() {
        glBegin(GL_LINE_STRIP);
        glLineWidth(100.0);
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
    //std::list<Parabola> list;
    vector<Site> controlPts;
    vector<GLfloat> knots;
    Site last;

public:
    Spline() : controlPts() { }

    void addPoint(Site &s) {
         int size;
         controlPts.push_back(s);
         size = controlPts.size();
         if(controlPts.size() > 1) {
              knots.push_back(knots[size-2] + (controlPts[size-2].p.dist(controlPts[size-1].p)));
         }
         last = s;
    }

    void insertPoint( Site &s, int index) {
         controlPts.insert(controlPts.begin()+index, s);
    }

    void findControlPts(GLfloat ratio) {
          

    }

    /*void addPoint(Point &pt) {
        if(!list.empty()) {
            Parabola bc;
            bc.setCtrlPoint(0, last);
            bc.setCtrlPoint(1, lerp(last, pt, 0.5));
            bc.setCtrlPoint(2, pt);

            list.push_back(bc);
        }

        last = pt;
    }

    GLfloat length() {
        GLfloat l = 0.0;
        for(std::list<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            l += iter->length();
        }

        return l;
    }

    GLfloat maxCurvature() {
        GLfloat max_curvature = 0.0;
        for(std::list<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            max_curvature = MAX(max_curvature, iter->maxCurvature());
        }

        return max_curvature;
    }
    

    void draw() {
        for(std::list<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            iter->draw();
        }
    }
    */
};

class Tour {
private:
     Spline spline;
     vector<Site> sites;

public:
    Tour() {}

    bool init(char* file) {
        std::ifstream in;
        in.open(file);

        Site s;
        while(in.good()) {
            in >> s.p.x >> s.p.y >> s.p.z;
            spline.addPoint(s);
            sites.push_back(s);
        }
        return in.eof();
    }

    void draw() {
        //spline.draw();
        glBegin(GL_LINE_STRIP);
        glLineWidth(100.0);
        for(std::vector<Site>::iterator iter = sites.begin();
            iter != sites.end(); ++iter) {
                glVertex3fv(iter->p.vs);
        }
        glEnd();
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
    int n_sites;
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
            GLfloat elevation = (triangles[i].maxZ()+triangles[i].minZ())/2.;
            setElevationColor(elevation);
            glVertex3fv((GLfloat *)&triangles[i].v1.vs);
            glVertex3fv((GLfloat *)&triangles[i].v2.vs);
            glVertex3fv((GLfloat *)&triangles[i].v3.vs);
        }
        glEnd();
        glPopMatrix();
    }

    void setElevationColor(GLfloat elevation) {
        GLfloat gradient[8][4] = {-1.,0.,0.,.5,
                                  -.25,0.,0.,1.,
                                  0.0,0.,.5,1.,
                                  .0625,.94,.94,.25,
                                  .125,.125,.625,0.,
                                  .375,.875,.875,0.,
                                  .75, .5,.5,.5,
                                  1.,1.,1.,1.};
        GLfloat relative_height = (elevation - min_coords.z) / (max_coords.z - min_coords.z);
        GLfloat scale = (2.0*relative_height)-1.;
        
        int gradientIndex = 0;
        for(int i = 0; i < NUM_GRADIENTS; i++) {
            if(scale <= gradient[i][0]) {
                scale = (scale - min_coords.z/max_coords.z-min_coords.z);
                //scale = (scale - gradient[i][0])/(gradient[i][0]-gradient[i-1][0]);
                //scale = fabs(scale);
                gradientIndex = i;
                break;
            }
        }
        glColor3f(scale*.6, scale*.0, scale*.0);
        /*glColor3f( ((gradient[gradientIndex][1]+gradient[gradientIndex-1][1])/2.)*scale,
                   ((gradient[gradientIndex][2]+gradient[gradientIndex-1][2])/2.)*scale,
                   ((gradient[gradientIndex][3]+gradient[gradientIndex-1][3])/2.)*scale);
        */
       // glColor3f(0.4,0.6,0.4);

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
                  0,0,1);

        //glTranslatef(0, 0, -60000);
    }

    /*
    void move(Tour* tour) {
        if(tour->isEnd()) {
             tour->reset();
             pos = tour->getSite(tour->getCurrSite());
             look = tour->getSite(tour->getCurrSite() + 1);
        }
        int step = tour->getStepSize();
        if(fabs(look.x - pos.x) > step) {
            if(look.x - pos.x > 0) { 
                pos.x += step;
            } else {
                pos.x -= step;
            }
        } else {
            pos.x = look.x;
        }
        if(fabs(look.y - pos.y) > step) { 
            if(look.y - pos.y > 0) { 
                pos.y += step;
            } else {
                pos.y -= step;
            }
        } else {
            pos.y = look.y;
        }
        if(fabs(look.z - pos.z) > step) { 
             if(look.z - pos.z > 0) { 
                pos.z += step;
            } else {
                pos.z -= step;
            }
        } else {
            pos.z = look.z;
        }
        if(pos.x == look.x && pos.y == look.y && pos.z == look.z) {
            tour->nextSite(); 
            look = tour->getSite(tour->getCurrSite()+1);
        }       
    }
    */

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

    void changePos(GLfloat x, GLfloat y, GLfloat z) {
        pos.x += x;
        pos.y += y;
        pos.z += z;
    }

    void changeLook(GLfloat x, GLfloat y, GLfloat z) {
        look.x += x;
        look.y += y;
        look.z += z;
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
    }
};

Terrain terrain;
Tour tour;
Camera camera;

void DefineLight() {
    GLfloat light0_ambient[]  = {0.2, 0.2, 0.2, 1.0};
    GLfloat light0_position[] = {1., 1., 1.0, 0.0};
    GLfloat light0_diffuse[]  = {.8, .8, .8, 1.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient); 
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse); 

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
}

void DefineMaterial() {

    GLfloat shininess[]    = {1.0};
    GLfloat mat_ambient [] = {.2, 0.2, 0.2, 1.0};
    GLfloat mat_specular[] = {.1, .1, .1, 1.0};
    GLfloat mat_diffuse [] = {0.8, 0.8, 0.8, 1.0};

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}



void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.draw();
    DefineLight();
    DefineMaterial();
    terrain.draw();
    tour.draw();
    glutSwapBuffers();
}

void key(unsigned char k, int x, int y) {
    float scaleX = 100;
    float scaleY = 50;
    float scaleZ = 200;
    float stepChange = 1;
    switch(k) {
        case 'q':
            exit(0);
            break;
        case 'w':
            camera.changePos(0.,0.,-scaleZ);
            break;
        case 's':
            camera.changePos(0.,0.,1*scaleZ);
            break;
        case 'a':
            camera.changePos(-1*scaleX,0.,0.);
            break;
        case 'd':
            camera.changePos(scaleX,0.,0.);
            break;
        case 'u':
            camera.changePos(0.,scaleY,.0);
            break;
        case 'j':
            camera.changePos(0.,-1*scaleY,0.);
            break;
        case '+':
            //tour.modifyStep(stepChange);
            break;
        case '-':
            //tour.modifyStep(-1*stepChange);
            break;
        case '/':
            //camera.move(&tour);
            break;
        case 'r':
            camera.moveTo(0,100,60000);
            camera.lookAt(0,0,0);
            break;
        default:
            break;
    }
    glutPostRedisplay();
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
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    Point max = terrain.getMaxCoords();
    Point min = terrain.getMinCoords();

    //Point pos = tour.getSite(tour.getCurrSite());
    //Point look =  tour.getSite(tour.getCurrSite()+1);
    //camera.moveTo(pos.x,pos.y,pos.z);
    //camera.lookAt(look.x,look.y,look.z);

    camera.moveTo(0,100,60000);
    camera.lookAt(0,0,0);
    camera.setClip(0.01, 50000);
}

void usage() {
    std::cout << "Usage ./tour terrain_data.tri terrain_data.tour" << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    if(argc < 3 || !terrain.init(argv[1]) || !tour.init(argv[2])) usage();
    glInit(&argc, argv);
    glutMainLoop();
    return 0;
}

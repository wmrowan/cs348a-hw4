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
#define TIMERMSECS 50

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

struct Point {
    GLfloat vs[0];
    GLfloat x;
    GLfloat y;
    GLfloat z;

    Point() : x(0), y(0), z(0) {}
    Point(GLfloat x_, GLfloat y_, GLfloat z_) : x(x_), y(y_), z(z_) {}

    GLfloat dist(Point &other) {
        return sqrt(pow(other.x-x,2)+pow(other.y-y,2)+pow(other.z-z,2));
    }
};

struct Vector {
    GLfloat vs[0];
    GLfloat x;
    GLfloat y;
    GLfloat z;

    Vector() : x(0), y(0), z(0) {}
    Vector(GLfloat x_, GLfloat y_, GLfloat z_) : x(x_), y(y_), z(z_) {}
    Vector(Point p) : x(p.x), y(p.y), z(p.z) {}

    void normalize() {
        GLfloat length = norm();
        x /= length;
        y /= length;
        z /= length;
    }

    GLfloat norm() {
        return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    }
};

Vector operator-(Vector v) {
    Vector r;
    r.x = -v.x;
    r.y = -v.y;
    r.z = -v.z;
    return r;
}

Point operator+(Point p, Vector v) {
    Point r;
    r.x = p.x + v.x;
    r.y = p.y + v.y;
    r.z = p.z + v.z;
    return r;
}

Vector operator+(Vector one, Vector two) {
    Vector r;
    r.x = one.x + two.x;
    r.y = one.y + two.y;
    r.z = one.z + two.z;
    return r;
}

Vector operator-(Vector one, Vector two) {
    return one + (-two);
}

Vector operator-(Point one, Point two) {
    Vector v;
    v.x = one.x - two.x;
    v.y = one.y - two.y;
    v.z = one.z - two.z;
    return v;
}


Vector operator*(Vector v, GLfloat by) {
    Vector r;
    r.x = v.x * by;
    r.y = v.y * by;
    r.z = v.z * by;
    return r;
}

Vector operator*(GLfloat by, Vector v) {
    return v*by;
}

Vector cross(Vector one, Vector two) {
    Vector r;
    r.x = one.y * two.z - one.z * two.y;
    r.y = one.z * two.x - one.x * two.z;
    r.z = one.x * two.y - one.y * two.x;
    r.normalize();
    return r;
}


GLfloat dot(Vector one, Vector two) {
    GLfloat dot = (one.x*two.x) + (one.y*two.y) + (one.z*two.z);
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

struct Site {
    Point p;
    bool locked;
};

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

    bool sameSide (Point p1, Point p2, Point a, Point b) {
        Vector cp1 = cross(b-a, p1-a);
        Vector cp2 = cross(b-a, p2-a);
        if(dot(cp1, cp2) >= 0)
           return true;
        return false;
    }

    bool inside(Point p) {
        Point a,b,c;
        a = v1;
        b = v2;
        c = v3;
        p.z = 0;
        a.z = 0;
        b.z = 0;
        c.z = 0;
        if(sameSide(p,a, b,c) && sameSide(p,b, a,c) && sameSide(p,c, a,b)) 
            return true;
        return false;
    }

    Point findBarycentric(Point &p) {
        Vector weights, location;
        Point zero;
        GLfloat detA = (v1.x*v2.y)-(v1.x*v3.y)-(v2.x*v1.y)+(v2.x*v3.y)+(v3.x*v1.y)-(v3.x*v2.y);
        weights.x = ((p.x*v2.y)-(p.x*v3.y)-(v2.x*p.y)+(v2.x*v3.y)+(v3.x*p.y)-(v3.x*v2.y))/detA;
        weights.y = ((v1.x*p.y)-(v1.x*v3.y)-(p.x*v1.y)+(p.x*v3.y)+(v3.x*v1.y)-(v3.x*p.y))/detA;
        weights.z = ((v1.x*v2.y)-(v1.x*p.y)-(v2.x*v1.y)+(v2.x*p.y)+(p.x*v1.y)-(p.x*v2.y))/detA;
        location = (weights.x*v1)+(weights.y*v2)+(weights.z*v3); 
        return (zero+location);
    }
    
};

class Terrain {
private:
    int n_triangles;
    int n_sites;
    Point max_coords;
    Point min_coords;
    Triangle *triangles;
    GLuint texture;

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

    GLfloat height(Point &p) {
        for(int i = 0; i < n_triangles; i++) {
            if(triangles[i].inside(p)) {
                Point pos = triangles[i].findBarycentric(p);
                p.z - pos.z;
            }               
        } 
    }

    void draw() {

        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for(int i = 0; i < n_triangles; ++i) {
            GLfloat elevation = AVG(triangles[i].maxZ(), triangles[i].minZ());
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
        if(relative_height < colors[0][0]) {
            glColor3fv(colors[0]+1);
        } else if(relative_height < colors[1][0]) {
            setColor(1, relative_height);
        } else if(relative_height < colors[2][0]) {
            setColor(2, relative_height);
        } else if(relative_height < colors[3][0]) {
            setColor(3, relative_height);
        } else if(relative_height < colors[4][0]) {
            setColor(4, relative_height);
        } else {
            glColor3fv(colors[5]+1);
        }
    }

    void setColor(int i, GLfloat relative_height) {
        GLfloat s = (relative_height-colors[i-1][0]) / (colors[i][0] - colors[i-1][0]);
        glColor3f(lerp(colors[i][1],colors[i+1][1],s),
                  lerp(colors[i][2],colors[i+1][2],s),
                  lerp(colors[i][3],colors[i+1][3],s)
        );
    }

    Triangle* getTriangles(){
        return triangles;
    }

    int numTriangles() {
        return n_triangles;
    }
    Point getMaxCoords() {
        return max_coords;
    }
  
    Point getMinCoords() {
        return min_coords; 
    }

    static const GLfloat colors[6][4];

};

const GLfloat Terrain::colors[6][4] = {
    {0.005, 0.2, 0.2, 1.0},
    {0.05, 0.8, 0.5, 0.2},
    {0.14, 0.3, 0.6, 0.3},
    {0.4, 0.3, 0.8, 0.3},
    {0.6, 0.5, 0.7, 0.5},
    {1.0, 1.0, 1.0, 1.0},
};

Terrain terrain;


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
        for(GLfloat t = step_size; t < 1.0; t += step_size) {
            max_curvature = MAX(max_curvature, curvature(t));
        }

        return max_curvature;
    }

    GLfloat minDistance(Triangle *triangles, int n_triangles) {
        GLfloat minDist = INT_MAX;
        Point zero;
        int count = 0;
        for(GLfloat t = step_size; t < 1.0; t += step_size) {
            count = 0;
            Point cur = evaluate(t);
            for(int i = 0; i < n_triangles; i++) {
                if(triangles[i].inside(cur)) {
                    Point p = triangles[i].findBarycentric(cur);
                    minDist = MIN(minDist, cur.z - p.z);
                    i = n_triangles;
                }              
            } 
        }
        return minDist;
    }

private:

    GLfloat curvature(GLfloat t) {
        Vector fd = ddt(t);
        Vector sd = d2dt2(t);

        return cross(fd, sd).norm() / pow(fd.norm(), 3);
    }
    
    // First derivative of this parabola
    Vector ddt(GLfloat t) {
        return 2*(1-t)*(ctrlpts[1] - ctrlpts[0]) + 2*(ctrlpts[2] - ctrlpts[1]);
    }

    // Second derivative of this parabola
    Vector d2dt2(GLfloat t) {
        return 2*(ctrlpts[2] - ctrlpts[1]) - 2*(ctrlpts[1] - ctrlpts[0]);
    }

};

class Spline {
private:
    std::vector<Parabola> list;
    vector<Site> sites;
    vector<Point> controlPts;
    GLfloat step_size;

public:
    Spline() : sites(), controlPts(), step_size(1) { }

    void addSite(Site &s) {
        sites.push_back(s);
    }

    void insertJoint(Point p, int i) {
        Site s;
        s.p = p;
        s.locked = false;
        sites.insert(sites.begin()+i, s);
    }

    void genSpline() {
        optimize();
        findControlPts();
        generateParabolas();
    }

    #define SAFETY_FACTOR (50)
    void optimize() {
        for(int i = 0; i < sites.size()-1; i+=2) {
            Point cur = sites[i].p;
            Point next = sites[i+1].p;
            Point candidate = lerp(cur, next, 0.5);
            
            GLfloat height = terrain.height(candidate);
            if(height < 0) {
                candidate.z -= height;
                candidate.z += SAFETY_FACTOR;
            }

            insertJoint(candidate, i+1);
        }
    }

    GLfloat length() {
        GLfloat l = 0.0;
        for(std::vector<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            l += iter->length();
        }

        return l;
    }

    GLfloat maxCurvature() {
        GLfloat max_curvature = 0.0;
        for(std::vector<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            max_curvature = MAX(max_curvature, iter->maxCurvature());
        }

        return max_curvature;
    }
    
    GLfloat minDistance(Triangle* tri, int n_triangles) {
        GLfloat minDistance = 0.0;
        for(std::vector<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            minDistance = MIN(minDistance, iter->minDistance(tri, n_triangles));
        }
        return minDistance;
    }
 
    Point getSite(int index) {
         return (sites[index].p);
    }

    Point getPoint(int para, GLfloat t) {
         return list.at(para).evaluate(t);
    }

    int numParabolas() {
         return list.size();
    }

    void draw() {
        for(std::vector<Parabola>::iterator iter = list.begin();
                iter != list.end(); ++iter) {
            iter->draw();
        }
    }

private:

    void findControlPts() {
        controlPts.clear();

        Point planeCenter = lerp(sites[0].p, sites[1].p, .5);
        Point newPt;

        newPt = lerp(sites[0].p, sites[1].p, 0.8);
        controlPts.insert(controlPts.begin(), newPt);

        for(int i = 1; i < sites.size(); ++i) {
            Point currSite = sites[i].p;
            Vector prevTangent = controlPts[i-1] - currSite;
            Vector nextTangent = -prevTangent;

            Point nextCtrlPt = currSite + nextTangent;
            controlPts.push_back(nextCtrlPt);
        }
    }

    void generateParabolas() {
        for(int i = 0; i < sites.size()-1; i++) {
            Parabola bc;
            bc.setCtrlPoint(0, sites[i].p);
            bc.setCtrlPoint(1, controlPts[i]);
            bc.setCtrlPoint(2, sites[i+1].p);
            list.push_back(bc);
        }
    }
   
    
};

class Tour {
private:
     Spline spline;
     vector<Site> sites;
     int currP;
     GLfloat time;
     int currSite;
     bool touring;

public:
    Tour() : touring(false) {}

    bool init(char* file) {
        std::ifstream in;
        in.open(file);

        Site s;
        s.locked = true;
        while(in.good()) {
            in >> s.p.x >> s.p.y >> s.p.z;
            if(in.good()) {
               spline.addSite(s);
               sites.push_back(s);
            }
        }
        return in.eof();
    }

    void genTour() {
        spline.genSpline();
        printMetrics();
    }

    void draw() {
        spline.draw();
        /*glBegin(GL_LINE_STRIP);
        glLineWidth(100.0);
        for(std::vector<Site>::iterator iter = sites.begin();
            iter != sites.end(); ++iter) {
                glVertex3fv(iter->p.vs);
        }
        glEnd();
        */
    }

    GLfloat minDistance(Triangle* tri, int n_triangles){
        spline.minDistance(tri, n_triangles);
    }

    void printSites() {
        for(std::vector<Site>::iterator iter = sites.begin();
               iter != sites.end(); ++iter) {
            printf("%f %f %f\n", iter->p.x, iter->p.y, iter->p.z);
        }
    }

    void printMetrics() {
        cout << "Maximum curvature: " << spline.maxCurvature() << endl;;
        cout << "Length: " << spline.length() << endl;;
    }

    void step(GLfloat s){
        time += s;
    }

    Point getCurrPoint() {
        if(time >= 1) {
            time = 0;
            currSite++;
            currP++;
        }
        if(currP >= spline.numParabolas()){
            currP = 0;
            currSite++;
        }
        return spline.getPoint(currP, time);
    }

    void reset() {
        time = 0;
        currP = 0;
    }

    void startTour() {
        touring = true;
    }

    void stopTour() {
        touring = false;
    }

    Point nextSite() {
        return spline.getSite(currSite+1);
    }

    bool isTouring() {
        return touring;
    }

};

class Camera {
private:
    Point pos;
    GLfloat close;
    GLfloat far;
    int window_width;
    int window_height;
    GLfloat aspect_ratio;

    GLfloat viewMat[16];

public:
    Camera() :  pos(), close(10), far(100000),
                window_width(INITIAL_WINDOW_SIZE),
                window_height(INITIAL_WINDOW_SIZE),
                aspect_ratio((GLfloat)window_width / (GLfloat)window_height)
    {
        updateProj();
    }

    void draw() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMultMatrixf(viewMat);
    }
    
    void look(Point pos, Point look) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(pos.x,pos.y,pos.z,pos.x+1,pos.y,pos.z-1,0,0,1);
    }

    void moveTo(Point to) {
        move(to - pos);
    }

    void move(Vector diff) {
        pos = pos + diff;
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glMultMatrixf(viewMat);
            glTranslatef(-diff.x, -diff.y, -diff.z);
            glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);
        glPopMatrix();
    }
    
    void lookAt(Point at) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            gluLookAt(pos.x, pos.y, pos.z,
                      at.x,at.y,at.z,0,0,1);
            glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);
        glPopMatrix();
    }

    void rotate(GLfloat theta, GLfloat phi) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glMultMatrixf(viewMat);

            // Rotate look theta around the vertical axis through pos
            glRotatef(theta, 0, 0, 1.0);
            
            // Rotate look phi about the horizontal axis through pos and 
            glRotatef(phi, 1, 0, 0);

            glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);
        glPopMatrix();
    }

    void reshape(int new_width, int new_height) {
        window_width = new_width;
        window_height = new_height;
        updateProj();
    }

private:
    void setClip(GLfloat min, GLfloat max) {
        close = min;
        far = max;
        updateProj();
    }

    void updateProj() {
        glViewport(0,0, window_width, window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, aspect_ratio, close, far);
    }
};


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
    if(!tour.isTouring()) {
        camera.draw();
    } else {
        camera.look(tour.getCurrPoint(), tour.nextSite());
    }
    DefineLight();
    DefineMaterial();
    terrain.draw();
    tour.draw();
    glutSwapBuffers();
}

void key(unsigned char k, int x, int y) {
    float scaleX = 300;
    float scaleY = 300;
    float stepChange = 1;
    switch(k) {
        case 'q':
            exit(0);
            break;
        case 'w':
            camera.move(Vector(0.0, scaleY, 0.0));
            break;
        case 's':
            camera.move(Vector(0.0, -scaleY, 0.0));
            break;
        case 'a':
            camera.move(Vector(-scaleX, 0.0, 0.0));
            break;
        case 'd':
            camera.move(Vector(scaleX, 0.0, 0.0));
            break;
        case '+':
            break;
        case '-':
            break;
        case '/':
            break;
        case 'r':
            camera.moveTo(Point(0,-100,60000));
            camera.lookAt(Point(0,0,0));
            break;
        case 'l':
            tour.startTour();
            tour.step(.05);
            draw();
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void reshape(int width, int height) {
    camera.reshape(width, height);
}

struct {
    bool inRotateMode;
    int lastX;
    int lastY;
} mouseState;

void wheel(int button, int state, int x, int y) {
    if(button == 3 || button == 4) {
        GLfloat dir = button == 3 ? -500 : 500;
        camera.move(Vector(0,0, dir));
        glutPostRedisplay();
    } else if(button == GLUT_MIDDLE_BUTTON) {
        mouseState.inRotateMode = !state;
        mouseState.lastX = x;
        mouseState.lastY = y;
    }
}

#define RAD_PER_UNIT 0.1
void motion(int x, int y) {
    if(mouseState.inRotateMode) {
        int diff_x = x - mouseState.lastX;
        int diff_y = y - mouseState.lastY;

        camera.rotate(RAD_PER_UNIT * diff_x, RAD_PER_UNIT * -diff_y);

        mouseState.lastX = x;
        mouseState.lastY = y;

        glutPostRedisplay();
    }
}

void animate(int value) { 
    if(tour.isTouring()) {
        tour.step(.025);
        glutPostRedisplay();

    }
    glutTimerFunc(TIMERMSECS, animate, 0);
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
    glutMouseFunc(wheel);
    glutMotionFunc(motion);
    glutTimerFunc(TIMERMSECS, animate, 0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    Point max = terrain.getMaxCoords();
    Point min = terrain.getMinCoords();

    camera.moveTo(Point(0,-100,60000));
    camera.lookAt(Point(0,0,0));
    
}

void usage() {
    std::cout << "Usage ./tour terrain_data.tri terrain_data.tour" << std::endl;
    exit(1);
}


int main(int argc, char **argv) {
    if(argc < 3 || !terrain.init(argv[1]) || !tour.init(argv[2])) usage();
    cout << tour.minDistance(terrain.getTriangles(), terrain.numTriangles()) << endl;
    glInit(&argc, argv);
    tour.genTour();
    glutMainLoop();
    return 0;
}

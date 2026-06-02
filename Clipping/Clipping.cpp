#include "../graphics.h"
#include <cmath>
#include <climits>
#include <algorithm>
#include <vector>
#include <stack>
using namespace std;

struct point {
    double x;
    double y;
    point() : x(0), y(0) {}
    point(double xVal, double yVal) : x(xVal), y(yVal) {}
    static point mid(point p1, point p2) {
        point m;
        m.x = (p1.x + p2.x) / 2.0;
        m.y = (p1.y + p2.y) / 2.0;
        return m;
    }
};

union outCode {
    unsigned all : 4;
    struct { unsigned L:1, R:1, B:1, T:1; };
};

void swapOutCode(outCode& code1, outCode& code2) {
    unsigned temp = code1.all;
    code1.all = code2.all;
    code2.all = temp;
}

outCode getOutCode(double x, double y, double xleft, double xright, double ybottom, double ytop) {
    outCode code;
    code.all = 0;
    if (x < xleft)       code.L = 1;
    else if (x > xright) code.R = 1;
    if (y < ybottom)     code.B = 1;
    else if (y > ytop)   code.T = 1;
    return code;
}

void VerticalIntercept(double xEdge, double x1, double y1, double x2, double y2, double& xi, double& yi) {
    xi = xEdge;
    yi = y1 + (y2 - y1) * (xEdge - x1) / (x2 - x1);
}

void HorizontalIntercept(double yEdge, double x1, double y1, double x2, double y2, double& xi, double& yi) {
    yi = yEdge;
    xi = x1 + (x2 - x1) * (yEdge - y1) / (y2 - y1);
}
// -----------------------------------------------
// line clipping using Sutherland-Hodgman algorithm
// -----------------------------------------------

bool LineClipping_SutherlandHodgman(double& x1, double& y1, double& x2, double& y2,
              double xleft, double xright, double ybottom, double ytop) {

    outCode code1 = getOutCode(x1, y1, xleft, xright, ybottom, ytop);
    outCode code2 = getOutCode(x2, y2, xleft, xright, ybottom, ytop);

    while (true) {
        if ((code1.all & code2.all) != 0) return false; // trivially outside
        if (code1.all == 0 && code2.all == 0) return true; // trivially inside

        // Make sure code1 is the outside point
        if (code1.all == 0) {
            swap(x1, x2);
            swap(y1, y2);
            swapOutCode(code1, code2);
        }

        double xi, yi;
        if (code1.L) {
            VerticalIntercept(xleft, x1, y1, x2, y2, xi, yi);
        } else if (code1.R) {
            VerticalIntercept(xright, x1, y1, x2, y2, xi, yi);
        } else if (code1.B) {
            HorizontalIntercept(ybottom, x1, y1, x2, y2, xi, yi);
        } else {
            HorizontalIntercept(ytop, x1, y1, x2, y2, xi, yi);
        }

        x1 = xi;
        y1 = yi;
        code1 = getOutCode(x1, y1, xleft, xright, ybottom, ytop);
    }
}

// -----------------------------------------------
// polygon clipping using Sutherland-Hodgman algorithm
// -----------------------------------------------

/*
* four phase
clip left
clip right
clip top
clip bottom
return clipped polygon vertices
*/

// tests
bool InLeft(point p, double xleft) {
    return p.x >= xleft;
}
bool InRight(point p, double xright) {
    return p.x <= xright;
}
bool InBottom(point p, double ybottom) {
    return p.y >= ybottom;
}
bool InTop(point p, double ytop) {
    return p.y <= ytop;
}

// initialization of output polygon
void init(point p, point dummy, vector<point>& outPoly) {
    outPoly.clear();
    outPoly.push_back(p);
}

// we will reuse the vertical and horizontal intersection functions from before
point VerticalIntercept(point& p1, point& p2, double xEdge) {
    double xi, yi;
    VerticalIntercept(xEdge, p1.x, p1.y, p2.x, p2.y, xi, yi);
    return point(xi, yi);
}

point HorizontalIntercept(point& p1, point& p2, double yEdge) {
    double xi, yi;
    HorizontalIntercept(yEdge, p1.x, p1.y, p2.x, p2.y, xi, yi);
    return point(xi, yi);
}

typedef point (*intersectF)(point&, point&, double);
typedef bool (*inFunc)(point, double);
typedef vector<point> polygon;

polygon ClipEdge(polygon& p, double edge, inFunc inside, intersectF intersect) {
    polygon outPoly;
    int n = p.size();
    init(p[0], p[0], outPoly);
    point V1 = p[n - 1];
    bool V1_in = inside(V1, edge);
    for (int i = 0; i < n; i++) {
        point V2 = p[i]; // v2
        bool V2_in = inside(V2, edge);

        if (V1_in && V2_in) {
            outPoly.push_back(V2);
        } else if (!V1_in && V2_in) {
            outPoly.push_back(intersect(V1, V2, edge));
            outPoly.push_back(V2);
        } else if (V1_in && !V2_in) {
            outPoly.push_back(intersect(V1, V2, edge));
        }

        V1 = V2;
        V1_in = V2_in;
    }
}

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

void floodFill(HDC hdc, int x, int y, COLORREF bc, COLORREF fc) {
    if (GetPixel(hdc, x, y) != bc && GetPixel(hdc, x, y) != fc) {
        SetPixel(hdc, x, y, fc);
        floodFill(hdc, x + 1, y, bc, fc);
        floodFill(hdc, x - 1, y, bc, fc);
        floodFill(hdc, x, y + 1, bc, fc);
        floodFill(hdc, x, y - 1, bc, fc);
    }
}

void nonRecursiveFloodFill(HDC hdc, int x, int y, COLORREF bc, COLORREF fc) {
    stack<point> s;
    s.push(point(x, y));

    while (!s.empty()) {
        point p = s.top();
        s.pop();

        if (GetPixel(hdc, (int)p.x, (int)p.y) != bc && GetPixel(hdc, (int)p.x, (int)p.y) != fc) {
            SetPixel(hdc, (int)p.x, (int)p.y, fc);
            s.push(point(p.x + 1, p.y));
            s.push(point(p.x - 1, p.y));
            s.push(point(p.x, p.y + 1));
            s.push(point(p.x, p.y - 1));
        }
    }

}

// ------------------------------------------
// scanLineFill polygon filling algorithm
// ------------------------------------------

//--------------------- convex -------------
// data structures for storing the left and right intersection points for each scan line
struct Rec{
    double x_left, x_right;
    Rec() : x_left(INT_MIN), x_right(INT_MIN) {}
    Rec(double xl, double xr) : x_left(xl), x_right(xr) {}
};


// typedef for an array of 800 scan lines (assuming a maximum height of 800 pixels)
typedef Rec Table[800];

void init(point p1, point p2, Table t) {
    for (int i = 0 ; i < 800; i++) {
        t[i] = Rec(INT_MIN, INT_MIN);
    }
}


void Edge2table(point p1, point p2, Table t){
    if (p1.y == p2.y) return; // horizontal edge, ignore
    if (p1.y > p2.y) swap(p1, p2);

    int y = p1.y;
    double mi = (p2.x - p1.x) / (p2.y - p1.y); // slope inverse
    double x = p1.x;

    while (y < p2.y) {
        if (x < t[y].x_left) t[y].x_left = x;
        if (x > t[y].x_right) t[y].x_right = x;
        x += mi;
        y++;
    }
}

void polygon2Table(vector<point> polyPoints, int n, Table t){
    point v = polyPoints[n-1];
    for (int i = 0; i < n; i++) {
        Edge2table(v, polyPoints[i], t);
        v = polyPoints[i];
    }
}


void Table2Screen(HDC hdc, Table t, int height, COLORREF color){
    for (int i = 0; i < 800 ; i++){
        if (t[i].x_left < t[i].x_right) {
            for (int x = (int)ceil(t[i].x_left); x <= (int)floor(t[i].x_right); x++) {
                SetPixel(hdc, x, i, color);
            }
        }
    }
}

void convexPolygonFill(HDC hdc, vector<point> polyPoints, int n, int height, COLORREF color) {
    Table t;
    init(polyPoints[0], polyPoints[0], t);
    polygon2Table(polyPoints, n, t);
    Table2Screen(hdc, t, height, color);
}


// --------------------- non-convex -------------
// --------------------- non-convex -------------

struct Edge {
    double x;
    int yMax;
    double minv; // inverse slope

};

typedef vector<Edge> EdgeTable[800];

void initEdgeTable(EdgeTable t) {
    for (int i = 0; i < 800; i++)
        t[i].clear();
}

void Edge2tableNonConvex(point p1, point p2, EdgeTable t) {
    if (p1.y == p2.y) return;
    if (p1.y > p2.y) swap(p1, p2);

    Edge e;
    e.yMax = (int)p2.y;
    e.x    = p1.x;
    e.minv = (p2.x - p1.x) / (p2.y - p1.y);

    t[(int)p1.y].push_back(e);
}

void polygon2TableNonConvex(vector<point> polyPoints, int n, EdgeTable t) {
    point v = polyPoints[n - 1];
    for (int i = 0; i < n; i++) {
        Edge2tableNonConvex(v, polyPoints[i], t);
        v = polyPoints[i];
    }
}

void EdgeTable2Screen(HDC hdc, EdgeTable t, COLORREF color) {
    vector<Edge> AET;

    for (int y = 0; y < 800; y++) {
        // Add new edges starting at this scanline
        for (auto& e : t[y])
            AET.push_back(e);

        // Remove edges that have reached their yMax
        AET.erase(
            remove_if(AET.begin(), AET.end(),
                [y](const Edge& e) { return e.yMax <= y; }),
            AET.end()
        );

        // Sort by x
        sort(AET.begin(), AET.end(),
            [](const Edge& a, const Edge& b) { return a.x < b.x; });

        // Fill between pairs
        for (int i = 0; i + 1 < (int)AET.size(); i += 2) {
            int xStart = (int)ceil(AET[i].x);
            int xEnd   = (int)floor(AET[i + 1].x);
            for (int x = xStart; x <= xEnd; x++)
                SetPixel(hdc, x, y, color);
        }

        // Advance x for each active edge
        for (auto& e : AET)
            e.x += e.minv;
    }
}

void nonConvexPolygonFill(HDC hdc, vector<point> polyPoints, int n, COLORREF color) {
    EdgeTable t;
    initEdgeTable(t);
    polygon2TableNonConvex(polyPoints, n, t);
    EdgeTable2Screen(hdc, t, color);
}

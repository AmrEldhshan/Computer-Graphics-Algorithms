#include "../graphics.h"
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

// ─────────────────────────────────────────────────────────────────
//  UNIFIED GEOMETRY DATA STRUCTURE
// ─────────────────────────────────────────────────────────────────
struct point {
    double x;
    double y;

    point() : x(0), y(0) {}
    point(double xVal, double yVal) : x(xVal), y(yVal) {}

    // Static helper method to calculate an exact midpoint between two coordinates
    static point mid(point p1, point p2) {
        point m;
        m.x = (p1.x + p2.x) / 2.0;
        m.y = (p1.y + p2.y) / 2.0;
        return m;
    }
};

// ─────────────────────────────────────────────
//  HERMITE CURVE
//  t-vector : [1, t, t², t³]
//  input    : [P0, T0, P1, T1]
//  basis    :
//   | 1   0   0   0 |
//   | 0   1   0   0 |
//   |-3  -2   3  -1 |
//   | 2   1  -2   1 |
// ─────────────────────────────────────────────
void DrawHermiteCurve(HDC hdc,
                      point P0, point T0,
                      point P1, point T1,
                      int numpoints, COLORREF color)
{
    double hermitMatrix[4][4] = {
        { 1,  0,  0,  0},
        { 0,  1,  0,  0},
        {-3, -2,  3, -1},
        { 2,  1, -2,  1}
    };

    double Gx[4] = { P0.x, T0.x, P1.x, T1.x };
    double Gy[4] = { P0.y, T0.y, P1.y, T1.y };

    double Cx[4] = {0, 0, 0, 0};
    double Cy[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Cx[i] += hermitMatrix[i][j] * Gx[j];
            Cy[i] += hermitMatrix[i][j] * Gy[j];
        }
    }

    if (numpoints < 2) return;

    double dt = 1.0 / (numpoints - 1);

    for (int step = 0; step < numpoints; ++step) {
        double t = step * dt;
        double T[4] = { 1, t, t*t, t*t*t };

        double x = 0, y = 0;
        for (int i = 0; i < 4; i++) {
            x += T[i] * Cx[i];
            y += T[i] * Cy[i];
        }

        SetPixel(hdc, (int)round(x), (int)round(y), color);
    }
}

// ─────────────────────────────────────────────
//  BEZIER CURVE  (built on top of Hermite)
//  T0 = 3*(P1 - P0)
//  T1 = 3*(P3 - P2)
// ─────────────────────────────────────────────
void DrawBezierCurve(HDC hdc, point& P0, point& P1, point& P2, point& P3, int numpoints, COLORREF color)
{
    point T0(3 * (P1.x - P0.x), 3 * (P1.y - P0.y));
    point T1(3 * (P3.x - P2.x), 3 * (P3.y - P2.y));
    DrawHermiteCurve(hdc, P0, T0, P3, T1, numpoints, color);
}

// ─────────────────────────────────────────────
//  INDEPENDENT BEZIER CURVE (Recursive / Pascal)
// ─────────────────────────────────────────────
point DrawBezierCurveRecursiveCalc(const vector<point>& P, int i, int j, double t) {
    if (i == j) {
        return P[i];
    } else {
        point left = DrawBezierCurveRecursiveCalc(P, i + 1, j, t);
        point right = DrawBezierCurveRecursiveCalc(P, i, j - 1, t);
        return point(
            t * left.x + (1.0 - t) * right.x,
            t * left.y + (1.0 - t) * right.y
        );
    }
}

void DrawBezierCurveRecursive(HDC hdc, const vector<point>& P, int numpoints, COLORREF color) {
    int n = P.size();
    if (n < 1 || numpoints < 2) return;

    double dt = 1.0 / (numpoints - 1);

    for (int step = 0; step < numpoints; ++step) {
        double t = step * dt;
        point pt = DrawBezierCurveRecursiveCalc(P, 0, n - 1, t);
        SetPixel(hdc, (int)round(pt.x), (int)round(pt.y), color);
    }
}

// ─────────────────────────────────────────────────────────────────
//  MIDPOINT SUBDIVISION BEZIER ALGORITHM (De Casteljau)
// ─────────────────────────────────────────────────────────────────
bool isFlat(point p1, point p2, point p3, point p4) {
    double d1 = abs(p2.x - p1.x) + abs(p2.y - p1.y);
    double d2 = abs(p3.x - p4.x) + abs(p3.y - p4.y);
    return (d1 + d2) < 2.0;
}

void bezierMidPoint(HDC hdc, point p1, point p2, point p3, point p4, COLORREF color) {
    if (isFlat(p1, p2, p3, p4)) {
        SetPixel(hdc, (int)round(p1.x), (int)round(p1.y), color);
        SetPixel(hdc, (int)round(p4.x), (int)round(p4.y), color);
        return;
    }

    point q1 = point::mid(p1, p2);
    point q2 = point::mid(p2, p3);
    point q3 = point::mid(p3, p4);

    point r1 = point::mid(q1, q2);
    point r2 = point::mid(q2, q3);

    point mid = point::mid(r1, r2);

    bezierMidPoint(hdc, p1, q1, r1, mid, color);
    bezierMidPoint(hdc, mid, r2, q3, p4, color);
}

// ─────────────────────────────────────────────
//  CARDINAL SPLINE  (chain of Hermite segments)
//  c = tension  [0 .. 1]
//  Ti = (1-c) * (P[i+1] - P[i-1])
// ─────────────────────────────────────────────
void DrawCardinalSpline(HDC hdc, const vector<point>& P, double c, int numpix, COLORREF color)
{
    int n = P.size();
    if (n < 4) return;

    double c1 = 1.0 - c;
    vector<point> T(n);

    for (int i = 1; i < n - 1; i++) {
        T[i].x = c1 * (P[i+1].x - P[i-1].x);
        T[i].y = c1 * (P[i+1].y - P[i-1].y);
    }

    for (int i = 1; i < n - 2; i++) {
        DrawHermiteCurve(hdc, P[i], T[i], P[i+1], T[i+1], numpix, color);
    }
}

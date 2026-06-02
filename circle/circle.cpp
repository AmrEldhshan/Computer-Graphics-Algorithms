#include "../graphics.h"
#include <cmath>
#include <algorithm>
#include "Utility.cpp"
using namespace std;


void drawDirect_Cartesian(HDC HDC, int xc, int yc, int R,COLORREF color) {
    int x = 0;
    int y = R;

    while (x <= y) {
        set8Pixels(HDC, xc, yc, x, y, color);
        x++;
        y = round(sqrt(R*R - x*x));
    }

}

void directPolar(HDC hdc , int xc, int yc, int R , COLORREF color){
    int x = R;
    int y = 0;
    double thete = 0, dtheta = 1.0/R;

    while (x<y){
        set8Pixels(hdc, xc, yc, round(x), round(y), color);
        thete += dtheta;
        x = round(R*cos(thete));
        y = round(R*sin(thete));

    }
}


void IterativePolar(HDC hdc , int xc, int yc, int R , COLORREF color){
    int x = R;
    int y = 0;
    double thete = 0, dtheta = 1.0/R;
    double cosd = cos(dtheta);
    double sind = sin(dtheta);
    while (x<y){
        set8Pixels(hdc, xc, yc, round(x), round(y), color);
        double xnew = x*cosd - y*sind;
        y = round(x*sind + y*cosd);
        x = round(xnew);
    }
}


void firstOrderMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color){
    int x = 0, y = R;
    int d = 1-R;
    while (x<=y){
        set8Pixels(hdc, xc,yc, x,y,color);
        if (d<0){
            d += 2*x + 3;
        }else{
            d += 2*(x-y) +5;
            y--;
        }
        x++;
    }
}



void secondOrderMidPoint(HDC hdc, int xc, int yc, int R, COLORREF color){
    int x = 0, y = R;
    int d = 1-R;
    int c1 = 3, c2 = 5-2*R;

    while (x<=y){
        set8Pixels(hdc, xc,yc, x,y,color);
        if (d<0){
            d+= c1;
            c2 +=2;
        }else{
            d+=c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
    }
}

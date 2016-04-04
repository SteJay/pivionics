#include <iostream>
#include <cmath>
#include "circle.h"

Element* fn_create_circle(void) { return new Circle; }

Circle::Circle(void):Element() {
	sect=100;
}

void Circle::construct(void) {
	points.clear();
	for( unsigned int i=0; i<sect; ++i) {
		double a1 = ((PI*2)/sect)*i; // Calculate angle to draw on this iteration
		double a2 = ((PI*2)/sect)*(i+1); // Calculate angle to draw on this iteration
		int w,h; w=geometry[2]/2; h=geometry[3]/2;
		double r = sqrt(w*w+h*h);
		double x1=r*cos(a1);
		double x2=r*cos(a2);
		double x3=(r-thick)*cos(a1);
		double x4=(r-thick)*cos(a2);
		double y1=r*sin(a1);
		double y2=r*sin(a2);
		double y3=(r-thick)*sin(a1);
		double y4=(r-thick)*sin(a2);
		// The first triangle...
		points.push_back(round(x1));
		points.push_back(round(y1));
		points.push_back(round(x2));
		points.push_back(round(y2));
		points.push_back(round(x3));
		points.push_back(round(y3));
		// The second triangle!
		points.push_back(round(x2));
		points.push_back(round(y2));
		points.push_back(round(x3));
		points.push_back(round(y3));
		points.push_back(round(x4));
		points.push_back(round(y4));
	}
	int i=0;
	cout << "[";
	for(auto iter=points.begin();iter!=points.end();++iter) {
		i++;
		int p1=*iter;
		auto i2=next(iter);
		int p2=*i2;
		iter=i2;
		cout << "(" << p1 << "," << p2 << ")";
		cout << ",";
	}
	cout << "]" << endl;

}

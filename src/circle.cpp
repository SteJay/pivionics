#include <iostream>
#include <cmath>
#include "circle.h"

Element* fn_create_circle(void) { return new Circle; }

Circle::Circle(void):Element() {
	sect=100;
	attrs["drawmode"]="torus"; // torus,filled,outline,radius
}

void Circle::construct(void) {
	points.clear();
	for( unsigned int n=0; n<sect; ++n) {
		double a1 = (angles[1]/sect)*n; // Calculate angle to draw on this iteration
		double a2 = (angles[1]/sect)*(n+0.5); // Calculate angle to draw on this iteration
		int w,h; w=geometry[2]/2; h=geometry[3]/2;
		double r = sqrt(w*w+h*h);
		double x1=r*cos(a1);
		double x2=(r-thick)*cos(a2);
		double y1=r*sin(a1);
		double y2=(r-thick)*sin(a2);
		points.push_back(round(x1));
		points.push_back(round(y1));
		points.push_back(round(x2));
		points.push_back(round(y2));
	}
//	cout << "{";
//	cout << "\"mode\":\"" << attrs["_draw_as"] << "\",";
//	cout << "\"points\":";
//	cout << "[";
	int i=0;
	for(auto iter=points.begin();iter!=points.end();++iter) {
		i++;
		int p1=*iter;
		auto i2=next(iter);
		int p2=*i2;
		iter=i2;
//		if(i>1) cout << ",";
//		cout << "[" << p1 << "," << p2 << "]";
	}
//	cout << "]}" << endl;
}

#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

using namespace std;

#define RAD2DEG(x) (x * 180 / M_PI)

double distance(double x1, double y1, double x2, double y2);


bool find_circle_collision(
	double circle_x, double circle_y,
	double x1, double y1,
	double x2, double y2,
	double radius_sum,
	double& collision_x, double& collision_y,
	bool& at_begining
);

bool find_convex_collision(
	const vector<double>& xs, const vector<double>& ys,
	double x1, double y1,
	double x2, double y2,
	double radius,
	double& collision_x, double& collision_y,
	bool& at_begining
);

bool leaving_box(
	double box_x, double box_y,
	double box_w, double box_h,
	double x1, double y1,
	double x2, double y2,
	double &collision_x, double& collision_y,
	bool& at_begining
);

#endif

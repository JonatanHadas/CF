#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#define _USE_MATH_DEFINES
#include <math.h>

#define RAD2ANG(x) (x * 180 / M_PI)

double distance(double x1, double y1, double x2, double y2);


bool find_circle_collision(
	double circle_x, double circle_y,
	double x1, double y1,
	double x2, double y2,
	double radius_sum,
	double& collision_x, double& collision_y,
	bool& at_begining
);

bool find_line_collision(
	double line_x1, double line_y1,
	double line_x2, double line_y2,
	double width,
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

#include "geometry.h"

#include <functional>

using namespace std;


double distance(double x1, double y1, double x2, double y2){
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

inline double leftness(
	double x0, double y0,
	double x1, double y1,
	double x2, double y2
){
	x0 -= x1;
	x2 -= x1;
	y0 -= y1;
	y2 -= y1;
	
	return x2*y0 - y2*x0;
}

inline bool line_intersects(
	double line_x1, double line_y1,
	double line_x2, double line_y2,
	double x1, double y1,
	double x2, double y2
) {
	return leftness(x1, y1, line_x1, line_y1, line_x2, line_y2) * leftness(x2, y2, line_x1, line_y1, line_x2, line_y2) < 0;
}

inline bool intersects(
	double x1a, double y1a,
	double x2a, double y2a,
	double x1b, double y1b,
	double x2b, double y2b
){
	return line_intersects(x1a, y1a, x2a, y2a, x1b, y1b, x2b, y2b) && line_intersects(x1b, y1b, x2b, y2b, x1a, y1a, x2a, y2a);
}

inline void project(
	double& x0, double& y0,
	double x1, double y1,
	double x2, double y2
){
	double t = ((x0-x1)*(x2-x1) + (y0-y1)*(y2-y1))/((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
	x0 = x1 + (x2-x1)*t;
	y0 = y1 + (x2-x1)*t;
}



inline double lower_quadratic(double a, double b, double c){
	return -(b + sqrt(b*b - 4*a*c))/(2*a);
}

inline double quadratic_min(double a, double b, double c){
	return c - b/(2*a);
}


void rotate(
	double x, double y,
	double rotate_x, double rotate_y,
	double& result_x, double& result_y
){
	double size = sqrt(rotate_x*rotate_x + rotate_y*rotate_y);
	rotate_x /= size; rotate_y /= size;
	result_x = x * rotate_x - y * rotate_y;
	result_y = x * rotate_y + y * rotate_x;
}

#define EPSILON 1e-6


bool find_circle_collision(
	double circle_x, double circle_y,
	double x1, double y1,
	double x2, double y2,
	double radius_sum,
	double& collision_x, double& collision_y,
	bool& at_begining
){
	x1 -= circle_x; x2 -= circle_x;
	y1 -= circle_y; y2 -= circle_y;
		
	if(x1 * x1 + y1 * y1 <= radius_sum * radius_sum) {
		collision_x = x1 + circle_x;
		collision_y = y1 + circle_y;
		at_begining = true;
		return true;
	}
	
	double dx = x2 - x1;
	double dy = y2 - y1;
	
	double a = dx*dx + dy*dy;
	double b = 2*(x1*dx + y1*dy);
	double c = x1*x1 + y1*y1;
	
	if(x1*dx + y1*dy > 0) return false;  // Going away from circle.
	
	if(x2*dx + y2*dy < 0){
		if(x2*x2 + y2*y2 > radius_sum*radius_sum) return false;
	}
	else if(quadratic_min(a, b, c) > radius_sum*radius_sum) return false;
	
	double t = lower_quadratic(a, b, c - radius_sum*radius_sum) + EPSILON;
	collision_x = x1 + dx*t + circle_x;
	collision_y = y1 + dy*t + circle_y;
	return true;
}

inline int find_point_index(
	const vector<double>& xs, const vector<double>& ys,
	function<double(double, double)> side  // assuming side(xs[0], ys[0]) * side(xs.back(), ys.back()) < 0
) {
	int start = 0;
	int end = xs.size() - 1;
	
	double side0 = side(xs[0], ys[0]);
	while(end - start > 2){
		int middle = (start + end) / 2;
		
		if(side(xs[middle], ys[middle]) * side0 < 0) end = middle;
		else start = middle;
	}
	
	return end;
}

template<class T>
inline void insert(vector<T>& vec, int index, T value){
	vec.insert(vec.begin() + index, value);
}

bool find_convex_collision(
	const vector<double>& xs, const vector<double>& ys,
	double x1, double y1,
	double x2, double y2,
	double radius,
	double& collision_x, double& collision_y,
	bool& at_begining
){
	vector<double> line_xs({x1,x2}), line_ys({y1, y2});
	vector<double> moved_xs = line_xs, moved_ys = line_ys;
	
	for(int j = 0; j < moved_xs.size(); j++){
		double line_x = moved_xs[j], line_y = moved_ys[j];
		for(int i = 0; i < xs.size(); i++){
			int next_i = (i+1)%xs.size();
			int prev_i = (i + xs.size() - 1)%xs.size();
			
			double x = xs[i], y = ys[i];
			double next_x = xs[next_i], next_y = ys[next_i];
			double prev_x = xs[prev_i], prev_y = ys[prev_i];
			
			double next_vx = next_x - x, next_vy = next_y - y;
			double prev_vx = x - prev_x, prev_vy = y - prev_y;

			if(
				(line_x - x)*next_vx + (line_y - y)*next_vy <= 0 &&
				(line_x - x)*prev_vx + (line_y - y)*prev_vy >= 0
			) {
				moved_xs[j] = x;
				moved_ys[j] = y;
				break;
			}
			
			if(
				leftness(moved_xs[j], moved_ys[j], x, y, next_x, next_y) < 0 &&
				(line_x - x)*next_vx + (line_y - y)*next_vy >= 0 &&
				(line_x - next_x)*next_vx + (line_y - next_y)*next_vy <= 0
			){
				project(moved_xs[j], moved_ys[j], x, y, next_x, next_y);
				break;
			}
		}
	}
	
	if(distance(moved_xs[0], moved_ys[0], line_xs[0], line_ys[0]) < radius){
		collision_x = x1;
		collision_y = y1;
		at_begining = true;
		return true;
	}
	
	for(int i = 0; i < xs.size(); i++){
		int next_i = (i + 1)%xs.size();
		int prev_i = (i + xs.size() - 1)%xs.size();
		
		double x = xs[i], y = ys[i];
		double next_x = xs[next_i], next_y = ys[next_i];
		double prev_x = xs[prev_i], prev_y = ys[prev_i];
		
		double next_vx = next_x - x, next_vy = next_y - y;
		double prev_vx = x - prev_x, prev_vy = y - prev_y;

		if(intersects(x1, y1, x2, y2, x, y, next_x, next_y)){
			int index = find_point_index(line_xs, line_ys, [&](double test_x, double test_y) {
				return leftness(test_x, test_y, x, y, next_x, next_y);
			});
			
			double x_before = line_xs[index - 1];
			double y_before = line_ys[index - 1];
			double vx = line_xs[index] - x_before;
			double vy = line_ys[index] - y_before;
			
			double t = ((x - x_before)*next_vy - (y - y_before)*next_vx)/(vx*next_vy - vy*next_vx);
			double x_inter = x_before + vx*t;
			double y_inter = y_before + vy*t;
			
			insert(line_xs, index, x_inter);
			insert(line_ys, index, y_inter);
			insert(moved_xs, index, x_inter);
			insert(moved_ys, index, y_inter);
		}
		
		if(
			((x1 - x)*next_vx + (y1 - y)*next_vy) * ((x2 - x)*next_vx + (y2 - y)*next_vy) < 0
		){
			int index = find_point_index(line_xs, line_ys, [&](double test_x, double test_y) {
				return (test_x - x)*next_vx + (test_y - y)*next_vy;
			});
			double x_before = line_xs[index - 1];
			double y_before = line_ys[index - 1];
			double vx = line_xs[index] - x_before;
			double vy = line_ys[index] - y_before;
			
			double t = ((x - x_before)*next_vx + (y - y_before)*next_vy)/(vx*next_vx + vy*next_vy);
			double x_inter = x_before + vx*t;
			double y_inter = y_before + vy*t;
			
			if(leftness(x_inter, y_inter, x, y, next_x, next_y) < 0){
				insert(line_xs, index, x_inter);
				insert(line_ys, index, y_inter);
				insert(moved_xs, index, x);
				insert(moved_ys, index, y);
			}
		}

		if(
			((x1 - x)*prev_vx + (y1 - y)*prev_vy) * ((x2 - x)*prev_vx + (y2 - y)*prev_vy) < 0
		){
			int index = find_point_index(line_xs, line_ys, [&](double test_x, double test_y) {
				return (test_x - x)*prev_vx + (test_y - y)*prev_vy;
			});
			double x_before = line_xs[index - 1];
			double y_before = line_ys[index - 1];
			double vx = line_xs[index] - x_before;
			double vy = line_ys[index] - y_before;
			
			double t = ((x - x_before)*prev_vx + (y - y_before)*prev_vy)/(vx*prev_vx + vy*prev_vy);
			double x_inter = x_before + vx*t;
			double y_inter = y_before + vy*t;
			
			if(leftness(x_inter, y_inter, prev_x, prev_y, x, y) < 0){
				insert(line_xs, index, x_inter);
				insert(line_ys, index, y_inter);
				insert(moved_xs, index, x);
				insert(moved_ys, index, y);
			}
		}
	}
	
	for(int i = 1; i < line_xs.size(); i++){
		double dx = line_xs[i] - moved_xs[i], dy = line_ys[i] - moved_ys[i];
		double prev_dx = line_xs[i-1] - moved_xs[i-1], prev_dy = line_ys[i-1] - moved_ys[i-1];
		
		double vx = dx - prev_dx, vy = dy - prev_dy;
		
		// assuming prev_dx^2 + prev_dy^2 > radius^2
		
		if(prev_dx*vx + prev_dy*vy >= 0) return false;  // going away
		
		double c = prev_dx*prev_dx + prev_dy*prev_dy;
		double b = 2*(prev_dx*vx + prev_dy*vy);
		double a = vx*vx + vy*vy;
		if(dx*dx + dy*dy > radius*radius){
			if(dx*vx + dy*vy < 0) continue;  // not colliding yet
			
			if(quadratic_min(a, b, c) > radius*radius) return false;  // not colliging at minumum
		}
		// colliding
		double t = lower_quadratic(a, b, c - radius*radius) + EPSILON;
		collision_x = (1-t)*line_xs[i-1] + t*line_xs[i];
		collision_y = (1-t)*line_ys[i-1] + t*line_ys[i];
		return true;
	}
	return false;
}


bool leaving_box(
	double box_x, double box_y,
	double box_w, double box_h,
	double x1, double y1,
	double x2, double y2,
	double &collision_x, double& collision_y,
	bool& at_begining
){
	if(x1 < box_x || x1 > box_x + box_w || y1 < box_y || y1 > box_y + box_h){
		collision_x = x1;
		collision_y = y1;
		at_begining = true;
		return true;
	}
	
	bool collision = false;
	if(x2 < box_x){
		collision = true;
		y2 = y1 + (box_x - x1)*(y2 - y1)/(x2 - x1);
		x2 = box_x;
	} else if (x2 > box_x + box_w){
		collision = true;
		y2 = y1 + (box_x + box_w - x1)*(y2 - y1)/(x2 - x1);
		x2 = box_x + box_w;
	}

	if(y2 < box_y){
		collision = true;
		x2 = x1 + (box_y - y1)*(x2 - x1)/(y2 - y1);
		y2 = box_y;
	} else if (y2 > box_y + box_h){
		collision = true;
		x2 = x1 + (box_y + box_h - y1)*(x2 - x1)/(y2 - y1);
		y2 = box_y + box_h;
	}
	
	if(collision){
		collision_x = x2;
		collision_y = y2;
	}
	return collision;
}
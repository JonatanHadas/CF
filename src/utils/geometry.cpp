#include "geometry.h"

#include <map>

using namespace std;


double distance(double x1, double y1, double x2, double y2){
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
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

bool find_line_collision(
	double line_x1, double line_y1,
	double line_x2, double line_y2,
	double width,
	double x1, double y1,
	double x2, double y2,
	double radius,
	double& collision_x, double& collision_y,
	bool& at_begining
){
	line_x2 -= line_x1;
	x1 -= line_x1;
	x2 -= line_x1;
	line_y2 -= line_y1;
	y1 -= line_y1;
	y2 -= line_y1;
	
	double length = sqrt(line_x2*line_x2 + line_y2*line_y2);
	rotate(x1, y1, line_x2, -line_y2, x1, y1);
	rotate(x2, y2, line_x2, -line_y2, x2, y2);
	
	map<double, double> x_dist;
	map<double, double> y_dist;
		
	if(x1 <= 0) x_dist[0] = -x1;
	else if(x1 > length) x_dist[0] = x1 - length;
	else x_dist[0] = 0;
	
	if(y1 > width/2) y_dist[0] = y1 - width/2;
	else if(-y1 > width/2) y_dist[0] = -y1 - width/2;
	else y_dist[0] = 0;
	
	double end_x_dist, end_y_dist;
	if(x2 <= 0) end_x_dist = -x2;
	else if(x2 > length) end_x_dist = x2 - length;
	else end_x_dist = 0;
	
	if(y2 > width/2) end_y_dist = y2 - width/2;
	else if(-y2 > width/2) end_y_dist = -y2 - width/2;
	else end_y_dist = 0;
	
	if(x1*x2 < 0){  // Find intersection with x=0
		double t = -x1/(x2-x1);
		x_dist[t] = 0;
	}
	if((x1-length)*(x2-length) < 0) {  // Find intersection with x=length
		double t = (length-x1)/(x2-x1);
		x_dist[t] = 0;
	}

	if((y1-width/2)*(y2-width/2) < 0) {  // Find intersection with y=width/2
		double t = (width/2-y1)/(y2-y1);
		y_dist[t] = 0;
	}
	if((y1+width/2)*(y2+width/2) < 0) {  // Find intersection with y=-width/2
		double t = -(width/2+y1)/(y2-y1);
		y_dist[t] = 0;
	}
	
	auto next_x_it = x_dist.begin(), next_y_it = y_dist.begin();
	auto it_x = next_x_it++, it_y = next_y_it++;
		
	if(it_x->second * it_x->second + it_y->second * it_y->second < radius*radius){
		collision_x = x1;
		collision_y = y1;
		at_begining = true;
		goto transform_result;
	}
	
	{
		double x = it_x->second;
		double y = it_y->second;
		double next_x = next_x_it == x_dist.end() ? end_x_dist : next_x_it->second;
		double next_xt = next_x_it == x_dist.end() ? 1 : next_x_it->first;
		double next_y = next_y_it == y_dist.end() ? end_y_dist : next_y_it->second;
		double next_yt = next_y_it == y_dist.end() ? 1 : next_y_it->first;
		
		if(x * (next_x - x) / next_xt + y * (next_y - y) / next_yt > 0) return false;  // Going away
	
		bool done = false;
		while(!done){
			if(next_x_it == x_dist.end() && next_y_it == y_dist.end()) done = true;  // Last iteration;
			double next_x = next_x_it == x_dist.end() ? end_x_dist : next_x_it->second;
			double next_xt = next_x_it == x_dist.end() ? 1 : next_x_it->first;
			double next_y = next_y_it == y_dist.end() ? end_y_dist : next_y_it->second;
			double next_yt = next_y_it == y_dist.end() ? 1 : next_y_it->first;
			
			double dx = next_x - it_x->second;
			double dxt = next_xt - it_x->first;
			double dy = next_y - it_y->second;
			double dyt = next_yt - it_y->first;
			
			double t, x, y, end_t, end_x, end_y;
			if(it_x->first > it_y->first){
				t = it_x->first;
				x = it_x->second;
				y = it_y->second + (t-it_y->first)*dy/dyt;
			}
			else{
				t = it_y->first;
				y = it_y->second;
				x = it_x->second + (t-it_x->first)*dx/dxt;
			}
			if(next_x < next_y){
				end_t = next_xt;
				end_x = next_x;
				end_y = it_y->second + (end_t-it_y->first)*dy/dyt;
				if(!done) it_x = next_x_it++;
			}
			else{
				end_t = next_yt;
				end_y = next_y;
				end_x = it_x->second + (end_t-it_x->first)*dx/dxt;
				if(!done) it_y = next_y_it++;
			}
			
			double a = dx*dx/(dxt*dxt) + dy*dy/(dyt*dyt);
			double b = 2*(x*dx/dxt + y*dy/dyt);
			double c = x*x + y*y;
			if(end_x*end_x + end_y*end_y > radius*radius){
				if(end_x * dx/dxt + end_y*dy/dyt <= 0) continue;  // Still not close enough, and not at minimum yet.
				
				if(quadratic_min(a, b, c) > radius*radius) return false;  // Minimum not close enough.
			}
			
			double dt = lower_quadratic(a, b, c - radius*radius) + EPSILON;
			collision_x = x1 + (x2-x1)*(t+dt);
			collision_y = y1 + (y2-y1)*(t+dt);
			goto transform_result;
		}
		return false;  // Still to far away at the end.
	}
	
transform_result:
	rotate(collision_x, collision_y, line_x2, line_y2, collision_x, collision_y);
	collision_x += line_x1;
	collision_y += line_y1;
	return true;
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
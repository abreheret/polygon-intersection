#include "utils.hpp"

float distPoint(CvPoint2D32f v,CvPoint2D32f w) { 
	return sqrtf((v.x - w.x)*(v.x - w.x) + (v.y - w.y)*(v.y - w.y)) ;
}

float distPoint(CvPoint p1,CvPoint p2) { 
	int x = p1.x - p2.x;
	int y = p1.y - p2.y;
	return sqrt((float)x*x+y*y);
}

static CvPoint2D32f cvPoint32f( CvPoint p) {
	CvPoint2D32f s;
	s.x= (float)p.x;
	s.y= (float)p.y;
	return s;
}

bool segementIntersection(CvPoint p0,CvPoint p1,CvPoint p2,CvPoint p3,CvPoint * intersection) {
	CvPoint2D32f p;
	bool ok = segementIntersection(cvPoint32f(p0),cvPoint32f(p1),cvPoint32f(p2),cvPoint32f(p3),&p);
	if( ok ) 
		*intersection = cvPoint((int)p.x,(int)p.y);
	return ok;
}

bool segementIntersection(CvPoint2D32f p0,CvPoint2D32f p1,CvPoint2D32f p2,CvPoint2D32f p3,CvPoint2D32f * intersection) {
	CvPoint2D32f s1, s2;
	s1 = cvPoint2D32f(p1.x - p0.x, p1.y - p0.y);
	s2 = cvPoint2D32f(p3.x - p2.x, p3.y - p2.y);

	float s10_x = p1.x - p0.x;
	float s10_y = p1.y - p0.y;
	float s32_x = p3.x - p2.x;
	float s32_y = p3.y - p2.y;
	float denom = s10_x * s32_y - s32_x * s10_y;

	if(denom == 0) {
		return false;
	}

	bool denom_positive = denom > 0;

	float s02_x = p0.x - p2.x;
	float s02_y = p0.y - p2.y;
	float s_numer = s10_x * s02_y - s10_y * s02_x;

	if((s_numer < 0.f) == denom_positive) {
		return false;
	}

	float t_numer = s32_x * s02_y - s32_y * s02_x;
	if((t_numer < 0) == denom_positive) {
		return false;
	}

	if((s_numer > denom) == denom_positive || (t_numer > denom) == denom_positive) {
		return false;
	}

	float t = t_numer / denom;

	*intersection = cvPoint2D32f(p0.x + (t * s10_x), p0.y + (t * s10_y) );
	return true;
}

bool pointInPolygon(CvPoint2D32f p, CvPoint2D32f * points,int n) {
	int i, j ;
	bool c = false;
	for(i = 0, j = n - 1; i < n; j = i++) {
		if( ( (points[i].y >= p.y ) != (points[j].y >= p.y) ) &&
			(p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
			)
			c = !c;
	}
	return c;
}

bool pointInPolygon(CvPoint p, CvPoint * points,int n) {
	int i, j ;
	bool c = false;
	for(i = 0, j = n - 1; i < n; j = i++) {
		if( ( (points[i].y >= p.y ) != (points[j].y >= p.y) ) &&
			(p.x <= (points[j].x - points[i].x) * (p.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
			)
			c = !c;
	}
	return c;
}

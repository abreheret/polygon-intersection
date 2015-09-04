#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <cxcore.h>

float distPoint( CvPoint p1, CvPoint p2 ) ;
float distPoint(CvPoint2D32f p1,CvPoint2D32f p2) ;
bool segementIntersection(CvPoint p0_seg0,CvPoint p1_seg0,CvPoint p0_seg1,CvPoint p1_seg1,CvPoint * intersection) ;
bool segementIntersection(CvPoint2D32f p0_seg0,CvPoint2D32f p1_seg0,CvPoint2D32f p0_seg1,CvPoint2D32f p1_seg1,CvPoint2D32f * intersection) ;

bool pointInPolygon(CvPoint p, CvPoint * points,int n) ;
bool pointInPolygon(CvPoint2D32f p, CvPoint2D32f * points,int n) ;

#endif //
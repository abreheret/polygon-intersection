#include <highgui.h>
#include <cxcore.h>
#include <vector>
#include "utils.hpp"

struct PointAngle{
	CvPoint p;
	float angle;
};

int comp_point_with_angle(const void * a, const void * b) {
	if ( ((PointAngle*)a)->angle <  ((PointAngle*)b)->angle ) return -1;
	else if ( ((PointAngle*)a)->angle > ((PointAngle*)b)->angle ) return 1;
	else //if ( ((PointAngle*)a)->angle == ((PointAngle*)b)->angle ) return 0;
		return 0;
}

const char * title = "Convex Polygon Intersection";
class UIPolyIntersection {
	float _resolution;
	int _nb_grid;
	int _width ;
	int _height;
	std::vector<CvPoint> _poly[2];
	std::vector<CvPoint> _inter;
	std::vector<CvPoint *> _all;
	IplImage * curr_rgb;
	CvPoint pCurr;
	int key;
	int selected_point;
	int hover_point;
	static void mouseCallback(int event, int x, int y, int flags, void* param) {
		static_cast<UIPolyIntersection*>(param)->mouseEvent(event, x, y, flags,param);       
	}
public:

	UIPolyIntersection(int width) { 
		_nb_grid = 50;
		_width = width;
		_height = _width*9/16;
		hover_point = selected_point = -1;
		curr_rgb = cvCreateImage(cvSize(_width,_height),8,3);
		pCurr = cvPoint(0,0);
		_resolution = _width /(float)_nb_grid;
		initRandom();
		cvZero(curr_rgb);
		cvNamedWindow(title, CV_WINDOW_AUTOSIZE);
		cvSetMouseCallback(title, &UIPolyIntersection::mouseCallback , this );
	}

	~UIPolyIntersection() {
		cvReleaseImage(&curr_rgb);
	}

	void initRandom() {
		for (int j = 0 ; j < 2 ; j++ ) {
			int n = 5;
			_poly[j].reserve(n);
			_poly[j].clear();
			for (int i = 0 ; i < n ; i++ ) {
				CvPoint p = cvPoint(rand()%_width,rand()%_height);
				_poly[j].push_back(p);
				_all.push_back(&_poly[j][i]);
			}
		}
		updateIntersection();
	}

	void mouseEvent( int event, int x, int y, int flags, void* ) {
		pCurr = cvPoint(x,y);
		hover_point = -1;
		for(size_t i  = 0 ; i < _all.size() ; i++ ){
			if(distPoint(pCurr,*_all[i]) < 5) {
				hover_point = i ;break;
			}
		}
		if( event == CV_EVENT_LBUTTONDOWN && hover_point != -1 ) {
			selected_point = hover_point;
		} else if(event == CV_EVENT_MOUSEMOVE && selected_point != -1 ) {
			_all[selected_point]->x = pCurr.x;
			_all[selected_point]->y = pCurr.y;
		} else if(event == CV_EVENT_LBUTTONUP ) {
			if(selected_point != -1) {
				updateIntersection();
				selected_point  = -1;
			} else {

			}
		} 
	}
	
	void drawGrid() {
		for (int r = 0 ; r < _nb_grid ; r++ ) {
			int var = (int)(_resolution * r);
			cvDrawLine(curr_rgb,cvPoint(0,var),cvPoint(_width,var),CV_RGB(50,50,50));
			cvDrawLine(curr_rgb,cvPoint(var,0),cvPoint(var,_width),CV_RGB(50,50,50));
		}
	}

	void drawPoints() {
		char buff[128];
		CvFont font = cvFont(1,1);
		CvScalar c[3];
		c[0] = CV_RGB(255,0,0);
		c[1] = CV_RGB(84,255,0);
		c[2] = CV_RGB(0,169,255);

		for (int j = 0 ; j < 2 ; j++ ) {
			for (size_t i = 0 ; i < _poly[j].size() ; i++ ) {
				cvDrawLine(curr_rgb,_poly[j][i],_poly[j][(i+1)%_poly[j].size()],c[j],1,CV_AA);
				sprintf(buff,"%d",i); cvPutText(curr_rgb,buff,_poly[j][i],&font,c[j]);
			}
			float area = computeArea(&_poly[j][0],(int)_poly[j].size());
			CvPoint center = getCenter(&_poly[j][0],(int)_poly[j].size());
			sprintf(buff,"Area = %0.1f",area/(_resolution*_resolution)); cvPutText(curr_rgb,buff,center,&font,c[j]);
		}

		for (size_t i = 0 ; i < _all.size() ; i++ ) {
			cvDrawCircle(curr_rgb,*_all[i],5,CV_RGB(0,255,255),1,CV_AA);
		}

		for (size_t i = 0 ; i < _inter.size() ; i++ ) {
			cvDrawLine(curr_rgb,_inter[i],_inter[(i+1)%_inter.size()],c[2],2,CV_AA);
		}

		for (size_t i = 0 ; i < _inter.size() ; i++ ) {
			cvDrawCircle(curr_rgb,_inter[i],1,CV_RGB(250,250,250),2,CV_AA);
		}
		if( _inter.size() ) {
			float area = computeArea(&_inter[0],(int)_inter.size());
			CvPoint center = getCenter(&_inter[0],(int)_inter.size());
			sprintf(buff,"Area intersected = %0.1f",area/(_resolution*_resolution)); cvPutText(curr_rgb,buff,center,&font,c[2]);
		}
	}

	void paintImage() {
		cvZero(curr_rgb);
		drawGrid();
		cvDrawLine(curr_rgb,cvPoint(pCurr.x,0),cvPoint(pCurr.x,curr_rgb->height),CV_RGB(255,0,255));
		cvDrawLine(curr_rgb,cvPoint(0,pCurr.y),cvPoint(curr_rgb->width,pCurr.y),CV_RGB(255,0,255));
		drawPoints();

		if( hover_point != -1) {
			cvDrawCircle(curr_rgb,*_all[hover_point],10,CV_RGB(255,255,0),1,CV_AA,0);
		}

		if( selected_point != -1) {
			cvDrawCircle(curr_rgb,*_all[selected_point],10,CV_RGB(255,255,0),1,CV_AA,0);
		}
	}

	CvPoint getCenter(CvPoint * points,int n) {
		CvPoint center;
		center.x = 0;
		center.y = 0;
		for (int i = 0 ; i < n ; i++ ) {
			center.x += points[i].x;
			center.y += points[i].y;
		}
		center.x /= n;
		center.y /= n;
		return center;
	}

	float computeArea(const CvPoint * points,int n) {
		float area0 = 0.;
		for (int i = 0 ; i < n ; i++ ) {
			int j = (i+1)%n;
			area0 += points[i].x * points[j].y;
			area0 -= points[i].y * points[j].x;
		}
		return 0.5f * abs(area0);
	}

	void pointsOrdered(CvPoint * points,int n) {
		CvPoint center = getCenter(points,n);
		PointAngle pc[400];
		for (int i = 0 ; i < n ; i++ ) {
			pc[i].p.x = points[i].x;
			pc[i].p.y = points[i].y;
			pc[i].angle = atan2f((float)(points[i].y - center.y),(float)(points[i].x - center.x));
		}
		qsort(pc,n,sizeof(PointAngle),comp_point_with_angle);
		for (int i = 0 ; i < n ; i++ ) {
			points[i].x = pc[i].p.x;
			points[i].y = pc[i].p.y;
		}
	}

	void updateIntersection() {
		for (int j = 0 ; j < 2 ; j++ ) {
			pointsOrdered(&_poly[j][0],(int)_poly[j].size());
		}

		_inter.clear();
		for (size_t i = 0 ; i < _poly[0].size() ;i++) {
			if( pointInPolygon(_poly[0][i],&_poly[1][0],(int)_poly[1].size()) ) 
				_inter.push_back(_poly[0][i]);
		}

		for (size_t i = 0 ; i < _poly[1].size() ;i++) { 
			if( pointInPolygon(_poly[1][i],&_poly[0][0],(int)_poly[0].size()) ) 
				_inter.push_back(_poly[1][i]);
		}

		for (size_t i = 0 ; i < _poly[0].size() ;i++) {
			CvPoint p0,p1,p2,p3;
			p0 = _poly[0][i];
			p1 = _poly[0][(i+1)%_poly[0].size()];
			for (size_t j = 0 ; j < _poly[1].size() ;j++) {
				p2 = _poly[1][j];
				p3 = _poly[1][(j+1)%_poly[1].size()];
				CvPoint inter;
				if(segementIntersection(p0,p1,p2,p3,&inter)) {
					_inter.push_back(inter);
				}
			}
		}
		if(_inter.size())pointsOrdered(&_inter[0],(int)_inter.size());
	}

	void run() {
		while(true) {
			try {
				paintImage();
			} catch (std::exception &e) {
				printf("Err : %s\n",e.what());
			}
			cvShowImage(title,curr_rgb);
			switch( key = cvWaitKey(30)) {
			case 'r' : initRandom();break;
			case 27  : return ;
			}
		}
	}
};

int main(int argc,char **argv) {
	try {
		UIPolyIntersection ui(1280);
		ui.run();
	} catch (std::exception &e) {
		printf("Err : %s\n",e.what());
	}
	return 1;
}
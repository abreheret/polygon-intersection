#include <highgui.h>
#include <cxcore.h>
#include <vector>
#include "utils.hpp"

const char * title = "Convex Polygon Intersection";
class UIPolyIntersection {
	float _resolution;
	int _nb_grid;
	int _width ;
	int _height;
	Polygon _poly[2];
	Polygon _inter;
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
		_nb_grid = 30;
		_width = width;
		_height = _width*9/16;
		hover_point = selected_point = -1;
		curr_rgb = cvCreateImage(cvSize(_width,_height),8,3);
		pCurr = cvPoint(0,0);
		_resolution = _width /(float)_nb_grid;
		//initRandom();
		init();
		cvZero(curr_rgb);
		cvNamedWindow(title, CV_WINDOW_AUTOSIZE);
		cvSetMouseCallback(title, &UIPolyIntersection::mouseCallback , this );
	}

	~UIPolyIntersection() {
		cvReleaseImage(&curr_rgb);
	}

	void init() {
		_poly[0].clear();
		_poly[0].add(cvPoint(int(2* 3*_resolution),int(2*3*_resolution)));
		_poly[0].add(cvPoint(int(2*11*_resolution),int(2*3*_resolution)));
		_poly[0].add(cvPoint(int(2*11*_resolution),int(2*6*_resolution)));
		_poly[0].add(cvPoint(int(2* 1*_resolution),int(2*6*_resolution)));
		_poly[0].add(cvPoint(int(2* 1*_resolution),int(2*4*_resolution)));
							 
		_poly[1].clear();	 
		_poly[1].add(cvPoint(int(2*2*_resolution),int(2*1*_resolution)));
		_poly[1].add(cvPoint(int(2*4*_resolution),int(2*1*_resolution)));
		_poly[1].add(cvPoint(int(2*4*_resolution),int(2*5*_resolution)));
		_poly[1].add(cvPoint(int(2*2*_resolution),int(2*5*_resolution)));

		for (int j = 0 ; j < 2 ; j++ ) {
			for (int i = 0 ; i < _poly[j].size() ; i++ ) {
				_all.push_back(&_poly[j][i]);
			}
		}
		updateIntersection();
	}

	void initRandom() {
		for (int j = 0 ; j < 2 ; j++ ) {
			int n = 5;
			_poly[j].clear();
			for (int i = 0 ; i < n ; i++ ) {
				CvPoint p = cvPoint(rand()%_width,rand()%_height);
				_poly[j].push_back(p);
				_all.push_back(&_poly[j][i]);
			}
			_poly[j].pointsOrdered();
		}
		updateIntersection();
	}

	void updateIntersection() {
		//intersectPolygon(_poly[0],_poly[1],_inter);
		intersectPolygonSHPC(_poly[0],_poly[1],_inter);
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
			for (int i = 0 ; i < _poly[j].size() ; i++ ) {
				cvDrawLine(curr_rgb,_poly[j][i],_poly[j][(i+1)%_poly[j].size()],c[j],1,CV_AA);
				sprintf(buff,"%d",i); cvPutText(curr_rgb,buff,_poly[j][i],&font,c[j]);
			}
			float area = _poly[j].area();
			CvPoint center = _poly[j].getCenter();
			sprintf(buff,"Area = %0.1f",area/(_resolution*_resolution)); cvPutText(curr_rgb,buff,center,&font,c[j]);
		}

		for (size_t i = 0 ; i < _all.size() ; i++ ) {
			cvDrawCircle(curr_rgb,*_all[i],5,CV_RGB(0,255,255),1,CV_AA);
		}

		for (int i = 0 ; i < _inter.size() ; i++ ) {
			cvDrawLine(curr_rgb,_inter[i],_inter[(i+1)%_inter.size()],c[2],2,CV_AA);
		}

		for (int i = 0 ; i < _inter.size() ; i++ ) {
			cvDrawCircle(curr_rgb,_inter[i],1,CV_RGB(250,250,250),2,CV_AA);
		}
		if( _inter.size() ) {
			float area = _inter.area();
			CvPoint center = _inter.getCenter();
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

	float computeArea(const CvPoint * points,int n) {
		float area0 = 0.;
		for (int i = 0 ; i < n ; i++ ) {
			int j = (i+1)%n;
			area0 += points[i].x * points[j].y;
			area0 -= points[i].y * points[j].x;
		}
		return 0.5f * abs(area0);
	}

	void savePolygons() {
		FILE * file = fopen("log.txt","w");
		for (int i = 0 ; i < 2 ; i++) {
			fprintf(file,"%d\n",_poly[i].size());
			for (int j = 0 ; j < _poly[i].size() ; j++) {
				fprintf(file,"%d %d\n",_poly[i][j].x,_poly[i][j].y);
			}
		}
		fclose(file);
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
			case 's' : savePolygons();break;
			case 27  : return ;
			}
		}
	}
};

int main(int argc,char **argv) {
	try {
		UIPolyIntersection ui(1200);
		ui.run();
	} catch (std::exception &e) {
		printf("Err : %s\n",e.what());
	}
	return 1;
}
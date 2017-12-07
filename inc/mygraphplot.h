#ifndef	GRAPH_H
#define GRAPH_H

#include "qcustomplot.h"
#include "eventlabel.h"
//#include <QPainter>
//#include <QPaintEvent>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class MyGraphPlot: public QCustomPlot
{
	Q_OBJECT



public:
	MyGraphPlot(QFrame *frame_9);
	~MyGraphPlot(){}

	void getDataFilename(QString );
	void getImageSize(int, int);
	//void configMyGraphPlot();
	void clearData();
	void realtimeData(int msecs, int data);
	void clearGraph();
	void setupRealtimeDataDemo();
	void get8BitsFlag(bool);

	
signals:
	void sendPointData(int, int, int);

public slots:
     void updateLine(int lineType, int mouseX1Pos, int mouseY1Pos, int mouseX2Pos, int mouseY2Pos);
	 void getPointPos(int, int );
	
private:

	int row;
	int col;
	cv::Mat data;
	bool pngFlag;
	int point_data;
	bool bits8Flag;
	bool bitsChangeFlag;
	
};
 
#endif
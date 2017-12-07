#ifndef EVENTLABEL_H
#define EVENTLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

class EventLabel : public QLabel          
{
	Q_OBJECT   

public:
	EventLabel(QFrame * m_showFrame);
	~EventLabel(){}

	void setHorizontalLineFlag(bool flag);
	void setVerticalLineFlag(bool flag);
	void setHorizontalLine();
	void setVerticalLine();
	void setInputDataLine(int, int, int, int);
	//void setAutoLine();
	void unableSetLine();
	void getLabelSize(int width,int height);
	void getImageSize(int imageWidth, int imageHeight);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void setResolutionType(int);
	void setOpenCameraFlag(bool);
	void setRepaintFlagFalse();
	void setPointChoosed(int, int, int, int);
signals:
	void drawLine(int lineType, int x1, int y1, int x2, int y2);
	void sendPointPos(int, int);
	void sendPointPos(int x_orig, int y_orig,int y_orig_fake, int x_move, int y_move);
private:
	void mouseDoubleClickEvent(QMouseEvent *event);
	int setLine;
	int x;
	int y;
	int x1;
	int y1;
	int x2;
	int y2;
	
	void paintEvent(QPaintEvent *event);    
	bool mouseDoubleClickFlag;
	bool mouseReleaseFlag;
	bool setInputDataLineFlag;
	bool mouseClickEventFlag;
	bool openImage;
	bool mousePressFlag;
	int labelWidth;
	int labelHeight;
	double multiple;
	bool openCameraFlag;
	int x_move;
	int y_move;
	int x_orig;
	int y_orig;
	int y_orig_actual;
	int y_orig_fake;
	int x_move_click;
	int y_move_click;
	int resolutionType;
	bool repaintFlag;
};

#endif
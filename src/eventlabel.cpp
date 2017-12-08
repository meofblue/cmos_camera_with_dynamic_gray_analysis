#include "eventlabel.h"
#include <QToolTip>

EventLabel::EventLabel(QFrame * m_showFrame)
	:QLabel(m_showFrame),
	setLine(0),
	mouseDoubleClickFlag(false),
	mouseReleaseFlag(false),
	setInputDataLineFlag(false),
	mousePressFlag(false),
	mouseClickEventFlag(false),
	openImage(false),
	openCameraFlag(false),
	repaintFlag(false)
{
	multiple = 1;

}

void EventLabel::setHorizontalLine()
{
	if (openImage)	
	{ 
		setLine = 1;
	}
}

void EventLabel::setVerticalLine()
{
	if (openImage)
	{
		setLine = 2;
	}
}

//void EventLabel::setAutoLine()
//{
//	setLine = 4;
//}

void EventLabel::unableSetLine()
{
	setLine = 0;
	mouseDoubleClickFlag = false;
	mouseReleaseFlag = false;
	mousePressFlag=false;
	setInputDataLineFlag = false;
	openImage = false;
//	setMouseTracking(false);
}


void EventLabel::setInputDataLine(int beginPoint_x, int beginPoint_y, int endPoint_x, int endPoint_y)
{
	if (openImage)
	{
		setLine = 3;
		x1 = beginPoint_x ;//如果你写int x1 = beginPoint_x.toInt(); 那么你就又重新声明了一个x1
		y1 = beginPoint_y;
		x2 = endPoint_x;
		y2 = endPoint_y;
		setInputDataLineFlag = true;
		update();
	}
}

void EventLabel::mouseMoveEvent(QMouseEvent *event)
{
	
	if (openImage)
	{	
		x_move = event->pos().x();
		y_move = event->pos().y();
    	 x_orig = x_move*multiple;
		y_orig = y_move*multiple;		
		QString x_move_string = QString::number(x_orig);
		QString y_move_string = QString::number(y_orig);
		QToolTip::showText(event->globalPos(), x_move_string + "," + y_move_string);
	}
	else if (openCameraFlag)
	{
		x_move = event->pos().x();
		y_move = event->pos().y();
		QString x_move_string;
		QString y_move_string;
		switch (resolutionType)
		{
		case 6:
			x_orig = x_move * 2;
			y_orig_fake = (y_move + 1) * 2 - 1;
			y_orig_actual = y_move * 1.98765432 + 0.5 + 10;
			x_move_string = QString::number(x_orig);
			y_move_string = QString::number(y_orig_fake);
			break;
		case 5:
			x_orig = x_move * 1.6;
			y_orig = y_move * 1.6;
			x_move_string = QString::number(x_orig);
			 y_move_string = QString::number(y_orig);
			break;
		case 4:
			x_orig = x_move * 1.5;
			y_orig = y_move * 1.5;
			 x_move_string = QString::number(x_orig);
			 y_move_string = QString::number(y_orig);
			break;
		case 3:
			x_orig = x_move ;
			y_orig = y_move ;
			 x_move_string = QString::number(x_orig);
			 y_move_string = QString::number(y_orig);
			break;
		case 2:
			x_orig = x_move;
			y_orig = y_move;
			 x_move_string = QString::number(x_orig);
			 y_move_string = QString::number(y_orig);
			break;
		}		
		QToolTip::showText(event->globalPos(), x_move_string + "," + y_move_string);
	}	
	//	QLabel::mouseMoveEvent(event);
}

/*void EventLabel::mouseClickEvent(QMouseEvent *event)
{
	if (openImage)
	{
		if (setLine == 0)
		{
			if (event->button() == Qt::LeftButton)
			{

				 x = event->pos().x();
				 y= event->pos().y();

				mouseClickEventFlag = true;
				update();
			}
		}
	}
	QLabel::mouseClickEvent(event);
}
*/
void EventLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (setLine == 1 || 2)
	{
		if (event->button() == Qt::LeftButton)
		{

			x = event->pos().x();
			y = event->pos().y();
			mouseDoubleClickFlag = true;
			update();
		}
	}
	//QLabel::mouseDoubleClickEvent(event);
}

void EventLabel::mousePressEvent(QMouseEvent *event)
{
	
	if (openImage)
	{ 
		if (event->button() == Qt::RightButton)
		{
			setLine = 4;
			x1 = event->pos().x();
			y1 = event->pos().y();
			mousePressFlag = true;
		}
		else
		{
			x = event->pos().x();
			y = event->pos().y();
			mouseClickEventFlag = true;
			update();
		}
	}
	else if (openCameraFlag)
	{
		if (event->button() == Qt::LeftButton)
		{
			x_move_click = x_move;
			y_move_click = y_move;
			mouseClickEventFlag = true;
			update();
		}
	}
	//QLabel::mousePressEvent(event);
} 

void EventLabel::mouseReleaseEvent(QMouseEvent *event)
{
	if (openImage)
	{ 
		if (event->button() == Qt::RightButton)
		{
			x2 = event->pos().x();
			y2 = event->pos().y();
			mouseReleaseFlag = true;
			update();
		}
	}
	QLabel::mouseReleaseEvent(event);
}


void EventLabel::paintEvent(QPaintEvent *event)
{
	QLabel::paintEvent(event);
	if (mouseDoubleClickFlag)
	{ 
		QLabel::paintEvent(event);
		QPainter painter(this);
		if (setLine == 1)//设置横向分析线
		{
			painter.drawLine(0, y, labelWidth, y);
			//QLabel::paintEvent(event);
			emit drawLine(1, 0, y*multiple, labelWidth*multiple, y*multiple);
		}
		if (setLine == 2)
		{
			painter.drawLine(x, 0, x, labelHeight);
			emit drawLine(2, x*multiple, 0, x*multiple, labelHeight*multiple);
		}
	   mouseDoubleClickFlag = false;		
	}
	else if (mousePressFlag&&mouseReleaseFlag)
	{
		QLabel::paintEvent(event);
		QPainter painter(this);
		painter.drawLine(x1, y1, x2, y2);
	//	
		emit drawLine(4, x1*multiple, y1*multiple, x2*multiple, y2*multiple);
		mouseReleaseFlag = false;
		mousePressFlag = false;
	}
	else if (setInputDataLineFlag)
	{
		QLabel::paintEvent(event);
		QPainter painter(this);
		painter.drawLine(x1 / multiple, y1 / multiple, x2 / multiple, y2 / multiple);
		//	
		emit drawLine(3, x1, y1, x2, y2);
		setInputDataLineFlag = false;
	}
	else if (mouseClickEventFlag)
	{
		if (openImage)
		{
			QLabel::paintEvent(event);
			QPainter painter(this);
			painter.setPen(QPen(Qt::black, 5));
			painter.drawPoint(x, y);
			mouseClickEventFlag = false;
			emit sendPointPos(x*multiple, y*multiple);
		}
		else if (openCameraFlag)
		{
			//	QLabel::paintEvent(event);
			if (resolutionType == 6)
			{
				emit sendPointPos(x_orig, y_orig_actual, y_orig_fake, x_move_click, y_move_click);
			}
			else	
		     	emit sendPointPos(x_orig, y_orig,0, x_move_click, y_move_click);//发送点击时刻的位置数据,只发送一次
			QPainter painter(this);
			painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
			int labelWidth = width();
			int labelHeight = height();
			QLineF line3(0, labelHeight / 2 - 1, labelWidth - 1, labelHeight / 2 - 1);
			painter.drawLine(line3);
			QLineF line4(labelWidth / 2 - 1, 0, labelWidth / 2 - 1, labelHeight - 1);
			painter.drawLine(line4);
			painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));
			QLineF line(x_move_click - 5, y_move_click, x_move_click + 5, y_move_click);
			painter.drawLine(line);
		//	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
			QLineF line1(x_move_click, y_move_click - 5, x_move_click, y_move_click + 5);
			painter.drawLine(line1);
			//painter.setPen(QPen(Qt::red, 5));
			//painter.drawPoint(x_move_click, y_move_click);
			mouseClickEventFlag = false;
			repaintFlag = true;
		}
	}
	else if (repaintFlag)//图像一刷新，就重画该点
	{
			QPainter painter(this);
			painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
			int labelWidth = width();
			int labelHeight = height();
			QLineF line3(0, labelHeight / 2 - 1, labelWidth - 1, labelHeight / 2 - 1);
			painter.drawLine(line3);
			QLineF line4(labelWidth / 2 - 1, 0, labelWidth / 2 - 1, labelHeight - 1);
			painter.drawLine(line4);
			painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));
			QLineF line(x_move_click - 5, y_move_click, x_move_click + 5, y_move_click);
			painter.drawLine(line);
			//	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
			QLineF line1(x_move_click, y_move_click - 5, x_move_click, y_move_click + 5);
			painter.drawLine(line1);
			
		//	painter.setPen(QPen(Qt::red, 5));
		//	painter.drawPoint(x_move_click, y_move_click);
	}
//	else
//	{
	//QLabel::paintEvent(event);
//	}
}

void EventLabel::getLabelSize(int width, int height)
{
	labelWidth = width;
	labelHeight = height;
	openImage = true;
	//setMouseTracking(true);
}

void EventLabel::getImageSize(int imageWidth, int imageHeight)
{
	if ((double)imageWidth / 1280>(double)imageHeight / 960)
	{ 
	   multiple = (double)imageWidth / 1280;
	}
	else
	{
		multiple = (double)imageHeight / 960;
	}
}


void EventLabel::setResolutionType(int type)
{
	resolutionType = type;
}

void EventLabel::setOpenCameraFlag(bool flag)
{
	openCameraFlag = flag;
}

void EventLabel::setRepaintFlagFalse()
{
	repaintFlag = false;
}

void EventLabel::setPointChoosed(int xMove, int yMove, int xOrig, int yOrig)
{
	x_move_click = xMove;
	y_move_click = yMove;
	x_orig = xOrig;
	if (resolutionType == 6)
	{
		if (yMove == 0)
			y_orig_fake = 0;
		else
		    y_orig_fake = (yMove+1) * 2 -1;
		y_orig_actual = yMove * 1.98765432 + 0.5 + 10;
	}
	else
	  y_orig = yOrig;
	mouseClickEventFlag = true;
	update();
}
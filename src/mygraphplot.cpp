#include "mygraphplot.h"

//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\imgproc\imgproc.hpp>

#define pointSize 50
QVector<double> x2(pointSize), y2(pointSize);

MyGraphPlot::MyGraphPlot(QFrame *frame_9)
	:QCustomPlot(frame_9),
	pngFlag(false)
{
	QBrush qBrush(QColor(255, 0xF2, 0xCD));//设置背景色  
	setBackground(qBrush);

}

//获取文件名，并且提取数据出来
void MyGraphPlot::getDataFilename(QString filename)
{
	xAxis->setLabel(QStringLiteral("图片横坐标"));
	yAxis->setLabel(QStringLiteral("灰度值"));
	QString file_png = filename.replace(QString(".bmp"), QString(".png"));//filename本身的string也变了
	QFileInfo file_png_info(file_png);
	if (file_png_info.isFile())
		{
			data = cv::imread(file_png.toLocal8Bit().toStdString(), cv::IMREAD_ANYDEPTH);
			pngFlag=true;
        }
	else
	    { 
	        QString file_bmp;
			file_bmp = filename.replace(QString(".png"), QString(".bmp"));
	        data = cv::imread(file_bmp.toLocal8Bit().toStdString(), cv::IMREAD_GRAYSCALE);
			pngFlag=false;
	    }
}

void MyGraphPlot::getImageSize(int imageWidth, int imageHeight)
{
	row = imageHeight;
	col = imageWidth;
}

void MyGraphPlot::updateLine(int lineType, int x1, int y1, int x2, int y2)
{
/*	//提起数据
	DWORD start = GetTickCount();
	QFile dataFile(dataFile_name);//只有新建一个QFile对象时才能使用QFile::QFile(const QString & name)函数，
	                               //可是这样updateLine函数就无法访问这个函数里的 dataFile，所以需要把对文件的操作写在同一个文件里

	
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::information(NULL, "FileOpenError", "This file does't exist!");
		return;
	}
	QTextStream in(&dataFile);
	QString dataString = in.readAll();
	QStringList dataStringList = dataString.split(QRegExp("\\s+"));

	DWORD end = GetTickCount();
	qDebug() << (end - start) << "    file time";
//	if (dataStringList.size() != row* col) // 数据对齐有问题
//	{
//		QMessageBox::information(NULL, "FileOpenError", "The data is error!");
//		return;
//	}  
	// 下面把数据保存成二维整数矩阵中去  
	int **result;
	result = new int*[row];
	for (int i = 0; i<row; i++)
	{
		result[i] = new int[col];
	}
	int index = 0;

	

	for (int i = 0; i<row; i++)
	{
		for (int j = 0; j<col; j++)
		{
			result[i][j] = dataStringList.at(index++).toInt();//i代表行数，j代表列数
		}
	}
*/	
   //开始画曲线
	if (pngFlag)
	{
		yAxis->setRange(0, 4096);
	}
	else
	{
		yAxis->setRange(0, 255);
	}
	if (lineType==1)
	{ 
		QVector<double> x(col), y(col); //   初始化动态数组,有col个坐标点需要绘制
		for (int i = 0; i<col; ++i)
	    {
		   x[i] = i ; 
		//   y[i] = result[y1][i];
		   if (pngFlag)
			    y[i] = data.at<ushort>(y1, i);
		   else
			   y[i] = data.at<uchar>(y1, i);//uchar取值范围是0~255
	    }
        addGraph();
        graph(0)->setData(x, y);
	    xAxis->setRange(0, col);
      //  yAxis->setRange(0, 255);
	    replot();
	}
	if (lineType == 2)
	{
		QVector<double> x(row), y(row); 
		for (int i = 0; i < row; ++i)
		{
			x[i] = i;
			if (pngFlag)
				y[i] = data.at<ushort>(i, x1);
			else
				y[i] = data.at<uchar>(i, x1);
		}
		addGraph();
		graph(0)->setData(x, y);
		xAxis->setRange(0, row);
	//	yAxis->setRange(0, 255);
		replot();
	}
	if (lineType == 3)
	{
		double x_len = x2 - x1;
		double y_len = y2 - y1;
		double lenth = sqrt(x_len*x_len + y_len*y_len);
		int len = lenth + 0.5;  //将小数四舍五入转换为整数
		double cosine = x_len / lenth;
		double sine = y_len / lenth;

		QVector<double> x(len), y(len);
		for (int i = 0; i < len; ++i)
		{
			x[i] = (x_len / lenth)*i + x1;
			double x_pos = x1 + i*cosine;
			double y_pos = y1 + i*sine;
			int x_pos_int = x_pos;
			int y_pos_int = y_pos;//这里的x，y和数组x[i],y[i]含义不同
			if (pngFlag)
				y[i] = data.at<ushort>(y_pos_int, x_pos_int);
			else
				y[i] = data.at<uchar>(y_pos_int, x_pos_int);
		}
		addGraph();
		graph(0)->setData(x, y);
		xAxis->setRange(x1, x2);
		//xAxis->setRange(0, len);
	//	yAxis->setRange(0, 255);
		replot();
	}
	if (lineType == 4)
	{
		double x_len = x2 - x1;
		double y_len = y2 - y1;
		double lenth = sqrt(x_len*x_len + y_len*y_len);
		int len = lenth+0.5;  //将小数四舍五入转换为整数
		double cosine = x_len / lenth;
		double sine = y_len / lenth;

		QVector<double> x(len), y(len);
		for (int i = 0; i < len; ++i)
		{
			
			x[i] = (x_len / lenth)*i + x1;
			double x_pos = x1 + i*cosine;
			double y_pos = y1 + i*sine;
			int x_pos_int = x_pos;
			int y_pos_int = y_pos;//这里的x，y和数组x[i],y[i]含义不同
			if (pngFlag)
				y[i] = data.at<ushort>(y_pos_int, x_pos_int);
			else
				y[i] = data.at<uchar>(y_pos_int, x_pos_int);
		}
		addGraph();
		graph(0)->setData(x, y);
		xAxis->setRange(x1, x2);
	//	yAxis->setRange(0, 255);
		replot();
	}	
}

void MyGraphPlot::clearData()
{
	removeGraph(0);
	replot();
}

void MyGraphPlot::getPointPos(int x, int y)
{
	if (pngFlag)
		point_data = data.at<ushort>(y, x);
	else
		point_data = data.at<uchar>(y, x);
	emit sendPointData(x, y, point_data);
}

void MyGraphPlot::setupRealtimeDataDemo()
{
	xAxis->setLabel(QStringLiteral("时间"));
	yAxis->setLabel(QStringLiteral("灰度值"));
	xAxis->setRange(0, 1000);
	yAxis->setRange(0, 255);
	replot();
}
void MyGraphPlot::realtimeData(int msecs, int data)
{
	addGraph();
	QVector<QCPGraphData> *mData;
	mData = graph(0)->data()->coreData();
	mData->clear();
	QCPGraphData newPoint;

	for (int i = 0; i < pointSize-1; i++)
	{
		x2[i] = x2[i + 1];
		y2[i] = y2[i + 1];
		if (x2[i])
		{
			newPoint.key = x2[i];
			newPoint.value = y2[i];  // let's plot a quadratic function	
			mData->append(newPoint);
		}
	}

	x2[pointSize - 1] = msecs;
	y2[pointSize - 1] = data;
	newPoint.key = x2[pointSize - 1];
	newPoint.value = y2[pointSize - 1];
	mData->append(newPoint);
	//graph(0)->rescaleKeyAxis(true);
	rescaleAxes(true);//只能x,y轴一起recale
		if (bits8Flag)
			yAxis->setRange(0, 255);
		else
			yAxis->setRange(0, 4095);
	
	replot();
}

void MyGraphPlot::clearGraph()
{
	//if (graph(0))
//		graph(0)->data()->clear();
	for (int i = 0; i < pointSize ; i++)
	{
		x2[i] = 0;
		y2[i] = 0;
	}
//	xAxis->setRange(0, 10000);
//	yAxis->setRange(0, 5);
}

void MyGraphPlot::get8BitsFlag(bool flag)
{
	bits8Flag = flag;
	bitsChangeFlag = true;
}
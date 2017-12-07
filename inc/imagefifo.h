#ifndef IMAGEFIFO_H
#define IMAGEFIFO_H

#include "qqueue.h"

class ImageData
{
public:
	uchar* m_data;
	int m_bitsPerPixel;
	int m_imageHeight;
	int m_imageWidth;
};

class ImageFifo
{
public:
	//static bool isFifoFull();
	//static bool isFifoEmpty();

	static bool inFifo(ImageData *imageData);
	static void outFifo();
	
	static ImageData* getFifoHead();
	static void setFifoSize(int size);
	static void emptyFifo();

private:
	static QQueue<ImageData*> m_fifo;
	static int m_fifoSize;
	static QMutex m_mutex;
};

#endif
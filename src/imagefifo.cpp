#include "imagefifo.h"

//bool ImageFifo::isFifoFull()
//{
//	return	fifo.size() == FIFO_SIZE;
//}
//
//bool ImageFifo::isFifoEmpty()
//{
//	return	fifo.size() == 0;
//}
QQueue<ImageData*> ImageFifo::m_fifo;
int ImageFifo::m_fifoSize;
QMutex ImageFifo::m_mutex;

void ImageFifo::setFifoSize(int size)
{
	m_fifoSize = size;
}

bool ImageFifo::inFifo(ImageData *data)
{
	QMutexLocker locker(&m_mutex);
	if (m_fifo.size() < m_fifoSize - 1)             //m_fifoSize-1而不是m_fifoSize，原因是图像存储需要缓冲
	{
		m_fifo.enqueue(data);
		return true;
	}
	else
	{
		return false;
	}
	
}

void ImageFifo::outFifo()
{
	QMutexLocker locker(&m_mutex);
	if (m_fifo.size() > 0)
	{
		m_fifo.dequeue();
	}
}

ImageData* ImageFifo::getFifoHead()
{
	QMutexLocker locker(&m_mutex);
	if (m_fifo.size() > 0)
	{
		return m_fifo.head();
	}
	else
	{
		return nullptr;
	}

		
}

void ImageFifo::emptyFifo()
{
	QMutexLocker locker(&m_mutex);
	if (m_fifo.size() > 0)
	{
		m_fifo.clear();
	}
}
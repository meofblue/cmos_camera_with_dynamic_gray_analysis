#include "cydevice.h"
#include "qmessagebox.h"
#include "qdebug.h"

/**************对于不同的分辨率需要使用不同的传输大小，等待完成  TODO**********************************************/
#define     PACKETS_PER_TRANSFER				120                      //1020: 100      034: 120 
#define		PACKETS_SIZE						1024
#define     NUM_TRANSFER_PER_TRANSACTION        10                         //1020: 9       034: 10
#define     TIMEOUT_MILLI_SEC_PER_TRANSFER      50


CyDevice::CyDevice(int height, int width)
	:m_height(height),
	m_width(width),
	m_device(nullptr),
	//m_pauseFlag(false),
	m_bitsPerPixel(8),
	m_whichBuffer(0)
{
	m_size = m_height * m_width * 2; 
}

CyDevice::~CyDevice()
{
	delete m_imageBuffer;
}

bool CyDevice::openDevice(HANDLE hnd)
{ 
	if (m_device != nullptr) return false;
	m_device = new CCyUSBDevice(hnd);
	if (!m_device->Open(0))
	{
		delete m_device;
		m_device = nullptr;
		return false;
	}
	m_dataInEndPoint = m_device->BulkInEndPt;
	//m_dataInEndPoint = m_device->EndPointOf(0x82);
	m_controlEndPoint = m_device->ControlEndPt;



	sendControlCode(0xb2);    //usb设备初始化

	m_mutex.lock();
	if (m_bitsPerPixel)
	{
		sendControlCode(0xb8);    //发送8位图像数据
	}
	else
	{
		sendControlCode(0xb9);    //发送12位图像数据
	}
	m_mutex.unlock();
	
	sendControlCode(0xb4);    //清空设备帧缓存

	//m_dataInEndPoint->SetXferSize(m_size);

	m_whichBuffer = 0;
	return true;
}

void CyDevice::closeDevice()
{
	//DWORD x = GetCurrentThreadId();
	if (m_device != nullptr)
	{
		delete m_device;
		m_device = nullptr;
	}
}

void CyDevice::setDataSavingSpace(UCHAR **data, int size)
{
	m_dataBuffer = data;
	m_bufferSize = size;

	m_imageBuffer = new ImageData[size];
}


void CyDevice::enableReceving()
{
	m_mutex.lock();
	m_recevingFlag = true;
	m_mutex.unlock();
}

void CyDevice::disableReceving()
{
	m_mutex.lock();
	while (!m_recevingFlag);
	m_recevingFlag = false;//系统在运行时才会调用这个语句
	m_mutex.unlock();
}

bool CyDevice::sendControlCode(int code)
{
	m_controlEndPoint->Target = TGT_DEVICE;
	m_controlEndPoint->ReqType = REQ_VENDOR;
	m_controlEndPoint->Direction = DIR_TO_DEVICE;
	m_controlEndPoint->Value = 0;
	m_controlEndPoint->Index = 0;

	UCHAR buf = 0;
	LONG len = 0;
	m_controlEndPoint->ReqCode = code;
	return m_controlEndPoint->XferData(&buf, len);
}

bool CyDevice::sendRequestCode(int code, uchar *buf, LONG bufLen)
{
	m_controlEndPoint->Target = TGT_DEVICE;
	m_controlEndPoint->ReqType = REQ_VENDOR;
	m_controlEndPoint->Direction = DIR_TO_DEVICE;
	m_controlEndPoint->Value = 0;
	m_controlEndPoint->Index = 0;

	m_controlEndPoint->ReqCode = code;
	return m_controlEndPoint->XferData(buf, bufLen);
}

void CyDevice::setWidth(int width)
{
	m_width = width;
}

void CyDevice::setHeight(int height)
{
	m_height = height;
}

//void CyDevice::setPauseFlag(bool flag)
//{
//	m_pauseFlag = flag;
//}

void CyDevice::changeWidthTo8bitsPerPixel()
{
	m_mutex.lock();
	sendControlCode(0xb8);
	m_bitsPerPixel = 8;
	m_mutex.unlock();
}

void CyDevice::changeWidthTo16bitsPerPixel()
{
	m_mutex.lock();
	sendControlCode(0xb9);
	m_bitsPerPixel = 12;
	m_mutex.unlock();
}

void CyDevice::changeResolution(int width, int height, int req)
{
	m_mutex.lock();
	if (sendControlCode(req))
	{
		m_width = width;
		m_height = height;
	}
	m_recevingFlag = true;
	m_mutex.unlock();
//	emit changeResolutionDone();
}

//void CyDevice::receiveData()
//{
//	uchar *data;
//
//	//bool flag;
//
//	int count = 0;
//
//	LONG size = m_size;
//
//	int whichBuffer = 0;
//	data = m_dataBuffer[whichBuffer];
//
//	LONG sizePerXfer = PACKETS_SIZE * PACKETS_PER_TRANSFER;
//	LONG sizePerDataXfer;
//	OVERLAPPED inOverlaps[NUM_TRANSFER_PER_TRANSACTION];
//	UCHAR *inContexts[NUM_TRANSFER_PER_TRANSACTION];
//	int transfer_count;
//	int transfer_queue_count;
//	int transfer_count_limit;
//
//
//	//m_dataInEndPoint->SetXferSize(PACKETS_SIZE * PACKETS_PER_TRANSFER);
//
//	for (int i = 0; i < NUM_TRANSFER_PER_TRANSACTION; ++i)
//	{
//		inOverlaps[i].hEvent = CreateEvent(NULL, false, false, L"CYUSB_IN");
//	}
//
//	while (m_recevingFlag)
//	{
//		/**********receive frames in the loop*****************/
//		m_mutex.lock();
//		m_imageBuffer[whichBuffer].m_data = data;
//		m_imageBuffer[whichBuffer].m_bitsPerPixel = m_bitsPerPixel;
//		m_imageBuffer[whichBuffer].m_imageHeight = m_height;
//		m_imageBuffer[whichBuffer].m_imageWidth = m_width;
//		m_size = m_bitsPerPixel > 8 ? m_width * m_height * 2 : m_width * m_height;
//
//		//sendControlCode(0xb4);
//		sendControlCode(0xb5);
//
//		if (m_width == 320)
//			sizePerDataXfer = 320 * 240;
//		else if (m_width == 640)
//			sizePerDataXfer = 640 * 480;
//
//		m_mutex.unlock();
//
//		transfer_count = 0;
//		sizePerDataXfer = sizePerXfer;     // FinishDataXfer will modify value of sizePerDataXfer, do the copy
//		transfer_count_limit = m_size / sizePerDataXfer;
//
//
//
//		for (int i = 0; i < NUM_TRANSFER_PER_TRANSACTION; ++i)
//		{
//			inContexts[i] = m_dataInEndPoint->BeginDataXfer(data + i * sizePerDataXfer, sizePerDataXfer, &inOverlaps[i]);
//			if (m_dataInEndPoint->NtStatus || m_dataInEndPoint->UsbdStatus)      //BeginDataXfer failed
//			{
//				qDebug() << "BeginDataXfer failed!";
//				return;
//				//TODO 
//				//做些BeginDataXfer失败的事
//			}
//		}
//
//		transfer_queue_count = NUM_TRANSFER_PER_TRANSACTION;
//
//		int i = 0;
//
//		/*******receive a frame in the loop***********/
//		while (true)
//		{
//			LONG sizePerXfer = sizePerDataXfer;
//
//			if (!m_dataInEndPoint->WaitForXfer(&inOverlaps[i], TIMEOUT_MILLI_SEC_PER_TRANSFER))
//			{
//				m_dataInEndPoint->Abort();
//				/*if (m_dataInEndPoint->LastError == ERROR_IO_PENDING)
//				WaitForSingleObject(inOverlaps[i].hEvent, 2000);*/
//
//				//TODO
//				qDebug() << "WaitForXfer failed!" << i << "  " << transfer_count;
//				break;
//			}
//
//			if (!m_dataInEndPoint->FinishDataXfer(data + transfer_count * sizePerDataXfer, sizePerDataXfer, &inOverlaps[i], inContexts[i]))
//			{
//				//TODO
//				qDebug() << "FinishDataXfer failed!";
//				break;
//			}
//
//			//qDebug() << "xfer succeed" << transfer_count << "  " << transfer_queue_count;
//			++transfer_count;
//
//			if (transfer_queue_count < transfer_count_limit)
//			{
//
//				inContexts[i] = m_dataInEndPoint->BeginDataXfer(data + transfer_queue_count * sizePerDataXfer, sizePerDataXfer, &inOverlaps[i]);
//				if (m_dataInEndPoint->NtStatus || m_dataInEndPoint->UsbdStatus)      //BeginDataXfer failed
//				{
//					qDebug() << "BeginDataXfer in loop failed!";
//					return;
//					//TODO 
//					//做些BeginDataXfer失败的事
//				}
//
//
//				++transfer_queue_count;
//			}
//
//			++i;
//
//			if (i == NUM_TRANSFER_PER_TRANSACTION)
//				i = 0;
//
//			if (transfer_count == transfer_count_limit)
//			{
//				emit completeFrameTransmission();
//				//if (!m_pauseFlag)
//				//{
//				if (ImageFifo::inFifo(&m_imageBuffer[whichBuffer]))
//				{
//					++whichBuffer;
//					if (whichBuffer == m_bufferSize)
//						whichBuffer = 0;
//					data = m_dataBuffer[whichBuffer];
//				}
//				//}
//
//				break;
//			}
//
//		}
//
//	}
//
//	for (int i = 0; i < NUM_TRANSFER_PER_TRANSACTION; ++i)
//	{
//		CloseHandle(inOverlaps[i].hEvent);
//	}
//
//}

void CyDevice::receiveData()
{
	//初始传输1280 * 960
	//changeResolution(1280, 960, 0xa1);
	//receiveData(m_width * m_height / 9, 9, 1000);
	//receiveData(m_width * m_height, 1, 1000);
	//receiveData(m_width * m_height / 24, 24, 1000);
//	receiveData(120 * 1024, 10, 50);
	sendControlCode(0xc3);
	receiveData(120 * 1024 , 10, 1000);
	//初始传输640 * 480
	//changeResolution(640, 480, 0xa2);
	//sendControlCode(0xb1);
	//receiveData(76800, 4, 200);
}

void CyDevice::receiveData(LONG sizePerXfer, int xferQueueSize, int timeOut)
{
	uchar *data;

	//bool flag;

	int count = 0;

	LONG size = m_size;

	data = m_dataBuffer[m_whichBuffer];

	LONG sizePerDataXfer;
	OVERLAPPED* inOverlaps = new OVERLAPPED[xferQueueSize];
	UCHAR **inContexts = new UCHAR*[xferQueueSize];

	int transfer_count;
	int transfer_queue_count;
	int transfer_count_limit;


	//m_dataInEndPoint->SetXferSize(PACKETS_SIZE * PACKETS_PER_TRANSFER);

	for (int i = 0; i < xferQueueSize; ++i)
	{
		inOverlaps[i].hEvent = CreateEvent(NULL, false, false, L"CYUSB");
	}

	
	while (m_recevingFlag)
	{
		/**********receive frames in the loop*****************/
		m_mutex.lock();
		m_imageBuffer[m_whichBuffer].m_data = data;
		m_imageBuffer[m_whichBuffer].m_bitsPerPixel = m_bitsPerPixel;
		m_imageBuffer[m_whichBuffer].m_imageHeight = m_height;
		m_imageBuffer[m_whichBuffer].m_imageWidth = m_width;
		m_size = m_bitsPerPixel > 8 ? m_width * m_height * 2 : m_width * m_height;

		//sendControlCode(0xb4);
		sendControlCode(0xb5);

		m_mutex.unlock();

		transfer_count = 0;
		sizePerDataXfer = sizePerXfer;     // FinishDataXfer will modify value of sizePerDataXfer, do the copy
		transfer_count_limit = m_size / sizePerDataXfer;



		for (int i = 0; i < xferQueueSize; ++i)
		{
			inContexts[i] = m_dataInEndPoint->BeginDataXfer(data + i * sizePerDataXfer, sizePerDataXfer, &inOverlaps[i]);
			if (m_dataInEndPoint->NtStatus || m_dataInEndPoint->UsbdStatus)      //BeginDataXfer failed
			{
				qDebug() << "BeginDataXfer failed!";
				return;
				//TODO 
				//做些BeginDataXfer失败的事
			}
		}

		transfer_queue_count = xferQueueSize;

		int j = 0;

		/*******receive a frame in the loop***********/
		while (true)
		{
			  

			if (!m_dataInEndPoint->WaitForXfer(&inOverlaps[j], timeOut))
			{
				m_dataInEndPoint->Abort();
				/*if (m_dataInEndPoint->LastError == ERROR_IO_PENDING)
				WaitForSingleObject(inOverlaps[i].hEvent, 2000);*/

				//TODO
				qDebug() << "WaitForXfer failed!" << j << "  " << transfer_count;
				break;
			}

			if (!m_dataInEndPoint->FinishDataXfer(data + transfer_count * sizePerDataXfer, sizePerDataXfer, &inOverlaps[j], inContexts[j]))
			{
				//TODO
				qDebug() << "FinishDataXfer failed!";
				break;
			}

			//qDebug() << "xfer succeed" << transfer_count << "  " << transfer_queue_count;
			++transfer_count;

		//	sizePerDataXfer = sizePerXfer;

			if (transfer_queue_count < transfer_count_limit)
			{

				inContexts[j] = m_dataInEndPoint->BeginDataXfer(data + transfer_queue_count * sizePerDataXfer, sizePerDataXfer, &inOverlaps[j]);
				if (m_dataInEndPoint->NtStatus || m_dataInEndPoint->UsbdStatus)      //BeginDataXfer failed
				{
					qDebug() << "BeginDataXfer in loop failed!";
					return;
					//TODO 
					//做些BeginDataXfer失败的事
				}


			++transfer_queue_count;
			}

			++j;

			if (j == xferQueueSize)
				j = 0;

			if (transfer_count == transfer_count_limit)
			{
				//emit completeFrameTransmission();
				//if (!m_pauseFlag)
				//{
				if (ImageFifo::inFifo(&m_imageBuffer[m_whichBuffer]))
				{
					++m_whichBuffer;
					if (m_whichBuffer == m_bufferSize)
						m_whichBuffer = 0;
					data = m_dataBuffer[m_whichBuffer];
				}
				//}

				break;
			}

		}

	}

	for (int i = 0; i < xferQueueSize; ++i)
	{
		CloseHandle(inOverlaps[i].hEvent);
	}

	delete[] inOverlaps;
	delete[] inContexts;
}

void CyDevice::changeResolution(int width, int height, int req, long sizePerXfer, int xferQueueSize, int timeOut)
{
	changeResolution(width, height, req);
	receiveData(sizePerXfer, xferQueueSize, timeOut);
}

bool CyDevice::isReceving()
{
	QMutexLocker locker(&m_mutex);
	return m_recevingFlag;
}

void CyDevice::waitChangeDone()
{
	while (!m_recevingFlag);//等到为true
}
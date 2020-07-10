#include "stdafx.h"
#include "CameraImage.h"
#include "resource.h"
#include "afxdialogex.h"

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include<direct.h>
#include<atltime.h>
#include<io.h>
#include<stdio.h>
#include<iostream>

//3.31�޸�
//int staticCount = 1;

using namespace cv;

CCameraImage::CCameraImage()
{
	saveIndex = 1;  // ����һ��������
	stripIndex = 1; // ͼ���������
	stripShowIndex = 1;  // ����ͼ����ʾ����
	stripCount = 1; // ͼ���������
	frameCount = 1; // һ����ͼ��֡��
	frameCountIndex = 0;  // һ����ͼ������

	m_threshold = 120;  // �����ʼ����ֵ�����ȡ��
	//3.29��
	m_DiaThreshold = 100;   // �������������ʼ����ֵ�����ȡ��

	//4.10
	graythreshold = 36;

	//3.28
	sampleCountStrip = 2;        // ÿ��ȡ������   //3.28����
	sampleShowIndex = 1;         // ȡ����ʾ����


	//int i = rand() % 7 - 3;
	//cropLine = 190 + i;   // �ü���
	cropLine = 3072;

	//3.28
	

	m_strErrorMsg = _T("");

	m_bIsLoadConfig = FALSE;

	m_area = 0.0;        // ����ƽ�����
	m_length = 0.0;      // ����ƽ���ܳ�
	m_ratioArea = 0.0;   // ����������
	//areaCount = 0;       // ��������
	areaMax = 0.0;       // ������
	areaMin = 0.0;       // ��С���

	valueCount = 0;
	meanArea = 0.0;
	meanLength = 0.0;
	ratioArea = 0.0;
	meanMaxDia = 0.0;
	meanMinDia = 0.0;
	maxBigDia = 0.0;
	minBigDia = 0.0;
	maxSmallDia = 0.0;
	minSmallDia = 0.0;
	maxLength = 0.0;
	minLength = 0.0;
	maxArea = 0.0;
	minArea = 0.0;

	sumBigDia = 0.0;
	sumSmallDia = 0.0;
	sumLength = 0.0;
	sumArea = 0.0;

	maxDiaSD = 0.0;
	minDiaSD = 0.0;
	lengthSD = 0.0;
	areaSD = 0.0;

	m_resolution = 5.378;


	// ��⿪ʼʱ���ʼ��
	startTime = CTime::GetCurrentTime();
	CString startTimeStr = startTime.Format(_T("%Y.%m.%d  %H:%M"));



	m_pBmpInfo = (BITMAPINFO*)new char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	if (m_pBmpInfo)
	{
		m_pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBmpInfo->bmiHeader.biSizeImage = 0;
		m_pBmpInfo->bmiHeader.biWidth = 0;
		m_pBmpInfo->bmiHeader.biHeight = 0;
		m_pBmpInfo->bmiHeader.biBitCount = 8;
		m_pBmpInfo->bmiHeader.biClrImportant = 0;
		m_pBmpInfo->bmiHeader.biClrUsed = 256;
		m_pBmpInfo->bmiHeader.biCompression = BI_RGB;
		m_pBmpInfo->bmiHeader.biPlanes = 1;
		m_pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
		m_pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
		for (int i = 0; i <= 255; i++)
		{
			m_pBmpInfo->bmiColors[i].rgbBlue = i;
			m_pBmpInfo->bmiColors[i].rgbGreen = i;
			m_pBmpInfo->bmiColors[i].rgbRed = i;
			m_pBmpInfo->bmiColors[i].rgbReserved = 0;
		}
	}
	/*612*/
	//pRawData = NULL;
	pData = NULL;

}


CCameraImage::~CCameraImage()
{
	DestroyObjects();

	if (m_AcqDevice)
		delete m_AcqDevice;
	if (m_Buffers)
		delete m_Buffers;
	if (m_Xfer)
		delete m_Xfer;
	
	if(m_pBmpInfo)
		delete m_pBmpInfo;
	if (pBmpInfoStrip)
		delete pBmpInfoStrip;
	if(pColorTable)
		delete pColorTable;

	if (pBmpDataShow)
		delete pBmpDataShow;

	/*if (pRawData)
	{
		delete[] pRawData;
	}*/

}


void CCameraImage::XferCallback(SapXferCallbackInfo *pInfo)
{
	CCameraImage *pDlg = (CCameraImage *)pInfo->GetContext();

	// If grabbing in trash buffer, do not display the image, update the
	// appropriate number of frames on the status bar instead
	if (pInfo->IsTrash())
	{
		CString str;
		str.Format(_T("Frames acquired in trash buffer: %d"), pInfo->GetEventCount());
		// pDlg->m_statusWnd.SetWindowText(str);
	}
	else
	{
		//BYTE *pData;

		int nWidth, nHeight;

		if (pDlg->m_Buffers->GetAddress((VOID**)&(pDlg->pData)))
		{
			nWidth = pDlg->m_Buffers->GetWidth();
			nHeight = pDlg->m_Buffers->GetHeight();
			//memcpy(pDlg->pRawData, pDlg->pData, nWidth*nHeight);

			pDlg->TransferData(pDlg->pData, nWidth, nHeight);

			//pDlg->TransferData(pData, nWidth, nHeight);
			
			//AfxMessageBox(_T("�ص��������óɹ���"));

			// pDlg->m_Buffers->SetIndex(1);

			/*
			staticCount++;
			if (staticCount > 6)
			{
				staticCount = 1;
				pDlg->m_Buffers->Clear();
				AfxMessageBox(_T("�������ɹ���"));
			}
			*/

		}
	}
}



// ----------------------------------[����ת������]----------------------------------
// ��ʾ��ǰ֡ͼ��
// ����һ֡ͼ��
// ����FillData����
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::TransferData(unsigned char *pData, int nWidth, int nHeight)
{
	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;
	pwnd->GetWindowRect(&srect);

	// -----------------[����ͼ��]---------------
	// ͼ�񱣴����
	m_pBmpInfo->bmiHeader.biWidth = nWidth;
	m_pBmpInfo->bmiHeader.biHeight = nHeight;
	m_pBmpInfo->bmiHeader.biSizeImage = nWidth*nHeight;

	//��һ�����ɼ��꣬��ʼ�ɼ��ڶ���
	if ((stripIndex < stripCount) && (saveIndex > frameCount))
	{
		stripIndex++;
		saveIndex = 1;
	}


	//ԭ�Ȳ�һ��ÿ֡��ȡ�����޸ĳ�ÿ֡��ȡ������gapValue����û����,��ɾ����
	if (saveIndex <= sampleCountStrip)
	{
		// ����ͼ��·��
		CString CstrPath;
		CString CstrIndex;
		CstrIndex.Format(_T("\\%d-%d"), stripIndex, saveIndex);
		CstrPath = childFolderCStr + CstrIndex;
		CstrPath += ".bmp";

		// ����ÿһ֡ͼ��
		SaveImage(pData, nWidth, nHeight, CstrPath);
	}
	

	saveIndex++;

	// ��ʾһ֡ͼ��
	int nShowWidth = srect.Width();
	int nShowHeight = nShowWidth*nHeight / (nWidth*1.355);

	SetStretchBltMode(pdc->GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(pdc->GetSafeHdc(),
		0,
		0,
		nShowWidth,
		nShowHeight,
		0,
		0,
		nWidth,
		nHeight,
		pData,
		m_pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	//AfxMessageBox(_T("���ȡ��ǰ"));
	////saveIndex��ÿ�ɼ�һ֡��ִ��һ�λص���������һ��
	////6.12�޸�
	//if (stripIndex >= stripCount && (saveIndex == (frameCount+1)))
	//{ 
	////if (stripIndex >= stripCount && saveIndex > sampleCountStrip)
	//	ImageProcessAccuracy();
	//	ResetAllIndex();
	//
	//}
	
	//AfxMessageBox(_T("���ȡ����"));
	//2019.3.22�¼�
	pwnd->ReleaseDC(pdc);
	// -------------------[���ͼ��]------------------
	// FillData(pData, nWidth, nHeight);
}



// ----------------------------------[ͼ�񱣴�]----------------------------------
// ͨ�����ݹ�����ͼ����������ߺ�·������ͼ��
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::SaveImage(unsigned char *pData, int nWidth, int nHeight, CString strPath)
{
	long imgBufSize = nWidth*nHeight;
	// λͼ�ļ�ͷ
	BITMAPFILEHEADER BmpFH;
	BmpFH.bfReserved1 = 0;
	BmpFH.bfReserved2 = 0;
	BmpFH.bfType = 0x4D42;
	BmpFH.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256 + imgBufSize;
	BmpFH.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;

	CFile bf; 
	if (bf.Open(strPath, CFile::modeCreate | CFile::modeWrite))
	{
		bf.Write(&BmpFH, sizeof(BITMAPFILEHEADER));
		bf.Write(&m_pBmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));
		bf.Write(m_pBmpInfo->bmiColors, 1024);
		bf.Write(pData, imgBufSize);
		bf.Close();
	}
}


// ----------------------------------[�������]----------------------------------
// ��ͼ���԰�֡ΪһС������
// �����һ���ĵ��������ź͵�����һ��ͼ��
// ͼ��ɨ����������ͼ������

/////////////////////////////////////////////////////////////////////////////////


// int OtsuAlgThreshold(Mat image);
int CCameraImage::OtsuAlgThreshold(Mat image)
{
	if (image.channels() != 1)
	{
		// cout << "Please input Gray-image!" << endl;
		return 128;
	}

	int T = 0; //Otsu�㷨��ֵ  
	double varValue = 0; //��䷽���м�ֵ����  
	double w0 = 0; //ǰ�����ص�����ռ����  
	double w1 = 0; //�������ص�����ռ����  
	double u0 = 0; //ǰ��ƽ���Ҷ�  
	double u1 = 0; //����ƽ���Ҷ�  
	double Histogram[256] = { 0 }; //�Ҷ�ֱ��ͼ���±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ�����ص�����  
	uchar *data = image.data;
	double totalNum = image.rows*image.cols; //��������  

											 //����Ҷ�ֱ��ͼ�ֲ���Histogram�����±��ǻҶ�ֵ�����������ǻҶ�ֵ��Ӧ���ص���  
	for (int i = 0; i<image.rows; i++)   //Ϊ������������û�а�rows��cols���������  
	{
		for (int j = 0; j<image.cols; j++)
		{
			Histogram[data[i*image.step + j]]++;
		}
	}
	for (int i = 0; i<255; i++)
	{
		//ÿ�α���֮ǰ��ʼ��������  
		w1 = 0;
		u1 = 0;
		w0 = 0;
		u0 = 0;
		//***********����������ֵ����**************************  
		for (int j = 0; j <= i; j++) //�������ָ�ֵ����  
		{
			w1 += Histogram[j];  //�����������ص�����  
			u1 += j*Histogram[j]; //�������������ܻҶȺ�  
		}
		//if (w1 == 0) //�����������ص���Ϊ0ʱ�˳�  
		//{
		//	break;
		//}
		u1 = u1 / w1; //��������ƽ���Ҷ�  
		w1 = w1 / totalNum; // �����������ص�����ռ����  


							//***********ǰ��������ֵ����**************************  
		for (int k = i + 1; k<255; k++)
		{
			w0 += Histogram[k];  //ǰ���������ص�����  
			u0 += k*Histogram[k]; //ǰ�����������ܻҶȺ�  
		}
		if (w0 == 0) //ǰ���������ص���Ϊ0ʱ�˳�  
		{
			break;
		}
		u0 = u0 / w0; //ǰ������ƽ���Ҷ�  
		w0 = w0 / totalNum; // ǰ���������ص�����ռ����  
							//***********ǰ��������ֵ����**************************  

							//***********��䷽�����******************************  
		double varValueI = w0*w1*(u1 - u0)*(u1 - u0); //��ǰ��䷽�����  
		if (varValue<varValueI)
		{
			varValue = varValueI;
			T = i;
			// cout << T << endl;
		}
	}
	return T;
}





// ----------------------------------[֮ǰͼ����]----------------------------------
/////////////////////////////////////////////////////////////////////////////////





void CCameraImage::ImageProcessAccuracy(CWnd *pWnd)
{
	// *************************������ͼ����**************************

	for (int i = 1; i <= stripCount; i++)   // ͼ�����ѭ��
	{
		Mat srcImg_all;
		for (int j = 1; j <= sampleCountStrip; j++)  // ͼ��С��ѭ��
		{
			String strPath;
			strPath = childFolderStr + "\\" + to_string(i) + "-" + to_string(j) + ".BMP";

			// ��ȡͼ��
			Mat srcImg1 = imread(strPath, 0);  // Դͼ��

			//ͼ��߶�ѹ��  ����ԭʼͼ��·��
			resize(srcImg1, srcImg1, Size(8192, 2048), 0, 0, INTER_LINEAR);
			cv::imwrite(strPath, srcImg1);

			// �ü�������ͼƬ��С����2048*2048
			Mat srcImg = srcImg1(Range(0, srcImg1.rows), Range(cropLine, 8192 - cropLine));

			//����ü�֮���ԭͼ
			string strPathSrc;
			strPathSrc = childFolderStr + "\\" + to_string(i) + "-" + to_string(j) + "-src.BMP";
			cv::imwrite(strPathSrc, srcImg);

			//��ͼƬƴ������
			if (j == 1)
				srcImg_all = srcImg;
			else
				vconcat(srcImg, srcImg_all, srcImg_all);
		}
		//����ͼ����
		String strPath_all;
		strPath_all = childFolderStr + "\\" + to_string(i) + "-unite.BMP";
		cv::imwrite(strPath_all, srcImg_all);

		//// ʹ�þ�ȷ����
		//ComputeAccuracy(srcImg_all, i);

		//// �����д�뵽�ļ�
		//CTime time;
		//time = CTime::GetCurrentTime();
		//CString curData = time.Format(_T("%Y%m%d_%H%M_%S"));

		//CString cstrWriteResultPath;
		//cstrWriteResultPath = "D:\\Det531\\x64\\result\\"; //��ݷ�ʽΨһ����·��!!!
		////cstrWriteResultPath = "D:\\Det531\\result\\"; //����ʱ��ݷ�ʽΨһ����·��!!!
		//cstrWriteResultPath += startTimeCStr;
		//cstrWriteResultPath += "\\";
		//cstrWriteResultPath += curData;
		//cstrWriteResultPath += ".xls";

		////3.28
		//if (i == 1)
		//	WriteResultToExcelAccuracy(cstrWriteResultPath, TRUE);
		//else
		//	WriteResultToExcelAccuracy(cstrWriteResultPath, FALSE);

		// ���ý�����ֵ
		CProgressCtrl *pProgressImage = (CProgressCtrl *)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_PROGRESS_IMAGE);
		int progressValue = i / stripCount * 100;
		pProgressImage->SetPos(progressValue);

		srcImg_all.deallocate();
	}
}



void CCameraImage::ComputeAccuracy(Mat& srcImg, int i)
{
	Mat dstImg;
	int thresholdValue = OtsuAlgThreshold(srcImg);
	//4.10
	thresholdValue = thresholdValue - graythreshold;
	threshold(srcImg, dstImg, thresholdValue, 255, CV_THRESH_BINARY_INV);


	// �ṹԪ
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));

	// ������
	morphologyEx(dstImg, dstImg, MORPH_OPEN, element1);
	// ������
	morphologyEx(dstImg, dstImg, MORPH_CLOSE, element2);

	// ��������
	vector < vector < cv::Point >> contours;
	vector<Vec4i> hierarchy;
	findContours(dstImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	Mat dstImg2 = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);

	// �������ж�������
	int index = 0;
	Scalar color(0, 0, 255);
	double area, length;
	double areaSum = 0.0, lengthSum = 0.0, areaSumNew = 0.0;  //3.7

	double maxRadius = 0.0;   // �������
	double minRadius = 0.0;   // ��С����
	double radiusMaxSum = 0.0;   // �������֮��
	double radiusMinSum = 0.0;   // ��С����֮��

	// ��С��Ӿ���
	double l1 = 0.0;
	double l2 = 0.0;
	double bigL = 0.0;
	double smallL = 0.0;

	// ����
	valueCount = 0;
	vecArea.clear();
	vecArea.swap(vector<double>());
	vecLength.clear();
	vecLength.swap(vector<double>());
	vecMaxDia.clear();
	vecMaxDia.swap(vector<double>());
	vecMinDia.clear();
	vecMinDia.swap(vector<double>());

	for (index = 0; index < contours.size(); index++)
	{
		area = contourArea(contours[index]);
		length = arcLength(contours[index], true);

		double l1 = 0.0, l2 = 0.0;   // ��С��Ӿ��εĳ���
		// ��С��Ӿ��μ���
		RotatedRect rect = minAreaRect(contours[index]);
		Point2f P[4];
		rect.points(P);
		Size s = rect.size;
		l1 = s.width;
		l2 = s.height;

		if (l1 > l2)
		{
			bigL = l1 * m_resolution;
			smallL = l2 * m_resolution;
		}
		else
		{
			bigL = l2 * m_resolution;
			smallL = l1 * m_resolution;
		}

		// ��С����
		minRadius = smallL;

		if (minRadius >= m_DiaThreshold)
		{
			if (length * m_resolution > 50000)  //��ֵ50000΢��
			{
				cv::drawContours(dstImg2, contours, index, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else if (bigL / smallL > 6)   //��С��Ӿ��γ���ȴ���6���ж�Ϊ����
			{
				cv::drawContours(dstImg2, contours, index, Scalar(0, 255, 0), CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else if (vecArea.size() < 800 * sampleCountStrip)
			{
				double maxLength = 0.0;
				double curLenth = 0.0;
				double fValue1 = 0, fValue2 = 0;
				// ѭ����ȷ����
				for (int i = 0; i < contours[index].size(); i++)
				{
					for (int j = 0; j < contours[index].size(); j++)
					{
						fValue1 = (contours[index][i].x - contours[index][j].x);
						fValue2 = (contours[index][i].y - contours[index][j].y);
						curLenth = sqrt(fValue1* fValue1 + fValue2 * fValue2);
						if (curLenth > maxLength)
							maxLength = curLenth;
					}
				}

				maxRadius = maxLength * m_resolution;// �õ��������

				valueCount++;
				areaSum += area;
				lengthSum += length;
				radiusMaxSum += maxRadius;
				radiusMinSum += minRadius;

				area = area * m_resolution*m_resolution;
				length = length * m_resolution;

				vecArea.push_back(area);           // ���
				vecLength.push_back(length);       // �ܳ�
				vecMaxDia.push_back(maxRadius);    // ÿ���������
				vecMinDia.push_back(minRadius);    // ÿ����С����
				cv::drawContours(dstImg2, contours, index, Scalar(0, 0, 255), CV_FILLED, 8, hierarchy);
			}
		}
	}

	// ������ͼ��
	string strPathColor;
	strPathColor = childFolderStr + "\\" + to_string(i) + "-color.BMP";
	cv::imwrite(strPathColor, dstImg2);
	dstImg.deallocate();
	dstImg2.deallocate();
	// ******************��ƽ���� ������ʡ� ���������������
	meanMaxDia = radiusMaxSum / valueCount;                                 // ƽ���������
	meanMinDia = radiusMinSum / valueCount;                                 // ƽ����С����
	meanArea = areaSum / valueCount * m_resolution * m_resolution;          // ƽ�����
	meanLength = lengthSum / valueCount * m_resolution;                     // ƽ���ܳ�
	areaStrip += srcImg.rows*srcImg.cols;          // һС�����������
	ratioArea = areaSum / areaStrip * 100;                  // �����
	//ratioAreaNew = areaSumNew / (areaSum * m_resolution * m_resolution) * 100;									// �������90000���������ռ�ܿ�������ı���
	areaStrip *= m_resolution*m_resolution;          // һС�������

	// ******************���ܼơ�
	sumBigDia = radiusMaxSum;
	sumSmallDia = radiusMinSum;
	sumLength = lengthSum * m_resolution;
	sumArea = areaSum * m_resolution*m_resolution;

	// *****************����� ����С�� ����׼�
	for (int i = 0; i < valueCount; i++)
	{
		// ѭ����һ�γ�ʼ��
		if (i == 0)
		{
			maxBigDia = vecMaxDia[i];
			minBigDia = vecMaxDia[i];

			maxSmallDia = vecMinDia[i];
			minSmallDia = vecMinDia[i];

			maxLength = vecLength[i];
			minLength = vecLength[i];

			maxArea = vecArea[i];
			minArea = vecArea[i];
		}
		else
		{
			if (maxBigDia < vecMaxDia[i])
				maxBigDia = vecMaxDia[i];
			if (minBigDia > vecMaxDia[i])
				minBigDia = vecMaxDia[i];

			if (maxSmallDia < vecMinDia[i])
				maxSmallDia = vecMinDia[i];
			if (minSmallDia > vecMinDia[i])
				minSmallDia = vecMinDia[i];

			if (maxLength < vecLength[i])
				maxLength = vecLength[i];
			if (minLength > vecLength[i])
				minLength = vecLength[i];

			if (maxArea < vecArea[i])
				maxArea = vecArea[i];
			if (minArea > vecArea[i])
				minArea = vecArea[i];

		}
		// ��׼��ļ���
		maxDiaSD += (vecMaxDia[i] - meanMaxDia)*(vecMaxDia[i] - meanMaxDia) / valueCount;
		minDiaSD += (vecMinDia[i] - meanMinDia)*(vecMinDia[i] - meanMinDia) / valueCount;
		lengthSD += (vecLength[i] - meanLength)*(vecLength[i] - meanLength) / valueCount;
		areaSD += (vecArea[i] - meanArea)*(vecArea[i] - meanArea) / valueCount;
	}

	maxDiaSD = sqrt(maxDiaSD);
	minDiaSD = sqrt(minDiaSD);
	lengthSD = sqrt(lengthSD);
	areaSD = sqrt(areaSD);

	//3.28
	//---------------------���������ʾ���б��С�---------------------
	CListCtrl  *pCtrl = static_cast<CListCtrl *>(AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST));

	CString cstrResultToList;
	//// ��������ʾ
	//cstrResultToList.Format(_T("%ld"), valueCount);
	//pCtrl->SetItemText(0, 1, cstrResultToList);

	//3.28���
	// ���������ʾ
	cstrResultToList.Format(_T("%.2f"), maxBigDia);
	pCtrl->SetItemText(0, 1, cstrResultToList);
	// ƽ��ֱ����ʾ
	cstrResultToList.Format(_T("%.2f"), meanMaxDia);
	pCtrl->SetItemText(0, 2, cstrResultToList);
	// ����ʵ���ʾ
	cstrResultToList.Format(_T("%.6f"), ratioArea);
	pCtrl->SetItemText(0, 3, cstrResultToList);
}

// ----------------------------------[Ѱ�ҷֽ���]----------------------------------
/////////////////////////////////////////////////////////////////////////////////




void CCameraImage::ReflashImageDlg()
{
	pBmpDataClear = new unsigned char[100 * 100];
	memset(pBmpDataClear, 240, 100 * 100);

	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;
	pwnd->GetWindowRect(&srect);

	// ͼ�񱣴����
	m_pBmpInfo->bmiHeader.biWidth = 100;
	m_pBmpInfo->bmiHeader.biHeight = 100;
	m_pBmpInfo->bmiHeader.biSizeImage = 100*100;

	int nShowWidth = srect.Width();
	int nShowHeight = srect.Height();

	SetStretchBltMode(pdc->GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(pdc->GetSafeHdc(),
		0,
		0,
		nShowWidth,
		nShowHeight,
		0,
		0,
		100,
		100,
		pBmpDataClear,
		m_pBmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

}





// ----------------------------------[ͼ���⺯��]----------------------------------
/////////////////////////////////////////////////////////////////////////////////


// ----------------------------------[�õ���⿪ʼʱ��]----------------------------------
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::GetStartTime()
{
	startTime = CTime::GetCurrentTime();
	// startTimeStr = startTime.Format(_T("%Y.%m.%d  %H:%M"));
	startTimeCStr = startTime.Format("%Y%m%d_%H%M");
}

void CCameraImage::SetDetectCount(int count)
{
	detectCount = count;
}


// ----------------------------------[�½�ͼƬ�ļ��к����ļ���]----------------------------------
// ----------------------------------[�½�������ļ��к����ļ���]----------------------------------
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::MkdirPic()
{
	// ͼƬ�ļ���
	if (_access("..\\image", 0) != 0)  // ͼƬ�ļ��в�����
	{
		_mkdir("..\\image");   // �½�ͼƬ�ļ���
	}

	// ������ļ���
	if (_access("..\\result", 0) != 0)  // ������ļ��д���
	{
		_mkdir("..\\result");  // �½�������ļ���
	}

	// �½�ͼƬ���ļ���
	childFolderCStr = _T("..\\image\\") + startTimeCStr;
	USES_CONVERSION;
	childFolderP = W2A(childFolderCStr);
	childFolderStr = CW2A(childFolderCStr.GetString());
	_mkdir(childFolderP);

	// �½���������ļ���
	childFolderExcelCStr = _T("..\\result\\") + startTimeCStr;
	char *excelP = W2A(childFolderExcelCStr);
	_mkdir(excelP);


}


void CCameraImage::WriteResultToExcelAccuracy(CString cstrWritePath, BOOL flagFirst)
{
	// ��ʼ�����
	//3.28
	if (flagFirst && (detectCount == 1))
	{
		//if (CoInitialize(NULL) != NULL)
		//if (SUCCEEDED(CoInitialize(NULL)))
		/*if (CoInitialize(NULL) == 0)
		{
			AfxMessageBox(_T("��ʼ��COMʧ��"));
		}*/

		CoUninitialize();
		if (CoInitialize(NULL) == S_FALSE)
		{
			AfxMessageBox(_T("��ʼ��COM֧�ֿ�ʧ�ܣ�"));
		}


		/*if (!AfxOleInit())
		{
			AfxMessageBox(_T("�޷���ʼ��COM�Ķ�̬���ӿ�"));
		}*/

	}

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		AfxMessageBox(_T("�޷�����ExcelӦ�ã�"));
	}

	books = app.get_Workbooks();
	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));

	//���ж��뷽ʽ
	val.vt = VT_I2;
	val.iVal = -4108;




	// *****************************����ͷ��********************************
	//�������Ϊ��A��1���ͣ�D��1����������Ԫ�� 
	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("E1")));
	range.put_RowHeight(_variant_t((long)50));  //�����п�
	range.Merge(_variant_t((long)0));//�ϲ���Ԫ��

									 //���ж���
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);

	//���õ�Ԫ������
	range.put_Value2(COleVariant(_T("���ͳ�Ʊ���")));

	//�����������
	font = range.get_Font();
	font.put_Bold(COleVariant((short)TRUE));
	font.put_Name(_variant_t("����"));
	font.put_Size(_variant_t(30));


	// *************************������ʱ�䡿********************************
	range = sheet.get_Range(COleVariant(_T("A2")), COleVariant(_T("A2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("����ʱ�䣺")));

	CTime curTime = CTime::GetCurrentTime();
	CString curTimeCStr;

	curTimeCStr = curTime.Format(_T("%Y.%m.%d"));
	range = sheet.get_Range(COleVariant(_T("B2")), COleVariant(_T("B2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(curTimeCStr));

	curTimeCStr = curTime.Format(_T("%H:%M"));
	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(curTimeCStr));


	// *************************��ͳ�ƽ����********************************
	range = sheet.get_Range(COleVariant(_T("A3")), COleVariant(_T("A3")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("��ͳ�ơ�")));

	range = sheet.get_Range(COleVariant(_T("B4")), COleVariant(_T("B4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���ֱ��/um")));

	range = sheet.get_Range(COleVariant(_T("C4")), COleVariant(_T("C4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("��Сֱ��/um")));

	range = sheet.get_Range(COleVariant(_T("D4")), COleVariant(_T("D4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("�ܳ�/um")));

	range = sheet.get_Range(COleVariant(_T("E4")), COleVariant(_T("E4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���/um2")));

	range = sheet.get_Range(COleVariant(_T("A5")), COleVariant(_T("A5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("ƽ��")));

	range = sheet.get_Range(COleVariant(_T("A6")), COleVariant(_T("A6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("��׼��")));

	range = sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("A7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���")));

	range = sheet.get_Range(COleVariant(_T("A8")), COleVariant(_T("A8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("��С")));

	range = sheet.get_Range(COleVariant(_T("A9")), COleVariant(_T("A9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("�ܼ�")));



	// ******************��ƽ����
	// ���ֱ��
	CString strResult;
	strResult.Format(_T("%.0f"), meanMaxDia);
	range = sheet.get_Range(COleVariant(_T("B5")), COleVariant(_T("B5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ��Сֱ��
	strResult.Format(_T("%.0f"), meanMinDia);
	range = sheet.get_Range(COleVariant(_T("C5")), COleVariant(_T("C5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// �ܳ�
	strResult.Format(_T("%.0f"), meanLength);
	range = sheet.get_Range(COleVariant(_T("D5")), COleVariant(_T("D5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ���
	strResult.Format(_T("%.0f"), meanArea);
	range = sheet.get_Range(COleVariant(_T("E5")), COleVariant(_T("E5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************����׼�
	// ���ֱ��
	strResult.Format(_T("%.0f"), maxDiaSD);
	range = sheet.get_Range(COleVariant(_T("B6")), COleVariant(_T("B6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ��Сֱ��
	strResult.Format(_T("%.0f"), minDiaSD);
	range = sheet.get_Range(COleVariant(_T("C6")), COleVariant(_T("C6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// �ܳ�
	strResult.Format(_T("%.0f"), lengthSD);
	range = sheet.get_Range(COleVariant(_T("D6")), COleVariant(_T("D6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ���
	strResult.Format(_T("%.0f"), areaSD);
	range = sheet.get_Range(COleVariant(_T("E6")), COleVariant(_T("E6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));


	// ******************�����
	// ���ֱ��
	strResult.Format(_T("%.0f"), maxBigDia);
	range = sheet.get_Range(COleVariant(_T("B7")), COleVariant(_T("B7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ��Сֱ��
	strResult.Format(_T("%.0f"), maxSmallDia);
	range = sheet.get_Range(COleVariant(_T("C7")), COleVariant(_T("C7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// �ܳ�
	strResult.Format(_T("%.0f"), maxLength);
	range = sheet.get_Range(COleVariant(_T("D7")), COleVariant(_T("D7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ���
	strResult.Format(_T("%.0f"), maxArea);
	range = sheet.get_Range(COleVariant(_T("E7")), COleVariant(_T("E7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************����С��
	// ���ֱ��
	strResult.Format(_T("%.0f"), minBigDia);
	range = sheet.get_Range(COleVariant(_T("B8")), COleVariant(_T("B8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ��Сֱ��
	strResult.Format(_T("%.0f"), minSmallDia);
	range = sheet.get_Range(COleVariant(_T("C8")), COleVariant(_T("C8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// �ܳ�
	strResult.Format(_T("%.0f"), minLength);
	range = sheet.get_Range(COleVariant(_T("D8")), COleVariant(_T("D8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ���
	strResult.Format(_T("%.0f"), minArea);
	range = sheet.get_Range(COleVariant(_T("E8")), COleVariant(_T("E8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************���ܼơ�
	// ���ֱ��
	strResult.Format(_T("%.0f"), sumBigDia);
	range = sheet.get_Range(COleVariant(_T("B9")), COleVariant(_T("B9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ��Сֱ��
	strResult.Format(_T("%.0f"), sumSmallDia);
	range = sheet.get_Range(COleVariant(_T("C9")), COleVariant(_T("C9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// �ܳ�
	strResult.Format(_T("%.0f"), sumLength);
	range = sheet.get_Range(COleVariant(_T("D9")), COleVariant(_T("D9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ���
	strResult.Format(_T("%.0f"), sumArea);
	range = sheet.get_Range(COleVariant(_T("E9")), COleVariant(_T("E9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	//3.7  ����������һ��
	// *************************������Ϣ��********************************
	range = sheet.get_Range(COleVariant(_T("A13")), COleVariant(_T("A13")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("������Ϣ��")));

	range = sheet.get_Range(COleVariant(_T("A14")), COleVariant(_T("A14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("No.")));

	range = sheet.get_Range(COleVariant(_T("B14")), COleVariant(_T("B14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���ֱ��/um")));

	range = sheet.get_Range(COleVariant(_T("C14")), COleVariant(_T("C14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("��Сֱ��/um")));

	range = sheet.get_Range(COleVariant(_T("D14")), COleVariant(_T("D14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("�ܳ�/um")));

	range = sheet.get_Range(COleVariant(_T("E14")), COleVariant(_T("E14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���/um2")));


	long irow;
	CRange start_range, write_range;
	CString strValue;
	long sizeVec = vecArea.size();
	for (irow = 1; irow <= sizeVec; irow++)
	{
		// No.
		strValue.Format(_T("%ld"), irow);
		COleVariant new_value0(strValue);
		start_range = sheet.get_Range(COleVariant(_T("A14")), covOptional);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)0));
		write_range.put_Value2(new_value0);
		write_range.put_HorizontalAlignment(val);

		// ���ֱ��/um
		strValue.Format(_T("%.0f"), vecMaxDia[irow - 1]);
		COleVariant new_value1(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)1));
		write_range.put_Value2(new_value1);
		write_range.put_HorizontalAlignment(val);

		// ��Сֱ��/um
		strValue.Format(_T("%.0f"), vecMinDia[irow - 1]);
		COleVariant new_value2(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)2));
		write_range.put_Value2(new_value2);
		write_range.put_HorizontalAlignment(val);

		// �ܳ�/um
		strValue.Format(_T("%.0f"), vecLength[irow - 1]);
		COleVariant new_value3(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)3));
		write_range.put_Value2(new_value3);
		write_range.put_HorizontalAlignment(val);

		// ���/um2
		strValue.Format(_T("%.0f"), vecArea[irow - 1]);
		COleVariant new_value4(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)4));
		write_range.put_Value2(new_value4);
		write_range.put_HorizontalAlignment(val);
	}


	// ֵ
	start_range = sheet.get_Range(COleVariant(_T("A14")), covOptional);
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("ֵ")));

	// ��λ
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("��λ")));


	// �����
	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("�����")));

	strValue.Format(_T("%.0f"), meanArea*valueCount);
	COleVariant new_value_areaSum(strValue);
	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(new_value_areaSum));

	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("um2")));


	//8.18�޸ģ�ע�͵�
	//// ����
	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("����")));

	//strValue.Format(_T("%ld"), valueCount);
	//COleVariant new_value_valueCount(strValue);
	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)1));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(new_value_valueCount));

	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)2));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("��")));

	//// �����
	//write_range = start_range.get_Offset(COleVariant((long)irow + 4), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("�����")));

	//strValue.Format(_T("%.2f"), ratioArea);
	//COleVariant new_value_ratioArea(strValue);
	//write_range = start_range.get_Offset(COleVariant((long)irow + 4), COleVariant((long)1));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(new_value_ratioArea));

	//write_range = start_range.get_Offset(COleVariant((long)irow + 4), COleVariant((long)2));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("%")));

	//// �����������
	//write_range = start_range.get_Offset(COleVariant((long)irow + 5), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("�����������")));

	//strValue.Format(_T("%.0f"), areaStrip);
	//COleVariant new_value_areaStrip(strValue);
	//write_range = start_range.get_Offset(COleVariant((long)irow + 5), COleVariant((long)1));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(new_value_areaStrip));

	//write_range = start_range.get_Offset(COleVariant((long)irow + 5), COleVariant((long)2));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("um2")));


	// �����������
	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("�����������")));

	strValue.Format(_T("%.0f"), areaStrip);
	COleVariant new_value_areaStrip(strValue);
	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(new_value_areaStrip));

	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("um2")));





	//// ���ֱ��

	/*range = sheet.get_Range(COleVariant(_T("B4")), COleVariant(_T("B4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("���ֱ��/um")));*/


	//CString strResult;
	//strResult.Format(_T("%.0f"), meanMaxDia);
	//range = sheet.get_Range(COleVariant(_T("B5")), COleVariant(_T("B5")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(strResult));


	//8.18�޸�
	// ����
	range = sheet.get_Range(COleVariant(_T("A10")), COleVariant(_T("A10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("����/��")));

	strValue.Format(_T("%ld"), valueCount);
	range = sheet.get_Range(COleVariant(_T("B10")), COleVariant(_T("B10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));



	//�����
	range = sheet.get_Range(COleVariant(_T("A11")), COleVariant(_T("A11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("�����/%")));

	strValue.Format(_T("%.2f"), ratioArea);
	range = sheet.get_Range(COleVariant(_T("B11")), COleVariant(_T("B11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));


	////�������90000���������ռ�ܿ�������ı���  3.7
	//range = sheet.get_Range(COleVariant(_T("A12")), COleVariant(_T("A12")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(_T("�¼������/%")));

	//strValue.Format(_T("%.2f"), ratioAreaNew);
	//range = sheet.get_Range(COleVariant(_T("B12")), COleVariant(_T("B12")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(strValue));



	//���ñ߿�
	// range.BorderAround(COleVariant((long)1), (long)2, ((long)-4105), vtMissing);//���ñ߿�

	//�����ʾ
	//app.put_Visible(TRUE);
	book.put_Saved(false);

	//����ʾ��ʾ�Ի���
	app.put_DisplayAlerts(false);



	//����excel
	book.SaveAs(COleVariant(cstrWritePath),
		_variant_t(39),        //07��
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		0,
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		_variant_t(vtMissing),
		_variant_t(vtMissing));




	//��β���ͷ�
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();

	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();

	book.ReleaseDispatch();								   // �ͷ�Workbook����
	books.ReleaseDispatch();							   // �ͷ�Workbooks����
	book.Close(covOptional, covOptional, covOptional);	   // �ر�Workbook����
	books.Close();										   // �ر�Workbooks����

	app.Quit();
	app.ReleaseDispatch();


}


// ----------------------------------[�����д�뵽�ļ�]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[ͼ�����¾���]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[ͼ������ҷֽ���]----------------------------------
/////////////////////////////////////////////////////////////////////////////////


// ----------------------------------[�ͷŷ����ڴ�]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[����������ֵ]----------------------------------
/////////////////////////////////////////////////////////////////////////////////


void CCameraImage::ResetAllIndex()
{
	saveIndex = 1;
	stripIndex = 1;
	frameCountIndex = 0;
	stripShowIndex = 1;
	sampleShowIndex = 1;
}






BOOL CCameraImage::CreateObjects()
{
	CWaitCursor wait;

	// Create acquisition object
	if (m_AcqDevice && !*m_AcqDevice && !m_AcqDevice->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	// Create buffer object
	if (m_Buffers && !*m_Buffers)
	{
		if (!m_Buffers->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers->Clear();
	}


	// Set next empty with trash cycle mode for transfer
	if (m_Xfer && m_Xfer->GetPair(0))
	{
		if (!m_Xfer->GetPair(0)->SetCycleMode(SapXferPair::CycleNextWithTrash))
		{
			DestroyObjects();
			return FALSE;
		}
	}

	// Create transfer object
	if (m_Xfer && !*m_Xfer && !m_Xfer->Create())
	{
		DestroyObjects();
		return FALSE;
	}
}


BOOL CCameraImage::DestroyObjects()
{
	// Destroy transfer object
	if (m_Xfer && *m_Xfer) m_Xfer->Destroy();

	// Destroy buffer object
	if (m_Buffers && *m_Buffers) m_Buffers->Destroy();

	// Destroy acquisition object
	if (m_AcqDevice && *m_AcqDevice) m_AcqDevice->Destroy();

	//if (pRawData)
	//{
	//	delete[] pRawData;
	//}

	return TRUE;
}

BOOL CCameraImage::IfGrabStart() 
{ 
	return m_Xfer&&m_Xfer->IsGrabbing(); 
}



// ���ش�������
CString CCameraImage::Get_Error()
{
	return m_strErrorMsg;
}

// ���������ļ�
BOOL CCameraImage::LoadConfig()
{
	// Destroy all objects
	DestroyObjects();
	if (m_Xfer)
	{
		delete m_Xfer;
		m_Xfer = NULL;
	}
	if (m_Buffers)
	{
		delete m_Buffers;
		m_Buffers = NULL;
	}
	if (m_AcqDevice)
	{
		delete m_AcqDevice;
		m_AcqDevice = NULL;
	}

	CAcqConfigDlg dlg(NULL, CAcqConfigDlg::ServerAcqDevice);
	if (dlg.DoModal() != IDOK)
	{
		m_strErrorMsg = _T("No GigE-Vision cameras found or selecte!");
		return FALSE;
	}

	// Define objects
	m_AcqDevice = new SapAcqDevice(dlg.GetLocation(), dlg.GetConfigFile());

	m_Buffers = new SapBufferWithTrash(2, m_AcqDevice);
	m_Xfer = new SapAcqDeviceToBuf(m_AcqDevice, m_Buffers, XferCallback, this);



	///*
	// Create all objects
	if (!CreateObjects()) 
	{ 
		// EndDialog(TRUE); 
		return FALSE; 
	}
	//*/

	m_bIsLoadConfig = TRUE;

	return TRUE;
}



BOOL CCameraImage::StartGrab()
{
	if (!m_bIsLoadConfig || !m_Xfer)
	{
		m_strErrorMsg = _T("Config file is not loaded or SapTransfer is not created!");
		return FALSE;
	}
	//else
	//	/*612*/

	//	if (pRawData)
	//	{
	//		delete[] pRawData;
	//	}
	//int _width = m_Buffers->GetWidth();
	//int _height = m_Buffers->GetHeight();
	//pRawData = new byte[_width*_height];

	/*612*/
		//m_Buffers->Clear();
		return m_Xfer->Grab();
}


BOOL CCameraImage::StopGrab()
{
	if (!m_Xfer)
		m_strErrorMsg = _T("SapTransfer is not created!");


	if (!m_Xfer->IsGrabbing())    // ���û�вɼ�
		return TRUE;

	if (m_Xfer->Freeze())
	{
		//if (CAbortDlg(pWnd, m_Xfer).DoModal() != IDOK) 
		m_Xfer->Abort();
		if (m_Xfer->Wait(0))
			return TRUE;
	}
	else
	{
		m_strErrorMsg = _T("Freeze error!");
		return FALSE;
	}
	
	return TRUE;
}



// ����ͼ�����
void CCameraImage::SetImagePara(double resolution, int threshold, int Count, int threshold2)  //3.28����
{
	m_resolution = resolution;
	m_DiaThreshold = threshold;
	sampleCountStrip = Count;
	graythreshold = threshold2;
}

// ���ô���ͼ��С�����Լ����һС����֡��
void CCameraImage::SetFrameCount(int count)
{
	// һ����ͼ��֡��
	frameCount = count;
}


// ������ͼ������
void CCameraImage::SetStripCount(int count)
{
	stripCount = count;
}




void CCameraImage::ShowColorStripImage(CDC *pDC,CRect rcDraw)
{

	// ��ʾһ�����ͼ��
	CFile bmpFile;   // ��¼���ļ�
	CString filepathName = childFolderCStr + _T("\\");
	CString strStripShowIndex, strSampleShowIndex;        // 3.26�޸�
	strStripShowIndex.Format(_T("%d"), stripShowIndex);
	strSampleShowIndex.Format(_T("%d"), sampleShowIndex); // 3.26�޸�
	filepathName = filepathName + strStripShowIndex + _T("-") + strSampleShowIndex + _T("-color.BMP");   // 3.26�޸�


	BITMAPFILEHEADER fileHeader;																									 //��ֻ���ķ�ʽ���ļ�  ��ȡbmpͼ������֣�bmp�ļ�ͷ����Ϣ������
	if (!bmpFile.Open(filepathName, CFile::modeRead/* | CFile::typeBinary*/))
		return;
	if (bmpFile.Read(&fileHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
		return;

	if (bmpFile.Read(&bmpInfo, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER))
		return;
	BITMAPINFO btInfo;
	memcpy(&btInfo, &bmpInfo, sizeof(BITMAPINFOHEADER));
	//if (m_pBmpInfo)	delete[]m_pBmpInfo;
	//m_pBmpInfo = (BITMAPINFO *)new char[sizeof(BITMAPINFOHEADER)];
	//memset(m_pBmpInfo, 0, sizeof(BITMAPINFO));
	//Ϊͼ����������ռ�
	//memcpy(m_pBmpInfo, &bmpInfo, sizeof(BITMAPINFOHEADER));

	// ������ͼ��Ŀ��
	

	DWORD dataBytes = fileHeader.bfSize - fileHeader.bfOffBits;
	if (pBmpDataShow)
		delete[]pBmpDataShow;
	pBmpDataShow = (BYTE *) new char[dataBytes];

	bmpFile.Read(pBmpDataShow, dataBytes);
	bmpFile.Close();

	// ��ʾ
	//CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	//CDC *pdc = pwnd->GetDC();
	//CRect srect;
	//pwnd->GetWindowRect(&srect);

	int nHeight = bmpInfo.biHeight;
	int nWidth = bmpInfo.biWidth;   // 3.26�޸�

	SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(),
		rcDraw.left,
		rcDraw.top,
		rcDraw.Width(),
		rcDraw.Height(),
		0,
		0,
		nWidth,     // 3.26�޸�
		nHeight,
		pBmpDataShow,
		&btInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

// ���·�ҳ��ʾͼ��
BOOL CCameraImage::PageShow(int flag)
{
	//3.28
	// ǰһҳ
	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;pwnd->GetClientRect(&srect);
	if (flag == -1)
	{
		if (stripShowIndex <= 1 && sampleShowIndex <= 1)  //1-1
		{
			AfxMessageBox(_T("�Ѿ��ǵ�һ��ͼƬ��"));
			return FALSE;
		}
		else
		{
			if (sampleShowIndex > 1)  //����2-2
			{
				sampleShowIndex--;
				// ��ʾ
				ShowColorStripImage(pdc, srect);
				pwnd->ReleaseDC(pdc);
				return TRUE;
			}
			else
			{
				if (stripShowIndex > 1)  //����2-1
				{
					stripShowIndex--;
					sampleShowIndex = sampleCountStrip;
					ShowColorStripImage(pdc, srect);
					pwnd->ReleaseDC(pdc);
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
	}
	// ��һҳ
	else if (flag == 1)
	{
		if (stripShowIndex >= stripCount && sampleShowIndex >= sampleCountStrip)
		{
			AfxMessageBox(_T("�Ѿ������һ��ͼƬ��"));
			return FALSE;
		}
		else
		{
			if (sampleShowIndex < sampleCountStrip) // ���������·�  2-1
			{

				sampleShowIndex++;
				// ��ʾ
				ShowColorStripImage(pdc, srect);
				pwnd->ReleaseDC(pdc);
				return TRUE;
			}
			else     // 3.24����һ����ҳ   1-2
			{
				if (stripShowIndex < stripCount)
				{
					stripShowIndex++;
					sampleShowIndex = 1;
					ShowColorStripImage(pdc, srect);
					pwnd->ReleaseDC(pdc);
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}
	}
}


// ��������ʾͼ��

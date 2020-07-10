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

//3.31修改
//int staticCount = 1;

using namespace cv;

CCameraImage::CCameraImage()
{
	saveIndex = 1;  // 保存一大条索引
	stripIndex = 1; // 图像大条索引
	stripShowIndex = 1;  // 大条图像显示索引
	stripCount = 1; // 图像大条数量
	frameCount = 1; // 一大条图像帧数
	frameCountIndex = 0;  // 一大条图像索引

	m_threshold = 120;  // 这个初始化阈值是随便取的
	//3.29晚
	m_DiaThreshold = 100;   // 这个过滤粒径初始化阈值是随便取的

	//4.10
	graythreshold = 36;

	//3.28
	sampleCountStrip = 2;        // 每条取样数量   //3.28下午
	sampleShowIndex = 1;         // 取样显示索引


	//int i = rand() % 7 - 3;
	//cropLine = 190 + i;   // 裁减线
	cropLine = 3072;

	//3.28
	

	m_strErrorMsg = _T("");

	m_bIsLoadConfig = FALSE;

	m_area = 0.0;        // 计算平均面积
	m_length = 0.0;      // 计算平均周长
	m_ratioArea = 0.0;   // 计算的面积率
	//areaCount = 0;       // 粒径数量
	areaMax = 0.0;       // 最大面积
	areaMin = 0.0;       // 最小面积

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


	// 检测开始时间初始化
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
			
			//AfxMessageBox(_T("回调函数调用成功！"));

			// pDlg->m_Buffers->SetIndex(1);

			/*
			staticCount++;
			if (staticCount > 6)
			{
				staticCount = 1;
				pDlg->m_Buffers->Clear();
				AfxMessageBox(_T("清除缓存成功！"));
			}
			*/

		}
	}
}



// ----------------------------------[数据转换函数]----------------------------------
// 显示当前帧图像
// 保存一帧图像
// 调用FillData函数
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::TransferData(unsigned char *pData, int nWidth, int nHeight)
{
	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;
	pwnd->GetWindowRect(&srect);

	// -----------------[保存图像]---------------
	// 图像保存参数
	m_pBmpInfo->bmiHeader.biWidth = nWidth;
	m_pBmpInfo->bmiHeader.biHeight = nHeight;
	m_pBmpInfo->bmiHeader.biSizeImage = nWidth*nHeight;

	//第一大条采集完，开始采集第二条
	if ((stripIndex < stripCount) && (saveIndex > frameCount))
	{
		stripIndex++;
		saveIndex = 1;
	}


	//原先不一定每帧都取，现修改成每帧都取，所以gapValue参数没用了,就删除了
	if (saveIndex <= sampleCountStrip)
	{
		// 保存图像路径
		CString CstrPath;
		CString CstrIndex;
		CstrIndex.Format(_T("\\%d-%d"), stripIndex, saveIndex);
		CstrPath = childFolderCStr + CstrIndex;
		CstrPath += ".bmp";

		// 保存每一帧图像
		SaveImage(pData, nWidth, nHeight, CstrPath);
	}
	

	saveIndex++;

	// 显示一帧图像
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

	//AfxMessageBox(_T("间隔取样前"));
	////saveIndex是每采集一帧（执行一次回调函数）加一次
	////6.12修改
	//if (stripIndex >= stripCount && (saveIndex == (frameCount+1)))
	//{ 
	////if (stripIndex >= stripCount && saveIndex > sampleCountStrip)
	//	ImageProcessAccuracy();
	//	ResetAllIndex();
	//
	//}
	
	//AfxMessageBox(_T("间隔取样后"));
	//2019.3.22新加
	pwnd->ReleaseDC(pdc);
	// -------------------[填充图像]------------------
	// FillData(pData, nWidth, nHeight);
}



// ----------------------------------[图像保存]----------------------------------
// 通过传递过来的图像参数、宽、高和路径保存图像
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::SaveImage(unsigned char *pData, int nWidth, int nHeight, CString strPath)
{
	long imgBufSize = nWidth*nHeight;
	// 位图文件头
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


// ----------------------------------[填充数据]----------------------------------
// 将图像以八帧为一小条保存
// 保存第一条的倒数第三张和倒数第一张图像
// 图像扫描结束后调用图像处理函数

/////////////////////////////////////////////////////////////////////////////////


// int OtsuAlgThreshold(Mat image);
int CCameraImage::OtsuAlgThreshold(Mat image)
{
	if (image.channels() != 1)
	{
		// cout << "Please input Gray-image!" << endl;
		return 128;
	}

	int T = 0; //Otsu算法阈值  
	double varValue = 0; //类间方差中间值保存  
	double w0 = 0; //前景像素点数所占比例  
	double w1 = 0; //背景像素点数所占比例  
	double u0 = 0; //前景平均灰度  
	double u1 = 0; //背景平均灰度  
	double Histogram[256] = { 0 }; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	uchar *data = image.data;
	double totalNum = image.rows*image.cols; //像素总数  

											 //计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  
	for (int i = 0; i<image.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
	{
		for (int j = 0; j<image.cols; j++)
		{
			Histogram[data[i*image.step + j]]++;
		}
	}
	for (int i = 0; i<255; i++)
	{
		//每次遍历之前初始化各变量  
		w1 = 0;
		u1 = 0;
		w0 = 0;
		u0 = 0;
		//***********背景各分量值计算**************************  
		for (int j = 0; j <= i; j++) //背景部分各值计算  
		{
			w1 += Histogram[j];  //背景部分像素点总数  
			u1 += j*Histogram[j]; //背景部分像素总灰度和  
		}
		//if (w1 == 0) //背景部分像素点数为0时退出  
		//{
		//	break;
		//}
		u1 = u1 / w1; //背景像素平均灰度  
		w1 = w1 / totalNum; // 背景部分像素点数所占比例  


							//***********前景各分量值计算**************************  
		for (int k = i + 1; k<255; k++)
		{
			w0 += Histogram[k];  //前景部分像素点总数  
			u0 += k*Histogram[k]; //前景部分像素总灰度和  
		}
		if (w0 == 0) //前景部分像素点数为0时退出  
		{
			break;
		}
		u0 = u0 / w0; //前景像素平均灰度  
		w0 = w0 / totalNum; // 前景部分像素点数所占比例  
							//***********前景各分量值计算**************************  

							//***********类间方差计算******************************  
		double varValueI = w0*w1*(u1 - u0)*(u1 - u0); //当前类间方差计算  
		if (varValue<varValueI)
		{
			varValue = varValueI;
			T = i;
			// cout << T << endl;
		}
	}
	return T;
}





// ----------------------------------[之前图像处理]----------------------------------
/////////////////////////////////////////////////////////////////////////////////





void CCameraImage::ImageProcessAccuracy(CWnd *pWnd)
{
	// *************************【进行图像处理】**************************

	for (int i = 1; i <= stripCount; i++)   // 图像大条循环
	{
		Mat srcImg_all;
		for (int j = 1; j <= sampleCountStrip; j++)  // 图像小条循环
		{
			String strPath;
			strPath = childFolderStr + "\\" + to_string(i) + "-" + to_string(j) + ".BMP";

			// 读取图像
			Mat srcImg1 = imread(strPath, 0);  // 源图像

			//图像高度压缩  覆盖原始图像路径
			resize(srcImg1, srcImg1, Size(8192, 2048), 0, 0, INTER_LINEAR);
			cv::imwrite(strPath, srcImg1);

			// 裁减：计算图片大小都是2048*2048
			Mat srcImg = srcImg1(Range(0, srcImg1.rows), Range(cropLine, 8192 - cropLine));

			//保存裁减之后的原图
			string strPathSrc;
			strPathSrc = childFolderStr + "\\" + to_string(i) + "-" + to_string(j) + "-src.BMP";
			cv::imwrite(strPathSrc, srcImg);

			//将图片拼接起来
			if (j == 1)
				srcImg_all = srcImg;
			else
				vconcat(srcImg, srcImg_all, srcImg_all);
		}
		//将大图保存
		String strPath_all;
		strPath_all = childFolderStr + "\\" + to_string(i) + "-unite.BMP";
		cv::imwrite(strPath_all, srcImg_all);

		//// 使用精确计算
		//ComputeAccuracy(srcImg_all, i);

		//// 将结果写入到文件
		//CTime time;
		//time = CTime::GetCurrentTime();
		//CString curData = time.Format(_T("%Y%m%d_%H%M_%S"));

		//CString cstrWriteResultPath;
		//cstrWriteResultPath = "D:\\Det531\\x64\\result\\"; //快捷方式唯一绝对路径!!!
		////cstrWriteResultPath = "D:\\Det531\\result\\"; //调试时快捷方式唯一绝对路径!!!
		//cstrWriteResultPath += startTimeCStr;
		//cstrWriteResultPath += "\\";
		//cstrWriteResultPath += curData;
		//cstrWriteResultPath += ".xls";

		////3.28
		//if (i == 1)
		//	WriteResultToExcelAccuracy(cstrWriteResultPath, TRUE);
		//else
		//	WriteResultToExcelAccuracy(cstrWriteResultPath, FALSE);

		// 设置进度条值
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


	// 结构元
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));

	// 开运算
	morphologyEx(dstImg, dstImg, MORPH_OPEN, element1);
	// 闭运算
	morphologyEx(dstImg, dstImg, MORPH_CLOSE, element2);

	// 轮廓查找
	vector < vector < cv::Point >> contours;
	vector<Vec4i> hierarchy;
	findContours(dstImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	Mat dstImg2 = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);

	// 遍历所有顶层轮廓
	int index = 0;
	Scalar color(0, 0, 255);
	double area, length;
	double areaSum = 0.0, lengthSum = 0.0, areaSumNew = 0.0;  //3.7

	double maxRadius = 0.0;   // 最大粒径
	double minRadius = 0.0;   // 最小粒径
	double radiusMaxSum = 0.0;   // 最大粒径之和
	double radiusMinSum = 0.0;   // 最小粒径之和

	// 最小外接矩形
	double l1 = 0.0;
	double l2 = 0.0;
	double bigL = 0.0;
	double smallL = 0.0;

	// 重置
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

		double l1 = 0.0, l2 = 0.0;   // 最小外接矩形的长宽
		// 最小外接矩形计算
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

		// 最小粒径
		minRadius = smallL;

		if (minRadius >= m_DiaThreshold)
		{
			if (length * m_resolution > 50000)  //阈值50000微米
			{
				cv::drawContours(dstImg2, contours, index, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else if (bigL / smallL > 6)   //最小外接矩形长宽比大于6的判定为褶皱
			{
				cv::drawContours(dstImg2, contours, index, Scalar(0, 255, 0), CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else if (vecArea.size() < 800 * sampleCountStrip)
			{
				double maxLength = 0.0;
				double curLenth = 0.0;
				double fValue1 = 0, fValue2 = 0;
				// 循环精确计算
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

				maxRadius = maxLength * m_resolution;// 得到最大粒径

				valueCount++;
				areaSum += area;
				lengthSum += length;
				radiusMaxSum += maxRadius;
				radiusMinSum += minRadius;

				area = area * m_resolution*m_resolution;
				length = length * m_resolution;

				vecArea.push_back(area);           // 面积
				vecLength.push_back(length);       // 周长
				vecMaxDia.push_back(maxRadius);    // 每个最大粒径
				vecMinDia.push_back(minRadius);    // 每个最小粒径
				cv::drawContours(dstImg2, contours, index, Scalar(0, 0, 255), CV_FILLED, 8, hierarchy);
			}
		}
	}

	// 保存结果图像
	string strPathColor;
	strPathColor = childFolderStr + "\\" + to_string(i) + "-color.BMP";
	cv::imwrite(strPathColor, dstImg2);
	dstImg.deallocate();
	dstImg2.deallocate();
	// ******************【平均】 【面积率】 【区域整体面积】
	meanMaxDia = radiusMaxSum / valueCount;                                 // 平均最大粒径
	meanMinDia = radiusMinSum / valueCount;                                 // 平均最小粒径
	meanArea = areaSum / valueCount * m_resolution * m_resolution;          // 平均面积
	meanLength = lengthSum / valueCount * m_resolution;                     // 平均周长
	areaStrip += srcImg.rows*srcImg.cols;          // 一小条的像素面积
	ratioArea = areaSum / areaStrip * 100;                  // 面积率
	//ratioAreaNew = areaSumNew / (areaSum * m_resolution * m_resolution) * 100;									// 面积大于90000颗粒总面积占总颗粒面积的比例
	areaStrip *= m_resolution*m_resolution;          // 一小条的面积

	// ******************【总计】
	sumBigDia = radiusMaxSum;
	sumSmallDia = radiusMinSum;
	sumLength = lengthSum * m_resolution;
	sumArea = areaSum * m_resolution*m_resolution;

	// *****************【最大】 【最小】 【标准差】
	for (int i = 0; i < valueCount; i++)
	{
		// 循环第一次初始化
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
		// 标准差的计算
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
	//---------------------【将结果显示在列表中】---------------------
	CListCtrl  *pCtrl = static_cast<CListCtrl *>(AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_LIST));

	CString cstrResultToList;
	//// 颗粒数显示
	//cstrResultToList.Format(_T("%ld"), valueCount);
	//pCtrl->SetItemText(0, 1, cstrResultToList);

	//3.28意见
	// 最大粒径显示
	cstrResultToList.Format(_T("%.2f"), maxBigDia);
	pCtrl->SetItemText(0, 1, cstrResultToList);
	// 平均直径显示
	cstrResultToList.Format(_T("%.2f"), meanMaxDia);
	pCtrl->SetItemText(0, 2, cstrResultToList);
	// 面积率的显示
	cstrResultToList.Format(_T("%.6f"), ratioArea);
	pCtrl->SetItemText(0, 3, cstrResultToList);
}

// ----------------------------------[寻找分界线]----------------------------------
/////////////////////////////////////////////////////////////////////////////////




void CCameraImage::ReflashImageDlg()
{
	pBmpDataClear = new unsigned char[100 * 100];
	memset(pBmpDataClear, 240, 100 * 100);

	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;
	pwnd->GetWindowRect(&srect);

	// 图像保存参数
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





// ----------------------------------[图像检测函数]----------------------------------
/////////////////////////////////////////////////////////////////////////////////


// ----------------------------------[得到检测开始时间]----------------------------------
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


// ----------------------------------[新建图片文件夹和子文件夹]----------------------------------
// ----------------------------------[新建检测结果文件夹和子文件夹]----------------------------------
/////////////////////////////////////////////////////////////////////////////////
void CCameraImage::MkdirPic()
{
	// 图片文件夹
	if (_access("..\\image", 0) != 0)  // 图片文件夹不存在
	{
		_mkdir("..\\image");   // 新建图片文件夹
	}

	// 检测结果文件夹
	if (_access("..\\result", 0) != 0)  // 检测结果文件夹存在
	{
		_mkdir("..\\result");  // 新建检测结果文件夹
	}

	// 新建图片子文件夹
	childFolderCStr = _T("..\\image\\") + startTimeCStr;
	USES_CONVERSION;
	childFolderP = W2A(childFolderCStr);
	childFolderStr = CW2A(childFolderCStr.GetString());
	_mkdir(childFolderP);

	// 新建检测结果子文件夹
	childFolderExcelCStr = _T("..\\result\\") + startTimeCStr;
	char *excelP = W2A(childFolderExcelCStr);
	_mkdir(excelP);


}


void CCameraImage::WriteResultToExcelAccuracy(CString cstrWritePath, BOOL flagFirst)
{
	// 初始化组件
	//3.28
	if (flagFirst && (detectCount == 1))
	{
		//if (CoInitialize(NULL) != NULL)
		//if (SUCCEEDED(CoInitialize(NULL)))
		/*if (CoInitialize(NULL) == 0)
		{
			AfxMessageBox(_T("初始化COM失败"));
		}*/

		CoUninitialize();
		if (CoInitialize(NULL) == S_FALSE)
		{
			AfxMessageBox(_T("初始化COM支持库失败！"));
		}


		/*if (!AfxOleInit())
		{
			AfxMessageBox(_T("无法初始化COM的动态连接库"));
		}*/

	}

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		AfxMessageBox(_T("无法创建Excel应用！"));
	}

	books = app.get_Workbooks();
	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));

	//居中对齐方式
	val.vt = VT_I2;
	val.iVal = -4108;




	// *****************************【表头】********************************
	//获得坐标为（A，1）和（D，1）的两个单元格 
	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("E1")));
	range.put_RowHeight(_variant_t((long)50));  //设置列宽
	range.Merge(_variant_t((long)0));//合并单元格

									 //居中对齐
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);

	//设置单元格内容
	range.put_Value2(COleVariant(_T("结果统计报告")));

	//设置字体粗体
	font = range.get_Font();
	font.put_Bold(COleVariant((short)TRUE));
	font.put_Name(_variant_t("宋体"));
	font.put_Size(_variant_t(30));


	// *************************【保存时间】********************************
	range = sheet.get_Range(COleVariant(_T("A2")), COleVariant(_T("A2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("保存时间：")));

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


	// *************************【统计结果】********************************
	range = sheet.get_Range(COleVariant(_T("A3")), COleVariant(_T("A3")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("【统计】")));

	range = sheet.get_Range(COleVariant(_T("B4")), COleVariant(_T("B4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大直径/um")));

	range = sheet.get_Range(COleVariant(_T("C4")), COleVariant(_T("C4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小直径/um")));

	range = sheet.get_Range(COleVariant(_T("D4")), COleVariant(_T("D4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("周长/um")));

	range = sheet.get_Range(COleVariant(_T("E4")), COleVariant(_T("E4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积/um2")));

	range = sheet.get_Range(COleVariant(_T("A5")), COleVariant(_T("A5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("平均")));

	range = sheet.get_Range(COleVariant(_T("A6")), COleVariant(_T("A6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("标准差")));

	range = sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("A7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大")));

	range = sheet.get_Range(COleVariant(_T("A8")), COleVariant(_T("A8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小")));

	range = sheet.get_Range(COleVariant(_T("A9")), COleVariant(_T("A9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("总计")));



	// ******************【平均】
	// 最大直径
	CString strResult;
	strResult.Format(_T("%.0f"), meanMaxDia);
	range = sheet.get_Range(COleVariant(_T("B5")), COleVariant(_T("B5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), meanMinDia);
	range = sheet.get_Range(COleVariant(_T("C5")), COleVariant(_T("C5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), meanLength);
	range = sheet.get_Range(COleVariant(_T("D5")), COleVariant(_T("D5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), meanArea);
	range = sheet.get_Range(COleVariant(_T("E5")), COleVariant(_T("E5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【标准差】
	// 最大直径
	strResult.Format(_T("%.0f"), maxDiaSD);
	range = sheet.get_Range(COleVariant(_T("B6")), COleVariant(_T("B6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), minDiaSD);
	range = sheet.get_Range(COleVariant(_T("C6")), COleVariant(_T("C6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), lengthSD);
	range = sheet.get_Range(COleVariant(_T("D6")), COleVariant(_T("D6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), areaSD);
	range = sheet.get_Range(COleVariant(_T("E6")), COleVariant(_T("E6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));


	// ******************【最大】
	// 最大直径
	strResult.Format(_T("%.0f"), maxBigDia);
	range = sheet.get_Range(COleVariant(_T("B7")), COleVariant(_T("B7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), maxSmallDia);
	range = sheet.get_Range(COleVariant(_T("C7")), COleVariant(_T("C7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), maxLength);
	range = sheet.get_Range(COleVariant(_T("D7")), COleVariant(_T("D7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), maxArea);
	range = sheet.get_Range(COleVariant(_T("E7")), COleVariant(_T("E7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【最小】
	// 最大直径
	strResult.Format(_T("%.0f"), minBigDia);
	range = sheet.get_Range(COleVariant(_T("B8")), COleVariant(_T("B8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), minSmallDia);
	range = sheet.get_Range(COleVariant(_T("C8")), COleVariant(_T("C8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), minLength);
	range = sheet.get_Range(COleVariant(_T("D8")), COleVariant(_T("D8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), minArea);
	range = sheet.get_Range(COleVariant(_T("E8")), COleVariant(_T("E8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【总计】
	// 最大直径
	strResult.Format(_T("%.0f"), sumBigDia);
	range = sheet.get_Range(COleVariant(_T("B9")), COleVariant(_T("B9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), sumSmallDia);
	range = sheet.get_Range(COleVariant(_T("C9")), COleVariant(_T("C9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), sumLength);
	range = sheet.get_Range(COleVariant(_T("D9")), COleVariant(_T("D9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), sumArea);
	range = sheet.get_Range(COleVariant(_T("E9")), COleVariant(_T("E9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	//3.7  依次往后移一列
	// *************************【主信息】********************************
	range = sheet.get_Range(COleVariant(_T("A13")), COleVariant(_T("A13")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("【主信息】")));

	range = sheet.get_Range(COleVariant(_T("A14")), COleVariant(_T("A14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("No.")));

	range = sheet.get_Range(COleVariant(_T("B14")), COleVariant(_T("B14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大直径/um")));

	range = sheet.get_Range(COleVariant(_T("C14")), COleVariant(_T("C14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小直径/um")));

	range = sheet.get_Range(COleVariant(_T("D14")), COleVariant(_T("D14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("周长/um")));

	range = sheet.get_Range(COleVariant(_T("E14")), COleVariant(_T("E14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积/um2")));


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

		// 最大直径/um
		strValue.Format(_T("%.0f"), vecMaxDia[irow - 1]);
		COleVariant new_value1(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)1));
		write_range.put_Value2(new_value1);
		write_range.put_HorizontalAlignment(val);

		// 最小直径/um
		strValue.Format(_T("%.0f"), vecMinDia[irow - 1]);
		COleVariant new_value2(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)2));
		write_range.put_Value2(new_value2);
		write_range.put_HorizontalAlignment(val);

		// 周长/um
		strValue.Format(_T("%.0f"), vecLength[irow - 1]);
		COleVariant new_value3(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)3));
		write_range.put_Value2(new_value3);
		write_range.put_HorizontalAlignment(val);

		// 面积/um2
		strValue.Format(_T("%.0f"), vecArea[irow - 1]);
		COleVariant new_value4(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)4));
		write_range.put_Value2(new_value4);
		write_range.put_HorizontalAlignment(val);
	}


	// 值
	start_range = sheet.get_Range(COleVariant(_T("A14")), covOptional);
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("值")));

	// 单位
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("单位")));


	// 总面积
	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("总面积")));

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


	//8.18修改，注释掉
	//// 计数
	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("计数")));

	//strValue.Format(_T("%ld"), valueCount);
	//COleVariant new_value_valueCount(strValue);
	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)1));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(new_value_valueCount));

	//write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)2));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("个")));

	//// 面积率
	//write_range = start_range.get_Offset(COleVariant((long)irow + 4), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("面积率")));

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

	//// 区域整体面积
	//write_range = start_range.get_Offset(COleVariant((long)irow + 5), COleVariant((long)0));
	//write_range.put_ColumnWidth(_variant_t((long)12));
	//write_range.put_HorizontalAlignment(val);
	//write_range.put_Value2(COleVariant(_T("区域整体面积")));

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


	// 区域整体面积
	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("区域整体面积")));

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





	//// 最大直径

	/*range = sheet.get_Range(COleVariant(_T("B4")), COleVariant(_T("B4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大直径/um")));*/


	//CString strResult;
	//strResult.Format(_T("%.0f"), meanMaxDia);
	//range = sheet.get_Range(COleVariant(_T("B5")), COleVariant(_T("B5")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(strResult));


	//8.18修改
	// 计数
	range = sheet.get_Range(COleVariant(_T("A10")), COleVariant(_T("A10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("计数/个")));

	strValue.Format(_T("%ld"), valueCount);
	range = sheet.get_Range(COleVariant(_T("B10")), COleVariant(_T("B10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));



	//面积率
	range = sheet.get_Range(COleVariant(_T("A11")), COleVariant(_T("A11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积率/%")));

	strValue.Format(_T("%.2f"), ratioArea);
	range = sheet.get_Range(COleVariant(_T("B11")), COleVariant(_T("B11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));


	////面积大于90000颗粒总面积占总颗粒面积的比例  3.7
	//range = sheet.get_Range(COleVariant(_T("A12")), COleVariant(_T("A12")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(_T("新加面积率/%")));

	//strValue.Format(_T("%.2f"), ratioAreaNew);
	//range = sheet.get_Range(COleVariant(_T("B12")), COleVariant(_T("B12")));
	//range.put_ColumnWidth(_variant_t((long)12));
	//range.put_HorizontalAlignment(val);
	//range.put_VerticalAlignment(val);
	//range.put_Value2(COleVariant(strValue));



	//设置边框
	// range.BorderAround(COleVariant((long)1), (long)2, ((long)-4105), vtMissing);//设置边框

	//表格显示
	//app.put_Visible(TRUE);
	book.put_Saved(false);

	//不显示提示对话框
	app.put_DisplayAlerts(false);



	//保存excel
	book.SaveAs(COleVariant(cstrWritePath),
		_variant_t(39),        //07版
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




	//结尾，释放
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();

	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();

	book.ReleaseDispatch();								   // 释放Workbook对象
	books.ReleaseDispatch();							   // 释放Workbooks对象
	book.Close(covOptional, covOptional, covOptional);	   // 关闭Workbook对象
	books.Close();										   // 关闭Workbooks对象

	app.Quit();
	app.ReleaseDispatch();


}


// ----------------------------------[将结果写入到文件]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[图像上下镜像]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[图像相减找分界线]----------------------------------
/////////////////////////////////////////////////////////////////////////////////


// ----------------------------------[释放分配内存]----------------------------------
/////////////////////////////////////////////////////////////////////////////////



// ----------------------------------[重置索引的值]----------------------------------
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



// 返回错误类型
CString CCameraImage::Get_Error()
{
	return m_strErrorMsg;
}

// 加载配置文件
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


	if (!m_Xfer->IsGrabbing())    // 相机没有采集
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



// 设置图像参数
void CCameraImage::SetImagePara(double resolution, int threshold, int Count, int threshold2)  //3.28下午
{
	m_resolution = resolution;
	m_DiaThreshold = threshold;
	sampleCountStrip = Count;
	graythreshold = threshold2;
}

// 设置大条图像小条数以及最后一小条的帧数
void CCameraImage::SetFrameCount(int count)
{
	// 一大条图像帧数
	frameCount = count;
}


// 设置条图像数量
void CCameraImage::SetStripCount(int count)
{
	stripCount = count;
}




void CCameraImage::ShowColorStripImage(CDC *pDC,CRect rcDraw)
{

	// 显示一条结果图像
	CFile bmpFile;   // 记录打开文件
	CString filepathName = childFolderCStr + _T("\\");
	CString strStripShowIndex, strSampleShowIndex;        // 3.26修改
	strStripShowIndex.Format(_T("%d"), stripShowIndex);
	strSampleShowIndex.Format(_T("%d"), sampleShowIndex); // 3.26修改
	filepathName = filepathName + strStripShowIndex + _T("-") + strSampleShowIndex + _T("-color.BMP");   // 3.26修改


	BITMAPFILEHEADER fileHeader;																									 //以只读的方式打开文件  读取bmp图像各部分，bmp文件头，信息，数据
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
	//为图像数据申请空间
	//memcpy(m_pBmpInfo, &bmpInfo, sizeof(BITMAPINFOHEADER));

	// 保存条图像的宽高
	

	DWORD dataBytes = fileHeader.bfSize - fileHeader.bfOffBits;
	if (pBmpDataShow)
		delete[]pBmpDataShow;
	pBmpDataShow = (BYTE *) new char[dataBytes];

	bmpFile.Read(pBmpDataShow, dataBytes);
	bmpFile.Close();

	// 显示
	//CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	//CDC *pdc = pwnd->GetDC();
	//CRect srect;
	//pwnd->GetWindowRect(&srect);

	int nHeight = bmpInfo.biHeight;
	int nWidth = bmpInfo.biWidth;   // 3.26修改

	SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(),
		rcDraw.left,
		rcDraw.top,
		rcDraw.Width(),
		rcDraw.Height(),
		0,
		0,
		nWidth,     // 3.26修改
		nHeight,
		pBmpDataShow,
		&btInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

// 上下翻页显示图像
BOOL CCameraImage::PageShow(int flag)
{
	//3.28
	// 前一页
	CWnd *pwnd = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	CDC *pdc = pwnd->GetDC();
	CRect srect;pwnd->GetClientRect(&srect);
	if (flag == -1)
	{
		if (stripShowIndex <= 1 && sampleShowIndex <= 1)  //1-1
		{
			AfxMessageBox(_T("已经是第一张图片！"));
			return FALSE;
		}
		else
		{
			if (sampleShowIndex > 1)  //例如2-2
			{
				sampleShowIndex--;
				// 显示
				ShowColorStripImage(pdc, srect);
				pwnd->ReleaseDC(pdc);
				return TRUE;
			}
			else
			{
				if (stripShowIndex > 1)  //例如2-1
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
	// 后一页
	else if (flag == 1)
	{
		if (stripShowIndex >= stripCount && sampleShowIndex >= sampleCountStrip)
		{
			AfxMessageBox(_T("已经是最后一张图片！"));
			return FALSE;
		}
		else
		{
			if (sampleShowIndex < sampleCountStrip) // 本条内往下翻  2-1
			{

				sampleShowIndex++;
				// 显示
				ShowColorStripImage(pdc, srect);
				pwnd->ReleaseDC(pdc);
				return TRUE;
			}
			else     // 3.24往下一条翻页   1-2
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


// 滚动条显示图像

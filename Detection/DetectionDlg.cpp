
// DetectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Detection.h"
#include "DetectionDlg.h"
#include "afxdialogex.h"
#include "CameraImage.h"

#include "MPC2810.h"

#include<io.h>
#include<direct.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CDetectionDlg 对话框


// 构造函数
CDetectionDlg::CDetectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDetectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_GUI_ICON);

	// GetClientRect(&m_minDlgRect);

	m_DlgRect.SetRect(0, 0, 0, 0);

	m_sSpeedCamera = 0;
	m_sDistanceCamera = 0;
	m_sSpeedPlatform = 0;
	m_sDistancePlatform = 0;

	changeratiodlg = double(5.378/5);
	heightnew = 2203; //2048*5.378/5四舍五入为2203

	m_IsCameraConnected = FALSE;

	strInfo = _T("");

	
	detectCountDlg = 0;

	stopdetectionflag = TRUE;

	// m_IsMotionParaSet = FALSE;

}

// 析构函数
CDetectionDlg::~CDetectionDlg()
{
	m_camImg.DestroyObjects();
}


void CDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME_HMS, m_time_hms);
	DDX_Control(pDX, IDC_TIME_DAY, m_time_weekDay);
	DDX_Control(pDX, IDC_BTN_MOTION_PARA, m_buttonMotion);
	
	DDX_Control(pDX, IDC_BTN_OPEN_PICTURE, m_buttonOpenPicture);
	DDX_Control(pDX, IDC_BTN_OPEN_RESULT, m_buttonOpenResult);
	DDX_Control(pDX, IDC_LABEL_MOTION_ADJUST, m_label_motionAdjust);
	DDX_Control(pDX, IDC_LABEL_OPEN_PICTURE, m_label_openPicture);
	DDX_Control(pDX, IDC_LABEL_OPEN_RESULT, m_label_openResult);
	DDX_Control(pDX, IDC_LABEL_IMAGE_PARA, m_label_imagePara);
	DDX_Control(pDX, IDC_LABEL_MOTION_PARA, m_label_motionPara);
	DDX_Control(pDX, IDC_LABEL_STATISTIC, m_label_statistic);
	DDX_Control(pDX, IDC_BTN_EXIT, m_buttonExit);
	DDX_Control(pDX, IDC_BTN_START_DETECTION, m_buttonStartDetection);
	DDX_Control(pDX, IDC_BTN_STOP_DETECTION, m_buttonStopDetection);
	DDX_Control(pDX, IDC_BTN_MOTION_ADJUST, m_buttonMotionAdjust);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_EDIT_INFORMATION_OUT, m_infoOut);
	DDX_Control(pDX, IDC_SCROLLBAR_HORIZON, m_horizon_scrollbar);
	DDX_Control(pDX, IDC_SPIN, m_spin);
	DDX_Text(pDX, IDC_STATIC_PAGEUP_VALUE, m_pageUp_value);
	DDX_Text(pDX, IDC_STATIC_PAGEDOWM_VALUE, m_pageDowm_value);
	DDX_Control(pDX, IDC_PROGRESS_IMAGE, m_progressImage);
	DDX_Control(pDX, IDC_BTN_LOAD_TEST, m_buttonLoad);
	DDX_Control(pDX, IDC_LABEL_LOAD_CONFIG, m_label_loadConfig);
}

BEGIN_MESSAGE_MAP(CDetectionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_OPEN_PICTURE, &CDetectionDlg::OnBnClickedBtnOpenPicture)
	ON_BN_CLICKED(IDC_BTN_OPEN_RESULT, &CDetectionDlg::OnBnClickedBtnOpenResult)
	ON_BN_CLICKED(IDC_BTN_MOTION_PARA, &CDetectionDlg::OnBnClickedBtnMotionPara)
	ON_WM_LBUTTONDOWN()
	
	ON_BN_CLICKED(IDC_BTN_START_DETECTION, &CDetectionDlg::OnBnClickedBtnStartDetection)
	ON_BN_CLICKED(IDC_BTN_MOTION_ADJUST, &CDetectionDlg::OnBnClickedBtnMotionAdjust)
	ON_BN_CLICKED(IDC_BTN_LOAD_TEST, &CDetectionDlg::OnBnClickedBtnLoadTest)
	
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDetectionDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_STOP_DETECTION, &CDetectionDlg::OnBnClickedBtnStopDetection)
	ON_COMMAND(ID_MODIFY_SYSTEM_PARA, &CDetectionDlg::OnModifySystemPara)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, &CDetectionDlg::OnDeltaposSpin)
	ON_MESSAGE(WM_MOTORSTOP,&CDetectionDlg::OnMotorStop)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDetectionDlg 消息处理程序

BOOL CDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	// 获取系统参数
	GetSystemPara();

	// 初始化运动控制卡
	InitBoard();


	// 按钮状态设置
	//GetDlgItem(IDC_BTN_GRAB_TEST)->EnableWindow(0);
	//GetDlgItem(IDC_BTN_FREEZE_TEST)->EnableWindow(0);

	// ----------------------------【显示LOGO】---------------------------
	HBITMAP hBitmap;
	//关联图片ID  
	hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDB_BITMAP_LOGO),
		IMAGE_BITMAP, 0, 0,
		LR_LOADMAP3DCOLORS);
	//获得指向静态控件的指针    
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LOGO);//控件ID     
	//设置静态控件的样式，使其可以使用位图，并试位标显示使居中    
	pStatic->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);
	//设置静态控件显示位图    
	pStatic->SetBitmap(hBitmap);

	// ----------------------------【左边图片按钮】---------------------------
	// 相机文件载入
	HINSTANCE hInstance;
	HBITMAP hBitmapLoad;
	hInstance = ::AfxGetInstanceHandle();
	hBitmapLoad = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_LOAD));
	m_buttonLoad.SetBitmap(hBitmapLoad);

	// 运动参数设置
	HBITMAP hBitmapMotion;
	hInstance = ::AfxGetInstanceHandle();
	hBitmapMotion = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_MOTION_PARA));
	m_buttonMotion.SetBitmap(hBitmapMotion);

	// 打开图片文件夹
	HBITMAP hBitmapOpenPicture;
	hBitmapOpenPicture = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_OPEN_PICTURE));
	m_buttonOpenPicture.SetBitmap(hBitmapOpenPicture);

	// 打开统计结果文件夹
	HBITMAP hBitmapOpenResult;
	hBitmapOpenResult = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_OPEN_EXCEL));
	m_buttonOpenResult.SetBitmap(hBitmapOpenResult);

	// 手动运动调节
	HBITMAP hBitmapMotionAdjust;
	hBitmapMotionAdjust = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_MOTION_ADJUST));
	m_buttonMotionAdjust.SetBitmap(hBitmapMotionAdjust);


	// ----------------------------【右边图片按钮】---------------------------
	// 开始检测按钮
	HBITMAP hBitmapStart;
	hBitmapStart = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
	m_buttonStartDetection.SetBitmap(hBitmapStart);

	// 停止检测按钮
	HBITMAP hBitmapStop;
	hBitmapStop = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_STOP));
	m_buttonStopDetection.SetBitmap(hBitmapStop);

	// 退出系统按钮
	HBITMAP hBitmapExit;
	hBitmapExit = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_EXIT));
	m_buttonExit.SetBitmap(hBitmapExit);




	// ----------------------【时间设置】------------------------
	// 星期年月日字体
	CFont* font_time_weekDay;
	font_time_weekDay = m_time_weekDay.GetFont();//获取CFont对象
	LOGFONT lf_TimeWeekDay;
	font_time_weekDay->GetLogFont(&lf_TimeWeekDay);//获取LOGFONT结构体
	lf_TimeWeekDay.lfHeight = 26;    //修改字体大小
	lf_TimeWeekDay.lfWidth = 8;     // 修改字体宽度
	lf_TimeWeekDay.lfItalic = FALSE;        //倾斜
	lf_TimeWeekDay.lfWeight = 560;   //修改字体的粗细
	m_font_time_weekDay.CreateFontIndirectW(&lf_TimeWeekDay);//创建一个新的字体
	m_time_weekDay.SetFont(&m_font_time_weekDay);
	
	// 时分秒
	CFont* font_time_hms;
	font_time_hms = m_time_hms.GetFont();//获取CFont对象
	LOGFONT lf_TimeHMS;
	font_time_hms->GetLogFont(&lf_TimeHMS);//获取LOGFONT结构体
	lf_TimeHMS.lfHeight = 36;    //修改字体大小
	lf_TimeWeekDay.lfWidth = 8; // 修改字体宽度
	lf_TimeHMS.lfItalic = FALSE;        //倾斜
	lf_TimeHMS.lfWeight = 560;   //修改字体的粗细
	m_font_time_hms.CreateFontIndirectW(&lf_TimeHMS);//创建一个新的字体
	m_time_hms.SetFont(&m_font_time_hms);

	// ----------------------【时间更新】------------------------
	SetTimer(1, 1000, NULL);

	// ----------------【设置左边按钮标签字体】------------------
	CFont* font_button_label;
	font_button_label = m_label_motionPara.GetFont();//获取CFont对象
	LOGFONT lf_button_label;
	font_button_label->GetLogFont(&lf_button_label);//获取LOGFONT结构体
	lf_button_label.lfHeight = 14;    //修改字体大小
	// lf_button_label.lfWidth = 8; // 修改字体宽度
	lf_button_label.lfItalic = FALSE;        //倾斜
	lf_button_label.lfWeight = 560;   //修改字体的粗细
	m_font_button_label.CreateFontIndirectW(&lf_button_label);//创建一个新的字体

	m_label_loadConfig.SetFont(&m_font_button_label);
	m_label_motionPara .SetFont(&m_font_button_label);
	// m_label_imagePara.SetFont(&m_font_button_label);
	// m_label_statistic.SetFont(&m_font_button_label);
	m_label_openPicture.SetFont(&m_font_button_label);
	m_label_openResult.SetFont(&m_font_button_label);
	m_label_motionAdjust.SetFont(&m_font_button_label);


	// ----------------------【list设置】-----------------------
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;   //鼠标选中某行使整行高亮（只适用与属性为report 风格的listctrl ）
	dwStyle |= LVS_EX_GRIDLINES;       // 列表有网格线（只适用与report 风格的listctrl ）
	m_list.SetExtendedStyle(dwStyle);  // 设置扩展风格

	// 行高
	CImageList m_l;
	m_l.Create(1, 30, TRUE | ILC_COLOR32, 1, 0);
	m_list.SetImageList(&m_l, LVSIL_SMALL);
	// 字号以及字体
	/*
	CFont m_font;
	m_font.CreatePointFont(100, _T("新宋体"));
	m_list.SetFont(&m_font);
	*/
	// 列标题
	int nIndex = 0;
	m_list.InsertColumn(nIndex++, _T(""), LVCFMT_CENTER, 68);
	//m_list.InsertColumn(nIndex++, _T("颗粒数/个"), LVCFMT_CENTER, 118);
	m_list.InsertColumn(nIndex++, _T("最大粒径/um"), LVCFMT_CENTER, 118);  // 3.28意见
	m_list.InsertColumn(nIndex++, _T("平均直径/um"), LVCFMT_CENTER, 118);
	// m_list.InsertColumn(nIndex++, _T("颗粒总面积/um2"), LVCFMT_CENTER, 112);
	m_list.InsertColumn(nIndex++, _T("面积率/%"), LVCFMT_CENTER, 118);
	// 行标题
	m_list.InsertItem(1, _T("值"));
	// m_list.InsertItem(2, _T("标准差"));
	// m_list.InsertItem(3, _T("最大"));
	// m_list.InsertItem(4, _T("最小"));
	// m_list.InsertItem(5, _T("总计"));


	// 编辑框信息输出字体修改
	CEdit *m_editInfo = (CEdit*)GetDlgItem(IDC_EDIT_INFORMATION_OUT);
	fontEditInfo.CreatePointFont(100, _T("新宋体"));
	m_editInfo->SetFont(&fontEditInfo);

	// 图像处理进度条
	m_progressImage.SetRange(0, 100);
	// m_progressImage.SetPos(60);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}




void CDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 设置Group Box的背景色
		CRect rectLeft, rectRight;
		GetDlgItem(IDC_GROUP_LEFT_BUTTON)->GetWindowRect(&rectLeft);
		GetDlgItem(IDC_GROUP_RIGHT_BUTTON)->GetWindowRect(&rectRight);
		ScreenToClient(&rectLeft);
		ScreenToClient(&rectRight);

		//CDC* pDC;
		//pDC = GetDC();
		CPaintDC dc(this);
		CDC* pDC = &dc;
		CBrush *pBrushLeft, *pBrushRight;
		pBrushLeft = new CBrush(RGB(200, 255, 255));
		pBrushRight = new CBrush(RGB(245, 245, 200));
		pDC->FillRect(&rectLeft, pBrushLeft);
		pDC->FillRect(&rectRight, pBrushRight);
		pDC->SetBkMode(OPAQUE);

		delete pBrushLeft;
		delete pBrushRight;

		//CDialogEx::OnPaint();
	}

	// 在显示图区域周围画边框线
	CRect rect;
	GetDlgItem(IDC_STATIC_SHOW_IMAGE)->GetWindowRect(&rect);
	ScreenToClient(rect);

	int left, right, top, bottom;
	left = rect.left;
	right = rect.right;
	top = rect.top;
	bottom = rect.bottom;


	CClientDC dc(this);
	CPen pen(PS_SOLID, 3, RGB(0, 255, 0));
	CPen *pOpen = dc.SelectObject(&pen);

	// 偏移量
	int offset = 3;
	// 左
	dc.MoveTo(left - offset, top - offset);
	dc.LineTo(left - offset, bottom + offset);

	// 上
	dc.MoveTo(left - offset, top - offset);
	dc.LineTo(right + offset, top - offset);

	// 右
	dc.MoveTo(right + offset, top - offset);
	dc.LineTo(right + offset, bottom + offset);

	// 下
	dc.MoveTo(right + offset, bottom + offset);
	dc.LineTo(left - offset, bottom + offset);

	dc.SelectObject(pOpen);
	dc.SelectObject(pOpen);

	//------------------------------------------
	CRect rcDraw;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_SHOW_IMAGE);
	pWnd->GetWindowRect(&rcDraw);
	ScreenToClient(rcDraw);
	rcDraw.InflateRect(1, 1, 1, 1);
	m_camImg.ShowColorStripImage(&dc, rcDraw);

}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDetectionDlg::repaint(UINT id, int last_width, int now_width, int last_height, int now_height)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("相应控件不存在"));
	}
	wnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.left = (long)((double)rect.left / (double)last_width*(double)now_width);
	rect.right = (long)((double)rect.right / (double)last_width*(double)now_width);
	rect.top = (long)((double)rect.top / (double)last_height*(double)now_height);
	rect.bottom = (long)((double)rect.bottom / (double)last_height*(double)now_height);
	// wnd->MoveWindow(&rect);

	wnd->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


// --------------------------【重绘LOGO】----------------------------
void CDetectionDlg::repaintLogo(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("相应控件不存在"));
	}
	wnd->GetWindowRect(&rect);

	ScreenToClient(&rect);
	// rect.left = (long)((double)now_rect.right - (double)rect.Width() - 20);
	rect.left = (long)((double)now_rect.right - (double)rect.Width() + last_rect.right - rect.left);
	rect.right = 0;
	rect.top = (long)((double)rect.top / (double)last_rect.Height()*(double)now_rect.Height());
	rect.bottom = 0;
	// wnd->MoveWindow(&rect);
	wnd->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


// --------------------------【重绘左边按钮】----------------------------
void CDetectionDlg::repaintLeftBTN(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("相应控件不存在"));
	}
	wnd->GetWindowRect(&rect);

	ScreenToClient(&rect);
	rect.left = (long)((double)rect.left);
	rect.right = 0;
	rect.top = (long)((double)rect.top / (double)last_rect.Height()*(double)now_rect.Height());
	// rect.top = (long)((double)rect.top);
	rect.bottom = 0;
	// wnd->MoveWindow(&rect);
	wnd->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

// --------------------------【重绘右边按钮】----------------------------
void CDetectionDlg::repaintRightBTN(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("相应控件不存在"));
	}
	wnd->GetWindowRect(&rect);

	ScreenToClient(&rect);
	rect.left = (long)((double)now_rect.right - (double)rect.Width() + last_rect.right - rect.left);
	rect.right = 0;
	rect.top = (long)((double)rect.top / (double)last_rect.Height()*(double)now_rect.Height());
	rect.bottom = 0;
	// wnd->MoveWindow(&rect);
	wnd->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}



void CDetectionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	// 星期和年月日
	// char weekday[7] = { '日','一', '二', '三', '四', '五', '六' };
	CTime t_weekday = CTime::GetCurrentTime();

	/*
	int weekdayNum = (int)_ttof(t_weekday.Format("%w"));
	CString weekdayStr;
	weekdayStr.Format(_T("%s"), weekday[weekdayNum]);
	*/
	CString str_weekday =  t_weekday.Format("星期%w     %Y.%m.%d");
	SetDlgItemText(IDC_TIME_WEEKDAY, str_weekday);


	// 时分秒
	CTime t_hms = CTime::GetCurrentTime();
	CString str_hms = t_hms.Format("%H:%M:%S");
	SetDlgItemText(IDC_TIME_HMS, str_hms);


	CDialogEx::OnTimer(nIDEvent);
}


HBRUSH CDetectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// 时分秒字体背景颜色
	if (pWnd->GetDlgCtrlID() == IDC_LABEL_MOTION_PARA || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_IMAGE_PARA || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_STATISTIC || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_OPEN_PICTURE || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_OPEN_RESULT || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_MOTION_ADJUST ||
		pWnd->GetDlgCtrlID() == IDC_LABEL_LOAD_CONFIG)
	{
		// pDC->SetTextColor(RGB(0, 255, 0));       // 设置文本颜色
		pDC->SetBkMode(OPAQUE);                  // 设置文本背景模式为透明
		pDC->SetBkColor(RGB(187, 255, 255));           // 设置文本背景颜色
		hbr = CreateSolidBrush(RGB(200, 255, 255));    // 控件背景颜色

		return hbr;
	}


	// 时分秒字体背景颜色
	if (pWnd->GetDlgCtrlID() == IDC_TIME_HMS)
	{
		pDC->SetTextColor(RGB(0, 255, 0));       // 设置文本颜色
		pDC->SetBkMode(OPAQUE);                  // 设置文本背景模式为透明
		pDC->SetBkColor(RGB(0, 0, 0));           // 设置文本背景颜色
		hbr = CreateSolidBrush(RGB(0, 0, 0));    // 控件背景颜色

		return hbr;
	}

	// 星期年月日字体背景更改
	if (pWnd->GetDlgCtrlID() == IDC_TIME_WEEKDAY)
	{
		pDC->SetTextColor(RGB(0, 255, 0));       // 设置文本颜色
		pDC->SetBkMode(OPAQUE);                  // 设置文本背景模式为透明
		pDC->SetBkColor(RGB(0, 0, 0));           // 设置文本背景颜色
		hbr = CreateSolidBrush(RGB(0, 0, 0));    // 控件背景颜色

		return hbr;
	}


	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
	// return m_brush;
}


// -----------【运动控制参数设置对话框】-----------------
void CDetectionDlg::OnBnClickedBtnMotionPara()
{
	// TODO:  在此添加控件通知处理程序代码


	// ----------------【模态对话框】---------------
	dlgMotionPara.DoModal();

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Set motion parameters!\r\n");
	subStr = _T("设置运动参数!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


/*
// -----------【图像处理参数设置对话框】-----------------

*/


// -----------【手动调节参数设置对话框】-----------------
void CDetectionDlg::OnBnClickedBtnMotionAdjust()
{
	// TODO:  在此添加控件通知处理程序代码

	

	dlgMotionAdjust.DoModal();

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Manual adjustment motion parameters!\r\n");
	subStr = _T("手动调节运动参数!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());

}



/*
// ----------------【统计结果曲线】---------------------

*/



// ------------------【打开图片文件夹】-----------------------
void CDetectionDlg::OnBnClickedBtnOpenPicture()
{
	// TODO:  在此添加控件通知处理程序代码
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

	CString path = _T("..\\image");

	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Open the picture folder!\r\n");
	subStr = _T("打开图片文件夹!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


// ----------------【打开统计结果文件夹】---------------------
void CDetectionDlg::OnBnClickedBtnOpenResult()
{
	// TODO:  在此添加控件通知处理程序代码

	// CString path = _T("E:/项目/code/code_12.21/薄膜/code/main/VS2015/Detection/检测结果");
	CString path = _T("..\\result");

	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Open the result file folder!\r\n");
	subStr = _T("打开结果文件夹!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


void CDetectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	// CDialogEx::OnLButtonDown(nFlags, point);
}


// 修改系统参数，获取系统参数
void CDetectionDlg::GetSystemPara()
{
	m_steps_mm = (int)_ttof(dlgModifyPara.m_sPulseEquivalent);
	m_delayTime = (int)_ttof(dlgModifyPara.m_sDelayTime);
	m_resolution = (double)_ttof(dlgModifyPara.m_sResolution);
	m_threshold = (double)_ttof(dlgModifyPara.m_sThreshold);
	sampleCountStrip = (int)_ttof(dlgModifyPara.m_sSampleCountStrip);
	graythreshold = (int)_ttof(dlgModifyPara.m_sGrayThreshold);
}


void CDetectionDlg::OnModifySystemPara()
{
	// TODO: 在此添加命令处理程序代码
	// UpdateData(TRUE);

	// 原先
	// dlgPasswordLogin.DoModal();

	dlgModifyPara.DoModal();

	GetSystemPara();



	// UpdateData(FALSE);


}


void CDetectionDlg::OnBnClickedBtnLoadTest()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_camImg.LoadConfig())
	{
		// MessageBox(_T("No GigE-Vision cameras found or selected!"), _T("提示"), MB_ICONEXCLAMATION);

		MessageBox(_T("相机未找到或未选择!"), _T("提示"), MB_ICONEXCLAMATION);

		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("No GigE-Vision cameras found or selected!\r\n");
		subStr = _T("相机未找到或未选择!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}
	else
	{
		m_IsCameraConnected = TRUE;

		// GetDlgItem(IDC_BTN_GRAB_TEST)->EnableWindow(1);

		// 输出状态信息
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("Load camera config file success!\r\n");
		subStr = _T("加载相机配置文件成功!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo); 
		m_infoOut.LineScroll(m_infoOut.GetLineCount()); 
	}
}




// 初始化控制卡
void CDetectionDlg::InitBoard()
{
	int i;
	int glTotalAxes, glTotalCards;

	//初始化板卡
	glTotalAxes = auto_set();			//自动设置
	if (glTotalAxes <= 0)
	{
		MessageBox(_T("检测不到卡!"));
		return;
	}

	glTotalCards = init_board();		//初始化
	if (glTotalCards <= 0)
	{
		MessageBox(_T("初始化错误!"));
		return;
	}

	// 这里要注意？  可能要改
	for (i = 0; i<glTotalAxes; i++)
	{
		set_sd_logic(i + 1, 0);    
		set_el_logic(i + 1, 0);     
		set_org_logic(i + 1, 0);   
		set_alm_logic(i + 1, 0);
	}


	// 电机每转需要的脉冲数  换算脉冲当量：2000p/10mm = 200p/mm
	// 电机每转需要的脉冲数  换算脉冲当量：10000p/10mm = 1000p/mm  5.23修改脉冲当量为1000p/mm
	set_steps_pr(1, 10* m_steps_mm);     // 轴1   相机
	set_steps_pr(2, 10 * m_steps_mm);     // 轴2   平台


	// -----------------【轴的限位、电平】--------------------
	// 轴1
	enable_el(1, 1);    // 轴1限位
	set_el_logic(1, 1); // 轴1限位高电平有效
	// 轴2
	enable_el(2, 1);    // 轴2限位
	set_el_logic(2, 1); // 轴2限位高电平有效

	// -----------------【轴的原点、电平】--------------------
	// 轴1
	enable_org(1, 1);    // 轴1原点
	set_org_logic(1, 1); // 轴1原点高电平有效
	// 轴2
	enable_org(2, 1);    // 轴2原点
	set_org_logic(2, 1); // 轴2原点高电平有效

	/*
	// -----------------【轴的加减速、电平】--------------------
	enable_sd(1, 1);
	set_sd_logic(1, 1);

	enable_sd(2, 1);
	set_sd_logic(2, 1);
	*/
}


void CDetectionDlg::OnBnClickedBtnStartDetection()
{
	// TODO:  在此添加控件通知处理程序代码
	

	ofstream outfile("file.txt", ios::app);

	stopdetectionflag = TRUE;
	m_bStopFlag = false;

	// 获取检测开始时间
	m_camImg.GetStartTime();

	// 新建子文件夹
	m_camImg.MkdirPic();


	// 初始化运动控制卡
	// InitBoard();

	// ----------------------[运动控制指令]-----------------------
	// 获取运动参数  mm
	m_sSpeedCamera = (int)_ttof(dlgMotionPara.m_sSetSpeedCamera);
	m_sDistanceCamera = (int)_ttof(dlgMotionPara.m_sSetDistanceCamera);
	m_sSpeedPlatform = (int)_ttof(dlgMotionPara.m_sSetSpeedPlatform);
	m_sDistancePlatform = (int)_ttof(dlgMotionPara.m_sSetDistancePlatform);


	// ---------[获取系统参数]--------

	//  将分辨率、阈值、每条取样帧数、灰度阈值减数传递给图像处理类
	GetSystemPara();
	m_camImg.SetImagePara(m_resolution, m_threshold, sampleCountStrip, graythreshold);

	//  重置所有索引
	m_camImg.ResetAllIndex();

	// 相机轴总脉冲数
	stepCamera = (m_sDistanceCamera)*m_steps_mm;

	// 判断设定数值是否合理
	if (m_sDistancePlatform <= 45)
	{
		AfxMessageBox(_T("平台运动距离太小，需要大于45mm！"));
		return;
	}
	if (m_sDistancePlatform > 520)
	{
		AfxMessageBox(_T("平台运动距离太大，需要小于520mm！"));
		return;
	}

	if (m_sDistanceCamera <= 30)
	{
		AfxMessageBox(_T("相机运动距离太小，需要大于30mm！"));
		return;
	}
	else if (m_sDistanceCamera > 1100)
	{
		AfxMessageBox(_T("相机运动距离太大，需要小于1100mm！"));
		return;
	}

	else
	{
		stepPlatform = (int)(d * m_steps_mm);
		stripCountDlg = (int)((m_sDistancePlatform - m) / d);  // 图像条数
	}

	//保证每长条截取长度小于设定距离
	if ((sampleCountStrip * 2138 * m_resolution) > m_sDistanceCamera * 1000) //2138是随便取了一个比2048略大的值
	{
		AfxMessageBox(_T("每条取样帧数太大，需要变小！"));
		return;
	}

	// 一条图像帧数
	if (stepCamera > 0)
	{
		frameCountDlg = (int)(stepCamera / heightnew);
	}
	else
	{
		frameCountDlg = (int)(-stepCamera / heightnew); 
	}

	/////////////////////  设置条图像帧数和条图像数量

	m_camImg.SetFrameCount(frameCountDlg);
	m_camImg.SetStripCount(stripCountDlg);


	////////////////////// 相机和平台运动的每秒脉冲数
	speedCamera = m_sSpeedCamera / 60 * m_steps_mm;
	speedPlatform = m_sSpeedPlatform / 60 * m_steps_mm;


	//5.28新加位置清零
	reset_pos(1);
	reset_pos(2);

	// ----------------------[相机开始指令]-----------------------
	if (m_IsCameraConnected && m_sSpeedCamera)    // 相机连接 && 运动参数已经设置
	{
		// if (!m_camImg.StartGrab() || m_camImg.IfGrabStart())    // 开始采集失败:相机无法采集或相机已经开始采集
		if (!m_camImg.StartGrab())    // 开始采集失败:相机无法采集或相机已经开始采集
		{
			CString m_sError;
			m_sError = m_camImg.Get_Error();
			// MessageBox(m_sError, _T("错误"), MB_ICONEXCLAMATION);
			MessageBox(_T("相机启动失败"), _T("错误"), MB_ICONEXCLAMATION);
			return;
		}
		else  // 开始采集成功，电机运动
		{
			// 检测次数加一
			detectCountDlg++;
			// 将检测次数传递给相机类
			m_camImg.SetDetectCount(detectCountDlg);

			// 开始采集信息输出
			CTime time1;
			time1 = CTime::GetCurrentTime();
			CString curData1 = time1.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
			strInfo += curData1;

			CString subStr1;
			subStr1 = _T("平台开始运动!\r\n");
			strInfo += subStr1;
			this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
			m_infoOut.LineScroll(m_infoOut.GetLineCount());


			//3.24刘老师新加多线程
			unsigned int threadID;
			_beginthreadex(NULL, 0, &CDetectionDlg::OnThreadFunc,this,0, &threadID);

		}
	}
	// 相机未连接、配置文件未加载或运动参数为设置
	else
	{
		MessageBox(_T("Camera is not connected, config file is not loaded or motion parameters are not set!"), _T("提示"), MB_ICONWARNING);

		// 输出状态信息
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		subStr = _T("相机未连接或相机配置文件未加载或运动参数未设置!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());

		return;
	}
}

//3.24刘老师新加多线程
UINT CDetectionDlg::OnThreadFunc(LPVOID lpParam)
{
	CDetectionDlg* dlg = static_cast<CDetectionDlg*>(lpParam);
	if(dlg)
	{
		dlg->OnMotorMoving();
		dlg->PostMessage(WM_MOTORSTOP);
	}
	_endthreadex(0);
	return 0L;
}


void CDetectionDlg::OnMotorMoving()
{
	int speedCamera = m_sSpeedCamera / 60 * m_steps_mm;
	int speedPlatform = m_sSpeedPlatform / 60 * m_steps_mm;
	// 脉冲数
	int stepCamera = (m_sDistanceCamera)*m_steps_mm;
	int stepPlatform = (int)(d * m_steps_mm);
	// 轴1
	set_maxspeed(1, 10 * m_steps_mm);
	set_conspeed(1, 10 * m_steps_mm);  // 常速运动
	set_profile(1, speedCamera / 6, speedCamera, speedCamera * 30);  // 快速运动

	// 轴2
	set_maxspeed(2, 10 * m_steps_mm);
	set_conspeed(2, 10 * m_steps_mm);  // 常速运动
	set_profile(2, speedPlatform / 6, speedPlatform, speedPlatform * 20);  // 快速运动


	// 电机开始运动
	///*
	int flag = 1;

	//3.12进制报警信号输入，外部未接线
	enable_alm(1, 0);
	enable_alm(2, 0);


	if (stopdetectionflag == TRUE)  //先判断stopdetectionflag
	{
		open_list();
		for (int i = 0; i < stripCountDlg; i++)
		{
			set_maxspeed(1, 10 * m_steps_mm);
			set_conspeed(1, 10 * m_steps_mm);  // 常速运动
			set_profile(1, speedCamera / 6, speedCamera, speedCamera * 30);  // 快速运动

			set_maxspeed(2, 10 * m_steps_mm);
			set_conspeed(2, 10 * m_steps_mm);  // 常速运动
			set_profile(2, speedPlatform / 6, speedPlatform, speedPlatform * 20);  // 快速运动

			// 开始时让轴2负向运动43000p
			if (i == 0)
			{
				//fast_pmove(2, -240 * m_steps_mm); //2020.6.16 取消了开始检测时向前移动一段距离
				delay_time(2000);
			}


			if (i % 2)      // 相机负向运动
				flag = -1;
			else            // 相机正向运动
				flag = 1;

			// 相机运动距离                              
			fast_pmove(1, stepCamera*flag);
			//delay_time(2000);


			if (i < stripCountDlg - 1)
			{
				// 平台运动距离
				fast_pmove(2, -stepPlatform);
				delay_time(1000);
			}
		}

		close_list();
	}
	


	/*CTime time;
	time = CTime::GetCurrentTime();
	CString str = time.Format(_T("[%Y-%m-%d %H:%M:%S]"));
	string stdstr;
	stdstr = CW2A(str.GetString());*/
	//for (int i = 0; i < stdstr.size(); i++)
	//{
	//	outfile << stdstr[i];
	//}
	//outfile << endl;



	// 读取一个轴的位置
	double absPos1, absPos2;
	while (check_done(0))  //check_done参数为0时，表示检测批处理运动状态，
		//若批处理运动还在执行，则返回值为1；执行while内的循环体
		//若批处理运动已完成，则返回值为0，终止循环，执行下一步
	{
		Sleep(100);
		get_abs_pos(1, &absPos1);
		get_abs_pos(2, &absPos2);
		//outfile << absPos1 << endl;
		//outfile << absPos2 << endl << endl;
		//Sleep(100);
	}

	/*CTime time2;
	time2 = CTime::GetCurrentTime();
	CString str2 = time2.Format(_T("[%Y-%m-%d %H:%M:%S]"));
	string stdstr2;
	stdstr2 = CW2A(str2.GetString());
	for (int i = 0; i < stdstr2.size(); i++)
	{
		outfile << stdstr2[i];
	}
	outfile << endl;*/

	long temp;
	get_done_source(1, &temp);
	//outfile << temp << endl;
	get_done_source(2, &temp);
	//outfile << temp << endl;

	//-------------end----------
	// 获取终点位置
	get_abs_pos(1, &absPos1);
	get_abs_pos(2, &absPos2);


	if(stopdetectionflag == TRUE)
	{ 
		// 返回运动起点
		open_list();
		//delay_time(6000);
		Sleep(2000);
		fast_pmove(1, -absPos1);
		fast_pmove(2, -absPos2);
		close_list();
	}
	
}



LRESULT CDetectionDlg::OnMotorStop(WPARAM w,LPARAM l)
{

	// 相机停止采集
	if (m_camImg.StopGrab())
	{
		// 结束采集信息输出
		CTime time2;
		time2 = CTime::GetCurrentTime();
		CString curData2 = time2.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData2;

		CString subStr2;
		// subStr2 = _T("Camera stops grab!\r\n");
		subStr2 = _T("相机停止采集!\r\n");
		strInfo += subStr2;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}


	// 返回运动起点信息输出
	CTime time3;
	time3 = CTime::GetCurrentTime();
	CString curData3 = time3.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData3;

	CString subStr3;
	// subStr3 = _T("Return to origin position!\r\n");
	subStr3 = _T("开始回原点!\r\n");
	strInfo += subStr3;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());

	if (m_bStopFlag)
	{
		return 0L;
	}
	//saveIndex是每采集一帧（执行一次回调函数）加一次
	//6.12修改
	//if (m_camImg.stripIndex >= m_camImg.stripCount && (m_camImg.saveIndex == (m_camImg.frameCount + 1)))
	//{
		//if (stripIndex >= stripCount && saveIndex > sampleCountStrip)
		m_camImg.ImageProcessAccuracy(this);
		m_camImg.ResetAllIndex();
		Invalidate(FALSE);
		AfxMessageBox(_T("图片拍摄完成！"), MB_OK | MB_ICONINFORMATION);
	//}



	//m_camImg.m_Buffers->Clear();

	//m_camImg.m_Xfer->Abort();

	return 0L;
}




// 停止检测
void CDetectionDlg::OnBnClickedBtnStopDetection()
{
	// TODO: 在此添加控件通知处理程序代码

	// // ----------------------[运动停止指令]-----------------------
	/*if (check_done(1))
		sudden_stop(1);
	if (check_done(2))
		sudden_stop(2);*/

	
	sudden_stop_list();
	stopdetectionflag = FALSE;

	m_bStopFlag = true;

	

	// ----------------------[相机停止指令]-----------------------
	if (!m_camImg.IfGrabStart())
	{
		// MessageBox(_T("Camera is not grabbing, can't freeze!"), _T("提示"), MB_ICONWARNING);
		MessageBox(_T("相机未采集，无需停止!"), _T("提示"), MB_ICONWARNING);
	}
	else
	{
		m_camImg.StopGrab();

		// 输出状态信息
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("Stop detection!\r\n");
		subStr = _T("系统停止检测!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}
}


void CDetectionDlg::OnBnClickedBtnExit()
{
	// TODO: 在此添加控件通知处理程序代码

	if (MessageBox(_T("     确定退出系统！"), _T("提示"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		EndDialog(-1);
	}
	else
	{

	}
}




// 上下翻页响应
void CDetectionDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码



	//3.28
	if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上的箭头，前一页  
	{
		m_camImg.PageShow(-1);
	}
	else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往下的箭头，后一页
	{
		m_camImg.PageShow(1);
	}

	*pResult = 0;
}



//———————————————滚动条消息响应函数——————————————//


void CDetectionDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(1);

	// TODO: 在此处添加消息处理程序代码
}

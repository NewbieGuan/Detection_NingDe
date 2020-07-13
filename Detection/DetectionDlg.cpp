
// DetectionDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// CDetectionDlg �Ի���


// ���캯��
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
	heightnew = 2203; //2048*5.378/5��������Ϊ2203

	m_IsCameraConnected = FALSE;

	strInfo = _T("");

	
	detectCountDlg = 0;

	stopdetectionflag = TRUE;

	// m_IsMotionParaSet = FALSE;

}

// ��������
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


// CDetectionDlg ��Ϣ�������

BOOL CDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	// ��ȡϵͳ����
	GetSystemPara();

	// ��ʼ���˶����ƿ�
	InitBoard();


	// ��ť״̬����
	//GetDlgItem(IDC_BTN_GRAB_TEST)->EnableWindow(0);
	//GetDlgItem(IDC_BTN_FREEZE_TEST)->EnableWindow(0);

	// ----------------------------����ʾLOGO��---------------------------
	HBITMAP hBitmap;
	//����ͼƬID  
	hBitmap = (HBITMAP)LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDB_BITMAP_LOGO),
		IMAGE_BITMAP, 0, 0,
		LR_LOADMAP3DCOLORS);
	//���ָ��̬�ؼ���ָ��    
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_LOGO);//�ؼ�ID     
	//���þ�̬�ؼ�����ʽ��ʹ�����ʹ��λͼ������λ����ʾʹ����    
	pStatic->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);
	//���þ�̬�ؼ���ʾλͼ    
	pStatic->SetBitmap(hBitmap);

	// ----------------------------�����ͼƬ��ť��---------------------------
	// ����ļ�����
	HINSTANCE hInstance;
	HBITMAP hBitmapLoad;
	hInstance = ::AfxGetInstanceHandle();
	hBitmapLoad = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_LOAD));
	m_buttonLoad.SetBitmap(hBitmapLoad);

	// �˶���������
	HBITMAP hBitmapMotion;
	hInstance = ::AfxGetInstanceHandle();
	hBitmapMotion = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_MOTION_PARA));
	m_buttonMotion.SetBitmap(hBitmapMotion);

	// ��ͼƬ�ļ���
	HBITMAP hBitmapOpenPicture;
	hBitmapOpenPicture = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_OPEN_PICTURE));
	m_buttonOpenPicture.SetBitmap(hBitmapOpenPicture);

	// ��ͳ�ƽ���ļ���
	HBITMAP hBitmapOpenResult;
	hBitmapOpenResult = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_OPEN_EXCEL));
	m_buttonOpenResult.SetBitmap(hBitmapOpenResult);

	// �ֶ��˶�����
	HBITMAP hBitmapMotionAdjust;
	hBitmapMotionAdjust = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_MOTION_ADJUST));
	m_buttonMotionAdjust.SetBitmap(hBitmapMotionAdjust);


	// ----------------------------���ұ�ͼƬ��ť��---------------------------
	// ��ʼ��ⰴť
	HBITMAP hBitmapStart;
	hBitmapStart = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_START));
	m_buttonStartDetection.SetBitmap(hBitmapStart);

	// ֹͣ��ⰴť
	HBITMAP hBitmapStop;
	hBitmapStop = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_STOP));
	m_buttonStopDetection.SetBitmap(hBitmapStop);

	// �˳�ϵͳ��ť
	HBITMAP hBitmapExit;
	hBitmapExit = ::LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_EXIT));
	m_buttonExit.SetBitmap(hBitmapExit);




	// ----------------------��ʱ�����á�------------------------
	// ��������������
	CFont* font_time_weekDay;
	font_time_weekDay = m_time_weekDay.GetFont();//��ȡCFont����
	LOGFONT lf_TimeWeekDay;
	font_time_weekDay->GetLogFont(&lf_TimeWeekDay);//��ȡLOGFONT�ṹ��
	lf_TimeWeekDay.lfHeight = 26;    //�޸������С
	lf_TimeWeekDay.lfWidth = 8;     // �޸�������
	lf_TimeWeekDay.lfItalic = FALSE;        //��б
	lf_TimeWeekDay.lfWeight = 560;   //�޸�����Ĵ�ϸ
	m_font_time_weekDay.CreateFontIndirectW(&lf_TimeWeekDay);//����һ���µ�����
	m_time_weekDay.SetFont(&m_font_time_weekDay);
	
	// ʱ����
	CFont* font_time_hms;
	font_time_hms = m_time_hms.GetFont();//��ȡCFont����
	LOGFONT lf_TimeHMS;
	font_time_hms->GetLogFont(&lf_TimeHMS);//��ȡLOGFONT�ṹ��
	lf_TimeHMS.lfHeight = 36;    //�޸������С
	lf_TimeWeekDay.lfWidth = 8; // �޸�������
	lf_TimeHMS.lfItalic = FALSE;        //��б
	lf_TimeHMS.lfWeight = 560;   //�޸�����Ĵ�ϸ
	m_font_time_hms.CreateFontIndirectW(&lf_TimeHMS);//����һ���µ�����
	m_time_hms.SetFont(&m_font_time_hms);

	// ----------------------��ʱ����¡�------------------------
	SetTimer(1, 1000, NULL);

	// ----------------��������߰�ť��ǩ���塿------------------
	CFont* font_button_label;
	font_button_label = m_label_motionPara.GetFont();//��ȡCFont����
	LOGFONT lf_button_label;
	font_button_label->GetLogFont(&lf_button_label);//��ȡLOGFONT�ṹ��
	lf_button_label.lfHeight = 14;    //�޸������С
	// lf_button_label.lfWidth = 8; // �޸�������
	lf_button_label.lfItalic = FALSE;        //��б
	lf_button_label.lfWeight = 560;   //�޸�����Ĵ�ϸ
	m_font_button_label.CreateFontIndirectW(&lf_button_label);//����һ���µ�����

	m_label_loadConfig.SetFont(&m_font_button_label);
	m_label_motionPara .SetFont(&m_font_button_label);
	// m_label_imagePara.SetFont(&m_font_button_label);
	// m_label_statistic.SetFont(&m_font_button_label);
	m_label_openPicture.SetFont(&m_font_button_label);
	m_label_openResult.SetFont(&m_font_button_label);
	m_label_motionAdjust.SetFont(&m_font_button_label);


	// ----------------------��list���á�-----------------------
	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;   //���ѡ��ĳ��ʹ���и�����ֻ����������Ϊreport ����listctrl ��
	dwStyle |= LVS_EX_GRIDLINES;       // �б��������ߣ�ֻ������report ����listctrl ��
	m_list.SetExtendedStyle(dwStyle);  // ������չ���

	// �и�
	CImageList m_l;
	m_l.Create(1, 30, TRUE | ILC_COLOR32, 1, 0);
	m_list.SetImageList(&m_l, LVSIL_SMALL);
	// �ֺ��Լ�����
	/*
	CFont m_font;
	m_font.CreatePointFont(100, _T("������"));
	m_list.SetFont(&m_font);
	*/
	// �б���
	int nIndex = 0;
	m_list.InsertColumn(nIndex++, _T(""), LVCFMT_CENTER, 68);
	//m_list.InsertColumn(nIndex++, _T("������/��"), LVCFMT_CENTER, 118);
	m_list.InsertColumn(nIndex++, _T("�������/um"), LVCFMT_CENTER, 118);  // 3.28���
	m_list.InsertColumn(nIndex++, _T("ƽ��ֱ��/um"), LVCFMT_CENTER, 118);
	// m_list.InsertColumn(nIndex++, _T("���������/um2"), LVCFMT_CENTER, 112);
	m_list.InsertColumn(nIndex++, _T("�����/%"), LVCFMT_CENTER, 118);
	// �б���
	m_list.InsertItem(1, _T("ֵ"));
	// m_list.InsertItem(2, _T("��׼��"));
	// m_list.InsertItem(3, _T("���"));
	// m_list.InsertItem(4, _T("��С"));
	// m_list.InsertItem(5, _T("�ܼ�"));


	// �༭����Ϣ��������޸�
	CEdit *m_editInfo = (CEdit*)GetDlgItem(IDC_EDIT_INFORMATION_OUT);
	fontEditInfo.CreatePointFont(100, _T("������"));
	m_editInfo->SetFont(&fontEditInfo);

	// ͼ���������
	m_progressImage.SetRange(0, 100);
	// m_progressImage.SetPos(60);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDetectionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// ����Group Box�ı���ɫ
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

	// ����ʾͼ������Χ���߿���
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

	// ƫ����
	int offset = 3;
	// ��
	dc.MoveTo(left - offset, top - offset);
	dc.LineTo(left - offset, bottom + offset);

	// ��
	dc.MoveTo(left - offset, top - offset);
	dc.LineTo(right + offset, top - offset);

	// ��
	dc.MoveTo(right + offset, top - offset);
	dc.LineTo(right + offset, bottom + offset);

	// ��
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


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		MessageBox(_T("��Ӧ�ؼ�������"));
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


// --------------------------���ػ�LOGO��----------------------------
void CDetectionDlg::repaintLogo(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("��Ӧ�ؼ�������"));
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


// --------------------------���ػ���߰�ť��----------------------------
void CDetectionDlg::repaintLeftBTN(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("��Ӧ�ؼ�������"));
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

// --------------------------���ػ��ұ߰�ť��----------------------------
void CDetectionDlg::repaintRightBTN(UINT id, CRect last_rect, CRect now_rect)
{
	CRect rect;
	CWnd *wnd = NULL;
	wnd = GetDlgItem(id);
	if (NULL == wnd)
	{
		MessageBox(_T("��Ӧ�ؼ�������"));
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	// ���ں�������
	// char weekday[7] = { '��','һ', '��', '��', '��', '��', '��' };
	CTime t_weekday = CTime::GetCurrentTime();

	/*
	int weekdayNum = (int)_ttof(t_weekday.Format("%w"));
	CString weekdayStr;
	weekdayStr.Format(_T("%s"), weekday[weekdayNum]);
	*/
	CString str_weekday =  t_weekday.Format("����%w     %Y.%m.%d");
	SetDlgItemText(IDC_TIME_WEEKDAY, str_weekday);


	// ʱ����
	CTime t_hms = CTime::GetCurrentTime();
	CString str_hms = t_hms.Format("%H:%M:%S");
	SetDlgItemText(IDC_TIME_HMS, str_hms);


	CDialogEx::OnTimer(nIDEvent);
}


HBRUSH CDetectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// ʱ�������屳����ɫ
	if (pWnd->GetDlgCtrlID() == IDC_LABEL_MOTION_PARA || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_IMAGE_PARA || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_STATISTIC || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_OPEN_PICTURE || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_OPEN_RESULT || 
		pWnd->GetDlgCtrlID() == IDC_LABEL_MOTION_ADJUST ||
		pWnd->GetDlgCtrlID() == IDC_LABEL_LOAD_CONFIG)
	{
		// pDC->SetTextColor(RGB(0, 255, 0));       // �����ı���ɫ
		pDC->SetBkMode(OPAQUE);                  // �����ı�����ģʽΪ͸��
		pDC->SetBkColor(RGB(187, 255, 255));           // �����ı�������ɫ
		hbr = CreateSolidBrush(RGB(200, 255, 255));    // �ؼ�������ɫ

		return hbr;
	}


	// ʱ�������屳����ɫ
	if (pWnd->GetDlgCtrlID() == IDC_TIME_HMS)
	{
		pDC->SetTextColor(RGB(0, 255, 0));       // �����ı���ɫ
		pDC->SetBkMode(OPAQUE);                  // �����ı�����ģʽΪ͸��
		pDC->SetBkColor(RGB(0, 0, 0));           // �����ı�������ɫ
		hbr = CreateSolidBrush(RGB(0, 0, 0));    // �ؼ�������ɫ

		return hbr;
	}

	// �������������屳������
	if (pWnd->GetDlgCtrlID() == IDC_TIME_WEEKDAY)
	{
		pDC->SetTextColor(RGB(0, 255, 0));       // �����ı���ɫ
		pDC->SetBkMode(OPAQUE);                  // �����ı�����ģʽΪ͸��
		pDC->SetBkColor(RGB(0, 0, 0));           // �����ı�������ɫ
		hbr = CreateSolidBrush(RGB(0, 0, 0));    // �ؼ�������ɫ

		return hbr;
	}


	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
	// return m_brush;
}


// -----------���˶����Ʋ������öԻ���-----------------
void CDetectionDlg::OnBnClickedBtnMotionPara()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������


	// ----------------��ģ̬�Ի���---------------
	dlgMotionPara.DoModal();

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Set motion parameters!\r\n");
	subStr = _T("�����˶�����!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


/*
// -----------��ͼ����������öԻ���-----------------

*/


// -----------���ֶ����ڲ������öԻ���-----------------
void CDetectionDlg::OnBnClickedBtnMotionAdjust()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	

	dlgMotionAdjust.DoModal();

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Manual adjustment motion parameters!\r\n");
	subStr = _T("�ֶ������˶�����!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());

}



/*
// ----------------��ͳ�ƽ�����ߡ�---------------------

*/



// ------------------����ͼƬ�ļ��С�-----------------------
void CDetectionDlg::OnBnClickedBtnOpenPicture()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

	CString path = _T("..\\image");

	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Open the picture folder!\r\n");
	subStr = _T("��ͼƬ�ļ���!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


// ----------------����ͳ�ƽ���ļ��С�---------------------
void CDetectionDlg::OnBnClickedBtnOpenResult()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	// CString path = _T("E:/��Ŀ/code/code_12.21/��Ĥ/code/main/VS2015/Detection/�����");
	CString path = _T("..\\result");

	ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);

	CTime time;
	time = CTime::GetCurrentTime();
	CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData;

	CString subStr;
	// subStr = _T("Open the result file folder!\r\n");
	subStr = _T("�򿪽���ļ���!\r\n");
	strInfo += subStr;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());
}


void CDetectionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	// CDialogEx::OnLButtonDown(nFlags, point);
}


// �޸�ϵͳ��������ȡϵͳ����
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
	// TODO: �ڴ���������������
	// UpdateData(TRUE);

	// ԭ��
	// dlgPasswordLogin.DoModal();

	dlgModifyPara.DoModal();

	GetSystemPara();



	// UpdateData(FALSE);


}


void CDetectionDlg::OnBnClickedBtnLoadTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!m_camImg.LoadConfig())
	{
		// MessageBox(_T("No GigE-Vision cameras found or selected!"), _T("��ʾ"), MB_ICONEXCLAMATION);

		MessageBox(_T("���δ�ҵ���δѡ��!"), _T("��ʾ"), MB_ICONEXCLAMATION);

		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("No GigE-Vision cameras found or selected!\r\n");
		subStr = _T("���δ�ҵ���δѡ��!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}
	else
	{
		m_IsCameraConnected = TRUE;

		// GetDlgItem(IDC_BTN_GRAB_TEST)->EnableWindow(1);

		// ���״̬��Ϣ
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("Load camera config file success!\r\n");
		subStr = _T("������������ļ��ɹ�!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo); 
		m_infoOut.LineScroll(m_infoOut.GetLineCount()); 
	}
}




// ��ʼ�����ƿ�
void CDetectionDlg::InitBoard()
{
	int i;
	int glTotalAxes, glTotalCards;

	//��ʼ���忨
	glTotalAxes = auto_set();			//�Զ�����
	if (glTotalAxes <= 0)
	{
		MessageBox(_T("��ⲻ����!"));
		return;
	}

	glTotalCards = init_board();		//��ʼ��
	if (glTotalCards <= 0)
	{
		MessageBox(_T("��ʼ������!"));
		return;
	}

	// ����Ҫע�⣿  ����Ҫ��
	for (i = 0; i<glTotalAxes; i++)
	{
		set_sd_logic(i + 1, 0);    
		set_el_logic(i + 1, 0);     
		set_org_logic(i + 1, 0);   
		set_alm_logic(i + 1, 0);
	}


	// ���ÿת��Ҫ��������  �������嵱����2000p/10mm = 200p/mm
	// ���ÿת��Ҫ��������  �������嵱����10000p/10mm = 1000p/mm  5.23�޸����嵱��Ϊ1000p/mm
	set_steps_pr(1, 10* m_steps_mm);     // ��1   ���
	set_steps_pr(2, 10 * m_steps_mm);     // ��2   ƽ̨


	// -----------------�������λ����ƽ��--------------------
	// ��1
	enable_el(1, 1);    // ��1��λ
	set_el_logic(1, 1); // ��1��λ�ߵ�ƽ��Ч
	// ��2
	enable_el(2, 1);    // ��2��λ
	set_el_logic(2, 1); // ��2��λ�ߵ�ƽ��Ч

	// -----------------�����ԭ�㡢��ƽ��--------------------
	// ��1
	enable_org(1, 1);    // ��1ԭ��
	set_org_logic(1, 1); // ��1ԭ��ߵ�ƽ��Ч
	// ��2
	enable_org(2, 1);    // ��2ԭ��
	set_org_logic(2, 1); // ��2ԭ��ߵ�ƽ��Ч

	/*
	// -----------------����ļӼ��١���ƽ��--------------------
	enable_sd(1, 1);
	set_sd_logic(1, 1);

	enable_sd(2, 1);
	set_sd_logic(2, 1);
	*/
}


void CDetectionDlg::OnBnClickedBtnStartDetection()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	

	ofstream outfile("file.txt", ios::app);

	stopdetectionflag = TRUE;
	m_bStopFlag = false;

	// ��ȡ��⿪ʼʱ��
	m_camImg.GetStartTime();

	// �½����ļ���
	m_camImg.MkdirPic();


	// ��ʼ���˶����ƿ�
	// InitBoard();

	// ----------------------[�˶�����ָ��]-----------------------
	// ��ȡ�˶�����  mm
	m_sSpeedCamera = (int)_ttof(dlgMotionPara.m_sSetSpeedCamera);
	m_sDistanceCamera = (int)_ttof(dlgMotionPara.m_sSetDistanceCamera);
	m_sSpeedPlatform = (int)_ttof(dlgMotionPara.m_sSetSpeedPlatform);
	m_sDistancePlatform = (int)_ttof(dlgMotionPara.m_sSetDistancePlatform);


	// ---------[��ȡϵͳ����]--------

	//  ���ֱ��ʡ���ֵ��ÿ��ȡ��֡�����Ҷ���ֵ�������ݸ�ͼ������
	GetSystemPara();
	m_camImg.SetImagePara(m_resolution, m_threshold, sampleCountStrip, graythreshold);

	//  ������������
	m_camImg.ResetAllIndex();

	// �������������
	stepCamera = (m_sDistanceCamera)*m_steps_mm;

	// �ж��趨��ֵ�Ƿ����
	if (m_sDistancePlatform <= 45)
	{
		AfxMessageBox(_T("ƽ̨�˶�����̫С����Ҫ����45mm��"));
		return;
	}
	if (m_sDistancePlatform > 520)
	{
		AfxMessageBox(_T("ƽ̨�˶�����̫����ҪС��520mm��"));
		return;
	}

	if (m_sDistanceCamera <= 30)
	{
		AfxMessageBox(_T("����˶�����̫С����Ҫ����30mm��"));
		return;
	}
	else if (m_sDistanceCamera > 1100)
	{
		AfxMessageBox(_T("����˶�����̫����ҪС��1100mm��"));
		return;
	}

	else
	{
		stepPlatform = (int)(d * m_steps_mm);
		stripCountDlg = (int)((m_sDistancePlatform - m) / d);  // ͼ������
	}

	//��֤ÿ������ȡ����С���趨����
	if ((sampleCountStrip * 2138 * m_resolution) > m_sDistanceCamera * 1000) //2138�����ȡ��һ����2048�Դ��ֵ
	{
		AfxMessageBox(_T("ÿ��ȡ��֡��̫����Ҫ��С��"));
		return;
	}

	// һ��ͼ��֡��
	if (stepCamera > 0)
	{
		frameCountDlg = (int)(stepCamera / heightnew);
	}
	else
	{
		frameCountDlg = (int)(-stepCamera / heightnew); 
	}

	/////////////////////  ������ͼ��֡������ͼ������

	m_camImg.SetFrameCount(frameCountDlg);
	m_camImg.SetStripCount(stripCountDlg);


	////////////////////// �����ƽ̨�˶���ÿ��������
	speedCamera = m_sSpeedCamera / 60 * m_steps_mm;
	speedPlatform = m_sSpeedPlatform / 60 * m_steps_mm;


	//5.28�¼�λ������
	reset_pos(1);
	reset_pos(2);

	// ----------------------[�����ʼָ��]-----------------------
	if (m_IsCameraConnected && m_sSpeedCamera)    // ������� && �˶������Ѿ�����
	{
		// if (!m_camImg.StartGrab() || m_camImg.IfGrabStart())    // ��ʼ�ɼ�ʧ��:����޷��ɼ�������Ѿ���ʼ�ɼ�
		if (!m_camImg.StartGrab())    // ��ʼ�ɼ�ʧ��:����޷��ɼ�������Ѿ���ʼ�ɼ�
		{
			CString m_sError;
			m_sError = m_camImg.Get_Error();
			// MessageBox(m_sError, _T("����"), MB_ICONEXCLAMATION);
			MessageBox(_T("�������ʧ��"), _T("����"), MB_ICONEXCLAMATION);
			return;
		}
		else  // ��ʼ�ɼ��ɹ�������˶�
		{
			// ��������һ
			detectCountDlg++;
			// �����������ݸ������
			m_camImg.SetDetectCount(detectCountDlg);

			// ��ʼ�ɼ���Ϣ���
			CTime time1;
			time1 = CTime::GetCurrentTime();
			CString curData1 = time1.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
			strInfo += curData1;

			CString subStr1;
			subStr1 = _T("ƽ̨��ʼ�˶�!\r\n");
			strInfo += subStr1;
			this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
			m_infoOut.LineScroll(m_infoOut.GetLineCount());


			//3.24����ʦ�¼Ӷ��߳�
			unsigned int threadID;
			_beginthreadex(NULL, 0, &CDetectionDlg::OnThreadFunc,this,0, &threadID);

		}
	}
	// ���δ���ӡ������ļ�δ���ػ��˶�����Ϊ����
	else
	{
		MessageBox(_T("Camera is not connected, config file is not loaded or motion parameters are not set!"), _T("��ʾ"), MB_ICONWARNING);

		// ���״̬��Ϣ
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		subStr = _T("���δ���ӻ���������ļ�δ���ػ��˶�����δ����!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());

		return;
	}
}

//3.24����ʦ�¼Ӷ��߳�
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
	// ������
	int stepCamera = (m_sDistanceCamera)*m_steps_mm;
	int stepPlatform = (int)(d * m_steps_mm);
	// ��1
	set_maxspeed(1, 10 * m_steps_mm);
	set_conspeed(1, 10 * m_steps_mm);  // �����˶�
	set_profile(1, speedCamera / 6, speedCamera, speedCamera * 30);  // �����˶�

	// ��2
	set_maxspeed(2, 10 * m_steps_mm);
	set_conspeed(2, 10 * m_steps_mm);  // �����˶�
	set_profile(2, speedPlatform / 6, speedPlatform, speedPlatform * 20);  // �����˶�


	// �����ʼ�˶�
	///*
	int flag = 1;

	//3.12���Ʊ����ź����룬�ⲿδ����
	enable_alm(1, 0);
	enable_alm(2, 0);


	if (stopdetectionflag == TRUE)  //���ж�stopdetectionflag
	{
		open_list();
		for (int i = 0; i < stripCountDlg; i++)
		{
			set_maxspeed(1, 10 * m_steps_mm);
			set_conspeed(1, 10 * m_steps_mm);  // �����˶�
			set_profile(1, speedCamera / 6, speedCamera, speedCamera * 30);  // �����˶�

			set_maxspeed(2, 10 * m_steps_mm);
			set_conspeed(2, 10 * m_steps_mm);  // �����˶�
			set_profile(2, speedPlatform / 6, speedPlatform, speedPlatform * 20);  // �����˶�

			// ��ʼʱ����2�����˶�43000p
			if (i == 0)
			{
				//fast_pmove(2, -240 * m_steps_mm); //2020.6.16 ȡ���˿�ʼ���ʱ��ǰ�ƶ�һ�ξ���
				delay_time(2000);
			}


			if (i % 2)      // ��������˶�
				flag = -1;
			else            // ��������˶�
				flag = 1;

			// ����˶�����                              
			fast_pmove(1, stepCamera*flag);
			//delay_time(2000);


			if (i < stripCountDlg - 1)
			{
				// ƽ̨�˶�����
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



	// ��ȡһ�����λ��
	double absPos1, absPos2;
	while (check_done(0))  //check_done����Ϊ0ʱ����ʾ����������˶�״̬��
		//���������˶�����ִ�У��򷵻�ֵΪ1��ִ��while�ڵ�ѭ����
		//���������˶�����ɣ��򷵻�ֵΪ0����ֹѭ����ִ����һ��
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
	// ��ȡ�յ�λ��
	get_abs_pos(1, &absPos1);
	get_abs_pos(2, &absPos2);


	if(stopdetectionflag == TRUE)
	{ 
		// �����˶����
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

	// ���ֹͣ�ɼ�
	if (m_camImg.StopGrab())
	{
		// �����ɼ���Ϣ���
		CTime time2;
		time2 = CTime::GetCurrentTime();
		CString curData2 = time2.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData2;

		CString subStr2;
		// subStr2 = _T("Camera stops grab!\r\n");
		subStr2 = _T("���ֹͣ�ɼ�!\r\n");
		strInfo += subStr2;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}


	// �����˶������Ϣ���
	CTime time3;
	time3 = CTime::GetCurrentTime();
	CString curData3 = time3.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
	strInfo += curData3;

	CString subStr3;
	// subStr3 = _T("Return to origin position!\r\n");
	subStr3 = _T("��ʼ��ԭ��!\r\n");
	strInfo += subStr3;
	this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
	m_infoOut.LineScroll(m_infoOut.GetLineCount());

	if (m_bStopFlag)
	{
		return 0L;
	}
	//saveIndex��ÿ�ɼ�һ֡��ִ��һ�λص���������һ��
	//6.12�޸�
	//if (m_camImg.stripIndex >= m_camImg.stripCount && (m_camImg.saveIndex == (m_camImg.frameCount + 1)))
	//{
		//if (stripIndex >= stripCount && saveIndex > sampleCountStrip)
		m_camImg.ImageProcessAccuracy(this);
		m_camImg.ResetAllIndex();
		Invalidate(FALSE);
		AfxMessageBox(_T("ͼƬ������ɣ�"), MB_OK | MB_ICONINFORMATION);
	//}



	//m_camImg.m_Buffers->Clear();

	//m_camImg.m_Xfer->Abort();

	return 0L;
}




// ֹͣ���
void CDetectionDlg::OnBnClickedBtnStopDetection()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	// // ----------------------[�˶�ָֹͣ��]-----------------------
	/*if (check_done(1))
		sudden_stop(1);
	if (check_done(2))
		sudden_stop(2);*/

	
	sudden_stop_list();
	stopdetectionflag = FALSE;

	m_bStopFlag = true;

	

	// ----------------------[���ָֹͣ��]-----------------------
	if (!m_camImg.IfGrabStart())
	{
		// MessageBox(_T("Camera is not grabbing, can't freeze!"), _T("��ʾ"), MB_ICONWARNING);
		MessageBox(_T("���δ�ɼ�������ֹͣ!"), _T("��ʾ"), MB_ICONWARNING);
	}
	else
	{
		m_camImg.StopGrab();

		// ���״̬��Ϣ
		CTime time;
		time = CTime::GetCurrentTime();
		CString curData = time.Format(_T("[%Y-%m-%d %H:%M:%S]  "));
		strInfo += curData;

		CString subStr;
		// subStr = _T("Stop detection!\r\n");
		subStr = _T("ϵͳֹͣ���!\r\n");
		strInfo += subStr;
		this->SetDlgItemText(IDC_EDIT_INFORMATION_OUT, strInfo);
		m_infoOut.LineScroll(m_infoOut.GetLineCount());
	}
}


void CDetectionDlg::OnBnClickedBtnExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (MessageBox(_T("     ȷ���˳�ϵͳ��"), _T("��ʾ"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		EndDialog(-1);
	}
	else
	{

	}
}




// ���·�ҳ��Ӧ
void CDetectionDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������



	//3.28
	if (pNMUpDown->iDelta == -1) // �����ֵΪ-1 , ˵�������Spin�����ϵļ�ͷ��ǰһҳ  
	{
		m_camImg.PageShow(-1);
	}
	else if (pNMUpDown->iDelta == 1) // �����ֵΪ1, ˵�������Spin�����µļ�ͷ����һҳ
	{
		m_camImg.PageShow(1);
	}

	*pResult = 0;
}



//��������������������������������������Ϣ��Ӧ��������������������������������//


void CDetectionDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(1);

	// TODO: �ڴ˴������Ϣ����������
}

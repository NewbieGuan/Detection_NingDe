// MotionPara.cpp : 实现文件
//

#include "stdafx.h"
#include "Detection.h"
#include "MotionPara.h"
#include "afxdialogex.h"


// CMotionPara 对话框

IMPLEMENT_DYNAMIC(CMotionPara, CDialogEx)

CMotionPara::CMotionPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotionPara::IDD, pParent)
{
	m_sSetDistanceCamera = _T("55");
	m_sSetDistancePlatform = _T("46");
	m_sSetSpeedCamera = _T("600");
	m_sSetSpeedPlatform = _T("3000");
}

CMotionPara::~CMotionPara()
{
}

void CMotionPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SET_DISTANCE_CAMERA, m_sSetDistanceCamera);
	DDX_Text(pDX, IDC_EDIT_SET_DISTANCE_PLATFORM, m_sSetDistancePlatform);
	DDX_Text(pDX, IDC_EDIT_SET_SPEED_CAMERA, m_sSetSpeedCamera);
	DDX_Text(pDX, IDC_EDIT_SET_SPEED_PLATFORM, m_sSetSpeedPlatform);
}


BEGIN_MESSAGE_MAP(CMotionPara, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MOTION_OK, &CMotionPara::OnBnClickedBtnMotionOk)
	ON_BN_CLICKED(IDC_BTN_MOTION_CANCEAL, &CMotionPara::OnBnClickedBtnMotionCanceal)
	ON_BN_CLICKED(IDC_BTN_MOTION_APPLY, &CMotionPara::OnBnClickedBtnMotionApply)
END_MESSAGE_MAP()


// CMotionPara 消息处理程序


BOOL CMotionPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}



void CMotionPara::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

	// m_bShowDlg = FALSE;

	CDialogEx::OnCancel();
}
 

void CMotionPara::OnBnClickedBtnMotionOk()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);

	EndDialog(-1);
}


void CMotionPara::OnBnClickedBtnMotionCanceal()
{
	// TODO:  在此添加控件通知处理程序代码

	EndDialog(-1);
}


void CMotionPara::OnBnClickedBtnMotionApply()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData(TRUE);
}

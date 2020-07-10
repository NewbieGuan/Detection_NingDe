// MotionPara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Detection.h"
#include "MotionPara.h"
#include "afxdialogex.h"


// CMotionPara �Ի���

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


// CMotionPara ��Ϣ�������


BOOL CMotionPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}



void CMotionPara::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���

	// m_bShowDlg = FALSE;

	CDialogEx::OnCancel();
}
 

void CMotionPara::OnBnClickedBtnMotionOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);

	EndDialog(-1);
}


void CMotionPara::OnBnClickedBtnMotionCanceal()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	EndDialog(-1);
}


void CMotionPara::OnBnClickedBtnMotionApply()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
}

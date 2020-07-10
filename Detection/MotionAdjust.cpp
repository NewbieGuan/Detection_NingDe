// MotionAdjust.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Detection.h"
#include "MotionAdjust.h"
#include "afxdialogex.h"

#include "MPC2810.h"

long glTotalCards;
long glTotalAxes;

// CMotionAdjust �Ի���

IMPLEMENT_DYNAMIC(CMotionAdjust, CDialogEx)

CMotionAdjust::CMotionAdjust(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotionAdjust::IDD, pParent)
	, m_sDistanceCamera(_T(""))
	, m_sDistancePlatform(_T(""))
	, m_sSpeedCamera(_T(""))
	, m_sSpeedPlatform(_T(""))
	, m_offset_camera(_T("-950"))
	, m_offset_platform(_T("30"))
{
	
}

CMotionAdjust::~CMotionAdjust()
{
}

void CMotionAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_CAMERA, m_sDistanceCamera);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_PLATFORM, m_sDistancePlatform);
	DDX_Text(pDX, IDC_EDIT_SPEED_CAMERA, m_sSpeedCamera);
	DDX_Text(pDX, IDC_EDIT_SPEED_PLATFORM, m_sSpeedPlatform);
	DDX_Text(pDX, IDC_EDIT_OFFSET_CAMERA, m_offset_camera);
	DDX_Text(pDX, IDC_EDIT_OFFSET_PLATFORM, m_offset_platform);
}


BEGIN_MESSAGE_MAP(CMotionAdjust, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_POSITIVE_CAMERA, &CMotionAdjust::OnBnClickedRadioPositiveCamera)
	ON_BN_CLICKED(IDC_BTN_MOVE_CAMERA, &CMotionAdjust::OnBnClickedBtnMoveCamera)
	ON_BN_CLICKED(IDC_BTN_MOVE_PLATFORM, &CMotionAdjust::OnBnClickedBtnMovePlatform)
	ON_BN_CLICKED(IDC_RADIO_NEGATIVE_CAMERA, &CMotionAdjust::OnBnClickedRadioNegativeCamera)
	ON_BN_CLICKED(IDC_RADIO_POSITIVE_PLATFORM, &CMotionAdjust::OnBnClickedRadioPositivePlatform)
	ON_BN_CLICKED(IDC_RADIO_NEGATIVE_PLATFORM, &CMotionAdjust::OnBnClickedRadioNegativePlatform)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMotionAdjust::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_HOME_CAMERA, &CMotionAdjust::OnBnClickedBtnHomeCamera)
	ON_BN_CLICKED(IDC_BTN_HOME_PLATFORM, &CMotionAdjust::OnBnClickedBtnHomePlatform)
END_MESSAGE_MAP()


// CMotionAdjust ��Ϣ�������


BOOL CMotionAdjust::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	// ��ʼ���豸
	InitBoard();

	// �˶��������ó�ʼ��
	InitParam();
	SetRadioButton();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}



void CMotionAdjust::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���

	m_bShowDlg = FALSE;

	CDialogEx::OnCancel();		
}


int CMotionAdjust::InitBoard()
{

	///*
	// code start
	int i;

	//��ʼ���忨
	glTotalAxes = auto_set();			//�Զ�����
	if (glTotalAxes <= 0)
	{
		MessageBox(_T("��ⲻ����!"));
		return -1;
	}

	glTotalCards = init_board();		//��ʼ��
	if (glTotalCards <= 0)
	{
		MessageBox(_T("��ʼ������!"));
		return -1;
	}

	for (i = 0; i<glTotalAxes; i++)
	{
		set_sd_logic(i + 1, 0);
		set_el_logic(i + 1, 0);
		set_org_logic(i + 1, 0);
		set_alm_logic(i + 1, 0);
	}
	// code end
	//*/

	return 0;
}

void CMotionAdjust::InitParam()
{
	m_iDirectionCamera = IDC_RADIO_POSITIVE_CAMERA;        // ��ʼ������˶�����Ϊ����

	m_iDirectionPlatform = IDC_RADIO_POSITIVE_PLATFORM;    // ��ʼ��ƽ̨Ϊ�˶�����Ϊ��

	m_sSpeedCamera = "3000";         // ��ʼ������˶��ٶ�  3000 mm/min
	m_sDistanceCamera = "20";      // ��ʼ������˶�����   20 mm

	m_sSpeedPlatform = "3000";       // ��ʼ��ƽ̨�˶��ٶ�  600 mm/min
	m_sDistancePlatform = "20";    // ��ʼ��ƽ̨�˶�����    20 mm


	CString strPluse;
	::GetPrivateProfileString(_T("SaveInfo"), _T("���嵱��"), strPluse, strPluse.GetBuffer(MAX_PATH), MAX_PATH, _T("..\\SystemPara.ini"));
	GetDlgItem(IDC_EDIT_MODIFY_MOTION_PARA_PULSE)->SetWindowText(strPluse);
	m_steps_mm = _ttoi(strPluse); //200

	// ���ÿת��Ҫ��������   ע�⻻�����嵱��
	set_steps_pr(1, 10 * m_steps_mm);     // ��1   ���
	set_steps_pr(2, 10 * m_steps_mm);     // ��2   ƽ̨


	// /*
	// -----------------�������λ����ƽ��--------------------
	// ��1
	enable_el(1, 1);    // ��1��λ
	set_el_logic(1, 1); // ��1��λ�ߵ�ƽ��Ч
	// ��2
	enable_el(2, 1);    // ��2��λ
	set_el_logic(2, 1); // ��2��λ�ߵ�ƽ��Ч
	// */

	///*
	// ���ԭ�㡢��ƽ
	// ��1
	enable_org(1, 1);    // ��1ԭ��
	set_org_logic(1, 1); // ��1ԭ��ߵ�ƽ
	// ��2
	enable_org(2, 1);    // ��2ԭ��
	set_org_logic(2, 1); // ��2�˵�ߵ�ƽ
	//*/

	UpdateData(FALSE);

}


// ���õ�ѡ��ť״̬
void CMotionAdjust::SetRadioButton()
{
	CheckRadioButton(IDC_RADIO_POSITIVE_CAMERA, IDC_RADIO_NEGATIVE_CAMERA, m_iDirectionCamera);
	CheckRadioButton(IDC_RADIO_POSITIVE_PLATFORM, IDC_RADIO_NEGATIVE_PLATFORM, m_iDirectionPlatform);
}



void CMotionAdjust::OnBnClickedRadioPositiveCamera()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_iDirectionCamera = IDC_RADIO_POSITIVE_CAMERA;

	SetRadioButton();
}


void CMotionAdjust::OnBnClickedRadioNegativeCamera()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_iDirectionCamera = IDC_RADIO_NEGATIVE_CAMERA;

	SetRadioButton();
}


void CMotionAdjust::OnBnClickedRadioPositivePlatform()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_iDirectionPlatform = IDC_RADIO_POSITIVE_PLATFORM;

	SetRadioButton();

}



void CMotionAdjust::OnBnClickedRadioNegativePlatform()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	m_iDirectionPlatform = IDC_RADIO_NEGATIVE_PLATFORM;

	SetRadioButton();
}



// ------------------------������˶���ť��--------------------------
void CMotionAdjust::OnBnClickedBtnMoveCamera()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	// /*
	int stepCamera;
	int directionCamera;
	int speedCamera;

	// ��ȡ�˶�����
	UpdateData(TRUE);

	// ������
	stepCamera = ((int)_ttof(m_sDistanceCamera))*m_steps_mm;

	speedCamera = ((int)_ttof(m_sSpeedCamera)) / 60 * m_steps_mm;

	// �ж�������
	if (m_iDirectionCamera == IDC_RADIO_POSITIVE_CAMERA)
		directionCamera = 1;
	else
		directionCamera = -1;

	stepCamera = stepCamera*directionCamera;

	set_maxspeed(1, speedCamera);
	set_conspeed(1, speedCamera);
	set_profile(1, speedCamera / 6, speedCamera, speedCamera*20);

	// con_pmove(1, stepCamera);   // �����˶�
	fast_pmove(1, stepCamera);     // �����˶�
	// */


}


// ------------------------��ƽ̨�˶���ť��--------------------------
void CMotionAdjust::OnBnClickedBtnMovePlatform()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������


	int stepPlatform;
	int directionPlatform;
	int speedPlatform;

	// ��ȡ�˶�����
	UpdateData(TRUE);

	// ������
	stepPlatform = ((int)_ttof(m_sDistancePlatform))*m_steps_mm;

	speedPlatform = ((int)_ttof(m_sSpeedPlatform)) / 60 * m_steps_mm;

	// �ж�������
	if (m_iDirectionPlatform == IDC_RADIO_POSITIVE_PLATFORM)
		directionPlatform = 1;
	else
		directionPlatform= -1;

	stepPlatform = stepPlatform*directionPlatform;

	set_maxspeed(2, speedPlatform);
	set_conspeed(2, speedPlatform);
	set_profile(2, speedPlatform / 2, speedPlatform, speedPlatform / 2);

	con_pmove(2, stepPlatform);

}



// ----------------------------����ͣ��----------------------------
void CMotionAdjust::OnBnClickedBtnStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	// ������Ҫ�ж��ĸ������˶���Ȼ��ʹ�Ǹ���ֹͣ
	if(check_done(1))
		sudden_stop(1);
	if(check_done(2))
		sudden_stop(2);
}


void CMotionAdjust::OnBnClickedBtnHomeCamera()
{
	int stepCamera;
	int speedCamera;
	int flag;
	// ��ȡ�˶�����
	UpdateData(TRUE);
	check_done(0);
	// ������
	stepCamera = ((int)_ttof(m_offset_camera))*m_steps_mm;
	speedCamera = ((int)_ttof(m_sSpeedCamera)) / 60 * m_steps_mm;
	set_home_mode(1, 0); //���� 1 ���ԭ��ģʽ 0
	set_conspeed(1, speedCamera); //���û����ٶ�
	con_hmove(1, 1); //���� 1 ����������˶�
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(1);
		Sleep(10);
	}
	Sleep(100);
	con_pmove(1, stepCamera);
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(1);
		Sleep(10);
	}
	MessageBox(_T("����������!"));
	reset_pos(1);
}


void CMotionAdjust::OnBnClickedBtnHomePlatform()
{
	int stepPlatform;
	int speedPlatform;
	int flag;
	// ��ȡ�˶�����
	UpdateData(TRUE);
	check_done(0);
	// ������
	stepPlatform = ((int)_ttof(m_offset_platform))*m_steps_mm;
	speedPlatform = ((int)_ttof(m_sSpeedPlatform)) / 60 * m_steps_mm;
	set_home_mode(2, 0); //���� 2 ���ԭ��ģʽ 0
	set_conspeed(2, speedPlatform); //���û����ٶ�
	con_hmove(2, -1); //���� 2 �Ḻ������˶�
	Sleep(100);
	flag = 1;
	while (flag==1)
	{
		flag=check_done(2);
		Sleep(10);
	}
	Sleep(100);
	con_pmove(2, stepPlatform);
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(2);
		Sleep(10);
	}
	MessageBox(_T("ƽ̨�������!"));
	reset_pos(2);
}

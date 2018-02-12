/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// DlgPanel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EasyPusher.h"
#include "DlgPanel.h"
#include "afxdialogex.h"
#include "EasyPusherDlg.h"

// CDlgPanel �Ի���

IMPLEMENT_DYNAMIC(CDlgPanel, CEasySkinDialog)

CDlgPanel::CDlgPanel(CWnd* pParent /*=NULL*/)
	: CEasySkinDialog(CDlgPanel::IDD, pParent)
{
	pDlgVideo		=	NULL;
	m_pManager = NULL;
	m_nWndId = -1;
	m_pEdtServerIP = NULL;		
	m_pEdtServerPort = NULL;		
	m_pEdtServerStream = NULL;		
	m_pEditStartTime = NULL;
	m_pEditEndTime = NULL;

	m_pCmbType = NULL;//ֱ��/������ѡ
	m_pCmbSourceType = NULL;//Դ����ѡ��
	m_pCmbCamera = NULL;
	m_pCmbMic = NULL;
	m_pCmbScreenMode = NULL;
	m_pEdtRtspStream = NULL;		
	m_pMainDlg = NULL;
	m_sAVCapParamInfo.nVWidth = 640;
	m_sAVCapParamInfo.nVHeight = 480;
	m_sAVCapParamInfo.nFps = 25;
	m_sAVCapParamInfo.nBitrate = 2048;
	strcpy_s(m_sAVCapParamInfo.strColorFormat, "YUY2");
	m_sAVCapParamInfo.nASampleRate = 44100;
	m_sAVCapParamInfo.nAChannels = 2;
	memset(&m_sSourceInfo, 0x00, sizeof(SourceConfigInfo));
	m_pOrderRecord = NULL;
}

CDlgPanel::~CDlgPanel()
{
}

void CDlgPanel::SetMainDlg(CEasyPusherDlg* pMainDlg, int nId)
{
	m_pMainDlg = pMainDlg;
	m_nWndId = nId;
}

void CDlgPanel::LoadSourceConfigInfo( SourceConfigInfo*  pSourceInfo )
{
	memcpy(&m_sSourceInfo, pSourceInfo, sizeof(SourceConfigInfo));
}


void CDlgPanel::DoDataExchange(CDataExchange* pDX)
{
	CEasySkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
}


BEGIN_MESSAGE_MAP(CDlgPanel, CEasySkinDialog)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDlgPanel::OnBnClickedButtonStart)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_PANNEL_TYPE, &CDlgPanel::OnCbnSelchangeComboPannelType)
	ON_CBN_SELCHANGE(IDC_COMBO_PANNEL_SOURCE, &CDlgPanel::OnCbnSelchangeComboPannelSource)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO_CAPSCREEN_MODE, &CDlgPanel::OnCbnSelchangeComboCapscreenMode)
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(MSG_ORDER_RUN, OnOrderRun)
END_MESSAGE_MAP()


// CDlgPanel ��Ϣ�������

BOOL CDlgPanel::OnInitDialog()
{
	CEasySkinDialog::OnInitDialog();

	pDlgVideo = new CDlgVideo();
	pDlgVideo->SetMainDlg(this);

	pDlgVideo->Create(IDD_DIALOG_VIDEO, this);
	pDlgVideo->ShowWindow(SW_SHOW);

	__CREATE_WINDOW(m_pEdtServerIP, CEdit , IDC_EDIT_PANNEL_SERVER_IP );
	__CREATE_WINDOW(m_pEdtServerPort, CEdit , IDC_EDIT_PANNEL_SERVER_PORT );
	__CREATE_WINDOW(m_pEdtServerStream, CEdit , IDC_EDIT_PANNEL_SERVER_STREAM );
	__CREATE_WINDOW(m_pEdtRtspStream, CEdit , IDC_EDIT_PANNEL_RTSP );
	__CREATE_WINDOW(m_pEditStartTime, CEdit , IDC_EDIT_STARTTIME );
	__CREATE_WINDOW(m_pEditEndTime, CEdit , IDC_EDIT_ENDTIME );
	
	__CREATE_WINDOW(m_pCmbType, CComboBox , IDC_COMBO_PANNEL_TYPE );
	__CREATE_WINDOW(m_pCmbSourceType, CComboBox , IDC_COMBO_PANNEL_SOURCE );
	__CREATE_WINDOW(m_pCmbCamera, CComboBox , IDC_COMBO_PANNEL_CAMERA );
	__CREATE_WINDOW(m_pCmbMic, CComboBox , IDC_COMBO_PANNEL_MIC );
	__CREATE_WINDOW(m_pCmbScreenMode, CComboBox , IDC_COMBO_CAPSCREEN_MODE );
	
	//����Ƥ��
	UpdataResource();

	CString strIniPath = (CString)m_sSourceInfo.strFilePath;
	if (m_pEditStartTime)
	{
		CString strStartTime = _T("");
		strStartTime.Format(_T("%d"), m_sSourceInfo.nStartTime );
		m_pEditStartTime->SetWindowTextW( strStartTime );
	}
	if (m_pEditEndTime)
	{
		CString strEndTime = _T("");
		strEndTime.Format(_T("%d"), m_sSourceInfo.nEndTime );
		m_pEditEndTime->SetWindowTextW( strEndTime );
	}
	//��ʼ���ؼ�����
	if (NULL != m_pEdtRtspStream)
	{
		CString strPath = _T("");
		//strPath = 	m_sSourceInfo.strFilePath;
		m_pEdtRtspStream->SetWindowTextW(strPath);
	}
	if (m_pEdtServerIP)
	{
		m_pEdtServerIP->SetWindowTextW(TEXT("cloud.easydarwin.org"));
	}
	if (m_pEdtServerPort)
	{
		m_pEdtServerPort->SetWindowTextW(TEXT("554"));
	}
	if (m_pEdtServerStream)
	{
		m_pEdtServerStream->SetWindowTextW(TEXT("stream.sdp"));
	}

	if (!m_pManager)
	{
		m_pManager = new CSourceManager();
		m_pManager->InitSource();
		m_pManager->SetMainDlg(m_pMainDlg);
	}
// 	if (m_pManager)
// 	{
// 		m_pManager->EnumLocalAVDevInfo(m_pCmbCamera, m_pCmbMic);
// 	}
	if (m_pMainDlg)
	{
		m_pMainDlg->GetLocalDevInfo(m_pCmbCamera, m_pCmbMic);
		m_pCmbCamera->SetCurSel(0); 
		m_pCmbMic->SetCurSel(0); 
	}
	if (m_pCmbType)
	{
		m_pCmbType->AddString(_T("����"));
		m_pCmbType->AddString(_T("����"));
		m_pCmbType->SetCurSel(m_sSourceInfo.nPannelType);
	}
	if (m_pCmbSourceType)
	{
		m_pCmbSourceType->AddString(_T("��������Ƶ�ɼ�"));
		m_pCmbSourceType->AddString(_T("����RTSP���ɼ�"));
		//	pSouceCombo->AddString(_T("����Onvif���ɼ�"));
		m_pCmbSourceType->AddString(_T("��Ļ�ɼ�"));
		m_pCmbSourceType->AddString(_T("�ļ��ɼ�"));
		m_pCmbSourceType->SetCurSel(0);
	}
	if (m_pCmbScreenMode)
	{		
		m_pCmbScreenMode->AddString(_T("�̶�����"));
		m_pCmbScreenMode->AddString(_T("�Զ���"));
		m_pCmbScreenMode->AddString(_T("ȫ��"));
		//m_pCmbScreenMode->AddString(_T("������ƶ�"));
		m_pCmbScreenMode->SetCurSel(0);

	}
	int nSelType = m_pCmbType->GetCurSel();
	if (nSelType == 0)//����
	{
// 		m_pEdtServerIP->ShowWindow(SW_SHOW);
// 		m_pEdtServerPort->ShowWindow(SW_SHOW);
// 		m_pEdtServerStream->ShowWindow(SW_SHOW);
		m_pCmbSourceType->ShowWindow(SW_SHOW);
		int nSelSourceType  = 	m_pCmbSourceType->GetCurSel();
		if (nSelSourceType == 0)//��������Ƶ
		{
			m_pCmbCamera->ShowWindow(SW_SHOW);
			m_pCmbMic->ShowWindow(SW_SHOW);
			m_pEdtRtspStream->ShowWindow(SW_HIDE);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
		// 		m_edtVdieoWidth.ShowWindow(SW_SHOW);
			// 		m_edtVideoHeight.ShowWindow(SW_SHOW);
			// 		m_edtFPS.ShowWindow(SW_SHOW);
			// 		m_edtVideoBitrate.ShowWindow(SW_SHOW);
		} 
		else if(nSelType ==1 )//RTSP��
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_HIDE);
			m_pEdtRtspStream->ShowWindow(SW_SHOW);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
			// 		m_edtVdieoWidth.ShowWindow(SW_HIDE);
			// 		m_edtVideoHeight.ShowWindow(SW_HIDE);
			// 		m_edtFPS.ShowWindow(SW_HIDE);
			// 		m_edtVideoBitrate.ShowWindow(SW_HIDE);
		}
		else if (nSelType == 2)//��Ļ�ɼ�
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_SHOW);
			m_pEdtRtspStream->ShowWindow(SW_HIDE);
			m_pCmbScreenMode->ShowWindow(SW_SHOW);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
		}
		else if(nSelSourceType == 3 )//mp4�ļ���
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_HIDE);
			m_pEdtRtspStream->ShowWindow(SW_SHOW);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_SHOW);
			m_pEditEndTime->ShowWindow(SW_SHOW);
		}
	}
	else if (nSelType == 1)//ֱ��
	{
// 		m_pEdtServerIP->ShowWindow(SW_HIDE);
// 		m_pEdtServerPort->ShowWindow(SW_HIDE);
// 		m_pEdtServerStream->ShowWindow(SW_HIDE);
		m_pCmbSourceType->ShowWindow(SW_HIDE);

		//����Rtsp����ַ��Ϊֱ����ַ
		m_pCmbScreenMode->ShowWindow(SW_HIDE);
		m_pCmbCamera->ShowWindow(SW_HIDE);
		m_pCmbMic->ShowWindow(SW_HIDE);
		m_pEdtRtspStream->ShowWindow(SW_SHOW);
		m_pEditStartTime->ShowWindow(SW_HIDE);
		m_pEditEndTime->ShowWindow(SW_HIDE);
	}
	LoadOrderRecordInfo(strIniPath);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CDlgPanel::DestroyWindow()
{
	if (m_pManager)
	{
		m_pManager->RealseManager();
		delete m_pManager;
		m_pManager = NULL;
	}
	
	__DESTROY_WINDOW(pDlgVideo);

	return CEasySkinDialog::DestroyWindow();
}

CWnd* CDlgPanel::GetDlgVideo()
{
	if (!pDlgVideo)
	{
		return NULL;
	}
	return pDlgVideo;
}

void CDlgPanel::UpdataResource()
{

	HDC hParentDC = GetBackDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_ftSaticDefault.CreateFont(19,0,0,0,FW_NORMAL,0,FALSE,0,0,0,0,0,0,TEXT("΢���ź�"));
	//��ͼ
	m_btnStart.SetBackImage(TEXT("SkinUI\\ͼ��\\��ť����.png"),
		TEXT("SkinUI\\ͼ��\\��ťѡ��.png"),TEXT("SkinUI\\ͼ��\\��ťѡ��-2.png"),TEXT("SkinUI\\ͼ��\\��ť����.png"), &CRect(3,3,3,3));
	m_btnStart.SetButtonType(en_PushButton);//en_IconButton
	m_btnStart.SetParentBack(hParentDC);
	//m_btnLocalView.SetIconImage(TEXT("SkinUI\\Main\\Tools.png"));
	m_btnStart.SetWindowText(TEXT("Start"));
	m_btnStart.SetSize(39,24);//74,24
}

LRESULT CDlgPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
// 	if (WM_PAINT == message || WM_SIZE == message || WM_MOVE == message)
// 	{
// 		UpdateComponents();
// 	}

	return CEasySkinDialog::WindowProc(message, wParam, lParam);
}

void CDlgPanel::OnBnClickedButtonStart()
{
	if (m_pManager)
	{
		int nType = m_pCmbType->GetCurSel();
		if (nType == 0)
		{
			//�ɼ�
			CWnd* pCapWnd = GetDlgVideo();
			BOOL bInCap = m_pManager->IsInCapture();
			if (!bInCap)
			{
				CComboBox* pComboxMediaSource = m_pCmbSourceType;
				CComboBox* pVideoCombo = m_pCmbCamera ;
				CComboBox* pAudioCombo = m_pCmbMic ;
				CEdit* pEdtRtspSource = m_pEdtRtspStream;

				SOURCE_TYPE eType = (SOURCE_TYPE)pComboxMediaSource->GetCurSel();
				int nCamId = 0;
				int nAudioId = 0;
				char szURL[128] = {0,};
				CString strTemp = _T("");
				char szFilePath[MAX_PATH] = {0,};

				//��Ƶ��������
				int nWidth = m_sAVCapParamInfo.nVWidth;
				int nHeight = m_sAVCapParamInfo.nVHeight;
				int nFps = m_sAVCapParamInfo.nFps;
				int nBitrate = m_sAVCapParamInfo.nBitrate;
				char  szDataType[64];
				strcpy_s(szDataType, m_sAVCapParamInfo.strColorFormat )	;
				//��Ƶ��������
				int nASampleRate = m_sAVCapParamInfo.nASampleRate;
				int nAChannels =m_sAVCapParamInfo.nAChannels;
				int nStartTime = -1;
				int nStopTime = -1;
				bool bAutoLoop = false;

				if (eType == SOURCE_LOCAL_CAMERA)
				{
					nCamId = pVideoCombo->GetCurSel();
					nAudioId = pAudioCombo->GetCurSel();
					strTemp = _T("��������Ƶ�ɼ�");

				}
				else if (eType == SOURCE_SCREEN_CAPTURE)//��Ļ�ɼ�
				{
					nCamId = -1;
					nAudioId = pAudioCombo->GetCurSel();
					strTemp = _T("��Ļ�ɼ�");
					strcpy_s(szDataType , "RGB24");

					int nRet =m_pManager->GetScreenCapSize(nWidth, nHeight);
					if (nRet<1)
					{
						m_pManager->LogErr(_T("��Ļ�ɼ���ȡ����ʧ�ܣ�����Ԥ��ʧ�ܣ�"));
						return;
					}
				}
				else if(eType == SOURCE_FILE_STREAM)//�ļ����ɼ�
				{
					nCamId = -1;
					nAudioId = -1;
					strTemp = _T("MP4�ļ��ɼ�");
									
					wchar_t wszFilePath[128] = {0,};
					if (NULL != pEdtRtspSource)
						pEdtRtspSource->GetWindowTextW(wszFilePath, sizeof(wszFilePath));
					if (wcslen(wszFilePath) < 1)//��ǰ·��Ϊ��		
					{
						CString strPath =	OpenMp4File();
						if(!strPath.IsEmpty() )
						{
							wcscpy( wszFilePath , strPath.GetBuffer(strPath.GetLength()) );
						}
					}	
					__WCharToMByte(wszFilePath, szFilePath, sizeof(szFilePath)/sizeof(szFilePath[0]));

					wchar_t wszStartTime[128] = {0,};
					wchar_t wszEndTime[128] = {0,};
					char szStartTime[128] = {0,};
					char szEndTime[128] = {0,};

					if (m_pEditStartTime)
					{
						m_pEditStartTime->GetWindowTextW(wszStartTime, sizeof(wszStartTime));
						if (wcslen(wszStartTime)  > 0)//��ǰΪ��		
						{
							__WCharToMByte(wszStartTime, szStartTime, sizeof(szStartTime)/sizeof(szStartTime[0]));
							nStartTime = atoi( szStartTime );
						}
					} 

					if (m_pEditEndTime)
					{
						m_pEditEndTime->GetWindowTextW(wszEndTime, sizeof(wszEndTime));
						if (wcslen(wszEndTime)  > 0)//��ǰΪ��	
						{
							__WCharToMByte(wszEndTime, szEndTime, sizeof(szEndTime)/sizeof(szEndTime[0]));
							nStopTime = atoi( szEndTime );
						}
					} 				
				}
				else
				{
					//Start
					wchar_t wszURL[128] = {0,};
					if (NULL != pEdtRtspSource)
						pEdtRtspSource->GetWindowTextW(wszURL, sizeof(wszURL));
					if (wcslen(wszURL) < 1)		return;

					CString strURL = wszURL;
					CString strRTSP = strURL.Mid(0,4);
					if (strRTSP!=_T("rtsp")&&strRTSP!=_T("RTSP"))
					{
						strURL = _T("rtsp://")+strURL;
					}

					__WCharToMByte(strURL, szURL, sizeof(szURL)/sizeof(szURL[0]));
					strTemp = _T("��������Ƶ���ɼ�");
				}

				int nRet = m_pManager->StartCapture( eType,  nCamId, nAudioId, pCapWnd->GetSafeHwnd(),  szFilePath, -1, -1, bAutoLoop, szURL, nWidth, nHeight, nFps,nBitrate, szDataType, nASampleRate , nAChannels );
				if (nRet>0)
				{
					strTemp +=_T("�ɹ���"); 
					m_pManager->LogErr(strTemp);
				} 
				else
				{
					strTemp +=_T("ʧ�ܣ�"); 
					m_pManager->LogErr(strTemp);
					return;
				}

				//����
				//��ȡ����������ַ��Ϣ
				ServerURLInfo URLInfo;
				memset(&URLInfo, 0, sizeof(ServerURLInfo));
				if (m_pMainDlg)
				{
					m_pMainDlg->	GetPushServerInfo(&URLInfo);
				}

				bool bPushRtmp = false;
				//�����Ƹ�ʽ��
				FormatStreamName(URLInfo.sdpName);
				nRet = m_pManager->StartPush(URLInfo.pushServerAddr, URLInfo.pushServerPort,URLInfo.sdpName, URLInfo.nPushBufferLenth, bPushRtmp);
				CString strMsg = _T("");
				CString strIp;
				CString strName;
				strIp = URLInfo.pushServerAddr;
				strName = URLInfo.sdpName;
				if (nRet>=0)
				{
					strMsg.Format(_T("����EasyDarwin������URL��rtsp://%s:%d/%s �ɹ���"), strIp , URLInfo.pushServerPort, strName);
					m_pManager->LogErr(strMsg);
					m_btnStart.SetWindowText(TEXT("Stop"));
					if (bPushRtmp)
					{
						strMsg.Format(_T("����RTMP�������ɹ�������RTMP������URL��rtmp://%s:1935/live/%s �ɹ���"), strIp , strName);
						m_pManager->LogErr(strMsg);
					}
				} 
				else
				{
					strMsg.Format(_T("����EasyDarwin������URL��rtsp://%s:%d/%s ʧ�ܣ�"), strIp, URLInfo.pushServerPort,strName);
					m_pManager->LogErr(strMsg);
					if (bPushRtmp)
					{
						if (nRet == -1)
							strMsg.Format(_T("����RTMP������ʧ��!"), strIp , strName);
						else
							strMsg.Format(_T("����RTMP������URL��rtmp://%s:1935/live/%s ʧ�ܣ�"), strIp , strName);
						m_pManager->LogErr(strMsg);
					}
				}
				if(eType == SOURCE_FILE_STREAM)//�ļ����ɼ�
				{
					m_pManager->StartFileCapture();
				}
			}
			else
			{
				m_pManager->LogErr(_T("ֹͣ���ͣ�"));
				m_pManager->StopPush();

				m_pManager->StopCapture();
				m_btnStart.SetWindowText(TEXT("Start"));
				m_pManager->LogErr(_T("����Ԥ��ֹͣ"));

				pCapWnd->Invalidate();	
			}
		} 
		else //ֱ��
		{
			//RTSP
			CWnd* pPlayWnd = GetDlgVideo();
			BOOL bInPlay = m_pManager->IsInPlaying();
			if (!bInPlay)
			{
				char szIp[128] = {0,};
				wchar_t wszIp[128] = {0,};

				if (NULL != m_pEdtRtspStream)	
					m_pEdtRtspStream->GetWindowTextW(wszIp, sizeof(wszIp));
				if (wcslen(wszIp) < 1)		
					return;
				__WCharToMByte(wszIp, szIp, sizeof(szIp)/sizeof(szIp[0]));

				int nRet = m_pManager->StartPlay(szIp, pPlayWnd->GetSafeHwnd());
				m_btnStart.SetWindowText(TEXT("Stop"));
				CString strMsg = _T("");
				if (nRet>0)
				{
					strMsg.Format(_T("ֱ��Ԥ��URL��%s �ɹ���"), wszIp);
				} 
				else
				{
					strMsg.Format(_T("ֱ��Ԥ��URL��%s ʧ�ܣ�"), wszIp);
				}
				m_pManager->LogErr(strMsg);
			}
			else
			{
				m_pManager->StopPlay();
				m_btnStart.SetWindowText(TEXT("Start"));
				pPlayWnd->Invalidate();	
				m_pManager->LogErr(_T("ֱֹͣ��"));
			}
		}
	}
}


void CDlgPanel::OnSize(UINT nType, int cx, int cy)
{
	CEasySkinDialog::OnSize(nType, cx, cy);
	UpdateComponents();
	if (m_pManager)
	{
		m_pManager->ResizeVideoWnd();
	}
}

void CDlgPanel::UpdateComponents()
{
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	int nTop = rcClient.top;
	int nLeft = rcClient.left;
	int nBottom = rcClient.bottom-30;
	int nRight = rcClient.right;
// 	//������Ƶ���ڱ�������
// 	double dbWScale = (double)16/9;
// 	double dbHScale = (double)4/3;
// 	double dbRealScale = ((double)(rcClient.right-rcClient.left)/(double)(rcClient.bottom-30-rcClient.top));
// 	if (dbRealScale>dbWScale)
// 	{
// 		int nRealWidth = dbWScale*(rcClient.bottom-30-rcClient.top);
// 		nLeft = ((rcClient.right-rcClient.left)-nRealWidth)/2;
// 		nRight =  nRealWidth+nLeft;
// 	}
// 	else if (dbRealScale<dbHScale)
// 	{
// 		int nRealHeight = (rcClient.right-rcClient.left)/dbHScale;
// 		nTop = ((rcClient.bottom-30-rcClient.top)-nRealHeight)/2;
// 		nBottom =  nRealHeight+nTop;
// 	}
	CRect	rcVideo;
	rcVideo.SetRect(nLeft, nTop, nRight, nBottom);

	__MOVE_WINDOW(pDlgVideo, rcVideo);
	if (pDlgVideo)
	{
		pDlgVideo->Invalidate(FALSE);
	}
	int nStartH = rcClient.bottom-30;
	CRect rcCtrl;
	//����ѡ��
	rcCtrl.SetRect(30, nStartH+5, 30+60,  nStartH+5+25);
	__MOVE_WINDOW(m_pCmbType, rcCtrl);
	if (m_pCmbType)
	{
		m_pCmbType->SetFocus();
	}

	//Դ����ѡ��
	rcCtrl.SetRect(80+10, nStartH+5, 80+10+75,  nStartH+5+25);
	__MOVE_WINDOW(m_pCmbSourceType, rcCtrl);
	if (m_pCmbSourceType)
	{
		m_pCmbSourceType->SetFocus();
	}
	//��ʼʱ��
	//IDC_EDIT_STARTTIME
	rcCtrl.SetRect(80+10+75+3, nStartH+3, 80+10+75+53,  nStartH+3+25);
	__MOVE_WINDOW(m_pEditStartTime, rcCtrl);
	if (m_pEditStartTime)
	{
		m_pEditStartTime->SetFocus();
	}

	//����ʱ��
	//IDC_EDIT_ENDTIME
	rcCtrl.SetRect(80+10+75+54, nStartH+3, 80+10+75+103,  nStartH+3+25);
	__MOVE_WINDOW(m_pEditEndTime, rcCtrl);
	if (m_pEditEndTime)
	{
		m_pEditEndTime->SetFocus();
	}

	//Rtsp����ַ
	rcCtrl.SetRect(180+90, nStartH+3, rcClient.right-42,  nStartH+3+25);
	__MOVE_WINDOW(m_pEdtRtspStream, rcCtrl);
	if (m_pEdtRtspStream)
	{
		m_pEdtRtspStream->SetFocus();
	}

	int nCmbWidth = (rcClient.right-10-215-42)/2;
	//Camera
	rcCtrl.SetRect(215+10, nStartH+5, 215+10+nCmbWidth,  nStartH+5+25);
	__MOVE_WINDOW(m_pCmbCamera, rcCtrl);
	if (m_pCmbCamera)
	{
		m_pCmbCamera->SetFocus();
	}
	//Capture Screen
	rcCtrl.SetRect(215+10, nStartH+5, 215+10+nCmbWidth,  nStartH+5+25);
	__MOVE_WINDOW(m_pCmbScreenMode, rcCtrl);
	if (m_pCmbScreenMode)
	{
		m_pCmbScreenMode->SetFocus();
	}
	
	//Mic
	rcCtrl.SetRect(215+10+nCmbWidth+5, nStartH+5, rcClient.right-42,  nStartH+5+25);
	__MOVE_WINDOW(m_pCmbMic, rcCtrl);
	if (m_pCmbMic)
	{
		m_pCmbMic->SetFocus();
	}

	rcCtrl.SetRect(rcClient.right-41, nStartH+3, rcClient.right-2,  nStartH+3+24);
	if (m_btnStart.GetSafeHwnd())
	{
		m_btnStart.MoveWindow(rcCtrl);
	}

// 	//EasyDarwin���������Ͳ�������
// 	//IP
// 	rcCtrl.SetRect(10+150, nStartH+36, rcClient.right-200,  nStartH+36+25);
// 	__MOVE_WINDOW(m_pEdtServerIP, rcCtrl);
// 
// 	//Port
// 	rcCtrl.SetRect(rcClient.right-200+36, nStartH+40, rcClient.right-200+80,  nStartH+36+25);
// 	__MOVE_WINDOW(m_pEdtServerPort, rcCtrl);
// 	//Stream
// 	rcCtrl.SetRect(rcClient.right-200+130, nStartH+36, rcClient.right-10,  nStartH+36+25);
// 	__MOVE_WINDOW(m_pEdtServerStream, rcCtrl);

	Invalidate(FALSE);

}


void CDlgPanel::DrawClientArea( CDC*pDC,int nWidth,int nHeight )
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(0,0,nWidth,nHeight,COLOR_BACK);
	CRect	rcClient;
	GetClientRect(&rcClient);
	if (rcClient.IsRectEmpty())		return;

	//��������
	pDC->FillSolidRect(0,nHeight-30,nWidth,1,RGB(100,100,100));

	CFont *pOldFont=pDC->SelectObject(&m_ftSaticDefault);

	int nStartH = rcClient.bottom-30;

	CRect rcPosition;
	rcPosition.SetRect(0 , nStartH+2, 30, nStartH+25 );
	CString strWndId = _T("");
	strWndId.Format(_T("[%d]"), m_nWndId);
	pDC->DrawText(strWndId,rcPosition,DT_CENTER| DT_VCENTER |DT_SINGLELINE|DT_END_ELLIPSIS);

	pDC->SelectObject(pOldFont);  
}

void CDlgPanel::OnCbnSelchangeComboPannelType()
{
	if (m_pManager)
	{
		m_pManager->StopScreenCapture();
	}

	int nSelType = m_pCmbType->GetCurSel();
	if (nSelType == 0)//����
	{
		// 		m_pEdtServerIP->ShowWindow(SW_SHOW);
		// 		m_pEdtServerPort->ShowWindow(SW_SHOW);
		// 		m_pEdtServerStream->ShowWindow(SW_SHOW);
		m_pCmbSourceType->ShowWindow(SW_SHOW);
		int nSelSourceType  = 	m_pCmbSourceType->GetCurSel();
		if (nSelSourceType == 0)//��������Ƶ
		{
			m_pCmbCamera->ShowWindow(SW_SHOW);
			m_pCmbMic->ShowWindow(SW_SHOW);
			m_pEdtRtspStream->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);
			// 		m_edtVdieoWidth.ShowWindow(SW_SHOW);
			// 		m_edtVideoHeight.ShowWindow(SW_SHOW);
			// 		m_edtFPS.ShowWindow(SW_SHOW);
			// 		m_edtVideoBitrate.ShowWindow(SW_SHOW);
		} 
		else if(nSelType ==1 )//RTSP��
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
			m_pEdtRtspStream->ShowWindow(SW_SHOW);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);

			// 		m_edtVdieoWidth.ShowWindow(SW_HIDE);
			// 		m_edtVideoHeight.ShowWindow(SW_HIDE);
			// 		m_edtFPS.ShowWindow(SW_HIDE);
			// 		m_edtVideoBitrate.ShowWindow(SW_HIDE);
		}
		else if (nSelType == 2)//��Ļ�ɼ�
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_SHOW);
			m_pEdtRtspStream->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_HIDE);
			m_pEditEndTime->ShowWindow(SW_HIDE);
			m_pCmbScreenMode->ShowWindow(SW_SHOW);
			int nScreenMode = m_pCmbScreenMode->GetCurSel();
			if (m_pManager)
			{
				m_pManager->StartScreenCapture( GetDlgVideo()->GetSafeHwnd(), nScreenMode);
			}
		}
		else if(nSelSourceType == 3 )//mp4�ļ���
		{
			m_pCmbCamera->ShowWindow(SW_HIDE);
			m_pCmbMic->ShowWindow(SW_HIDE);
			m_pEdtRtspStream->ShowWindow(SW_SHOW);
			m_pCmbScreenMode->ShowWindow(SW_HIDE);
			m_pEditStartTime->ShowWindow(SW_SHOW);
			m_pEditEndTime->ShowWindow(SW_SHOW);
		}

	}
	else if (nSelType == 1)//ֱ��
	{
		// 		m_pEdtServerIP->ShowWindow(SW_HIDE);
		// 		m_pEdtServerPort->ShowWindow(SW_HIDE);
		// 		m_pEdtServerStream->ShowWindow(SW_HIDE);
		m_pCmbSourceType->ShowWindow(SW_HIDE);

		//����Rtsp����ַ��Ϊֱ����ַ
		m_pCmbScreenMode->ShowWindow(SW_HIDE);
		m_pCmbCamera->ShowWindow(SW_HIDE);
		m_pCmbMic->ShowWindow(SW_HIDE);
		m_pEdtRtspStream->ShowWindow(SW_SHOW);
		m_pEditStartTime->ShowWindow(SW_HIDE);
		m_pEditEndTime->ShowWindow(SW_HIDE);
	}
}

void CDlgPanel::OnCbnSelchangeComboPannelSource()
{
	if (m_pManager)
	{
		m_pManager->StopScreenCapture();
	}
	int nSelSourceType  = 	m_pCmbSourceType->GetCurSel();
	if (nSelSourceType == 0)
	{
		m_pCmbCamera->ShowWindow(SW_SHOW);
		m_pCmbMic->ShowWindow(SW_SHOW);
		m_pEdtRtspStream->ShowWindow(SW_HIDE);
		m_pCmbScreenMode->ShowWindow(SW_HIDE);
		m_pEditStartTime->ShowWindow(SW_HIDE);
		m_pEditEndTime->ShowWindow(SW_HIDE);

		// 		m_edtVdieoWidth.ShowWindow(SW_SHOW);
		// 		m_edtVideoHeight.ShowWindow(SW_SHOW);
		// 		m_edtFPS.ShowWindow(SW_SHOW);
		// 		m_edtVideoBitrate.ShowWindow(SW_SHOW);
	} 
	else if(nSelSourceType ==1 )//RTSP��
	{
		m_pCmbCamera->ShowWindow(SW_HIDE);
		m_pCmbMic->ShowWindow(SW_HIDE);
		m_pEdtRtspStream->ShowWindow(SW_SHOW);
		m_pCmbScreenMode->ShowWindow(SW_HIDE);
		m_pEditStartTime->ShowWindow(SW_HIDE);
		m_pEditEndTime->ShowWindow(SW_HIDE);

		// 		m_edtVdieoWidth.ShowWindow(SW_HIDE);
		// 		m_edtVideoHeight.ShowWindow(SW_HIDE);
		// 		m_edtFPS.ShowWindow(SW_HIDE);
		// 		m_edtVideoBitrate.ShowWindow(SW_HIDE);
	}
	else if (nSelSourceType == 2)//��Ļ�ɼ�
	{
		m_pCmbCamera->ShowWindow(SW_HIDE);
		m_pCmbMic->ShowWindow(SW_SHOW);
		m_pEdtRtspStream->ShowWindow(SW_HIDE);
		m_pEditStartTime->ShowWindow(SW_HIDE);
		m_pEditEndTime->ShowWindow(SW_HIDE);
		m_pCmbScreenMode->ShowWindow(SW_SHOW);
		int nScreenMode = m_pCmbScreenMode->GetCurSel();
		if (m_pManager)
		{
			m_pManager->StartScreenCapture( GetDlgVideo()->GetSafeHwnd(), nScreenMode);
		}
	}
	else if(nSelSourceType == 3 )//mp4�ļ���
	{
		m_pCmbCamera->ShowWindow(SW_HIDE);
		m_pCmbMic->ShowWindow(SW_HIDE);
		m_pEdtRtspStream->ShowWindow(SW_SHOW);
		m_pCmbScreenMode->ShowWindow(SW_HIDE);
		m_pEditStartTime->ShowWindow(SW_SHOW);
		m_pEditEndTime->ShowWindow(SW_SHOW);
	}
}


BOOL CDlgPanel::OnEraseBkgnd(CDC* pDC)
{
	return CEasySkinDialog::OnEraseBkgnd(pDC);
}


//�����Ƹ�ʽ��
void CDlgPanel::FormatStreamName(char* sStreamName)
{
	CString strStream = _T("");
	CString strFormatStreamName = _T("");
	strStream = sStreamName;
	int nTPos = strStream.Find(_T(".sdp"));
	if(nTPos>=0)
	{
		strFormatStreamName = strStream.Mid(0, nTPos);
	}
	else
	{
		strFormatStreamName = strStream;
	}
	strStream.Format(_T("%s%d%s"), strFormatStreamName, m_nWndId, _T(".sdp") ); 
	char sName[128] =  {0,};
	__WCharToMByte(strStream, sName, sizeof(sName)/sizeof(sName[0]));
	strcpy_s(sStreamName, 64, sName);
}


void CDlgPanel::OnCbnSelchangeComboCapscreenMode()
{
	if (m_pManager)
	{
		m_pManager->StopScreenCapture();
	}
	int nScreenMode  = 	m_pCmbScreenMode->GetCurSel();
	if (m_pManager)
	{
		m_pManager->StartScreenCapture( GetDlgVideo()->GetSafeHwnd(), nScreenMode);
	}
// 	if (nSelSourceType == 0)
// 	{
// 	}
// 	else if (nSelSourceType == 1)
// 	{
// 	} 
// 	else if(nSelSourceType == 2)
// 	{
// 	}
}


void CDlgPanel::OnRButtonUp(UINT nFlags, CPoint point)
{
	CEasySkinDialog::OnRButtonUp(nFlags, point);
}

// void SetUseGpac(BOOL bUse)
// {
// 	m_bUseGpac = bUse;
// }
// void SetWriteMP4(BOOL bUse)
// {
// 	m_bWriteMp4 = bUse;
// }
// void SetUseFFEncoder(BOOL bUse)
// {
// 	m_bUseFFEncoder = bUse;
// }
// void SetPushRtmp(BOOL bUse)
// {
// 	m_bPushRtmp = bUse;
// }
void CDlgPanel::ProcessVideoWndMenuMsg(int nId)
{
	if (!m_pManager)
	{
		return;
	}
	switch (nId)
	{
	case 4000://��������
		{
			CSettingDlg settingDlg;
			settingDlg.PreSetting(m_sAVCapParamInfo);
			settingDlg.DoModal();
			settingDlg.SaveSetting(m_sAVCapParamInfo);
		}
		break;
	case 4001://¼��MP4
		{
			m_pManager->SetWriteMP4( !m_pManager->IsWriteMP4() );
		}
		break;
	case 4002://??? null
		{
		}
		break;	
	case 4003://����RTMP
		{
			m_pManager->SetPushRtmp(!m_pManager->IsPushRtmp());
		}
		break;
	case 4004://??? null
		{
		}
		break;
	case 3000://MP4BOX
		{
			m_pManager->SetUseGpac(TRUE);
		}
		break;
	case 3001://MP4Creater
		{
			m_pManager->SetUseGpac(FALSE);
		}
		break;
	case 3002://x264+AAC
		{
			m_pManager->SetUseFFEncoder(FALSE);
		}
		break;
	case 3003://FFEncoder
		{
			m_pManager->SetUseFFEncoder(TRUE);
		}
		break;

	}
}

//��MP4�ļ�
CString CDlgPanel::OpenMp4File()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,TEXT("mp4�ļ�;ts�ļ�|*.mp4;*.ts||"));
	if ( dlg.DoModal() == IDOK )
	{
		CString strFileName = dlg.GetPathName();
		GetDlgItem( IDC_EDIT_PANNEL_RTSP )->SetWindowText( strFileName );
		return strFileName;
	}
	return _T("");
}

// ����:LoadOrderRecordInfo
// ����:���붨ʱ������Ϣ
void CDlgPanel::LoadOrderRecordInfo(CString strOrderRcPath)
{	
	if(m_pOrderRecord!=NULL)
	{
		delete m_pOrderRecord;
		m_pOrderRecord=NULL;
	}
	m_pOrderRecord = new COrderRecord();
	if(m_pOrderRecord)
	{
		BOOL bUse= m_pOrderRecord->LoadOrderRecordData(strOrderRcPath);
		if(bUse==FALSE)
		{
			delete m_pOrderRecord;
			m_pOrderRecord=NULL;
			return ;
		}
		if(m_pOrderRecord->m_bStartLoadList==TRUE&&m_pOrderRecord->m_bUserOrderRecord==TRUE)
		{
			CString strPath=m_pOrderRecord->m_strOrderInfoPath;
			m_pOrderRecord->LoadOrderRecordList(strPath);
			int nCheckTimer=m_pOrderRecord->m_nCheckTimer;
			if(nCheckTimer>0)
			{
				SetTimer(4,nCheckTimer,NULL);
			}
		}
	}	
}

//���ƻ��б�
void CDlgPanel::CheckOrderList()
{
	if(m_pOrderRecord)
	{
		BOOL bInCap = m_pManager->IsInCapture();//0=���� 1=����
		int nActiveOrderId = -1;
		int nCheck = m_pOrderRecord->CheckIsListFlag(bInCap, nActiveOrderId);
		int nRet = -1;
		if(nCheck==1)//��ʼ����
		{
			CString sName;
			CString sStartTime;
			CString sEndTime ;
			CString strPath1(_T("")),strPath2(_T("")),strPath3(_T(""));
			nRet = m_pOrderRecord->GetCurOrderPlanInfo(sName, sStartTime, sEndTime, strPath1, strPath2, strPath3);
			//������ʾ�����ļ���·����Ϣ
			CString strLog = _T("");
			strLog.Format(_T("��⵽���ͼƻ�  name=%s start=%s  end=%s path=%s��\r\n"), sName, sStartTime, sEndTime, strPath1 );
			m_pManager->LogErr( strLog );
			if (m_pEditStartTime)
			{
				CString strStartTime = _T("");
				strStartTime.Format(_T("%s"), sStartTime );
				m_pEditStartTime->SetWindowTextW( strStartTime );
			}
			if (m_pEditEndTime)
			{
				CString strEndTime = _T("");
				strEndTime.Format(_T("%s"), sEndTime );
				m_pEditEndTime->SetWindowTextW( strEndTime );
			}
			//��ʼ���ؼ�����
			if (NULL != m_pEdtRtspStream)
			{
				CString strPath = _T("");
				strPath = 	strPath1;
				m_pEdtRtspStream->SetWindowTextW(strPath);
			}
			//��������
			//OnBnClickedButtonStart();
			PostMessage(MSG_ORDER_RUN);

// 			if (nActiveOrderId>-1)
// 			{	
// 				//���ͼƻ����п�ʼ�����Ϊ������״̬
// 				COrderRecordInfo tOrderRecordInfo;
// 				nRet = m_pOrderRecord->GetOrderRecInfoById(nActiveOrderId, tOrderRecordInfo);
// 				CString strCmdString = _T("");
// 				CString strName = tOrderRecordInfo.name;
// 				CString strproperty1 = tOrderRecordInfo.property1;
// 				CString strproperty2 = tOrderRecordInfo.property2;
// 				CString strproperty3 = tOrderRecordInfo.property3;
// 
// 				CString strBeginTime = tOrderRecordInfo.starttime.Format("%Y-%m-%d %H:%M:%S");
// 				CString strEndTime = tOrderRecordInfo.endtime.Format("%Y-%m-%d %H:%M:%S");
// 				if(tOrderRecordInfo.nState==1)
// 				{
// 					strBeginTime = tOrderRecordInfo.starttime.Format("%H:%M:%S");
// 					strEndTime = tOrderRecordInfo.endtime.Format("%H:%M:%S");
// 				}				
// 				CString strKeyValue = _T("");
// 				strKeyValue.Format(_T("%d@%s@%s@%s@%s@%s@%s@%d@%d"),tOrderRecordInfo.id, strName, strBeginTime, 
// 					strEndTime, strproperty1, strproperty2, strproperty3, tOrderRecordInfo.nState, tOrderRecordInfo.nRunState);
// 
// 				CString strKeyItem = _T("");
// 				strKeyItem.Format(_T("LB_OrderRecord_Plan%d"), 0);
// 				PackageStringCMD(strCmdString, _T("LB_OrderRecord_OptType"), 0);//ˢ���б�
// 				PackageStringCMD(strCmdString, _T("LB_OrderRecord_Count"), 1);
// 				PackageStringCMD(strCmdString, strKeyItem, strKeyValue);
// 				SendIOCPNetCmd(0x03, Actor_User, 0x04, 0x26, strCmdString, 
// 					strCmdString.GetLength(), 0);//�㲥״̬��Ϣ
//			}		
		}
		else if(nCheck==2)//ֹͣ¼��
		{
			//������ʾ�����ļ���·����Ϣ
			CString strLog = _T("��⵽���ͼƻ� ֹͣ��\r\n");

			m_pManager->LogErr( strLog );
			//��������
			BOOL bInCap = m_pManager->IsInCapture();//0=���� 1=����
			if(bInCap)
			{
				//OnBnClickedButtonStart();
				PostMessage(MSG_ORDER_RUN);
			}

			//���ͼƻ�ֹͣ�����Ϊ�����״̬��
			//���ͼƻ����п�ʼ�����Ϊ������״̬
// 			COrderRecordInfo tOrderRecordInfo;
// 			nRet = m_pOrderRecord->GetOrderRecInfoById(nActiveOrderId, tOrderRecordInfo);
// 			CString strCmdString = _T("");
// 			CString strName = tOrderRecordInfo.name;
// 			CString strproperty1 = tOrderRecordInfo.property1;
// 			CString strproperty2 = tOrderRecordInfo.property2;
// 			CString strproperty3 = tOrderRecordInfo.property3;
// 
// 			CString strBeginTime = tOrderRecordInfo.starttime.Format("%Y-%m-%d %H:%M:%S");
// 			CString strEndTime = tOrderRecordInfo.endtime.Format("%Y-%m-%d %H:%M:%S");
// 			if(tOrderRecordInfo.nState==1)
// 			{
// 				strBeginTime = tOrderRecordInfo.starttime.Format("%H:%M:%S");
// 				strEndTime = tOrderRecordInfo.endtime.Format("%H:%M:%S");
// 			}				

// 			CString strKeyValue = _T("");
// 			strKeyValue.Format(_T("%d@%s@%s@%s@%s@%s@%s@%d@%d"),tOrderRecordInfo.id, strName, strBeginTime, 
// 				strEndTime, strproperty1, strproperty2, strproperty3, tOrderRecordInfo.nState, tOrderRecordInfo.nRunState);
// 
// 			CString strKeyItem = _T("");
// 			strKeyItem.Format(_T("LB_OrderRecord_Plan%d"), 0);
// 			PackageStringCMD(strCmdString, _T("LB_OrderRecord_OptType"), 0);//ˢ���б�
// 			PackageStringCMD(strCmdString, _T("LB_OrderRecord_Count"), 1);
// 			PackageStringCMD(strCmdString, strKeyItem, strKeyValue);
// 			SendIOCPNetCmd(0x03, Actor_User, 0x04, 0x26, strCmdString, 
// 				strCmdString.GetLength(), 0);//�㲥״̬��Ϣ
		}
	}
}


void CDlgPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//¼�Ƽƻ���ʱ��
	if(nIDEvent==4)
	{
		CheckOrderList();
	}
	CEasySkinDialog::OnTimer(nIDEvent);
}


void CDlgPanel::OnDestroy()
{
	CEasySkinDialog::OnDestroy();

	if(m_pOrderRecord!=NULL)
	{
		delete m_pOrderRecord;
		m_pOrderRecord=NULL;
	}	
}

LRESULT CDlgPanel::OnOrderRun(WPARAM wparam, LPARAM lparam)
{
	OnBnClickedButtonStart();
	return 0;
}
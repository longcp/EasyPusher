/*
	Copyright (c) 2013-2014 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.EasyDarwin.org
*/
// EasyDarwin���ͺͽ���Դ�˵Ĺ����� [11/8/2015 Dingshuai]
// Add by SwordTwelve
#pragma once

#define EasyPusher_VersionInfo _T("Version:1.3.17.0524 Powered By SwordTwelve/Gavin/Arno")

// //������Ƶ����
#include "AudioSource\DirectSound.h"
//������Ƶ����
#include "VideoSource\USBCamera\CameraDS.h"
//��������Ƶ���ɼ�
#include "EasyPlayerManager.h"
//DShow����Ƶ�ɼ���ͷ�ļ���ӣ���Ӹÿ���Ҫ��Ϊ�˽������ƵԴͷ�ϲ�ͬ�������⣩
#include "./DShowCapture/DShowCaptureAudioAndVideo_Interface.h"
//��Ļ�ɼ�
#include "CaptureScreen.h"
#include "D3DCaptureScreem.h"
//MP4 file �ɼ�
#include "EasyFileCapture.h"

#include "./FFEncoder/FFEncoderAPI.h"
#pragma comment(lib, "./FFEncoder/FFEncoder.lib")
//MP4Box Package MP4
#include "EasyMP4Writer.h"

//x264����+FAAC����+librtmp���� ֧��
#include "./EasyEncoder/FAACEncoder.h"
#include "./EasyEncoder/H264Encoder.h"
#include "./EasyEncoder/H264EncoderManager.h"
#include "./EasyEncoder/EasyRtmp.h"

#include "nupusher_rtsp_api.h"
#include "nutypes.h"

extern "C"
{
//MP4Creater Package MP4
#include "./mp4creator/libmp4creator.h"
#pragma comment(lib, "./mp4creator/libMp4Creator.lib")
}

typedef struct tagPushServerURLInfo
{
	char	pushServerAddr[128];
	int		pushServerPort;
	char	sdpName[64];
	int nPushBufferLenth;
}ServerURLInfo;

typedef enum tagSOURCE_TYPE
{
	SOURCE_LOCAL_CAMERA = 0,//��������Ƶ
	SOURCE_RTSP_STREAM=1,//RTSP��
	SOURCE_SCREEN_CAPTURE =2,//��Ļ����
	SOURCE_FILE_STREAM = 3,       //�ļ�������(mp4)
	// 	//SOURCE_ONVIF_STREAM=3,//Onvif��

}SOURCE_TYPE;

typedef struct tagSourceConfigInfo
{
	int nId;
	int nPannelType;
	int nSourceType;
	int nFileType;
	int nStartTime;
	int nEndTime;
	char strFilePath[MAX_PATH];
}SourceConfigInfo;

typedef struct tagAVCapParamInfo
{
	int nVWidth;
	int nVHeight;
	int nFps; 
	int nBitrate;
	char strColorFormat[64];//��Ƶɫ�ʸ�ʽ
	int nASampleRate;//��Ƶ������
	int nAChannels;//��Ƶͨ����
}AVCapParamInfo;

class CEasyPusherDlg ;

class CSourceManager
{
public:
	CSourceManager(void);
	~CSourceManager(void);

public:
	static CSourceManager* s_pSourceManager; 
	//��ʼ��Ψһ�Ĺ���ʵ��
	static CSourceManager* Instance();
	//����Ψһ�Ĺ���ʵ��
	static void UnInstance();
	//�ͷ�Master��ռ�����Դ
	void RealseManager();
	// Member Function Interface
public:
	//ִ����ƵԴ��ʼ������
	void InitSource();
	//����ʼ��
	void UnInitSource();
	//DShowʵʱ���ݻص�����
	static int CALLBACK RealDataCallbackFunc(int nDevId, unsigned char *pBuffer, int nBufSize, 
																	   RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	void DSRealDataManager(int nDevId, unsigned char *pBuffer, int nBufSize, 
		RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);
	//RTSP�����ݻص�
	static int CALLBACK __MediaSourceCallBack( int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);
	int SourceManager(int _channelId, int *_channelPtr, int _frameType, char *pBuf, RTSP_FRAME_INFO* _frameInfo);

	static int CALLBACK CaptureScreenCallBack(int nId, unsigned char *pBuffer, int nBufSize,  RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo, void* pMaster);
	void CaptureScreenManager(int nId, unsigned char *pBuffer, int nBufSize,  RealDataStreamType realDataType, /*RealDataStreamInfo*/void* realDataInfo);

	static  unsigned int CALLBACK CaptureFileCallBack(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);
	void CaptureFileManager(int nDevId, EASY_AV_Frame* frameInfo, void* pMaster);

	int StartDSCapture(int nCamId, int nAudioId,HWND hShowWnd, int nVideoWidth, int nVideoHeight, int nFps, int nBitRate, char* szDataype = "YUY2", int nSampleRate = 44100, int nChannel = 2);
	//�ļ�Դ�ɼ�
	// 
	
	int InitFileCapture(char* sFilePath, int nStartTime, int nEndTime, bool bAutoLoop);
	void StartFileCapture();

	//��ʼ����(�ɼ�)
	int StartCapture(SOURCE_TYPE eSourceType, int nCamId, int nAudioId,  HWND hCapWnd, char* szFilePath = NULL, int nStartTime=-1, int nEndTime=-1, bool bAutoLoop=true,
		char* szURL = NULL, int nVideoWidth=640, int nVideoHeight=480, int nFps=25, int nBitRate=2048, char* szDataType = "YUY2",  
		int nSampleRate=44100, int nChannel=2 );
	//ֹͣ�ɼ�
	void StopCapture();

	//��ʼ����
	int StartPush(char* ServerIp, int nPushPort, char* sPushName, int nPushBufSize = 1024, bool bPushRtmp = false);
	//ֹͣ����
	void StopPush();
	
	//��ʼ����
	int StartPlay(char* szURL, HWND hShowWnd);
	//ֹͣ����
	void StopPlay();
	void SetMainDlg(	CEasyPusherDlg* pMainDlg);
	void LogErr(CString strLog);
	void EnumLocalAVDevInfo(CWnd* pComboxMediaSource, CWnd* pComboxAudioSource);
	void ResizeVideoWnd();

	//��Ļ�ɼ�
	int StartScreenCapture(HWND hShowWnd, int nCapMode);
	void StopScreenCapture();
	void RealseScreenCapture();
	int GetScreenCapSize(int& nWidth, int& nHeight);

	//дMP4�ļ�(¼�����)
	int CreateMP4Writer(char* sFileName, int nVWidth, int nVHeight, int nFPS, int nSampleRate, int nChannel, int nBitsPerSample, int nFlag=ZOUTFILE_FLAG_FULL, BOOL bUseGpac = FALSE	);
	int WriteMP4VideoFrame(unsigned char* pdata, int datasize, bool keyframe, long nTimestamp, int nWidth, int nHeight);
	int WriteMP4AudioFrame(unsigned char* pdata,int datasize, long timestamp);
	void CloseMP4Writer();	

	//״̬
	BOOL IsInCapture()
	{
		BOOL bCap = FALSE;
		int nStreamCap = m_netStreamCapture.InRunning();
		//int nVideoCap = m_videoCamera.InRunning();
		if (m_bDSCapture ||nStreamCap>0 )
			bCap = TRUE;
		else
			bCap = FALSE;
		return bCap ;
	}
	BOOL IsInPushing()
	{
		//return (int)m_sPushInfo.pusherHandle>0?TRUE:FALSE;
		return m_bPushing;
	}
	BOOL IsInPlaying()
	{
		return m_netStreamPlayer.InRunning()>0?TRUE:FALSE;
	}

	BOOL IsRecording()
	{
		return m_bRecording;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL IsUseGpac()
	{
		return m_bUseGpac;
	}
	BOOL IsWriteMP4()
	{
		return m_bWriteMp4;
	}
	BOOL IsUseFFEncoder()
	{
		return m_bUseFFEncoder;
	}
	BOOL IsPushRtmp()
	{
		return m_bPushRtmp;
	}
	//////////////////////////////////////////////////////////////////////////
	void SetUseGpac(BOOL bUse)
	{
		 m_bUseGpac = bUse;
	}
	void SetWriteMP4(BOOL bUse)
	{
		 m_bWriteMp4 = bUse;
	}
	void SetUseFFEncoder(BOOL bUse)
	{
		 m_bUseFFEncoder = bUse;
	}
	void SetPushRtmp(BOOL bUse)
	{
		 m_bPushRtmp = bUse;
	}

protected:
		void	UpdateLocalVideo(unsigned char *pbuf, int size, int width, int height);

	//Member Var
private:
	CEasyPusherDlg* m_pMainDlg;
	CDirectSound	m_audioCapture;
	CCameraDS		m_videoCamera;
	CCaptureScreen* m_pScreenCaptrue;
	CD3DCaptureScreem* m_pD3dScreenCaptrue;
	CEasyFileCapture* m_pEasyFileCapture;

	int m_nScreenCaptureId;
	//MP4Box Writer
	EasyMP4Writer* m_pMP4Writer;
	//MP4 Creater Handler
	MP4C_Handler m_handler;
	BOOL m_bUseGpac;
	BOOL m_bWriteMp4;
	BOOL m_bUseFFEncoder;
	BOOL m_bPushRtmp;

	//��Ƶ�豸����ʵ��
	LPVideoCapturer m_pVideoManager;
	//��Ƶ�豸����ʵ��
	LPAudioCapturer m_pAudioManager;

	//��������RTSP����������
	EasyPlayerManager m_netStreamCapture;
	//����EasyDarwin�Ƴ���RTSP�����в���
	EasyPlayerManager m_netStreamPlayer;

	//����Dshow�����������
	DEVICE_CONFIG_INFO m_sDevConfigInfo;
	NU_MEDIA_INFO_T   m_mediainfo;
	EASY_LOCAL_SOURCE_T m_sSourceInfo;
	EASY_LOCAL_SOURCE_T m_sPushInfo;
	EASY_LOCAL_SOURCE_T m_sPlayInfo;

	BOOL m_bDSCapture;
	D3D_HANDLE		m_d3dHandle;
	HWND m_hCaptureWnd;
	HWND m_hPlayWnd;
	BOOL m_bPushing;
	BOOL m_bRecording;
	//���������
	//FFEncoder --- Start
	FFE_HANDLE m_hFfeVideoHandle;
	FFE_HANDLE m_hFfeAudioHandle;
	int m_nFrameNum;
	char * m_EncoderBuffer;// = new char[1920*1080];	//���������ڴ�ռ�
	// FFEncoder --- End
	// x264+faac Encoder --- Start
	//AAC������
	FAACEncoder m_AACEncoderManager;
	//H264������
	CH264EncoderManager m_H264EncoderManager;
	//������Ϣ����
	Encoder_Config_Info*	m_pEncConfigInfo;
	byte m_sps[100];
	byte  m_pps[100];
	long m_spslen;
	long m_ppslen;
	byte* m_pFrameBuf; 
	// x264+faac Encoder---End
	EasyRtmp* m_pEasyrtmp;
};


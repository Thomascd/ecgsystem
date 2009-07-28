// ecgDlg.h : header file
//
#if !defined(AFX_ECGDLG_H__447C45F1_D8F4_4D1C_9035_B4A190E4633F__INCLUDED_)
#define AFX_ECGDLG_H__447C45F1_D8F4_4D1C_9035_B4A190E4633F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFFER_MAX	512
#define NORMALIZE	2048
#define REFVOL 2.5
#define SAMPLERES	4096
#define DOWNSAMPLING	1
#define STORE_BUFFER 512 //the sie of storing ecg signal

//Low viewer selection
#define VIEW_ALOG_DEBUG		0
#define VIEW_HRV_TIME		1
#define VIEW_HRV_FREQ		2
#define VIEW_FFT			3
#define VIEW_CAP_ENVE		4
#define VIEW_ECG_TEMPLETE	5

//Display mode,real time or off line
#define RT		1
#define OFFLINE	2
/////////////////////////////////////////////////////////////////////////////
// CEcgDlg dialog

#include "SerialPort.h"
#include "afxwin.h"
#include "afxcmn.h"
//#include "DrawWave.h"
#include "ECGSignalProcessing.h"
#include "AcquisitionSetting.h"
#include "ECGFeature.h"
#include "Filter.h"
#include "ProcessingBar.h"

class CEcgDlg : public CDialog
{
// Construction
public:

	CEcgDlg(CWnd* pParent = NULL);	// standard constructor
	bool SignalProcViewInit();			//��l�ƪi����ܵ�
	UINT in_data,out_data,WaitingByte;  //�Ū�Pipe�|�Ψ쪺
	void StoreSampling(int samp);		//�x�s�߹q�ϸ��
	UINT UART_Enable;					//�ŪޭP��
	int sample_count;					//�ثe�ֿn���˼�		
	void AnalyseSignal(int samp,int Task,int Mode);//���R�߹q�T���A�����Y�ɻP���u
	
	void Read();						//�bTimer���I�s�AŪ��UART�����ƭ�
	void TransCommandWord(char *b);		//�ഫ�R�O�r��
	void GetDateTime(char *lt);			//���o�ثe�ɶ�
	UINT ECG_Sample_Encode(UINT Sample);//�קK�ǰeUART���~���s�X
	UINT ECG_Sample_Decode(BYTE High_Byte, BYTE Low_Byte);//�קK�ǰeUART���~���ѽX
	char Hex2Asc(char h2a);
	char Asc2Hex(char a2h);
	CSerialPort uart;
	BYTE *Queue_buffer;				//UART�@��Ū���즹buffer���A����A2�Ӥ@��Ū��	 
	CFont *f;
	UINT  PreviousPeakPos;			//�����W�@��R�i������sample_count�ơA�ûP�o���۴�s��PeakArrary��
	UINT  *PeakArrary;				//�x�s����쪺RR interval
//Dlg AcquisitionSetting
	int		Normalize;				//�̰��ѪR�סA12bit��4096
	CString ComPort_Number;

//UART
	void Uartopen();
	void Uartclose();
	int  baudrate;
//Display----------
	void DrawGrid(CDC *pDC,CRect *rect);
	void ECGScope();				//��bTimer���I�s�Aø�sCDrawWave������
	void DrawECG(CDC *pDC,CRect *rect);
	void DrawPeakAnalysis(CDC *pDC,CRect *rect);
	CDrawWave *Peak;				//�eR�i������m
	CDrawWave *HR_FreqDomain;		//
	CDrawWave *HFComponent;			//�߹q�Ϫ����W����
	CDrawWave *ECG;					//�߹q�ϸ��
	CDrawWave *TimeSlot;			//�����߹q��
	CDrawWave *SlidingWindowMax;	//Sliding Window
	CDrawWave *RRInterval;			//RR sequence
//	CDrawWave *Beat_aver;
	CECGFeature *Beat_aver;			//�����߹q��
	CDrawWave *TempleteMatch;		//������---�˪O���
	CDrawWave *SlidingWindowThreshold;//�ʺA�֭�
	CDrawWave *FFT;					
	CDrawWave *Capstrum;
	CDrawWave *Envenlope;
	CRect rect_top_view,rect_low_view;//top_view���߹q�ϧY����ܰϰ�Alow view���i�����\����ܰϰ�
//Filter

	CFilter *filter;				//�Ʀ��o�i��
//Simulation
	int Simulation;					//�����T��

//Record
	CAcquisitionSetting dlg_setting;//���R��ܤ��
	CECGRecorder recorder;			//�߹q�Ϭ���

// Dialog Data
	//{{AFX_DATA(CEcgDlg)
	enum { IDD = IDD_ECG_DIALOG };

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEcgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CDC	*pDC;
	CDC dcMem;
	int max_count;
	UINT m_Timer;
	bool isTimerActive;
	CString m_Data;
	CECGsp *SigProc;

protected:
	HICON m_hIcon;


	// Generated message map functions
	//{{AFX_MSG(CEcgDlg)
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:


//Control Member

	CSliderCtrl m_WaveSlideBar;
	afx_msg void OnBnClickedButton1();

	CString	m_rs232_status;
	int m_ComNum;
	float m_Threshold;
	float beat_interval;
	CTabCtrl m_SpecialTab;
	CStatic ECGViewer;
	UINT m_BeatAverageTimes;
	CString m_ComWord;
	CEdit m_BeatPerSec;
	CEdit m_BeatNum;

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonLtx();
	afx_msg void OnBnClickedButtonSendcom();
	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnBnClickedButton4();
public:
	afx_msg void OnBnClickedButton6();
public:
	afx_msg void OnBnClickedButton5();
	
public:
	CString m_RecordName;
public:
	CString m_RecordSex;
public:
	CString m_RecordAge;
public:
	int m_SamplingFreq;
public:
	int m_Resolution;
public:

public:
	afx_msg void OnBnClickedSexSel();
public:
	afx_msg void OnBnClickedRadio2();

public:
public:
	CEdit m_BeatInterval;
public:
	float m_QT;
	float m_ST;
	float m_QRS;
	float m_PS;
	float m_PR;

	afx_msg void OnBnClicked40hzlpf();
	afx_msg void OnBnClicked60hznotch();
	afx_msg void OnBnClicked50hzhpf();
	afx_msg void OnMouseMove(UINT nFlags,CPoint point);
	BOOL m_60HzHP;
	BOOL m_005HP;
	BOOL m_40LP;
public:
	BOOL m_Male;
public:
	afx_msg void OnTcnSelchangeTabSpecFunc(NMHDR *pNMHDR, LRESULT *pResult);

public:
	CString m_Freq;
public:
	afx_msg void OnEnChangeEdit12();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECGDLG_H__447C45F1_D8F4_4D1C_9035_B4A190E4633F__INCLUDED_)

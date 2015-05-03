
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication3.h"
#include "MFCApplication3Dlg.h"
#include "afxdialogex.h"

#include "src/Voice.h"
#include "src/Signal.h"
#include "src/DSPSoloSinWave.h"
#include "src/DSPPackInputs.h"

#include "src/MIDIMessage.h"

#include <Windows.h>
#include <mmsystem.h>
#include <xaudio2.h>

#include <math.h>

#include <exception>
#include <string>

#include <wrl.h>

#include <boost/thread/thread.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>
#include <boost/scoped_ptr.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Microsoft::WRL;//ComPtr


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{

public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
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


// CMFCApplication3Dlg dialog


int WM_THREAD_MESSAGE = RegisterWindowMessage(L"MYTHREADMESSAGE");
int WM_INI_AUDIO = RegisterWindowMessage(L"MYINIAUDIO");

CMFCApplication3Dlg::CMFCApplication3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplication3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_EStatus);
	DDX_Control(pDX, IDC_SLI_FREQUENCY, m_FrequencySli);
}

BEGIN_MESSAGE_MAP(CMFCApplication3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_REGISTERED_MESSAGE(WM_THREAD_MESSAGE, &CMFCApplication3Dlg::ThreadMessage)
	ON_REGISTERED_MESSAGE(WM_INI_AUDIO, &CMFCApplication3Dlg::IniAudio)
	ON_BN_CLICKED(IDC_BTN_START_I, &CMFCApplication3Dlg::OnBnClickedBtnStartI)
	ON_BN_CLICKED(IDC_BTN_STOP_I, &CMFCApplication3Dlg::OnBnClickedBtnStopI)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLI_FREQUENCY, &CMFCApplication3Dlg::OnNMReleasedcaptureSliFrequency)
END_MESSAGE_MAP()

BOOL CMFCApplication3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_FrequencySli.SetRange(0, 1000);

	PostMessageW(WM_INI_AUDIO, 0, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ******************************************************************************************************************* //
// ******************************************************************************************************************* //
// ******************************************************************************************************************* //
// ******************************************************************************************************************* //

static void Msg(const char* m){
	CString msg;
	msg.Format(_T("%S"), m); // Mind the caps "S"
	MessageBox(NULL, msg, _T("Hi"), NULL);
}

LRESULT CMFCApplication3Dlg::IniAudio(WPARAM wparam, LPARAM lparam)
{
	try{
		long nChannels = 2;
		long nSamplesPerSec = 192000;
		long numberOfBuffers = 4;
		long bufferLength = 4096;

		long midiMessageQueueCapacity = 1024;

		auto pVoice = boost::shared_ptr<IVoice>(new Voice(nChannels, nSamplesPerSec, numberOfBuffers, bufferLength));
		auto pWave = boost::shared_ptr<ISignalProcessor>(new DSPSoloSinWave(pVoice->getSamplesPerSec()));
		auto pPack = boost::shared_ptr<ISignalProcessor>(new DSPPackInputs(nChannels));
		auto pSignal = boost::shared_ptr<Signal>(new Signal(nChannels, bufferLength, pWave, pPack));
		pInstrument = boost::shared_ptr<Instrument>(new Instrument(pVoice, pSignal));

		pMIDIDevice = boost::shared_ptr<MIDIDevice>(new MIDIDevice("USB MIDI Interface"));

		return 0;
	}
	catch (std::exception ex){
		Msg(ex.what());
	}
}


class tmessage{
public:
	enum RecipientType{
		CEDIT,
	};

public:
	void* recipient;
	void* data;
	RecipientType recipientType;



	tmessage* setRecipient(void* recipient){
		this->recipient = recipient;
		return this;
	};
	tmessage* setData(void* data){
		this->data = data;
		return this;
	};

	tmessage* setRecipientType(RecipientType recipientType){
		this->recipientType = recipientType;
		return this;
	};

	tmessage(){};
	virtual ~tmessage(){};
};

// CMFCApplication3Dlg message handlers
LRESULT CMFCApplication3Dlg::ThreadMessage(WPARAM wparam, LPARAM lparam){
	tmessage* m = (tmessage*)(lparam);
	//TRACE("* <- \n\r");
	switch (m->recipientType){
	case tmessage::CEDIT:
		CString *pStr = (CString*)m->data;
		m_EStatus.SetWindowTextW(*pStr);
		delete pStr;
		break;
	}
	delete m;
	return 0;
}


void TimerHandler(Instrument::TimerHandlerInput* pInput){
	CString *pStr = new CString();
	pStr->Format(L"%02d (%010.3f ms)", pInput->pVoice->getNumberOfFreeBuffers(), pInput->msCycleDuration);
	CMFCApplication3Dlg *pApp = (CMFCApplication3Dlg*)pInput->pApp;
	pApp->PostMessageW(
		WM_THREAD_MESSAGE,
		0,
		(LPARAM)(new tmessage())
		->setRecipientType(tmessage::CEDIT)
		->setRecipient(pApp->m_EStatus)
		->setData(pStr)
		);

}

void CMFCApplication3Dlg::OnBnClickedBtnStartI()
{
	pInstrument->run(&TimerHandler, this, 100);
	pMIDIDevice->run(pInstrument);

}


void CMFCApplication3Dlg::OnBnClickedBtnStopI()
{
	pMIDIDevice->stop();
	pInstrument->stop();
}


void CMFCApplication3Dlg::OnNMReleasedcaptureSliFrequency(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	pInstrument->SetWave(m_FrequencySli.GetPos(), 1);
}

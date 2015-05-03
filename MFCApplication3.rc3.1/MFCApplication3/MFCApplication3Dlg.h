
// MFCApplication3Dlg.h : header file
//

#pragma once
#include "afxwin.h"

#include <boost/shared_ptr.hpp>

#include "src/Voice.h"
#include "src/Instrument.h"
#include "src/MIDIMessageQueue.h"
#include "src/MIDIDevice.h"

#include "afxcmn.h"

// CMFCApplication3Dlg dialog
class CMFCApplication3Dlg : public CDialogEx
{
	// Construction
public:
	CMFCApplication3Dlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	enum { IDD = IDD_MFCAPPLICATION3_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	afx_msg LRESULT ThreadMessage(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT IniAudio(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedBtnStartI();
	afx_msg void OnBnClickedBtnStopI();
	afx_msg void OnNMReleasedcaptureSliFrequency(NMHDR *pNMHDR, LRESULT *pResult);

	//void //CALLBACK 
	//	MidiInCallback(
	//	HMIDIIN hMidiIn,
	//	UINT wMsg,
	//	DWORD_PTR dwInstance,
	//	DWORD_PTR midiMessage,
	//	DWORD_PTR timestamp
	//	);

	void MidiInHandler();

	boost::shared_ptr<Instrument> pInstrument;
	boost::shared_ptr<MIDIDevice> pMIDIDevice;
//	boost::shared_ptr<MIDIMessageQueue> pMIDIMessageQueue;

	CEdit m_EStatus;
	CSliderCtrl m_FrequencySli;
};

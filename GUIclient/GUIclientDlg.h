
// GUIclientDlg.h: 头文件
//

#pragma once
#include "thread"



// CGUIclientDlg 对话框
class CGUIclientDlg : public CDialogEx
{
// 构造
public:
	CGUIclientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GUICLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
public:
	void InitWs2_32();
	afx_msg void OnBnClickedOnline();
	afx_msg void OnBnClickedOffline();
	void WorkThread();
	SOCKET m_sock;
	CListBox m_list;
	afx_msg void OnBnClickedPub();
	CString m_szHistory;
	afx_msg void OnBnClickedPri();
	bool bIsRunning = false;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

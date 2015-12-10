
// TradeServer_CDlg.h : 头文件
//

#pragma once

class CTradeServer_CDlgAutoProxy;


// CTradeServer_CDlg 对话框
class CTradeServer_CDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTradeServer_CDlg);
	friend class CTradeServer_CDlgAutoProxy;

// 构造
public:
	CTradeServer_CDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CTradeServer_CDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRADESERVER_C_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	CTradeServer_CDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};

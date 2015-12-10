
// TradeServer_CDlg.h : ͷ�ļ�
//

#pragma once

class CTradeServer_CDlgAutoProxy;


// CTradeServer_CDlg �Ի���
class CTradeServer_CDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTradeServer_CDlg);
	friend class CTradeServer_CDlgAutoProxy;

// ����
public:
	CTradeServer_CDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CTradeServer_CDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRADESERVER_C_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	CTradeServer_CDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};


// DlgProxy.h: ͷ�ļ�
//

#pragma once

class CTradeServer_CDlg;


// CTradeServer_CDlgAutoProxy ����Ŀ��

class CTradeServer_CDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CTradeServer_CDlgAutoProxy)

	CTradeServer_CDlgAutoProxy();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��

// ����
public:
	CTradeServer_CDlg* m_pDialog;

// ����
public:

// ��д
	public:
	virtual void OnFinalRelease();

// ʵ��
protected:
	virtual ~CTradeServer_CDlgAutoProxy();

	// ���ɵ���Ϣӳ�亯��

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CTradeServer_CDlgAutoProxy)

	// ���ɵ� OLE ����ӳ�亯��

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};


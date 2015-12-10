
// DlgProxy.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TradeServer_C.h"
#include "DlgProxy.h"
#include "TradeServer_CDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTradeServer_CDlgAutoProxy

IMPLEMENT_DYNCREATE(CTradeServer_CDlgAutoProxy, CCmdTarget)

CTradeServer_CDlgAutoProxy::CTradeServer_CDlgAutoProxy()
{
	EnableAutomation();
	
	// ΪʹӦ�ó������Զ��������ڻ״̬ʱһֱ���� 
	//	���У����캯������ AfxOleLockApp��
	AfxOleLockApp();

	// ͨ��Ӧ�ó����������ָ��
	//  �����ʶԻ���  ���ô�����ڲ�ָ��
	//  ָ��Ի��򣬲����öԻ���ĺ���ָ��ָ��
	//  �ô���
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CTradeServer_CDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CTradeServer_CDlg)))
		{
			m_pDialog = reinterpret_cast<CTradeServer_CDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CTradeServer_CDlgAutoProxy::~CTradeServer_CDlgAutoProxy()
{
	// Ϊ������ OLE �Զ����������ж������ֹӦ�ó���
	//	������������ AfxOleUnlockApp��
	//  ���������������⣬�⻹���������Ի���
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CTradeServer_CDlgAutoProxy::OnFinalRelease()
{
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease��  ���ཫ�Զ�
	// ɾ���ö���  �ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ע��: ��������˶� IID_ITradeServer_C ��֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡�  �� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {D0619233-2E26-4068-8BA4-1A83DE9D379A}
static const IID IID_ITradeServer_C =
{ 0xD0619233, 0x2E26, 0x4068, { 0x8B, 0xA4, 0x1A, 0x83, 0xDE, 0x9D, 0x37, 0x9A } };

BEGIN_INTERFACE_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CTradeServer_CDlgAutoProxy, IID_ITradeServer_C, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 ���ڴ���Ŀ�� StdAfx.h �ж���
// {D5C41A4F-3097-4E63-B095-4BEF012C937B}
IMPLEMENT_OLECREATE2(CTradeServer_CDlgAutoProxy, "TradeServer_C.Application", 0xd5c41a4f, 0x3097, 0x4e63, 0xb0, 0x95, 0x4b, 0xef, 0x1, 0x2c, 0x93, 0x7b)


// CTradeServer_CDlgAutoProxy ��Ϣ�������

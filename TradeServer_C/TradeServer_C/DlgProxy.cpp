
// DlgProxy.cpp : 实现文件
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
	
	// 为使应用程序在自动化对象处于活动状态时一直保持 
	//	运行，构造函数调用 AfxOleLockApp。
	AfxOleLockApp();

	// 通过应用程序的主窗口指针
	//  来访问对话框。  设置代理的内部指针
	//  指向对话框，并设置对话框的后向指针指向
	//  该代理。
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
	// 为了在用 OLE 自动化创建所有对象后终止应用程序，
	//	析构函数调用 AfxOleUnlockApp。
	//  除了做其他事情外，这还将销毁主对话框
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CTradeServer_CDlgAutoProxy::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。  基类将自动
	// 删除该对象。  在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 注意: 我们添加了对 IID_ITradeServer_C 的支持
//  以支持来自 VBA 的类型安全绑定。  此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {D0619233-2E26-4068-8BA4-1A83DE9D379A}
static const IID IID_ITradeServer_C =
{ 0xD0619233, 0x2E26, 0x4068, { 0x8B, 0xA4, 0x1A, 0x83, 0xDE, 0x9D, 0x37, 0x9A } };

BEGIN_INTERFACE_MAP(CTradeServer_CDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CTradeServer_CDlgAutoProxy, IID_ITradeServer_C, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 宏在此项目的 StdAfx.h 中定义
// {D5C41A4F-3097-4E63-B095-4BEF012C937B}
IMPLEMENT_OLECREATE2(CTradeServer_CDlgAutoProxy, "TradeServer_C.Application", 0xd5c41a4f, 0x3097, 0x4e63, 0xb0, 0x95, 0x4b, 0xef, 0x1, 0x2c, 0x93, 0x7b)


// CTradeServer_CDlgAutoProxy 消息处理程序

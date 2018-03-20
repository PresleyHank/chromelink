
#include "IChromeLink.h"
#include <sstream>

using namespace Komari;

/*
����: GetTabs
˵��: ��ȡ������������Ѵ򿪵ı�ǩҳ��Ϣ
szTraceAddr : ��������Ե�ַ
Tabs : ���ػ�õı�ǩҳ�б�
����: �����������˵�������ǳɹ��ġ�
*/
BOOL IChromeLink::GetTabs(LPCTSTR szTraceAddr, IChromeCurTabs &Tabs)
{
	Tabs.clear();
	BOOL bResult = FALSE;
	IStream strBuilderUrl;

	strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/list");
	IString strPullUrl = strBuilderUrl.str();
	http_client client(strPullUrl.c_str());
	try
	{
		client.request(methods::GET, TEXT("")).then([&](http_response response) {

			//�ο�����:  http://localhsot:9222/json/list
			IChromeTabInfo TabInfo;
			if (response.status_code() == status_codes::OK)
			{
				const json::value &root = response.extract_json().get();
				int nMax = root.size();
				for (size_t i = 0; i < nMax; i++)
				{
					auto child = root.at(i);
					TabInfo.strDescription = child.at(U("description")).as_string();
					TabInfo.strId = child.at(TEXT("id")).as_string();
					TabInfo.strTitle = child.at(TEXT("title")).as_string();
					TabInfo.strType = child.at(TEXT("type")).as_string();
					TabInfo.strUrl = child.at(TEXT("url")).as_string();
					try
					{
						TabInfo.strWebSocketDebuggerUrl = child.at(TEXT("webSocketDebuggerUrl")).as_string();
					}
					catch (const std::exception&)
					{
						TabInfo.strWebSocketDebuggerUrl = TEXT("ws://");
						TabInfo.strWebSocketDebuggerUrl += szTraceAddr;
						TabInfo.strWebSocketDebuggerUrl += TEXT("/devtools/page/");
						TabInfo.strWebSocketDebuggerUrl += TabInfo.strId;
					}
					Tabs.push_back(TabInfo);
				}
				bResult = TRUE;
			}

		}).wait();
	}
	catch (const std::exception&)
	{

	}
	return bResult;
}

/*
����: GetTabCount
˵��: ��ȡ��ǩҳ������
����: ����ɹ�������������
*/
int IChromeLink::GetTabCount(LPCTSTR szTraceAddr)
{

	int nResult = 0;
	IStream strBuilderUrl;

	strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/list");
	IString strPullUrl = strBuilderUrl.str();
	http_client client(strPullUrl.c_str());
	try
	{
		client.request(methods::GET, TEXT("")).then([&](http_response response) {

			//�ο�����:  http://localhsot:9222/json/list
			IChromeTabInfo TabInfo;
			if (response.status_code() == status_codes::OK)
			{
				const json::value &root = response.extract_json().get();
				nResult = root.size();
			}

		}).wait();
	}
	catch (const std::exception&)
	{

	}
	return nResult;
}

/*
����: NewTab
˵��: ��һ���µı�ǩҳ
szTraceAddr : ��������Ե�ַ
szUrl : ����ҳ���ʵĵ�ַ
Tab : �����Ѵ򿪵ı�ǩҳ��Ϣ
����: �����������˵�������ǳɹ��ġ�
*/
BOOL IChromeLink::NewTab(LPCTSTR szTraceAddr, LPCTSTR szUrl, IChromeTabInfo &Tab)
{
	IStream strBuilderUrl;
	BOOL bResult = FALSE;

	if (szUrl!=NULL)
	{
		strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/new?") << szUrl;
	}
	else
	{
		strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/new");
	}
	IString strPullUrl = strBuilderUrl.str();
	http_client client(strPullUrl);
	try
	{
		client.request(methods::GET, TEXT("")).then([&](http_response response) {

			//���ʲο�: http://localhost:9222/json/new?url
			IChromeTabInfo TabInfo;
			if (response.status_code() == status_codes::OK)
			{
				const web::json::value &root = response.extract_json().get();
				Tab.strDescription = root.at(TEXT("description")).as_string();
				Tab.strDevToolsFrontendUrl = root.at(TEXT("devtoolsFrontendUrl")).as_string();
				Tab.strId = root.at(TEXT("id")).as_string();
				Tab.strTitle = root.at(TEXT("title")).as_string();
				Tab.strType = root.at(TEXT("type")).as_string();
				Tab.strUrl = root.at(TEXT("url")).as_string();
				Tab.strWebSocketDebuggerUrl = root.at(TEXT("webSocketDebuggerUrl")).as_string();
				bResult = TRUE;
			}
		}).wait();
	}
	catch (const std::exception&)
	{

	}
	return bResult;
}

/*
����: CloseTab
˵��: �ر�һ����ǩҳ��
szTraceAddr : ��������Ե�ַ
szId : ��ǩ��ID
����: �����������˵�������ǳɹ��ġ�
*/
BOOL IChromeLink::CloseTab(LPCTSTR szTraceAddr, LPCTSTR szId)
{
	IStream strBuilderUrl;
	BOOL bResult = FALSE;

	strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/close/") << szId;
	IString strPullUrl = strBuilderUrl.str();

	http_client client(strPullUrl);

	try
	{
		client.request(methods::GET, TEXT("")).then([&](http_response response) {

			//���ʲο�: http://localhost:9222/json/close/id
			if (response.status_code() == status_codes::OK)
			{
				auto bodyStream = response.body();
				streams::stringstreambuf sBuffer;
				auto &strText = sBuffer.collection();
				bodyStream.read_to_end(sBuffer).get();
				if (_strcmpi(strText.c_str(), "Target is closing") == 0)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}
			}
		}).wait();
	}
	catch (const std::exception&)
	{

	}
	return bResult;
}

/*
����: ActivateTab
˵��: ����һ����ǩҳ��
szTraceAddr : ��������Ե�ַ
szId : ��ǩ��ID
����: �����������˵�������ǳɹ��ġ�
*/
BOOL IChromeLink::ActivateTab(LPCTSTR szTraceAddr, LPCTSTR szId)
{
	IStream strBuilderUrl;
	BOOL bResult = FALSE;

	strBuilderUrl << TEXT("http://") << szTraceAddr << TEXT("/json/activate/") << szId;
	IString strPullUrl = strBuilderUrl.str();

	http_client client(strPullUrl);

	try
	{
		client.request(methods::GET, TEXT("")).then([&](http_response response) {

			//���ʲο�: http://localhost:9222/json/activate/id
			if (response.status_code() == status_codes::OK)
			{
				auto bodyStream = response.body();
				streams::stringstreambuf sBuffer;
				auto &strText = sBuffer.collection();
				bodyStream.read_to_end(sBuffer).get();
				if (_strcmpi(strText.c_str(), "Target activated") == 0)
				{
					bResult = TRUE;
				}
				else
				{
					bResult = FALSE;
				}
			}

		}).wait();
	}
	catch (const std::exception&)
	{

	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
IChromeLink::IChromeLink(LPCTSTR szWebSocketDebuggerUrl)
{
	this->m_strTraceAddr = szWebSocketDebuggerUrl;
	this->m_bLinkStatus = FALSE;
	this->m_dwAutoIdentify = 0;
	this->m_bReadyState = FALSE;
	this->m_bInitializedSucess = FALSE;
	this->m_javascriptDialogInfomation.bDialogLifed = FALSE;
	this->m_javascriptDialogInfomation.type = IC_JSDT_ALERT;
	this->AddRef();
}

IChromeLink::~IChromeLink()
{
	this->Close();
	printf("�������� \n");

}

/*
����: LinkStart
˵��: ������ָ���ı�ǩҳ��
����: ������ӳɹ��������档
*/
BOOL IChromeLink::LinkStart(void)
{
	this->m_bLinkStatus = FALSE;
	if (this->m_strTraceAddr.length() <= 0)
	{
		return FALSE;
	}
	try
	{
		this->m_wsKeepLink.connect(this->m_strTraceAddr).get();
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	this->m_bLinkStatus = TRUE;
	std::thread t(&IChromeLink::OnEventDispatchs,this,this);
	t.detach();
	this->m_bInitializedSucess = FALSE;
	return this->OnLinkSucess();
}

void IChromeLink::Close(void)
{
	if (this->m_bLinkStatus == TRUE)
	{
		this->m_wsKeepLink.close().get();
		if (this->m_mapRequestQueues.size() > 0)
		{
			IChromeResponse::iterator iter = this->m_mapRequestQueues.begin();
			IChromeResponse::iterator iend = this->m_mapRequestQueues.end();
			for (;iter!=iend;iter++)
			{
				iter->second.set_exception(websocket_exception("miss connected!"));
			}
		}
		this->m_taskReady.set_exception("miss connected!");
		this->m_bLinkStatus = FALSE;
	}
}

//�ڲ���Ϣ�����߳�
void CALLBACK IChromeLink::OnEventDispatchs(IChromeLink *pChromeLink)
{
	if (pChromeLink != NULL)
	{
		pChromeLink->AddRef();
		while (true)
		{
			try
			{
				websocket_incoming_message objMsg = pChromeLink->m_wsKeepLink.receive().get();
				if (objMsg.message_type() == websocket_message_type::text_message)
				{
					streams::stringstreambuf sBuffer;
					auto &strBody = sBuffer.collection();
					objMsg.body().read_to_end(sBuffer).get();
					json::value objRoot = json::value::parse(conversions::to_string_t(strBody));
					if (pChromeLink->m_bLinkStatus == FALSE)
					{
						break;
					}
					if (!pChromeLink->m_mapRequestQueues.empty())
					{
						//����Request
						json::value objAutoIdentify = objRoot[U("id")];
						if (!objAutoIdentify.is_null())
						{
							//Ѱ��Request
							DWORD dwAutoId = objAutoIdentify.as_integer();
							IChromeResponse::iterator itRequest = pChromeLink->m_mapRequestQueues.find(dwAutoId);
							if (itRequest != pChromeLink->m_mapRequestQueues.end())
							{
								//�ҵ�Request
								itRequest->second.set(objRoot);	//����Request;
								pChromeLink->m_hUnionMutex.lock();
								pChromeLink->m_mapRequestQueues.erase(itRequest);
								pChromeLink->m_hUnionMutex.unlock();
								continue;
							}
						}
					}
					//���û��id��Ա,��Ϊ�¼���Ϣ
					this->SendEventDispatchs(objRoot);
				}
			}
			catch (const std::exception &ex)
			{
				break;
			}
		}
		pChromeLink->Close();
		pChromeLink->Release();
	}
}

//�����¼���Ϣ
void IChromeLink::SendEventDispatchs(const json::value &objRoot)
{
	IString strMethod = objRoot.at(U("method")).as_string();
	TCHAR szVar[100];
	if (lstrcmpi(strMethod.c_str(), TEXT("Runtime.executionContextCreated")) == 0)
	{
		//V8���洴������¼�
		IChromeExecutionContextId Info;

		json::value objParams = objRoot.at(U("params"));
		json::value objContext = objParams.at(U("context"));
		json::value objAuxData = objContext.at(U("auxData"));

		Info.dwExecutionContextId = objContext.at(U("id")).as_integer();
		Info.strFrameId = objAuxData.at(U("frameId")).as_string();
		this->m_vecExecutionContextId.push_back(Info);
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Runtime.executionContextDestroyed")) == 0)
	{
		//V8���������¼�
		json::value objParams = objRoot.at(U("params"));
		DWORD dwExecutionContextId = objParams.at(U("executionContextId")).as_integer();
		IChromeV8ExecutionContextIdVectors::iterator iter = this->m_vecExecutionContextId.begin();
		IChromeV8ExecutionContextIdVectors::iterator iend = this->m_vecExecutionContextId.end();
		for (;iter!=iend;iter++)
		{
			if (iter->dwExecutionContextId == dwExecutionContextId)
			{
				this->m_vecExecutionContextId.erase(iter);
				break;
			}
		}
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Runtime.executionContextsCleared")) == 0)
	{
		this->m_vecExecutionContextId.clear();
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Page.loadEventFired")) == 0)
	{
		this->m_vecChildFrameIdLists.clear();
		this->GetWebSheet().then([this](json::value root) {
			this->OnParserWebTree(root);
			this->m_bReadyState = FALSE;
		});
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Page.frameStartedLoading")) == 0)
	{
		this->m_bReadyState = TRUE;
		this->m_vecChildFrameIdLists.clear();
		m_consoleInfos.clear();
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Page. frameStoppedLoading")) == 0)
	{

	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Page.javascriptDialogOpening")) == 0)
	{
		json::value objParams = objRoot.at(U("params"));
		this->m_javascriptDialogInfomation.bDialogLifed = TRUE;
		try
		{
			this->m_javascriptDialogInfomation.strURL = objParams[U("url")].as_string().c_str();
		}
		catch (const std::exception&)
		{

		}
		
		try
		{
			this->m_javascriptDialogInfomation.strMessage = objParams[U("message")].as_string().c_str();
		}
		catch (const std::exception&)
		{

		}
		IString strType= objParams[U("type")].as_string();
		if (strType == TEXT("alert"))
		{
			this->m_javascriptDialogInfomation.type = IC_JSDT_ALERT;
		}
		else if (strType == TEXT("confirm"))
		{
			this->m_javascriptDialogInfomation.type = IC_JSDT_CONFIRM;
		}
		else if (strType == TEXT("prompt"))
		{
			this->m_javascriptDialogInfomation.type = IC_JSDT_PROMPT;
		}
		else if (strType == TEXT("beforeunload"))
		{
			this->m_javascriptDialogInfomation.type = IC_JSDT_BEFOREUNLOAD;
		}
		else this->m_javascriptDialogInfomation.type = IC_JSDT_ALERT;
		try
		{
			this->m_javascriptDialogInfomation.strDefaultPrompt = objParams[U("defaultPrompt")].as_string();
		}
		catch (const std::exception&)
		{

		}
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Page.javascriptDialogClosed")) == 0)
	{
		this->m_javascriptDialogInfomation.bDialogLifed = FALSE;
		this->m_javascriptDialogInfomation.strMessage = TEXT("");
		this->m_javascriptDialogInfomation.strURL = TEXT("");
		this->m_javascriptDialogInfomation.strDefaultPrompt = TEXT("");
	}
	else if (lstrcmpi(strMethod.c_str(), TEXT("Runtime.consoleAPICalled")) == 0)
	{
		json::value objParams = objRoot.at(U("params"));
		json::value objArgs = objParams[U("args")];
		int nMax = objArgs.size();
		std::wstringstream ssBuffer;
		std::wstring strType;
		for (size_t i = 0; i < nMax; i++)
		{
			ssBuffer.clear();
			strType = objArgs[i][U("type")].as_string();
			if (strType == TEXT("string"))
			{
				ssBuffer<<TEXT("[string]: ")<< objArgs[i][U("value")].as_string();
				std::wstring &strBuffer = ssBuffer.str();
				m_consoleInfos.push_back(strBuffer);
			}
			else if (strType == TEXT("number"))
			{
				json::value number = objArgs[i][U("value")];
				if(number.is_integer())
					ssBuffer << TEXT("[number]: ") << number.as_integer();
				else if (number.is_double())
					ssBuffer << TEXT("[number]: ") << number.as_double();
				std::wstring &strBuffer = ssBuffer.str();
				m_consoleInfos.push_back(strBuffer);
			}
			else if (strType == TEXT("boolean"))
			{
				ssBuffer << TEXT("[boolean]: ") << objArgs[i][U("value")].as_bool();
				std::wstring &strBuffer = ssBuffer.str();
				m_consoleInfos.push_back(strBuffer);
			}
		}
	}
}

//��ȡһ������ID
DWORD IChromeLink::GetAutoRand(void)
{
	InterlockedCompareExchange(&this->m_dwAutoIdentify, 0, 0x7FFFFFFF);
	DWORD dwNewIdentify = InterlockedIncrement(&this->m_dwAutoIdentify);
	return dwNewIdentify;
}

/*
����: OnLinkSucess
˵��: �ڳɹ����ӵ����������г�ʼ�����á�
*/
BOOL IChromeLink::OnLinkSucess(void)
{
	try
	{
		json::value objRoot1 = this->OpenPageEvents().get();
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	this->m_vecExecutionContextId.clear();
	this->OpenRuntimeEvents().then([](json::value objRoot2) {
		///

	}).then([](pplx::task<void> t) {
		try
		{
			t.get();
		}
		catch (const std::exception&)
		{

		}
	});

	try
	{
		json::value objRoot = this->GetWebSheet().get();
		this->OnParserWebTree(objRoot);
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	bResult = this->OpenNetwork();
	return bResult;
}

pplx::task<json::value> IChromeLink::GetWebSheet(void)
{
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.getResourceTree"));
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		objResponse.set_exception(ex);
		return pplx::create_task(objResponse);
	}
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	return pplx::create_task(objResponse);
}

/*
����: OpenPageEvents
˵��: ��������"Page"����¼���Ϣ
����: �ɹ�����Response��
*/
pplx::task<json::value> IChromeLink::OpenPageEvents(void)
{
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.enable"));
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		objResponse.set_exception(ex);
		return pplx::create_task(objResponse);
	}
	return pplx::create_task(objResponse);
}

/*
����: OpenRuntimeEvents
˵��: ��������"Runtime"����¼���Ϣ
����: �ɹ�����Response
*/
pplx::task<json::value> IChromeLink::OpenRuntimeEvents(void)
{
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Runtime.enable"));
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		objResponse.set_exception(ex);
		return pplx::create_task(objResponse);
	}
	return pplx::create_task(objResponse);
}

BOOL IChromeLink::OpenNetwork(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.enable"));
	json::value objParams;
	objParams[U("maxTotalBufferSize")] = json::value::number(0x8000);
	objParams[U("maxResourceBufferSize")] = json::value::number(524288);
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

//������ҳ�ṹ
void IChromeLink::OnParserWebTree(const json::value &objRoot)
{
	char szVar[100];
	this->m_vecChildFrameIdLists.clear();
	this->m_objMainFrameId.strFrameId = TEXT("");
	this->m_objMainFrameId.strLoaderId = TEXT("");
	this->m_objMainFrameId.strMimeType = TEXT("");
	this->m_objMainFrameId.strName = TEXT("");
	this->m_objMainFrameId.strParentId = TEXT("");
	this->m_objMainFrameId.strUrl = TEXT("");
	const json::value &objResult = objRoot.at(U("result"));
	const json::value &objFrameTree = objResult.at(U("frameTree"));
	EnumWebFrame(objFrameTree);
	/*json::value objChildFrames;
	int nMax = 0;
	MessageBox(NULL, objFrameTree.serialize().c_str(), NULL, NULL);
	try
	{
		objChildFrames = objFrameTree.at(U("childFrames"));
		nMax = objChildFrames.size();
		sprintf_s(szVar, 100, "%d", nMax);
		MessageBoxA(NULL, szVar, NULL, NULL);
	}
	catch (const std::exception&)
	{
		nMax = 0;
	}
	IChromeFrameId Info;
	for (size_t i = 0; i < nMax; i++)
	{
		json::value objFrame = objChildFrames[i].at(U("frame"));
		Info.strFrameId = objFrame.at(U("id")).as_string();
		Info.strLoaderId = objFrame.at(U("loaderId")).as_string();
		Info.strMimeType = objFrame.at(U("mimeType")).as_string();
		Info.strName = objFrame.at(U("name")).as_string();
		try
		{
			json::value objParentId = objFrame.at(U("parentId"));
			Info.strParentId = objParentId.as_string();
		}
		catch (const std::exception&)
		{
			Info.strParentId = TEXT("");
		}
		Info.strUrl = objFrame.at(U("url")).as_string();
		this->m_vecChildFrameIdLists.push_back(Info);
	}*/


	json::value objMainFrame= objFrameTree.at(U("frame"));
	this->m_objMainFrameId.strFrameId = objMainFrame.at(U("id")).as_string();
	this->m_objMainFrameId.strLoaderId = objMainFrame.at(U("loaderId")).as_string();
	this->m_objMainFrameId.strMimeType = objMainFrame.at(U("mimeType")).as_string();
	this->m_objMainFrameId.strName = TEXT("");
	this->m_objMainFrameId.strParentId = TEXT("");
	this->m_objMainFrameId.strUrl = objMainFrame.at(U("url")).as_string();
}

//������ҳ���
void IChromeLink::EnumWebFrame(const json::value &objRoot)
{
	try
	{
		const json::value &childFrames = objRoot.at(L"childFrames");
		int frameCount = childFrames.size();
		for (size_t i = 0; i < frameCount; i++)
		{
			const auto &element = childFrames.at(i);
			EnumWebFrame(element);
			GetWebFrame(element.at(L"frame"));
		}
	}
	catch (const std::exception&)
	{

	}
}

//��ȡ���
void IChromeLink::GetWebFrame(const json::value &objRoot)
{
	IChromeFrameId Info;
	Info.strFrameId = objRoot.at(U("id")).as_string();
	Info.strLoaderId = objRoot.at(U("loaderId")).as_string();
	Info.strMimeType = objRoot.at(U("mimeType")).as_string();
	Info.strName = objRoot.at(U("name")).as_string();
	try
	{
		json::value objParentId = objRoot.at(U("parentId"));
		Info.strParentId = objParentId.as_string();
	}
	catch (const std::exception&)
	{
		Info.strParentId = TEXT("");
	}
	Info.strUrl = objRoot.at(U("url")).as_string();
	this->m_vecChildFrameIdLists.push_back(Info);
}

pplx::task<BOOL> IChromeLink::GetExecutionContextId(void)
{
	return pplx::create_task(this->m_taskReady);
}

/*
����: GetMainFrame
˵��: ��ȡ��ǩҳ�е�����ܶ���
����: �ɹ��������������Ϣ��
*/
const IChromeFrameId &IChromeLink::GetMainFrame(void)
{
	return this->m_objMainFrameId;
}

/*
����: GetFrameCount
˵��: ��ȡ��ǩҳ�е��ӿ����
����: �ɹ������ӿ��������
*/
int IChromeLink::GetFrameCount(void)
{
	return this->m_vecChildFrameIdLists.size();
}

/*
����: GetChildFrame
˵��: ��ȡ�ӿ�ܶ���
index: �±�,��0��ʼ
����: �ɹ��������ӿ����Ϣ��
*/
const IChromeFrameId &IChromeLink::GetChildFrame(int index)
{
	return this->m_vecChildFrameIdLists[index];
}

/*
����: GetExecutionContextCount
˵��: ��ȡV8������ִ�л�������
����: �ɹ����ػ�������
*/
int IChromeLink::GetExecutionContextCount(void)
{
	return this->m_vecExecutionContextId.size();
}

/*
����: GetExecutionContextId
˵��: ��ȡV8������ִ�л���
����: �ɹ�����������ִ�л�������
*/
const IChromeExecutionContextId &IChromeLink::GetExecutionContextId(int index)
{
	return this->m_vecExecutionContextId[index];
}

/*
����: Evaluate
˵��: ִ�б��ʽ
szExpression: ���ʽ�ı�
szObjectGroup: �������ͷŶ������ķ��������ơ�
bIncludeCommandLineAPI: ȷ���������ڼ�Command Line API�Ƿ���á�
bSilent: �ھ�Ĭģʽ�£������ڼ��׳����쳣�����棬��Ҫ��ִͣ�С� ����setPauseOnException״̬��
dwExecutionContextId: ָ��ִ��������ִ�������� ���ʡ�Բ��������ڼ��ҳ�����������ִ��������
bReturnByValue: ����Ƿ�Ӧ����Ӧ��ͨ��ֵ���͵�JSON����
bGeneratePreview: �Ƿ�ӦΪ�������Ԥ����
bUserGesture: ִ���Ƿ�Ӧ�ñ���Ϊ�û���UI�з���ġ�
bAwaitPromise: ִ���Ƿ�Ӧ�õȴ���ŵ����� �������������ǳ�ŵ������Ϊ�Ǵ���
objResult: ���ڷ��ر��ʽ���
����: �ɹ����ر�����Ϣ��
*/
BOOL IChromeLink::Evaluate(LPCTSTR szExpression, LPCTSTR szObjectGroup, BOOL bIncludeCommandLineAPI, BOOL bSilent, DWORD dwExecutionContextId, BOOL bReturnByValue, BOOL bGeneratePreview, BOOL bUserGesture, BOOL bAwaitPromise, IChromeEvalResult &objResult)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Runtime.evaluate"));
	json::value objParams;
	objParams[U("expression")] = json::value::string(szExpression);
	objParams[U("objectGroup")] = json::value::string(szObjectGroup);
	objParams[U("includeCommandLineAPI")] = json::value::boolean(bIncludeCommandLineAPI);
	objParams[U("silent")] = json::value::boolean(bSilent);
	objParams[U("contextId")] = json::value::number((int)dwExecutionContextId);
	objParams[U("returnByValue")] = json::value::boolean(bReturnByValue);
	objParams[U("generatePreview")] = json::value::boolean(bGeneratePreview);
	objParams[U("userGesture")] = json::value::boolean(bUserGesture);
	objParams[U("awaitPromise")] = json::value::boolean(bAwaitPromise);

	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}


	try
	{
		json::value objRetResult = pplx::create_task(objResponse).get();
		json::value objReturn = objRetResult.at(U("result"));
		try
		{
			json::value obExceptionDetails = objReturn.at(U("exceptionDetails"));
			objResult.bException = TRUE;
		}
		catch (const std::exception&)
		{
			objResult.bException = FALSE;
		}

		json::value objChild = objReturn.at(U("result"));
		try
		{
			objResult.strClassName = objChild.at(U("className")).as_string();
		}
		catch (const std::exception&)
		{
			
		}
		try
		{
			objResult.strSubType = objChild.at(U("subtype")).as_string();
		}
		catch (const std::exception&)
		{

		}
		try
		{
			objResult.strType = objChild.at(U("type")).as_string();
			if (objResult.strType == TEXT("string"))
			{
				objResult.strValue = objChild.at(U("value")).as_string();
			}
			else if (objResult.strType == TEXT("number"))
			{
				json::value val = objChild.at(U("value"));
				if (val.is_double())
				{
					objResult.flValue = val.as_double();
					objResult.strType = U("double");

				}
				else if (val.is_integer())
				{
					objResult.nValue = val.as_integer();
				}
			}
			else if (objResult.strType == TEXT("boolean"))
			{
				json::value val = objChild.at(U("value"));
				objResult.bValue = val.as_bool();
			}
		}
		catch (const std::exception&)
		{

		}
		try
		{
			objResult.strDescription = objChild.at(U("description")).as_string();
		}
		catch (const std::exception&)
		{

		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ReleaseObjectGroup
˵��: �ͷ����ڸ����������Զ�̶���
szObjectGroup: ��������
����: �ɹ�������,ʧ�ܷ��ؼ�
*/
BOOL IChromeLink::ReleaseObjectGroup(LPCTSTR szObjectGroup)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Runtime.releaseObjectGroup"));
	json::value objParams;
	objParams[U("objectGroup")] = json::value::string(szObjectGroup);
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����:  SetReady
˵��: ���þ���״̬
enStatus: ���þ���״̬����
*/
void IChromeLink::SetReady(IChromeReadyStatus enStatus)
{
	if (enStatus == ICR_SIGNAL)
	{
		SetEvent(this->m_hEventReady);
	}
	else
	{
		ResetEvent(this->m_hEventReady);
	}
}

/*
����: WaitReady
nWaitTime: �ȴ���ʱ��,Ĭ��Ϊ: INFINITE,�����Ƽ�ʹ��Ĭ��
˵��: �ȴ�������״̬
����: ���صȴ��Ľ��
*/
IChromeReadyStatus IChromeLink::WaitReady(int nWaitTime)
{
	if (WaitForSingleObject(this->m_hEventReady, nWaitTime) == WAIT_OBJECT_0)
	{
		return ICR_SIGNAL;
	}
	return ICR_WAITING;
}

/*
����: Navigate
˵��: ��ת��ָ��ҳ��
szUrl: �����ʵĵ�ַ
szReferrer: ��·����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::Navigate(LPCTSTR szUrl, LPCTSTR szReferrer)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.navigate"));
	json::value objParams;
	objParams[U("url")] = json::value::string(szUrl);
	if (szReferrer != NULL && lstrlen(szReferrer)>0)
	{
		objParams[U("referer")] = json::value::string(szReferrer);
	}
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ReLoad
˵��: ���¼���ҳ��
bIgnoreCache: �Ƿ���Ի���������¼���
szScriptToEvaluateOnLoad: ������ã��ű��������¼��غ�ע�뱻���ҳ�������֡��
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ReLoad(BOOL bIgnoreCache, LPCTSTR szScriptToEvaluateOnLoad)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.reload"));
	json::value objParams;
	objParams[U("ignoreCache")] = json::value::boolean(bIgnoreCache);
	if (szScriptToEvaluateOnLoad!=NULL)
	{
		objParams[U("scriptToEvaluateOnLoad")] = json::value::string(szScriptToEvaluateOnLoad);
	}
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: GetResourceContent
˵��: ���ظ�����Դ������
strRetContent: ���ػ�õ���Դ����
bBase64Encoded: ���ص������Ƿ���ʹ��base64����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::GetResourceContent(LPCTSTR szFrameId, LPCTSTR szUrl, IString &strRetContent, BOOL &bBase64Encoded)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.getResourceContent"));
	json::value objParams;
	objParams[U("frameId")] = json::value::string(szFrameId);
	if (szUrl != NULL)
	{
		objParams[U("url")] = json::value::string(szUrl);
	}
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());

	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		json::value objResult = objRoot[U("result")];
		bBase64Encoded = objResult[U("base64Encoded")].as_bool();
		strRetContent= objResult[U("content")].as_string();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

BOOL IChromeLink::UpdateFrame(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	try
	{
		json::value objRoot = this->GetWebSheet().get();
		//MessageBox(NULL, objRoot.serialize().c_str(), NULL, NULL);
		this->OnParserWebTree(objRoot);
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetDocumentContent
˵��: ����ָ������е�HTML����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetDocumentContent(LPCTSTR szFrameId, LPCTSTR szHtml)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	if (szFrameId == NULL || szHtml == NULL)
	{
		return FALSE;
	}
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.setDocumentContent"));
	json::value objParams;
	objParams[U("frameId")] = json::value::string(szFrameId);
	objParams[U("html")] = json::value::string(szHtml);
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: CaptureScreenshot
˵��: ����ҳ�����
szFormat: ���յĴ洢��ʽ,��ѡֵ: jpeg,png
nQuality: ѹ��������Χ(0-100),����jpeg
bFromSurface: �ӱ��沶����Ļ��ͼ����������ͼ�� Ĭ��Ϊtrue.
����: ���������������ǳɹ���
*/
BOOL IChromeLink::CaptureScreenshot(LPCTSTR szFormat, int nQuality, BOOL bFromSurface, IString &strData)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	if (szFormat == NULL)
	{
		return FALSE;
	}
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.captureScreenshot"));
	json::value objParams;
	objParams[U("format")] = json::value::string(szFormat);
	objParams[U("quality")] = json::value::number(nQuality);
	objParams[U("fromSurface")] = json::value::boolean(bFromSurface);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		json::value objResult = objRoot[U("result")];
		json::value objData= objResult[U("data")];
		strData = objData.as_string();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetDeviceMetricsOverride
˵��: �����豸��Ļ�ߴ��ֵ��window.screen.width��window.screen.height��window.innerWidth��window.innerHeight�͡�device-width��/��device-height������ص�CSSý���ѯ�������
nWidth: ���ǿ��ֵ��������Ϊ��λ������Сֵ0�����10000000���� 0���ø��ǡ�
nHeight: ���Ǹ߶�ֵ��������Ϊ��λ������Сֵ0�����ֵ10000000���� 0���ø��ǡ�
flDeviceScaleFactor: �����豸��������ֵ�� 0���ø��ǡ�
bMobile: �Ƿ�ģ���ƶ��豸�� ������ӿ�Ԫ��ǣ����ǹ��������ı��Զ������ȵȡ�
bFitWindow: ������������������������ͼ�Ƿ�Ӧ��С����Ӧ��
flScale: ������Ӧ���ڽ����ͼͼ�� ����| fitWindow | ģʽ��
nScreenWidth: ������Ļ���ֵ��������Ϊ��λ������Сֵ0�����10000000���� ֻ����| mobile == true |��
nScreenHeight: ������Ļ�߶�ֵ��������Ϊ��λ������С0�����10000000���� ֻ����| mobile == true |��
nPositionX: ����Ļ�ϸ�����ͼXλ�ã�������Ϊ��λ������Сֵ0�����ֵ10000000���� ֻ����| mobile == true |��
nPositionY: ����Ļ�ϸ�����ͼYλ�ã�������Ϊ��λ������Сֵ0�����ֵ10000000���� ֻ����| mobile == true |��
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetDeviceMetricsOverride(int nWidth,
	int nHeight,
	double flDeviceScaleFactor,
	BOOL bMobile,
	double flScale,
	int nScreenWidth,
	int nScreenHeight,
	int nPositionX,
	int nPositionY,
	BOOL dontSetVisibleSize,
	LPCTSTR szScreenOrientationType,
	int nScreenOrientationAngle)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setDeviceMetricsOverride"));
	json::value objParams;
	json::value objScreenOrientation;
	objParams[U("width")] = json::value::number(nWidth);
	objParams[U("height")] = json::value::number(nHeight);
	objParams[U("deviceScaleFactor")] = json::value::number(flDeviceScaleFactor);
	objParams[U("mobile")] = json::value::boolean(flDeviceScaleFactor);
	objParams[U("scale")] = json::value::number(flScale);
	objParams[U("screenWidth")] = json::value::number(nScreenWidth);
	objParams[U("screenHeight")] = json::value::number(nScreenHeight);
	objParams[U("positionX")] = json::value::number(nPositionX);
	objParams[U("positionY")] = json::value::number(nPositionY);
	objParams[U("fitWindow")] = json::value::boolean(true);
	if(szScreenOrientationType!=NULL)
		objScreenOrientation[U("type")] = json::value::string(szScreenOrientationType);
	objScreenOrientation[U("angle")] = json::value::number(nScreenOrientationAngle);

	objParams[U("screenOrientation")] = objScreenOrientation;

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ClearDeviceMetricsOverride
˵��: ��������豸ָ�ꡣ
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ClearDeviceMetricsOverride(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.clearDeviceMetricsOverride"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ForceViewport
˵��: ����ҳ��Ŀɼ����� ���Ĵ�ҳ�����أ����ɹ۲�Ĺ���λ�ú�ҳ��������ı䡣 ʵ���ϣ������ҳ���ָ�������ƶ�����ܵ����Ͻǡ�
x: �������Ͻǵ�X���꣨CSS���أ���
y: �������Ͻǵ�Y���꣨CSS���أ���
scale: ������Ӧ���ڸ����������1.0��ҳ���������
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ForceViewport(double x, double y, double scale)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.forceViewport"));
	json::value objParams;
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("scale")] = json::value::number(scale);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ResetViewport
˵��: ��ҳ��Ŀɼ���������Ϊԭʼ�ӿڣ�����forceViewport������κ�Ч����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ResetViewport(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.resetViewport"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ResetPageScaleFactor
˵��: ����ҳ��������ӱ�����Ϊ��ʼֵ��
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ResetPageScaleFactor(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.resetPageScaleFactor"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetPageScaleFactor
˵��: �����ض���ҳ���������
flPageScaleFactor: ҳ��������ӡ�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetPageScaleFactor(double flPageScaleFactor)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setPageScaleFactor"));
	json::value objParams;
	objParams[U("pageScaleFactor")] = json::value::number(flPageScaleFactor);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetVisibleSize
˵��: ����ҳ��Ŀ��/�ӿڴ�С�� ��ע�⣬�ⲻ��Ӱ���ܵ�������������������ڣ��� ����������ָ����С����Ļ��ͼ�� Android��֧��
nWidth: ��ȣ�DIP����
nHeight: �߶ȣ�DIP����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetVisibleSize(int nWidth, int nHeight)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setVisibleSize"));
	json::value objParams;
	objParams[U("width")] = json::value::number(nWidth);
	objParams[U("width")] = json::value::number(nWidth);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetGeolocationOverride
˵��: ���ǵ���λ��λ�û���� ʡ���κβ���ģ��λ�ò����á�
flLatitude: ģ��γ��
flLongitude: ģ�⾭��
flAccuracy: ģ�⾫��
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetGeolocationOverride(double flLatitude, double flLongitude, double flAccuracy)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setGeolocationOverride"));
	json::value objParams;
	objParams[U("latitude")] = json::value::number(flLatitude);
	objParams[U("longitude")] = json::value::number(flLongitude);
	objParams[U("accuracy")] = json::value::number(flAccuracy);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ClearGeolocationOverride
˵��: ������ǵĵ���λ��λ�úʹ���
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ClearGeolocationOverride(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.clearGeolocationOverride"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetTouchEmulationEnabled
˵��: �л���������¼��Ĵ����¼����档
bEnabled: �����¼������Ƿ�Ӧ����
szConfiguration: ����/�����¼����á� Ĭ�ϣ���ǰƽ̨�� �����ֵ��mobile, desktop
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetTouchEmulationEnabled(BOOL bEnabled, LPCTSTR configuration)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setEmitTouchEventsForMouse"));
	json::value objParams;
	objParams[U("enabled")] = json::value::boolean(bEnabled);
	objParams[U("configuration")] = json::value::string(configuration);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetEmulatedMedia
˵��: ΪCSSý���ѯģ�����ý�塣
szMedia: Ҫģ���ý������ ���ַ������ø��ǡ�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetEmulatedMedia(LPCTSTR szMedia)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setEmulatedMedia"));
	json::value objParams;
	if(szMedia!=NULL)
		objParams[U("media")] = json::value::string(szMedia);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetCPUThrottlingRate
˵��: ����CPU������ģ�⻺����CPU��
rate: ������Ϊ�������ӣ�1Ϊ�����ţ�2Ϊ2�����ٵȣ���
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetCPUThrottlingRate(double rate)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.setCPUThrottlingRate"));
	json::value objParams;
	objParams[U("rate")] = json::value::number(rate);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: CanEmulate
˵��: �����Ƿ�֧�ַ��档
����: �����������˵���ѿ�������֧��
*/
BOOL IChromeLink::CanEmulate(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Emulation.canEmulate"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		json::value objResult = objRoot[U("result")];
		return objResult[U("result")].as_bool();
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetUserAgentOverride
˵��: ����ʹ�ø������ַ��������û�����
szUserAgent: �û�����ʹ�á�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetUserAgentOverride(LPCTSTR szUserAgent)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.setUserAgentOverride"));
	json::value objParams;
	objParams[U("userAgent")] = json::value::string(szUserAgent);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetBlockedURLs
˵��: ������ֹ���ص�URL
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetBlockedURLs(std::vector<IString> &Urls)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	int nMax = Urls.size();
	if (nMax <= 0) return FALSE;
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.setBlockedURLs"));
	json::value objParams;
	json::value objArray;
	for (size_t i = 0; i < nMax; i++)
	{
		objArray[i] = json::value::string(Urls[i]);
	}
	objParams[U("urls")] = objArray;

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: CanClearBrowserCache
˵��: �����Ƿ�֧�������������档
����: �����������֧��������������
*/
BOOL IChromeLink::CanClearBrowserCache(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.canClearBrowserCache"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		json::value objResult = objRoot[U("result")];
		return objResult[U("result")].as_bool();
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ClearBrowserCache
˵��: ������������
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ClearBrowserCache(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.clearBrowserCache"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: CanClearBrowserCookies
˵��: �����Ƿ�֧����������cookie��
����: �����������֧����������cookie
*/
BOOL IChromeLink::CanClearBrowserCookies(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.canClearBrowserCookies"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		json::value objResult = objRoot[U("result")];
		return objResult[U("result")].as_bool();
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: ClearBrowserCookies
˵��: ���������е�����cookie
����: ���������������ǳɹ���
*/
BOOL IChromeLink::ClearBrowserCookies(void)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.clearBrowserCookies"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: GetCookies
˵��: ���ص�ǰ��ַ�����������Cookie�� ���ݺ��֧�֣�����Cookie�ֶ��з�����ϸ��Cookie��Ϣ��
szUrl: ����ȡ���õ�Cookie��URL�б�
Result: ���ȡ�õ�Cookie�б�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::GetCookies(LPCTSTR szUrl, IChromeCookieVectors &Result)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.getCookies"));
	json::value objParams;
	json::value objArray;
	objArray[0] = json::value::string(szUrl);
	Result.clear();
	objParams[U("urls")] = objArray;
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		try
		{
			json::value objResult = objRoot[U("result")];
			json::value objCookies = objResult[U("cookies")];
			int nMax = objCookies.size();
			IChromeCookie Info;
			for (size_t i = 0; i < nMax; i++)
			{
				json::value objChild = objCookies[i];
				Info.strDomain = objChild[U("domain")].as_string();
				Info.strName = objChild[U("name")].as_string();
				Info.strValue = objChild[U("value")].as_string();
				Info.strPath = objChild[U("path")].as_string();
				Info.bHttpOnly = objChild[U("httpOnly")].as_bool();
				Info.bSecure = objChild[U("secure")].as_bool();
				Result.push_back(Info);
			}
		}
		catch (const std::exception&)
		{
			return FALSE;
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����:GetAllCookies
˵��: �����������Cookie�� ���ݺ��֧�֣�����Cookie�ֶ��з�����ϸ��Cookie��Ϣ��
Result: ���ȡ�õ�Cookie�б�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::GetAllCookies(IChromeCookieVectors &Result)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.getAllCookies"));
	Result.clear();
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		try
		{
			json::value objResult = objRoot[U("result")];
			//MessageBox(NULL, objResult.serialize().c_str(), NULL, NULL);
			json::value objCookies = objResult[U("cookies")];
			int nMax = objCookies.size();
			IChromeCookie Info;
			for (size_t i = 0; i < nMax; i++)
			{
				json::value objChild = objCookies[i];
				Info.strDomain = objChild[U("domain")].as_string();
				Info.strName = objChild[U("name")].as_string();
				Info.strValue = objChild[U("value")].as_string();
				Info.strPath = objChild[U("path")].as_string();
				Info.bHttpOnly = objChild[U("httpOnly")].as_bool();
				Info.bSecure = objChild[U("secure")].as_bool();
				Info.expires = objChild[U("expires")].as_number().to_int64();
				Result.push_back(Info);
			}
		}
		catch (const std::exception&)
		{
			return FALSE;
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: DeleteCookie
˵��: ɾ�����и������ƣ����·���������cookie��
szCookieName: ����
szUrl: ƥ��domain��path��URL��
����: ���������������ǳɹ���
*/
BOOL IChromeLink::DeleteCookie(LPCTSTR szCookieName, LPCTSTR szUrl)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.deleteCookie"));
	json::value objParams;

	objParams[U("cookieName")] = json::value::string(szCookieName);
	objParams[U("url")] = json::value::string(szUrl);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SetCookie
˵��: ʹ�ø�����Cookie��������Cookie; ������ڵ�ͬ��cookie���ܻḲ�ǡ�
szUrl: ��cookie���ù�����request-URI�� ��ֵ���ܻ�Ӱ�촴����Cookie��Ĭ��domain��pathֵ��
szName: ����
szValue: ֵ
szDomain: ��
szPath: ·��
bSecure: Ĭ��Ϊfalse��
bHttpOnly: Ĭ��Ϊfalse.
����: ���������������ǳɹ���
*/
BOOL IChromeLink::SetCookie(LPCTSTR szUrl, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szDomain, LPCTSTR szPath, BOOL bSecure,__int64 expires, BOOL bHttpOnly)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Network.setCookie"));
	json::value objParams;

	objParams[U("url")] = json::value::string(szUrl);
	objParams[U("name")] = json::value::string(szName);
	objParams[U("value")] = json::value::string(szValue);
	objParams[U("domain")] = json::value::string(szDomain);
	objParams[U("path")] = json::value::string(szPath);
	objParams[U("secure")] = json::value::boolean(bSecure);
	objParams[U("expires")] = json::value::number(expires);
	objParams[U("httpOnly")] = json::value::boolean(bHttpOnly);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: GetResourceList
˵��: ��ȡҳ����Դ�б�
szFrameId: Ҫ��ȡ��Դ�Ŀ��
objVectors: ���ص���Դ�б�
����: ���������������ǳɹ���
*/
BOOL IChromeLink::GetResourceList( IChromeResourceVectors &objVectors)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	objVectors.clear();
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.getResourceTree"));

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();

		try
		{
			json::value  objResult = objRoot[U("result")];
			json::value objFrameTree = objResult[U("frameTree")];
			EnumResourceCallback(objVectors, objFrameTree);

		}
		catch (const std::exception&)
		{
			return FALSE;
		}
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

void IChromeLink::EnumResourceCallback(IChromeResourceVectors &result, json::value &root) {
	IChromeResource info;
	json::value &frame = root[U("frame")];
	info.strFrameId = frame[U("id")].as_string();

	json::value &res = root[U("resources")];
	int len = res.size();
	for (size_t i = 0; i < len; i++)
	{
		auto &elenment = res[i];
		info.strUrl = elenment[U("url")].as_string();
		info.strMimeType = elenment[U("mimeType")].as_string();
		info.strType = elenment[U("type")].as_string();
		info.dwContentSize = elenment[U("contentSize")].as_number().to_uint32();
		result.push_back(info);
	}

	try
	{
		auto &child = root[U("childFrames")];
		EnumResourceCallback(result, child);
		return;
	}
	catch (const std::exception&)
	{

	}

}

/*
����: DispatchKeyEvent
˵��: ��һ���ؼ��¼����͵�ҳ�档
szType: ���¼������͡� �����ֵ��keyDown��keyUp��rawKeyDown��char��
nModifiers: λ�ֶα�ʾ���µ��޸ļ��� Alt = 1��Ctrl = 2��Meta / Command = 4��Shift = 8��Ĭ��ֵ��0����
dwTimeSinceEpoch: UTCʱ�䣬����ƣ���1970��1��1�տ�ʼ
szText: ͨ��ʹ�ü��̲��ִ���������������ɵ��ı��� keyUp��rawKeyDown�¼�����Ҫ��Ĭ�ϣ�������
szUnModifiedText: ���û���޸��������£����ɼ������ɵ��ı�����λ���⣩�� ���ڿ�ݼ�����������������Ĭ�ϣ���������
szKeyIdfentifier: Ψһ����ʶ�������磬��U + 0041������Ĭ��ֵ����������
szCode: ÿ���������ΨһDOM������ַ���ֵ�����磬��KeyA������Ĭ�ϣ���������
szKey: Ψһ��DOM������ַ���ֵ�������˻���η������̲��ֵ��������м��ĺ��壨���磬��AltGr������Ĭ��ֵ����������
nWindowsVirtualKeyCode: Windows��������루Ĭ��ֵ��0����
nNativeVirtualKeyCode: ������������루Ĭ��ֵ��0����
bAutoRepeat: �¼��Ƿ����Զ��ظ����ɣ�Ĭ��ֵ��false����
bIsKeypad: �¼��Ƿ��С�������ɣ�Ĭ��ֵ��false����
bIsSysttemKey: �¼��Ƿ���ϵͳ���¼���Ĭ��ֵ��false����
��һ���ؼ��¼����͵�ҳ�档
*/
BOOL IChromeLink::DispatchKeyEvent(LPCTSTR szType,
	int nModifiers,
	DWORD dwTimeSinceEpoch,
	LPCTSTR szText,
	LPCTSTR szUnModifiedText,
	LPCTSTR szKeyIdfentifier,
	LPCTSTR szCode,
	LPCTSTR szKey,
	int nWindowsVirtualKeyCode,
	int nNativeVirtualKeyCode,
	BOOL bAutoRepeat,
	BOOL bIsKeypad, BOOL bIsSysttemKey)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.dispatchKeyEvent"));
	json::value objParams;
	objParams[U("type")] = json::value::string(szType);
	objParams[U("modifiers")] = json::value::number(nModifiers);
	objParams[U("timestamp")] = json::value::number((unsigned)time(NULL));
	objParams[U("text")] = json::value::string(szText);
	objParams[U("unmodifiedText")] = json::value::string(szUnModifiedText);
	objParams[U("keyIdentifier")] = json::value::string(szKeyIdfentifier);
	objParams[U("code")] = json::value::string(szCode);
	objParams[U("key")] = json::value::string(szKey);
	objParams[U("windowsVirtualKeyCode")] = json::value::number(nWindowsVirtualKeyCode);
	objParams[U("nativeVirtualKeyCode")] = json::value::number(nNativeVirtualKeyCode);
	objParams[U("autoRepeat")] = json::value::boolean(bAutoRepeat);
	objParams[U("isKeypad")] = json::value::boolean(bIsKeypad);
	objParams[U("isSystemKey")] = json::value::boolean(bIsKeypad);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: DispatchMouseEvent
˵��: ������¼����ȵ�ҳ�档
szType: ����¼������͡� �����ֵ��mousePressed��mouseReleased��mouseMoved��
x: �¼���X���������CSS�����е�����ܵ��ӿڡ�
y: �¼���Y���������CSS�����е�������ӿڡ� 0��ʾ�ӿڵĶ�����Y���Ž����ӿڵײ������ӡ�
dwTimeSinceEpoch: UTCʱ�䣬����ƣ���1970��1��1�տ�ʼ
szButton: ��갴ť��Ĭ��ֵ�����ޡ����� ����ֵ��none,left,middle,right
clickCount: ������갴ť�Ĵ�����Ĭ��ֵ��0����
����: ���������������ǳɹ���
*/
BOOL IChromeLink::DispatchMouseEvent(LPCTSTR szType,
	int x,
	int y,
	int modifiers,
	LPCTSTR szButton,
	int clickCount,
	DWORD deltaX,
	DWORD deltaY)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.dispatchMouseEvent"));
	json::value objParams;
	objParams[U("type")] = json::value::string(szType);
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("modifiers")] = json::value::number(modifiers);
	objParams[U("timestamp")] = json::value::number((unsigned)time(NULL));
	objParams[U("button")] = json::value::string(szButton);
	objParams[U("clickCount")] = json::value::number(clickCount);
	objParams[U("deltaX")] = json::value::number((uint32_t)deltaX);
	objParams[U("deltaY")] = json::value::number((uint32_t)deltaY);

	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: DispatchTouchEvent
˵��: ��ҳ���ϵ��ȴ����¼���
szType: �����¼������͡� �����ֵ��touchStart��touchEnd��touchMove��
nModifiers:
dwTimeStamp:
*/
BOOL IChromeLink::DispatchTouchEvent(LPCTSTR szType, IChromeTouchPoints *pTouchPoints, int nModifiers)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.dispatchTouchEvent"));
	json::value objParams;
	objParams[U("type")] = json::value::string(szType);
	if (pTouchPoints != NULL)
	{
		int nMax = pTouchPoints->size();
		if (nMax > 0)
		{
			json::value objTouchPoints = json::value::array(nMax);
			json::value objData;
			for (size_t i = 0; i < nMax; i++)
			{
				objData = json::value::null();
				objData[U("x")] = json::value::number((*pTouchPoints)[i].x);
				objData[U("y")] = json::value::number((*pTouchPoints)[i].y);
				objData[U("radiusX")] = json::value::number((*pTouchPoints)[i].rediusX);
				objData[U("radiusY")] = json::value::number((*pTouchPoints)[i].rediusY);
				objData[U("rotationAngle")] = json::value::number((*pTouchPoints)[i].rotationAngle);
				objData[U("force")] = json::value::number((*pTouchPoints)[i].force);
				objData[U("id")] = json::value::number((*pTouchPoints)[i].id);
				objTouchPoints[i] = objData;
			}
			objParams[U("touchPoints")] = objTouchPoints;
		}
	}
	else
	{
		json::value objTouchPoints = json::value::array(0);
		objParams[U("touchPoints")] = objTouchPoints;
	}
	objParams[U("modifiers")] = json::value::number(nModifiers);
	objParams[U("timestamp")] = json::value::number((unsigned)time(NULL));
	objRoot[TEXT("params")] = objParams;

	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;
	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot2 = pplx::create_task(objResponse).get();
		//MessageBox(NULL, objRoot2.serialize().c_str(), NULL, NULL);
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: EmulateTouchFromMouseEvent
˵��: ������¼�����ģ�ⷢ��һ�������¼���ҳ��
*/
BOOL IChromeLink::EmulateTouchFromMouseEvent(LPCTSTR szType, int x, int y, LPCTSTR szButton, double deltaX, double deltaY, int modifiers, int clickCount)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.emulateTouchFromMouseEvent"));
	json::value objParams;
	objParams[U("type")] = json::value::string(szType);
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("timestamp")] = json::value::number((unsigned)time(NULL));
	objParams[U("button")] = json::value::string(szButton);
	objParams[U("deltaX")] = json::value::number((uint32_t)deltaX);
	objParams[U("deltaY")] = json::value::number((uint32_t)deltaY);
	objParams[U("modifiers")] = json::value::number(modifiers);
	objParams[U("clickCount")] = json::value::number(clickCount);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����:  SynthesizePinchGesture
˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳ��������
*/
BOOL IChromeLink::SynthesizePinchGesture(double x, double y, double scaleFactor, int relativeSpeed, LPCTSTR gestureSourceType)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.synthesizePinchGesture"));
	json::value objParams;
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("scaleFactor")] = json::value::number(scaleFactor);
	objParams[U("relativeSpeed")] = json::value::number(relativeSpeed);
	objParams[U("gestureSourceType")] = json::value::string(gestureSourceType);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SynthesizeScrollGesture
˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳɹ�������
*/
BOOL IChromeLink::SynthesizeScrollGesture(double x, double y,
	double xDistance, double yDistance,
	double xOverscroll, double yOverscroll,
	BOOL preventFling, int speed, LPCTSTR gestureSourceType, int repeatCount, int repeatDelayMs)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.synthesizeScrollGesture"));
	json::value objParams;
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("xDistance")] = json::value::number(xDistance);
	objParams[U("yDistance")] = json::value::number(yDistance);
	objParams[U("xOverscroll")] = json::value::number(xOverscroll);
	objParams[U("yOverscroll")] = json::value::number(yOverscroll);
	objParams[U("preventFling")] = json::value::boolean(preventFling);
	objParams[U("speed")] = json::value::number(speed);
	objParams[U("gestureSourceType")] = json::value::string(gestureSourceType);
	objParams[U("repeatCount")] = json::value::number(repeatCount);
	objParams[U("repeatDelayMs")] = json::value::number(repeatDelayMs);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: SynthesizeTapGesture
˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳɵ��(Tep)����
*/
BOOL IChromeLink::SynthesizeTapGesture(double x, double y, int duration, int tapCount, LPCTSTR gestureSourceType)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Input.synthesizeTapGesture"));
	json::value objParams;
	objParams[U("x")] = json::value::number(x);
	objParams[U("y")] = json::value::number(y);
	objParams[U("duration")] = json::value::number(duration);
	objParams[U("tapCount")] = json::value::number(tapCount);
	objParams[U("gestureSourceType")] = json::value::string(gestureSourceType);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����:IsJavaScriptDialogOpening
˵��: ��鵱ǰҳ���Ƿ��ѵ���alert,confirm,prompt,beforeunload����ʾ��
����: ������������ʾ�жԻ������
*/
BOOL IChromeLink::IsJavaScriptDialogOpening(void)
{
	return this->m_javascriptDialogInfomation.bDialogLifed;
}

/*
����: HandleJavaScriptDialog
˵��: ���ܻ���JavaScript�����ĶԻ���alert��confirm��prompt��onbeforeunload����
*/
BOOL IChromeLink::HandleJavaScriptDialog(BOOL accept, LPCTSTR promptText)
{
	ASSERT_KEEP_LINKEDR(FALSE);
	DWORD dwAutoId = this->GetAutoRand();
	json::value objRoot;
	objRoot[U("id")] = json::value::number((int)dwAutoId);
	objRoot[U("method")] = json::value::string(U("Page.handleJavaScriptDialog"));
	json::value objParams;
	objParams[U("accept")] = json::value::boolean(accept);
	if(promptText!=NULL && lstrlen(promptText)>0)
		objParams[U("promptText")] = json::value::string(promptText);
	objRoot[TEXT("params")] = objParams;
	std::string strBody = conversions::utf16_to_utf8(objRoot.serialize());
	websocket_outgoing_message objMsg;
	pplx::task_completion_event<json::value> objResponse;

	objMsg.set_utf8_message(strBody);
	this->m_hUnionMutex.lock();
	this->m_mapRequestQueues.insert(std::make_pair(dwAutoId, objResponse));
	this->m_hUnionMutex.unlock();
	try
	{
		this->m_wsKeepLink.send(objMsg).get();
	}
	catch (const std::exception &ex)
	{
		return FALSE;
	}
	try
	{
		json::value objRoot = pplx::create_task(objResponse).get();
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
	return FALSE;
}

/*
����: GetJavaScriptDialogInfomation
Info: ���ػ�ȡ����javascript�Ի�����Ϣ
˵��: ��ȡJavaScript�Ի�����Ϣ
*/
void IChromeLink::GetJavaScriptDialogInfomation(IChromeJavaScriptDialogInfomation &Info)
{
	Info.bDialogLifed = m_javascriptDialogInfomation.bDialogLifed;
	Info.strURL = m_javascriptDialogInfomation.strURL;
	Info.strMessage = m_javascriptDialogInfomation.strMessage;
	Info.strDefaultPrompt = m_javascriptDialogInfomation.strDefaultPrompt;
	Info.type = m_javascriptDialogInfomation.type;
}

/*
����: GetConsoleLogs
logs: ���ؿ���̨��Ϣ
��ȡ������־��Ϣ
*/
void IChromeLink::GetConsoleLogs(std::wstring & logs)
{
	std::wstringstream ssBuffer;
	if (m_consoleInfos.empty())
		return;
	for (auto v: m_consoleInfos)
	{
		ssBuffer << v << TEXT("\r\n");
	}
	logs = ssBuffer.str();
	m_consoleInfos.clear();
}
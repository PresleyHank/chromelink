#pragma once
#include <Windows.h>
#include <sstream>
#include <string>
#include <Shlwapi.h>
#include <vector>
#include <list>
#include "cpprest\containerstream.h"
#include "cpprest\filestream.h"
#include "cpprest\http_client.h"
#include "cpprest\json.h"
#include "cpprest\producerconsumerstream.h"
#include "cpprest\details\web_utilities.h"
#include "cpprest\uri.h"
#include "cpprest\streams.h"
#include "cpprest\ws_client.h"
#include "cpprest\ws_msg.h"
#include "cpprest\asyncrt_utils.h"
#include <tchar.h>
#include <map>
#include <mutex>
#include <iostream>
#include <fstream>
#include <thread>
#include <malloc.h>
#include <Shellapi.h>
#include <thread>
#include <malloc.h>


#ifndef OVERRIDE
#define OVERRIDE override
#endif

/*
stable (1.2)

The 1.2 version of the protocol is the latest stable release of the protocol, tagged at Chrome 54.
It includes a smaller subset of the complete protocol compatibilities.

https://chromedevtools.github.io/devtools-protocol/1-2

*/

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"User32.lib")


using namespace ::pplx;
using namespace utility;
using namespace web;
using namespace concurrency::streams;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;
using namespace web::websockets::client;

namespace Komari {

#ifdef _UNICODE 
	typedef std::wstringstream IStream;
	typedef std::wstring IString;
#else
	typedef std::stringstream IStream;
	typedef std::string IString;
#endif

	/*
		����: IChromeTabInfo
		˵��: ��ǩ����Ϣ���ͽṹ
	*/
	struct IChromeTabInfo {
		//��ǩ����
		IString strDescription;

		//Web���Ե�ַ
		IString strDevToolsFrontendUrl;

		//��ǩID
		IString strId;

		//��ǩ����
		IString strTitle;

		//��ǩ����
		IString strType;

		//��ַ
		IString strUrl;

		//WS���Ե�ַ
		IString strWebSocketDebuggerUrl;
	};

	//���д򿪵ı�ǩҳ��Ϣ
	typedef std::vector<IChromeTabInfo> IChromeCurTabs;

	//��������
	typedef std::map<DWORD, pplx::task_completion_event<json::value>> IChromeResponse;

	//ExecutionContextId �б�
	struct IChromeExecutionContextId {
		DWORD dwExecutionContextId;
		IString strFrameId;
	};
	typedef std::vector<IChromeExecutionContextId> IChromeV8ExecutionContextIdVectors;

	//����б�
	struct IChromeFrameId {
		IString strFrameId;
		IString strLoaderId;
		IString strMimeType;
		IString strName;
		IString strParentId;
		IString strUrl;
	};
	typedef std::vector<IChromeFrameId> IChromeFrameIdVectors;

	//�ű�������Ϣ
	struct IChromeExceptionDetails {
		DWORD dwExceptionId;
		IString strText;
		DWORD dwLineNumber;
		DWORD dwColumnNumber;
		IString strScriptId;
		IString strUrl;
		DWORD dwexecutionContextId;
	};
	struct IChromeCompileResult {
		IString strScriptId;
		IChromeExceptionDetails objExceptionDetails;
		IChromeCompileResult()
		{
			objExceptionDetails.dwExceptionId = 0;
			objExceptionDetails.dwLineNumber = 0;
			objExceptionDetails.dwColumnNumber = 0;
			objExceptionDetails.dwexecutionContextId = 0;
		}
	};

	//���ʽִ�н��
	struct IChromeEvalResult {
		IString strDescription;
		IString strType;
		IString strSubType;
		IString strClassName;
		IString strValue;
		BOOL bValue;
		double flValue;
		int nValue;
		BOOL bException;
		IChromeEvalResult()
		{
			bException = FALSE;	//�Ƿ���ڴ���
			bValue = FALSE;
			flValue = 0.0f;
			nValue = 0;
		}
	};

	struct IChromeTouchPoint {
		double x;	//�¼���X�������������ܵ��ӿڡ�
		double y;//�¼���Y�������������ܵ��ӿڡ� 0��ʾ�ӿڵĶ�����Y���Ž����ӿڵײ������ӡ�
		double rediusX; //���������X�뾶��Ĭ��ֵ��1����
		double rediusY; //���������Y�뾶��Ĭ��ֵ��1����
		double rotationAngle; //��ת�Ƕȣ�Ĭ��ֵ��0.0����
		double force;//Force (default: 1.0).
		double id;//���ڸ����¼�֮��Ĵ���Դ�ı�ʶ�����¼��б�����Ψһ�ġ�
	};

	typedef std::vector<IChromeTouchPoint> IChromeTouchPoints;

	enum IChromeReadyStatus {
		ICR_SIGNAL,
		ICR_WAITING
	};

	//COOKIE��Ϣ
	struct IChromeCookie {
		IString strName;
		IString strValue;
		IString strDomain;
		IString strPath;
		BOOL bHttpOnly;
		BOOL bSecure;
		__int64 expires;
	};
	typedef std::vector<IChromeCookie> IChromeCookieVectors;

	//��Դ��Ϣ
	struct IChromeResource {
		IString strUrl;
		IString strType;
		IString strMimeType;
		DWORD dwContentSize;
		IString strFrameId;
	};
	typedef std::vector<IChromeResource> IChromeResourceVectors;

	//JavaScript�Ի�����Ϣ
	enum IChromeJavaScriptDialogType {
		IC_JSDT_ALERT,
		IC_JSDT_CONFIRM,
		IC_JSDT_PROMPT,
		IC_JSDT_BEFOREUNLOAD
	};

	struct IChromeJavaScriptDialogInfomation {
		IString strURL;
		IString strMessage;
		IChromeJavaScriptDialogType type;
		IString strDefaultPrompt;
		BOOL bDialogLifed;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	//��������ָ��
	class __declspec(novtable) IChromeAutoSink {
	public:
		void virtual AddRef(void) = 0;		//�������ü���
		bool virtual Release(void) = 0;		//�ͷ����ü���
		bool virtual HasOneRef(void) = 0;		//�Ƿ�ֻ��һ������
	};
	//���ü�����
	class IChromeRefCount : public IChromeAutoSink {
	public:
		IChromeRefCount() :ref_count_(0) {};

	public:
		virtual void AddRef(void) OVERRIDE { InterlockedIncrement(&ref_count_); }
		virtual bool Release(void) OVERRIDE { volatile long res = InterlockedDecrement(&ref_count_); if (res == 0) return true; else return false; }
		virtual bool HasOneRef(void) OVERRIDE { if (ref_count_ == 1) return true; else return false; }

	private:
		volatile long ref_count_;
	};

	//�������ü�����
#define IMPLEMENT_REFCOUNTING(ClassName) \
public: \
	void AddRef() { ref_.AddRef(); } \
	bool Release() {\
if (ref_.Release()) {\
	delete static_cast<const ClassName*>(this); \
	return true; \
} \
else return false; \
	}\
	bool HasOneRef() { return ref_.HasOneRef(); } \
private: \
	IChromeRefCount ref_;
	/////////////////////////////////////////////////////////////////////////////////////////////

	//Chrome���ʽӿ���
	class IChromeLink : public IChromeAutoSink {
	public:
		explicit IChromeLink(LPCTSTR szWebSocketDebuggerUrl);
		~IChromeLink();

	protected:
		IMPLEMENT_REFCOUNTING(IChromeLink);

	public:
		/////////////////////////��̬����/////////////////////////

		/*
				����: GetTabs
				˵��: ��ȡ������������Ѵ򿪵ı�ǩҳ��Ϣ
				szTraceAddr : ��������Ե�ַ
				Tabs : ���ػ�õı�ǩҳ�б�
				����: �����������˵�������ǳɹ��ġ�
		*/
		BOOL static GetTabs(LPCTSTR szTraceAddr, IChromeCurTabs &Tabs);

		/*
				����: NewTab
				˵��: ��һ���µı�ǩҳ
				szTraceAddr : ��������Ե�ַ
				szUrl : ����ҳ���ʵĵ�ַ
				Tab : �����Ѵ򿪵ı�ǩҳ��Ϣ
				����: �����������˵�������ǳɹ��ġ�
		*/
		BOOL static NewTab(LPCTSTR szTraceAddr, LPCTSTR szUrl, IChromeTabInfo &Tab);

		/*
				����: CloseTab
				˵��: �ر�һ����ǩҳ��
				szTraceAddr : ��������Ե�ַ
				szId : ��ǩ��ID
				����: �����������˵�������ǳɹ��ġ�
		*/
		BOOL static CloseTab(LPCTSTR szTraceAddr, LPCTSTR szId);

		/*
				����: ActivateTab
				˵��: ����һ����ǩҳ��
				szTraceAddr : ��������Ե�ַ
				szId : ��ǩ��ID
				����: �����������˵�������ǳɹ��ġ�
		*/
		BOOL static ActivateTab(LPCTSTR szTraceAddr, LPCTSTR szId);

		/*
				����: GetTabCount
				˵��: ��ȡ��ǩҳ������
				����: ����ɹ�������������
		*/
		int static GetTabCount(LPCTSTR szTraceAddr);

	public:

		/*
				����: LinkStart
				˵��: ������ָ���ı�ǩҳ��
				����: ������ӳɹ��������档
		*/
		BOOL LinkStart(void);

		/*
				����: Close
				˵��: �ر�����
		*/
		void Close(void);

	private:
		//ws���Ӷ���
		websocket_client m_wsKeepLink;

		//���ڹ���Request��Response�Ļ������
		std::mutex m_hUnionMutex;
		IChromeResponse m_mapRequestQueues;

		//����ID
		volatile DWORD m_dwAutoIdentify;

		//����״̬
		BOOL m_bLinkStatus;
#define ASSERT_KEEP_LINKEDR(x)	if(this->m_bLinkStatus == FALSE) return x;
#define ASSERT_KEEP_LINKED	if(this->m_bLinkStatus == FALSE) return;

		//V8 ExecutionContextId
		IChromeV8ExecutionContextIdVectors m_vecExecutionContextId;

		//��Ӧ�¼�
		pplx::task_completion_event<BOOL> m_taskReady;

		//�ӿ���б�
		IChromeFrameIdVectors m_vecChildFrameIdLists;

		//�����
		IChromeFrameId m_objMainFrameId;

		//������ҵ����
		HANDLE m_hEventReady;

		//����״̬
		BOOL m_bReadyState;

		//��ʼ��״̬
		BOOL m_bInitializedSucess;

		//����̨��Ϣ
		std::vector<std::wstring> m_consoleInfos;

	private:
		//���Ե�ַ
		IString m_strTraceAddr;

		//JavaScript�Ի�����Ϣ
		IChromeJavaScriptDialogInfomation m_javascriptDialogInfomation;

	private:
		//�ڲ���Ϣ�����߳�
		void CALLBACK OnEventDispatchs(IChromeLink *pChromeLink);

		//�����¼���Ϣ
		void SendEventDispatchs(const json::value &objRoot);

		//��ȡһ������ID
		DWORD GetAutoRand(void);

		//��ȡ��ҳ�ṹ
		pplx::task<json::value> GetWebSheet(void);

		//������ҳ�ṹ
		void OnParserWebTree(const json::value &objRoot);

		//������ҳ���
		void EnumWebFrame(const json::value &objRoot);

		//��ȡ���
		void GetWebFrame(const json::value &objRoot);

		//��ȡV8������ִ�л���
		pplx::task<BOOL> GetExecutionContextId(void);
	private:
		///////////////////////////��ӿ���///////////////////////

		/*
					����: OnLinkSucess
					˵��: �ڳɹ����ӵ����������г�ʼ�����á�
					����: ��ʼ���ɹ��򷵻��档
		*/
		BOOL OnLinkSucess(void);

		/*
					����: OpenPageEvents
					˵��: ��������"Page"����¼���Ϣ
					����: �ɹ�����Response��
		*/
		pplx::task<json::value> OpenPageEvents(void);

		/*
					����: OpenRuntimeEvents
					˵��: ��������"Runtime"����¼���Ϣ
					����: �ɹ�����Response
		*/
		pplx::task<json::value> OpenRuntimeEvents(void);


		BOOL OpenNetwork(void);

	public:
		/*
					����: GetMainFrame
					˵��: ��ȡ��ǩҳ�е�����ܶ���
					����: �ɹ��������������Ϣ��
		*/
		const IChromeFrameId &GetMainFrame(void);


		/*
					����: GetFrameCount
					˵��: ��ȡ��ǩҳ�е��ӿ����
					����: �ɹ������ӿ��������
		*/
		int GetFrameCount(void);

		/*
					����: GetChildFrame
					˵��: ��ȡ�ӿ�ܶ���
					index: �±�,��0��ʼ
					����: �ɹ��������ӿ����Ϣ��
		*/
		const IChromeFrameId &GetChildFrame(int index);

		/*
					����: GetExecutionContextCount
					˵��: ��ȡV8������ִ�л�������
					����: �ɹ����ػ�������
		*/
		int GetExecutionContextCount(void);

		/*
					����: GetExecutionContextId
					˵��: ��ȡV8������ִ�л���
					����: �ɹ�����������ִ�л�������
		*/
		const IChromeExecutionContextId &GetExecutionContextId(int index);

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
		BOOL Evaluate(LPCTSTR szExpression, LPCTSTR szObjectGroup, BOOL bIncludeCommandLineAPI, BOOL bSilent, DWORD dwExecutionContextId, BOOL bReturnByValue, BOOL bGeneratePreview, BOOL bUserGesture, BOOL bAwaitPromise, IChromeEvalResult &objResult);

		/*
					����: ReleaseObjectGroup
					˵��: �ͷ����ڸ����������Զ�̶���
					szObjectGroup: ��������
					����: �ɹ�������,ʧ�ܷ��ؼ�
		*/
		BOOL ReleaseObjectGroup(LPCTSTR szObjectGroup);

		/*
					����:  SetReady
					˵��: ���þ���״̬
					enStatus: ���þ���״̬����
		*/
		void SetReady(IChromeReadyStatus enStatus);

		/*
					����: WaitReady
					nWaitTime: �ȴ���ʱ��,Ĭ��Ϊ: INFINITE,�����Ƽ�ʹ��Ĭ��
					˵��: �ȴ�������״̬
					����: ���صȴ��Ľ��
		*/
		IChromeReadyStatus WaitReady(int nWaitTime = INFINITE);

		/*
					����: Navigate
					˵��: ��ת��ָ��ҳ��
					szUrl: �����ʵĵ�ַ
					szReferrer: ��·����
					����: ���������������ǳɹ���
		*/
		BOOL Navigate(LPCTSTR szUrl, LPCTSTR szReferrer);

		/*
					����: ReLoad
					˵��: ���¼���ҳ��
					bIgnoreCache: �Ƿ���Ի���������¼���
					szScriptToEvaluateOnLoad: ������ã��ű��������¼��غ�ע�뱻���ҳ�������֡��
					����: ���������������ǳɹ���
		*/
		BOOL ReLoad(BOOL bIgnoreCache, LPCTSTR szScriptToEvaluateOnLoad);

		/*
					����: GetResourceContent
					˵��: ���ظ�����Դ������
					strRetContent: ���ػ�õ���Դ����
					bBase64Encoded: ���ص������Ƿ���ʹ��base64����
					����: ���������������ǳɹ���
		*/
		BOOL GetResourceContent(LPCTSTR szFrameId, LPCTSTR szUrl, IString &strRetContent, BOOL &bBase64Encoded);

		/*
					����: IsLinked
					˵��: �Ƿ���Ȼ����������
					����: �����Ȼ�����򷵻���,�ѶϿ��򷵻ؼ�
		*/
		BOOL IsLinked(void) { return this->m_bLinkStatus; }

		/*
					����: UpdateFrame
					˵��: ��������¼�Ŀ����Ϣ
					����: ���������������ǳɹ���
		*/
		BOOL UpdateFrame(void);

		/*
					����: SetDocumentContent
					˵��: ����ָ������е�HTML����
					����: ���������������ǳɹ���
		*/
		BOOL SetDocumentContent(LPCTSTR szFrameId, LPCTSTR szHtml);

		/*
					����: CaptureScreenshot
					˵��: ����ҳ�����
					szFormat: ���յĴ洢��ʽ,��ѡֵ: jpeg,png
					nQuality: ѹ��������Χ(0-100),����jpeg
					bFromSurface: �ӱ��沶����Ļ��ͼ����������ͼ�� Ĭ��Ϊtrue.
					����: ���������������ǳɹ���
		*/
		BOOL CaptureScreenshot(LPCTSTR szFormat, int nQuality, BOOL bFromSurface, IString &strData);

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
					dontSetVisibleSize: ���ı��������С
					szScreenOrientationType: ��Ļ����
					nScreenOrientationAngle: ��Ļ�Ƕ�
					����: ���������������ǳɹ���
		*/
		BOOL SetDeviceMetricsOverride(int nWidth,
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
			int nScreenOrientationAngle);

		/*
					����: ClearDeviceMetricsOverride
					˵��: ��������豸ָ�ꡣ
					����: ���������������ǳɹ���
		*/
		BOOL ClearDeviceMetricsOverride(void);

		/*
					����: ForceViewport
					˵��: ����ҳ��Ŀɼ����� ���Ĵ�ҳ�����أ����ɹ۲�Ĺ���λ�ú�ҳ��������ı䡣 ʵ���ϣ������ҳ���ָ�������ƶ�����ܵ����Ͻǡ�
					x: �������Ͻǵ�X���꣨CSS���أ���
					y: �������Ͻǵ�Y���꣨CSS���أ���
					scale: ������Ӧ���ڸ����������1.0��ҳ���������
					����: ���������������ǳɹ���
		*/
		BOOL ForceViewport(double x, double y, double scale);

		/*
					����: ResetViewport
					˵��: ��ҳ��Ŀɼ���������Ϊԭʼ�ӿڣ�����forceViewport������κ�Ч����
					����: ���������������ǳɹ���
		*/
		BOOL ResetViewport(void);

		/*
					����: ResetPageScaleFactor
					˵��: ����ҳ��������ӱ�����Ϊ��ʼֵ��
					����: ���������������ǳɹ���
		*/
		BOOL ResetPageScaleFactor(void);

		/*
					����: SetPageScaleFactor
					˵��: �����ض���ҳ���������
					flPageScaleFactor: ҳ��������ӡ�
					����: ���������������ǳɹ���
		*/
		BOOL SetPageScaleFactor(double flPageScaleFactor);

		/*
					����: SetVisibleSize
					˵��: ����ҳ��Ŀ��/�ӿڴ�С�� ��ע�⣬�ⲻ��Ӱ���ܵ�������������������ڣ��� ����������ָ����С����Ļ��ͼ�� Android��֧��
					nWidth: ��ȣ�DIP����
					nHeight: �߶ȣ�DIP����
					����: ���������������ǳɹ���
		*/
		BOOL SetVisibleSize(int nWidth, int nHeight);

		/*
					����: SetGeolocationOverride
					˵��: ���ǵ���λ��λ�û���� ʡ���κβ���ģ��λ�ò����á�
					flLatitude: ģ��γ��
					flLongitude: ģ�⾭��
					flAccuracy: ģ�⾫��
					����: ���������������ǳɹ���
		*/
		BOOL SetGeolocationOverride(double flLatitude, double flLongitude, double flAccuracy);

		/*
					����: ClearGeolocationOverride
					˵��: ������ǵĵ���λ��λ�úʹ���
					����: ���������������ǳɹ���
		*/
		BOOL ClearGeolocationOverride(void);

		/*
					����: SetTouchEmulationEnabled
					˵��: �л���������¼��Ĵ����¼����档
					bEnabled: �����¼������Ƿ�Ӧ����
					szConfiguration: ����/�����¼����á� Ĭ�ϣ���ǰƽ̨�� �����ֵ��mobile, desktop
					����: ���������������ǳɹ���
		*/
		BOOL SetTouchEmulationEnabled(BOOL bEnabled, LPCTSTR configuration);

		/*
					����: SetEmulatedMedia
					˵��: ΪCSSý���ѯģ�����ý�塣
					szMedia: Ҫģ���ý������ ���ַ������ø��ǡ�
					����: ���������������ǳɹ���
		*/
		BOOL SetEmulatedMedia(LPCTSTR szMedia);

		/*
					����: SetCPUThrottlingRate
					˵��: ����CPU������ģ�⻺����CPU��
					rate: ������Ϊ�������ӣ�1Ϊ�����ţ�2Ϊ2�����ٵȣ���
					����: ���������������ǳɹ���
		*/
		BOOL SetCPUThrottlingRate(double rate);

		/*
					����: CanEmulate
					˵��: �����Ƿ�֧�ַ��档
					����: �����������˵���ѿ�������֧��
		*/
		BOOL CanEmulate(void);

		/*
					����: SetUserAgentOverride
					˵��: ����ʹ�ø������ַ��������û�����
					szUserAgent: �û�����ʹ�á�
					����: ���������������ǳɹ���
		*/
		BOOL SetUserAgentOverride(LPCTSTR szUserAgent);

		/*
					����: SetBlockedURLs
					˵��: ������ֹ���ص�URL
					����: ���������������ǳɹ���
		*/
		BOOL SetBlockedURLs(std::vector<IString> &Urls);

		/*
					����: CanClearBrowserCache
					˵��: �����Ƿ�֧�������������档
					����: �����������֧��������������
		*/
		BOOL CanClearBrowserCache(void);

		/*
					����: ClearBrowserCache
					˵��: ������������
					����: ���������������ǳɹ���
		*/
		BOOL ClearBrowserCache(void);

		/*
					����: CanClearBrowserCookies
					˵��: �����Ƿ�֧����������cookie��
					����: �����������֧����������cookie
		*/
		BOOL CanClearBrowserCookies(void);

		/*
					����: ClearBrowserCookies
					˵��: ���������е�����cookie
					����: ���������������ǳɹ���
		*/
		BOOL ClearBrowserCookies(void);

		/*
					����: GetCookies
					˵��: ���ص�ǰ��ַ�����������Cookie�� ���ݺ��֧�֣�����Cookie�ֶ��з�����ϸ��Cookie��Ϣ��
					szUrl: ����ȡ���õ�Cookie��URL�б�
					Result: ���ȡ�õ�Cookie�б�
					����: ���������������ǳɹ���
		*/
		BOOL GetCookies(LPCTSTR szUrl, IChromeCookieVectors &Result);

		/*
					����:GetAllCookies
					˵��: �����������Cookie�� ���ݺ��֧�֣�����Cookie�ֶ��з�����ϸ��Cookie��Ϣ��
					Result: ���ȡ�õ�Cookie�б�
					����: ���������������ǳɹ���
		*/
		BOOL GetAllCookies(IChromeCookieVectors &Result);

		/*
					����: DeleteCookie
					˵��: ɾ�����и������ƣ����·���������cookie��
					szCookieName: ����
					szUrl: ƥ��domain��path��URL��
					����: ���������������ǳɹ���
		*/
		BOOL DeleteCookie(LPCTSTR szCookieName, LPCTSTR szUrl);

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
		BOOL SetCookie(LPCTSTR szUrl, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szDomain, LPCTSTR szPath, BOOL bSecure,__int64 expires, BOOL bHttpOnly);

		/*
					����: GetResourceList
					˵��: ��ȡҳ����Դ�б�
					szFrameId: Ҫ��ȡ��Դ�Ŀ��
					objVectors: ���ص���Դ�б�
					����: ���������������ǳɹ���
		*/
		BOOL GetResourceList(IChromeResourceVectors &objVectors);
		void EnumResourceCallback(IChromeResourceVectors &objVectors,json::value &root);

		/*
					����: IsLoading
					˵��: ���������Ƿ���������ҳ����
					����: �����������˵������������ҳ��
		*/
		BOOL IsLoading(void) { return m_bReadyState; }

		/*
					����: SetIgnoreInputEvents
					˵��: ���������¼��������ҳ��ʱ�����ã���
					bIgnore: �Ƿ����
					����: ���������������ǳɹ���
		*/
		//BOOL SetIgnoreInputEvents(BOOL bIgnore);

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
		BOOL DispatchKeyEvent(LPCTSTR szType,
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
			BOOL bIsKeypad, BOOL bIsSysttemKey);

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
		BOOL DispatchMouseEvent(LPCTSTR szType,
			int x,
			int y,
			int modifiers,
			LPCTSTR szButton,
			int clickCount,
			DWORD deltaX,
			DWORD deltaY);

		/*
					����: DispatchTouchEvent
					˵��: ��ҳ���ϵ��ȴ����¼���
					szType: �����¼������͡� �����ֵ��touchStart��touchEnd��touchMove��
					pTouchPoints: �������б�
					nModifiers: ״̬��
		*/
		BOOL DispatchTouchEvent(LPCTSTR szType, IChromeTouchPoints *pTouchPoints, int nModifiers);

		/*
					����: EmulateTouchFromMouseEvent
					˵��: ������¼�����ģ�ⷢ��һ�������¼���ҳ��
		*/
		BOOL EmulateTouchFromMouseEvent(LPCTSTR szType, int x, int y, LPCTSTR szButton, double deltaX, double deltaY, int modifiers, int clickCount);

		/*
					����:  SynthesizePinchGesture
					˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳ��������
		*/
		BOOL SynthesizePinchGesture(double x, double y, double scaleFactor, int relativeSpeed, LPCTSTR gestureSourceType);

		/*
					����: SynthesizeScrollGesture
					˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳɹ�������
		*/
		BOOL SynthesizeScrollGesture(double x, double y,
			double xDistance, double yDistance,
			double xOverscroll, double yOverscroll,
			BOOL preventFling, int speed, LPCTSTR gestureSourceType, int repeatCount, int repeatDelayMs);

		/*
					����: SynthesizeTapGesture
					˵��: ͨ�������ʵ��Ĵ����¼�,��һ���¼��ںϳɵ��(Tep)����
		*/
		BOOL SynthesizeTapGesture(double x, double y, int duration, int tapCount, LPCTSTR gestureSourceType);


		/*
				����:IsJavaScriptDialogOpening
				˵��: ��鵱ǰҳ���Ƿ��ѵ���alert,confirm,prompt,beforeunload����ʾ��
				����: ������������ʾ�жԻ������
		*/
		BOOL IsJavaScriptDialogOpening(void);

		/*
				����: HandleJavaScriptDialog
				accept: ���ջ�Ӵ��Ի�
				promptText: �����ı�
				˵��: ���ܻ���JavaScript�����ĶԻ���alert��confirm��prompt��onbeforeunload����
		*/
		BOOL HandleJavaScriptDialog(BOOL accept, LPCTSTR promptText);

		/*
				����: GetJavaScriptDialogInfomation
				Info: ���ػ�ȡ����javascript�Ի�����Ϣ
				˵��: ��ȡJavaScript�Ի�����Ϣ
		*/
		void GetJavaScriptDialogInfomation(IChromeJavaScriptDialogInfomation &Info);

		/*
					����: GetConsoleLogs
					logs: ���ؿ���̨��Ϣ
					��ȡ������־��Ϣ
		*/
		void GetConsoleLogs(std::wstring & logs);
	};
}
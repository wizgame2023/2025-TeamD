
#include "stdafx.h"
#include "Project.h"

using namespace basecross;


LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

const wchar_t* pClassName = L"拳は銃より強し";
const wchar_t* pWndTitle = L"拳は銃より強し";
int g_ClientWidth = 1280;
int g_ClientHeight = 800;

BOOL IsMouseCursor = TRUE;
//--------------------------------------------------------------------------------------
//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//--------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = pClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	return RegisterClassEx(&wcex);
}

//--------------------------------------------------------------------------------------
//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//--------------------------------------------------------------------------------------
HWND InitInstance(HINSTANCE hInstance, int nCmdShow, bool isFullScreen, int iClientWidth, int iClientHeight)
{

	HWND hWnd = 0;
	if (isFullScreen) {
		iClientWidth = GetSystemMetrics(SM_CXSCREEN);
		iClientHeight = GetSystemMetrics(SM_CYSCREEN);
		hWnd = CreateWindow(
			pClassName,			
			pWndTitle,		
			WS_POPUP,			
			0,					
			0,					
			iClientWidth,		
			iClientHeight,		
			nullptr,			
			nullptr,			
			hInstance,			
			nullptr				
		);
		if (!hWnd) {
			MessageBox(nullptr, L"", L"", MB_OK);
			return 0;  
		}
	}
	else {
		RECT rc = { 0, 0, iClientWidth, iClientHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		hWnd = CreateWindow(
			pClassName,				
			pWndTitle,				
			WS_OVERLAPPEDWINDOW,	
			CW_USEDEFAULT,			
			CW_USEDEFAULT,			
			rc.right - rc.left,		
			rc.bottom - rc.top,		
			nullptr,				
			nullptr,				
			hInstance,				
			nullptr					
		);
		if (!hWnd) {
			MessageBox(nullptr, L"", L"", MB_OK);
			return 0;  
		}
	}
	ShowCursor(IsMouseCursor);
	ShowWindow(
		hWnd,      
		nCmdShow    
	);
	UpdateWindow(hWnd);
	return hWnd;
}

//--------------------------------------------------------------------------------------
//	int MainLoop(HINSTANCE hInstance, HWND hWnd, bool isFullScreen, int iClientWidth, int iClientHeight);
//	用途: メインループ
//--------------------------------------------------------------------------------------
int MainLoop(HINSTANCE hInstance, HWND hWnd, bool isFullScreen, int iClientWidth, int iClientHeight) {
	int RetCode = 0;
	WINDOWINFO WinInfo;
	ZeroMemory(&WinInfo, sizeof(WinInfo));
	try {
		if (FAILED(::CoInitialize(nullptr))) {
			throw exception("");
		}
		App::CreateApp(hInstance, hWnd, isFullScreen, iClientWidth, iClientHeight);
		auto ScenePtr = App::GetApp()->CreateScene<Scene>();
		MSG msg = { 0 };
		vector<DWORD> UseKeyVec = {
			VK_PRIOR,VK_NEXT,VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,VK_SPACE,
			VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_LCONTROL,
			'W','A','S','D','X','B','Z'
		};
		while (WM_QUIT != msg.message) {
			if (!App::GetApp()->ResetInputState(hWnd, UseKeyVec)) {
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			App::GetApp()->UpdateDraw(1);
		}
		RetCode = (int)msg.wParam;
	}
	catch (BaseException& e) {
		if (GetWindowInfo(hWnd, &WinInfo)) {
			MessageBox(hWnd, e.what_w().c_str(), L"", MB_OK);
		}
		else {
			MessageBox(nullptr, e.what_w().c_str(), L"", MB_OK);
		}
		RetCode = 1;
	}
	catch (BaseMBException& e) {
		if (GetWindowInfo(hWnd, &WinInfo)) {
			MessageBoxA(hWnd, e.what_m().c_str(), "", MB_OK);
		}
		else {
			MessageBoxA(nullptr, e.what_m().c_str(), "", MB_OK);
		}
		RetCode = 1;
	}
	catch (exception& e) {
		if (GetWindowInfo(hWnd, &WinInfo)) {
			MessageBoxA(hWnd, e.what(), "", MB_OK);
		}
		else {
			MessageBoxA(nullptr, e.what(), "", MB_OK);
		}
		RetCode = 1;
	}
	catch (...) {
		if (GetWindowInfo(hWnd, &WinInfo)) {
			MessageBox(hWnd, L"", L"", MB_OK);
		}
		else {
			MessageBox(nullptr, L"", L"", MB_OK);
		}
		RetCode = 1;
	}
	App::DeleteApp();
	::CoUninitialize();
	return RetCode;
}


//--------------------------------------------------------------------------------------
//	int APIENTRY _tWinMain();
//	用途: エントリポイント
//--------------------------------------------------------------------------------------
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	setlocale(LC_ALL, "JPN");

	bool isFullScreen = true;
	wstring wstrcmd = lpCmdLine;
	if (wstrcmd == L"/f" || wstrcmd == L"/F") {
		isFullScreen = true;
	}

	MyRegisterClass(hInstance);
	HWND hWnd = InitInstance(hInstance, nCmdShow, isFullScreen, g_ClientWidth, g_ClientHeight);

	if (!hWnd)
	{
		return FALSE;
	}

	return  MainLoop(hInstance, hWnd, isFullScreen, g_ClientWidth, g_ClientHeight);

}

//--------------------------------------------------------------------------------------
//
//  関数 : MakeWindowModeRectFunc()
//
//	文字化けにより不明
//
//--------------------------------------------------------------------------------------
void MakeWindowModeRectFunc(RECT& rc) {
	rc = { 0, 0, g_ClientWidth, g_ClientHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	int CXWidth = GetSystemMetrics(SM_CXSCREEN);
	int CYHeight = GetSystemMetrics(SM_CYSCREEN);
	int RCWidth = rc.right - rc.left;
	int RCHeight = rc.bottom - rc.top;
	int OffsetLeft = CXWidth / 2 - RCWidth / 2;
	int OffsetTop = CYHeight / 2 - RCHeight / 2;
	OffsetRect(&rc, OffsetLeft, OffsetTop);
}

//--------------------------------------------------------------------------------------
//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_RETURN:
			if (GetAsyncKeyState(VK_CONTROL)) {
				if (App::AppCheck()) {
					if (App::GetApp()->IsFullScreen()) {
						RECT rc;
						MakeWindowModeRectFunc(rc);
						App::GetApp()->SetWindowMode(rc);
					}
					else {
						App::GetApp()->SetFullScreenMode();
					}
				}
			}
			break;
		}
		break;
	case WM_SIZE:
		if (App::AppCheck()) {
			App::GetApp()->OnSize();
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

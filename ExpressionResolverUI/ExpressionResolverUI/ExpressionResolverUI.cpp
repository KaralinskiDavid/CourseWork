#include "expressionnode.h"
#include "framework.h"
#include "HistoryLine.h"
#include "ExpressionResolverUI.h"
#include <CommCtrl.h>
#include <vector>
#include <sstream>
#include <ctime>

#pragma comment(lib, "ComCtl32.Lib")
#pragma warning(disable : 4996)

#define MAX_LOADSTRING 100
#define POSTFIX 0
#define INFIX 1
#define PREFIX 2

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
std::vector<HWND> controls;
bool authorized = false;
std::string currentUser;
const std::vector<std::wstring> columnsName{ L"Date", L"Expression", L"Operation", L"Target notation", L"Result"};
HWND hWndListView;
int currentpage = 0;
int lastPage = 1;

HMODULE hmdCore = LoadLibrary(L"ExpressionResolverCore.dll");
HMODULE hmdDatabase = LoadLibrary(L"Database.dll");
typedef std::string(*rEX)(std::string, int to, bool calculate);
rEX resolveExpression = (rEX)GetProcAddress(hmdCore, "?resolveExpression@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V12@H_N@Z");
typedef ExpressionNode*(*gET)(std::string);
gET getExpressionTreeRoot = (gET)GetProcAddress(hmdCore, "?getExpressionTree@@YAPAVExpressionNode@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
typedef bool(*aCU)(std::wstring, std::wstring);
aCU authorize = (aCU)GetProcAddress(hmdDatabase, "?authorize@@YA_NV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@0@Z");
typedef bool(*rCU)(std::wstring, std::wstring);
rCU registerUser = (rCU)GetProcAddress(hmdDatabase, "?registerUser@@YA_NV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@0@Z");
typedef void(*aHL)(HistoryLine*);
aHL addToHistory = (aHL)GetProcAddress(hmdDatabase, "?addHistoryLine@@YAXPAVHistoryLine@@@Z");
typedef std::vector<HistoryLine*>(*gHI)(wstring, int);
gHI getHistory = (gHI)GetProcAddress(hmdDatabase, "?getUserHistory@@YA?AV?$vector@PAVHistoryLine@@V?$allocator@PAVHistoryLine@@@std@@@std@@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@2@H@Z");


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK       LoginProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK       RegisterProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK       ResolverProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK HistoryProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EXPRESSIONRESOLVERUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXPRESSIONRESOLVERUI));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXPRESSIONRESOLVERUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EXPRESSIONRESOLVERUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_SHOWMAXIMIZED);
   UpdateWindow(hWnd);

   return TRUE;
}


void showRegisterButton(HWND hWndParent)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    int width = rcClient.right / 5;
    int height = rcClient.bottom / 8;
    int positionX = rcClient.right/2 + width/2.5;
    int positionY = rcClient.bottom / 2.5 - height / 2;
    HWND hwndButton = CreateWindow(
        WC_BUTTON,
        L"Register",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        positionX, positionY, width, height,
        hWndParent,
        (HMENU)IDC_BUTTON_REGISTRATION,
        (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
        NULL);
    controls.push_back(hwndButton);
    ShowWindow(hwndButton, SW_SHOWDEFAULT);
}

void showLoginButton(HWND hWndParent)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    int width = rcClient.right / 5;
    int height = rcClient.bottom / 8;
    int positionX = rcClient.right / 2 + width / 2.5;
    int positionY = rcClient.bottom / 2.5 +height*3/2;
    HWND hwndButton = CreateWindow(
        WC_BUTTON,
        L"Log in",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        positionX, positionY, width, height,
        hWndParent,
        (HMENU)IDC_BUTTON_LOG_IN,
        (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
        NULL);
    controls.push_back(hwndButton);
    ShowWindow(hwndButton, SW_SHOWDEFAULT);
}

void showLogoutButton(HWND hWndParent)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    int width = rcClient.right / 5;
    int height = rcClient.bottom / 8;
    int positionX = rcClient.right / 3 + width / 2.5;
    int positionY = rcClient.bottom / 5 + height * 3 / 2;
    HWND hwndButton = CreateWindow(
        WC_BUTTON,
        L"Log out",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        positionX, positionY, width, height,
        hWndParent,
        (HMENU)IDC_BUTTON_LOGOUT,
        (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
        NULL);
    controls.push_back(hwndButton);
    ShowWindow(hwndButton, SW_SHOWDEFAULT);
}

void showWithoutAuthorizationButton(HWND hWndParent)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    int width = rcClient.right / 5;
    int height = rcClient.bottom / 8;
    int positionX = rcClient.right / 2 + width/2.5;
    int positionY = rcClient.bottom / 2.5 + height * 7 / 2;
    HWND hwndButton = CreateWindow(
        WC_BUTTON,
        L"Continue without authorization",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        positionX, positionY, width, height,
        hWndParent,
        (HMENU)IDC_BUTTON_WITHOUT_REGISTRATION,
        (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
        NULL);
    controls.push_back(hwndButton);
    ShowWindow(hwndButton, SW_SHOWDEFAULT);
}

void showResolveExpressionButton(HWND hWndParent)
{
    RECT rcClient;
    GetClientRect(hWndParent, &rcClient);
    int width = rcClient.right / 5;
    int height = rcClient.bottom / 8;
    int positionX = rcClient.right / 3 + width / 2.5;
    int positionY = rcClient.bottom / 5 + height * 7 / 2;
    HWND hwndButton = CreateWindow(
        WC_BUTTON,
        L"Resolve expression",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        positionX, positionY, width, height,
        hWndParent,
        (HMENU)IDC_BUTTON_WITHOUT_REGISTRATION,
        (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
        NULL);
    controls.push_back(hwndButton);
    ShowWindow(hwndButton, SW_SHOWDEFAULT);
}

void destroyAuthorizedControls(HWND hWnd)
{
    HWND hlogout = FindWindowEx(hWnd, NULL, WC_BUTTON, L"Log out");
    DestroyWindow(hlogout);
    controls.pop_back();
    HWND hresolve = FindWindowEx(hWnd, NULL, WC_BUTTON, L"Resolve expression");
    DestroyWindow(hresolve);
    controls.pop_back();
}

void destroyUnAuthorizedControls(HWND hWnd)
{
    HWND hregister = FindWindowEx(hWnd, NULL, WC_BUTTON, L"Register");
    DestroyWindow(hregister);
    controls.pop_back();
    HWND hlogin = FindWindowEx(hWnd, NULL, WC_BUTTON, L"Log in");
    DestroyWindow(hlogin);
    controls.pop_back();
    HWND hcontinue = FindWindowEx(hWnd, NULL, WC_BUTTON, L"Continue without authorization");
    DestroyWindow(hcontinue);
    controls.pop_back();
}

void showUnathorizedControls(HWND hWnd)
{
    if (controls.size() != 0)
        destroyAuthorizedControls(hWnd);
	showRegisterButton(hWnd);
	showLoginButton(hWnd);
	showWithoutAuthorizationButton(hWnd);
}

void showAuthorizedControls(HWND hWnd)
{
    destroyUnAuthorizedControls(hWnd);
    showLogoutButton(hWnd);
    showResolveExpressionButton(hWnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            showUnathorizedControls(hWnd);
            break;
        }
    case WM_SIZE:
        {
            break;
        }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDC_BUTTON_LOG_IN:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_AUTHORIZATION_DIALOG), hWnd, LoginProc);
                break;
            case IDC_BUTTON_REGISTRATION:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_REGISTER), hWnd, RegisterProc);
                break;
            case IDC_BUTTON_WITHOUT_REGISTRATION:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_RESOLVE_DIALOG), hWnd, ResolverProc);
                break;
            case IDC_BUTTON_LOGOUT:
                authorized = false;
                showUnathorizedControls(hWnd);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool isNotEmpty(string s)
{
    if (s.find_first_not_of(' ') != s.npos)
        return true;
    return false;
}

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

bool invokeAuthorization(HWND hdlg)
{
    WCHAR buffLogin[1024];
    WCHAR buffPassword[1024];
    GetDlgItemText(hdlg, IDC_LOGIN_EDIT, buffLogin, 1024);
    GetDlgItemText(hdlg, IDC_EDIT_PASSWORD, buffPassword, 1024);
    char lg[1024];
    char ps[1024];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, buffLogin, -1, lg, 1024, &DefChar, NULL);
    WideCharToMultiByte(CP_ACP, 0, buffPassword, -1, ps, 1024, &DefChar, NULL);
    std::string sl(lg);
    std::string sp(ps);
    if (!isNotEmpty(sl))
    {
        MessageBox(hdlg, L"Please, first enter login", L"No login entered", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (!isNotEmpty(sp))
    {
        MessageBox(hdlg, L"Please, first enter password", L"No password entered", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (authorize(s2ws(sl), s2ws(sp)))
    {
        authorized = true;
        currentUser = sl;
        return true;
    }
    return false;
}


bool invokeRegistration(HWND hdlg)
{
    WCHAR buffLogin[1024];
    WCHAR buffPassword[1024];
    GetDlgItemText(hdlg, IDC_EDIT_LOGIN, buffLogin, 1024);
    GetDlgItemText(hdlg, IDC_EDIT_PASSWORD, buffPassword, 1024);
    char lg[1024];
    char ps[1024];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, buffLogin, -1, lg, 1024, &DefChar, NULL);
    WideCharToMultiByte(CP_ACP, 0, buffPassword, -1, ps, 1024, &DefChar, NULL);
    std::string sl(lg);
    std::string sp(ps);
    if (!isNotEmpty(sl))
    {
        MessageBox(hdlg, L"Please, first enter login", L"No login entered", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (!isNotEmpty(sl))
    {
        MessageBox(hdlg, L"Please, first enter password", L"No pssword entered", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (registerUser(s2ws(sl), s2ws(sp)))
    {
        return true;
    }
    return false;
}

LPWSTR ConvertToLPWSTR(const std::wstring& s)
{
    LPWSTR ws = new wchar_t[s.size() + 1];
    copy(s.begin(), s.end(), ws);
    ws[s.size()] = 0;
    return ws;
}

int getChecked(HWND hdlg)
{
    if (IsDlgButtonChecked(hdlg, IDC_RADIO_POSTFIX))
        return POSTFIX;
    if (IsDlgButtonChecked(hdlg, IDC_RADIO_INFIX))
        return INFIX;
    if (IsDlgButtonChecked(hdlg, IDC_RADIO_PREFIX))
        return PREFIX;
    return -1;
}

void fillTranslation(HWND hdlg, int to)
{
    WCHAR buff[1024];
    GetDlgItemText(hdlg, IDC_EDIT_EXPRESSION, buff, 1024);
    char ch[260];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, buff, -1, ch, 260, &DefChar, NULL);
    std::string ss(ch);
    if (!isNotEmpty(ss))
    {
        MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
        return;
    }
    std::string s = resolveExpression(ss, to, false);
    HWND translationEdit = GetDlgItem(hdlg, IDC_EDIT_TRANSLATED);
    std::wstring ws = s2ws(s);
    SetWindowText(translationEdit, ws.c_str());
    wstring notation;
    switch (to)
    {
    case 0:
        notation = L"Postfix";
        break;
    case 1:
        notation = L"Infix";
        break;
    case 2:
        notation = L"Prefix";
        break;
    }
    if (authorized)
    {
        time_t now = time(0);
        string time(ctime(&now));
        time.pop_back();
        time.pop_back();
        HistoryLine* historyLine = new HistoryLine(s2ws(time), s2ws(ss), notation, ws);
        historyLine->username = s2ws(currentUser);
        addToHistory(historyLine);
    }
}

void fillCalculation(HWND hdlg)
{
    WCHAR buff[1024];
    GetDlgItemText(hdlg, IDC_EDIT_EXPRESSION, buff, 1024);
    char ch[1024];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, buff, -1, ch, 1024, &DefChar, NULL);
    std::string ss(ch);
    if (!isNotEmpty(ss))
    {
        MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
        return;
    }
    std::string s = resolveExpression(ss, 1, true);
    HWND calculationEdit = GetDlgItem(hdlg, IDC_EDIT_CALCULATED);
    std::wstring ws = s2ws(s);
    SetWindowText(calculationEdit, ws.c_str());
    if (authorized)
    {
        time_t now = time(0);
        string time(ctime(&now));
        time.pop_back();
        time.pop_back();
        HistoryLine* historyLine = new HistoryLine(s2ws(time), s2ws(ss), ws);
        historyLine->username = s2ws(currentUser);
        addToHistory(historyLine);
    }
}

HTREEITEM insertNode(HWND hTree, HTREEITEM hParent, ExpressionNode* node)
{
    TVITEM tvi;
    TVINSERTSTRUCT tvins;
    HTREEITEM hti;
    tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
    std::wstring text = s2ws(node->element);
    tvi.pszText = ConvertToLPWSTR(text);
    tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);
    tvins.item = tvi;
    tvins.hParent = hParent;
    HTREEITEM hCurr = (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM,
        0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);
    return hCurr;
}

void fillBranch(HWND hTree, ExpressionNode* node, HTREEITEM hPrev)
{
    if (node != NULL)
    {
        HTREEITEM hCurr = insertNode(hTree, hPrev, node);
        fillBranch(hTree, node->leftChild, hCurr);
        fillBranch(hTree, node->rightChild, hCurr);
    }
}

void fillTree(HWND hdlg)
{
    WCHAR buff[1024];
    GetDlgItemText(hdlg, IDC_EDIT_EXPRESSION, buff, 1024);
    char ch[1024];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, buff, -1, ch, 1024, &DefChar, NULL);
    std::string expression(ch);
    if (!isNotEmpty(expression))
    {
        MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
        return;
    }
    ExpressionNode* node = getExpressionTreeRoot(expression);
    if (node == NULL)
    {
        MessageBox(hdlg, L"Invalid expression", L"Error", MB_OK | MB_ICONWARNING);
        return;
    }
    HWND hTree = GetDlgItem(hdlg, IDC_TREE);
    TreeView_DeleteAllItems(hTree);
    fillBranch(hTree, node, NULL);
    if (authorized)
    {
        time_t now = time(0);
        string time(ctime(&now));
        time.pop_back();
        time.pop_back();
        HistoryLine* historyLine = new HistoryLine(s2ws(time), s2ws(expression));
        historyLine->username = s2ws(currentUser);
        addToHistory(historyLine);
    }
}

void fillListView(int currentPage)
{
    ListView_DeleteAllItems(hWndListView);
    std::vector<HistoryLine*> history = getHistory(s2ws(currentUser), currentPage);
    if (history.size() == 30)
        lastPage = currentPage+1;
    LVITEM lvI;

    lvI.pszText = LPSTR_TEXTCALLBACK;
    lvI.mask = LVIF_TEXT;
    lvI.iSubItem = 0;
    for (int i = 0; i < history.size(); i++) {
        lvI.iItem = i;

        ListView_InsertItem(hWndListView, &lvI);

        ListView_SetItemText(hWndListView, i, 0, ConvertToLPWSTR(history[i]->date));
        ListView_SetItemText(hWndListView, i, 1, ConvertToLPWSTR(history[i]->expression));
        ListView_SetItemText(hWndListView, i, 2, ConvertToLPWSTR(history[i]->operation));
        ListView_SetItemText(hWndListView, i, 3, ConvertToLPWSTR(history[i]->notation));
        ListView_SetItemText(hWndListView, i, 4, ConvertToLPWSTR(history[i]->result));

        delete history[i];
    }
}

HWND createListView(HWND hdlg)
{
    INITCOMMONCONTROLSEX icex;
    RECT rcClient;
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);
    GetClientRect(hdlg, &rcClient);
    hWndListView = CreateWindowEx(NULL, WC_LISTVIEW, L"", WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 0, 40, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdlg, (HMENU)7777, GetModuleHandle(NULL), NULL);
    RECT windowRect;
    LVCOLUMN lvc;
    GetClientRect(hdlg, &windowRect);
    lvc.mask = LVCF_WIDTH | LVCF_TEXT;
    lvc.cx = (windowRect.right - windowRect.left) / 5;
    for (int i = 0; i < columnsName.size(); i++) {
        lvc.pszText = const_cast<LPWSTR>(columnsName[i].c_str());
        ListView_InsertColumn(hWndListView, i, &lvc);
    }
    ShowWindow(hWndListView, SW_SHOWDEFAULT);
    return hWndListView;
}

BOOL CALLBACK RegisterProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam)
{
    switch (messg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDC_BUTTON_REGISTER:
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_LOGIN)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter login", L"No login entered", MB_OK | MB_ICONWARNING);
                break;
            }
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_PASSWORD)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter password", L"No password entered", MB_OK | MB_ICONWARNING);
                break;
            }
            if (!invokeRegistration(hdlg))
            {
                MessageBox(hdlg, L"This login is already taken", L"Duplicated login", MB_OK | MB_ICONWARNING);
                break;
            }
            else
            {
                MessageBox(hdlg, L"You are registered", L"Successful", MB_OK);
                EndDialog(hdlg, LOWORD(wParam));
                break;
            }
            break;
        case IDC_BUTTON_BACK:
            EndDialog(hdlg, LOWORD(wParam));
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hdlg, LOWORD(wParam));
        return TRUE;
        break;
    case WM_QUIT:
        DestroyWindow(hdlg);
        break;


    default: return false;
    }

}

BOOL CALLBACK LoginProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam)
{
    switch (messg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDC_BUTTON_LOGIN:
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_LOGIN_EDIT)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter login", L"No login entered", MB_OK | MB_ICONWARNING);
                break;
            }
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_PASSWORD)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter password", L"No password entered", MB_OK | MB_ICONWARNING);
                break;
            }
            if (!invokeAuthorization(hdlg))
                MessageBox(hdlg, L"Wrong login or password, please try again", L"Wrong login/password", MB_OK | MB_ICONWARNING);
            else
            {
                MessageBox(hdlg, L"You are logged in", L"Successful", MB_OK);
                showAuthorizedControls((GetParent(hdlg)));
                EndDialog(hdlg, LOWORD(wParam));
                break;
            }
            break;
        case IDC_BUTTON_BACK:
            EndDialog(hdlg, LOWORD(wParam));
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hdlg, LOWORD(wParam));
        return TRUE;
        break;
    case WM_QUIT:
        DestroyWindow(hdlg);
        break;


    default: return false;
    }

}

BOOL CALLBACK ResolverProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam)
{
    int to = 0;
    switch (messg)
    {
    case WM_INITDIALOG:
        InitCommonControls();
        if (!authorized)
            ShowWindow(GetDlgItem(hdlg, IDC_BUTTON_VIEW_HISTORY), SW_HIDE);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDC_BUTTON_TRANSLATE:
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_EXPRESSION)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
                break;
            }
            to = getChecked(hdlg);
            if (to == -1)
            {
                MessageBox(hdlg, L"Please, first select the notation ", L"No notation selected", MB_OK | MB_ICONWARNING);
                break;
            }
            fillTranslation(hdlg, to);
            break;
        case IDC_BUTTON_CALCULATE:
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_EXPRESSION)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
                break;
            }
            fillCalculation(hdlg);
            break;
        case IDC_BUTTON_VIEW_TREE:
            if (GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT_EXPRESSION)) < 1)
            {
                MessageBox(hdlg, L"Please, first enter expression", L"No expression entered", MB_OK | MB_ICONWARNING);
                break;
            }
            fillTree(hdlg);
            break;
        case IDC_BUTTON_VIEW_HISTORY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_HISTORY), hdlg, HistoryProc);
            break;
        case IDC_BUTTON_BACK:
            EndDialog(hdlg, LOWORD(wParam));
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hdlg, LOWORD(wParam));
        return TRUE;
        break;
    case WM_QUIT:
        DestroyWindow(hdlg);
        break;


    default: return false;
    }

}

BOOL CALLBACK HistoryProc(HWND hdlg, UINT messg, WPARAM wParam, LPARAM lParam)
{
    switch (messg)
    {
    case WM_INITDIALOG:
        hWndListView = createListView(hdlg);
        fillListView(currentpage);
        if(lastPage==currentpage)
            EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_NEXT_PAGE), false);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON_NEXT_PAGE:
            currentpage++;
            if (currentpage <= lastPage)
            {
                fillListView(currentpage);
                EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_PREV_PAGE), true);
            }
            else if(currentpage>lastPage && currentpage!=0)
            {
                currentpage--;
                EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_NEXT_PAGE), false);
            }
            break;
        case IDC_BUTTON_PREV_PAGE:
            currentpage--;
            if(currentpage==0)
                EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_PREV_PAGE), false);
            EnableWindow(GetDlgItem(hdlg, IDC_BUTTON_NEXT_PAGE), true);
            fillListView(currentpage);
            break;
        case IDC_BUTTON_BACK:
            EndDialog(hdlg, LOWORD(wParam));
            currentpage = 0;
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hdlg, LOWORD(wParam));
        currentpage = 0;
        return TRUE;
        break;
    case WM_QUIT:
        DestroyWindow(hdlg);
        break;


    default: return false;
    }

}
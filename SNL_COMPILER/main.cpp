#include "header.h"    //ȫ��ͷ�ļ�
#include "Grammar.h"    //�﷨����ͷ�ļ�
#include "Word.h"       //�ʷ�����ͷ�ļ�
#include "resource.h"   //��Դ
using namespace std;

#define ID_EDIT     1
#define BLOCK_HIGH 20
#define BLOCK_WIDTH 100
#define LINE 30
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);  //������ص�����
wordScanner* wordProject;                               //�ʷ�������Ŀ
GrammarAnalyzer* grammarProject;                        //�﷨������Ŀ
static TCHAR szAppName[] = TEXT ("snl") ;               //������
int maxWidth=0;
HINSTANCE hInst;                                        //������
OPENFILENAME ofn ;                                      //�ļ���Ϣ�ṹ
HWND hwndEdit ;                                         //�Ӵ��ھ��
HWND hDlgChild = NULL ;                                  //�Ӵ��ھ��
BOOL CALLBACK WordDlg (HWND, UINT, WPARAM, LPARAM);     //�ʷ������Ӵ��ڻص�����
BOOL CALLBACK GrammarDlg(HWND, UINT, WPARAM, LPARAM);   //�﷨�����Ӵ��ڻص�����
BOOL CALLBACK TreeDlg(HWND, UINT, WPARAM, LPARAM);   //�﷨���Ӵ��ڻص�����
int vScroll=0;
int constMaxWidth;
INT hScroll=0;
void dfs(HWND hwnd,Node* node,int depth,int width);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HACCEL   hAccel ;      //���ټ���Դ���
     HWND     hwnd ;        //�����ھ��
     MSG      msg ;         //��Ϣ
     WNDCLASS wndclass ;    //������
	 HMENU    hMenu;        //�˵����
	 hInst=hInstance;       //��������ֵ
     wndclass.style         = CS_HREDRAW | CS_VREDRAW ; //�趨���ڷ��
     wndclass.lpfnWndProc   = WndProc ;  //�趨�ص�����
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;  //������
     wndclass.hIcon         = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)) ; //����ͼ��
     wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW) ; //���ع��
     wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH) ; //��ȡˢ��
     wndclass.lpszMenuName  = szAppName ;  //�˵���
     wndclass.lpszClassName = szAppName ;  //��������

     if (!RegisterClass (&wndclass))  //�������Ƿ�ע��ɹ�
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     hMenu = LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU1)) ; //���ز˵�
     hwnd = CreateWindow (szAppName, szAppName,
                          WS_OVERLAPPEDWINDOW,
                          GetSystemMetrics (SM_CXSCREEN) / 4,
                          GetSystemMetrics (SM_CYSCREEN) / 4,
                          GetSystemMetrics (SM_CXSCREEN) / 2,
                          GetSystemMetrics (SM_CYSCREEN) / 2,
                          NULL, hMenu, hInstance, NULL) ;  //��������

     ShowWindow (hwnd, iCmdShow) ; //������ʾ״̬
     UpdateWindow (hwnd) ;         //���ƴ���

     hAccel = LoadAccelerators (hInstance, szAppName) ;  //���ټ���Դ����

     while (GetMessage (&msg, NULL, 0, 0))  //��ȡ��Ϣ
     {
         /*��һ���ж�,����ʷ������Ĵ���δ�򿪣��﷨�������﷨���Ĵ���ҲΪ��
           �ڶ����ж�,�����Ƿ���һ���Ӵ��ڴ򿪣�������������Ϣ*/

          if (hDlgChild == 0||!IsDialogMessage(hDlgChild, &msg))
		 {
             if (!TranslateAccelerator (hwnd, hAccel, &msg)) //������ټ�λΪ��ʶ�����Ϣ
			 {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
			 }
		 }
     }
     return msg.wParam ;
}

int toNum(string str)  //string����תint��
{
    int s=str.size();  //��ȡstring�ĳ���
    int i,num=0;
    for(i=0;i<s;i++)
    {
        num=num*10+(str[s-i-1]-'0'); //�ȵõ��͵�һλ�����10,�γ��µĵ�λ
    }
    return num;
}

bool isNum(string str)  //�ж�һ��string�Ƿ����תΪ����
{
    int i,s;
    s=str.size();
    for(i=0;i<s;i++)
    {
        if(i>'9'|i<'0')
            return false;
    }
    return true;
}
//����ѯ�ʴ���
AskConfirmation (HWND hwnd)
{
     return MessageBox (hwnd, TEXT ("�Ƿ���Ҫ�Ƴ�������?"),
                        szAppName, MB_YESNO | MB_ICONQUESTION) ;
}

//���ĳ���ı���
void DoCaption (HWND hwnd, TCHAR * szTitleName)
{
     TCHAR szCaption[64 + MAX_PATH] ;

     wsprintf (szCaption, TEXT ("%s - %s"), szAppName,
               szTitleName[0] ? szTitleName : UNTITLED) ;

     SetWindowText (hwnd, szCaption) ;
}

//�ļ���Ϣ�ṹ�ĳ�ʼ��
void PopFileInitialize (HWND hwnd)
{
     static TCHAR szFilter[] = TEXT ("Text Files (*.TXT)\0*.txt\0")  \
                               TEXT ("ASCII Files (*.ASC)\0*.asc\0") \
                               TEXT ("All Files (*.*)\0*.*\0\0") ;

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = TEXT ("txt") ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
}

//��һ���ļ�
BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

     return GetOpenFileName(&ofn) ;
}

//ok���ڵĺ���
void OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName)
{
     TCHAR szBuffer[64 + MAX_PATH] ;

     wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;

     MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
}

//��ȡһ���ļ�����ʾ
BOOL PopFileRead (HWND hwndEdit, PTSTR pstrFileName)
{
     BYTE   bySwap ;
     DWORD  dwBytesRead ;
     HANDLE hFile ;

     int    i, iFileLength, iUniTest ;
     PBYTE  pBuffer, pText, pConv ;


     if (INVALID_HANDLE_VALUE ==
               (hFile = CreateFile (pstrFileName, GENERIC_READ, FILE_SHARE_READ,
                                    NULL, OPEN_EXISTING, 0, NULL)))
          return FALSE ;

     iFileLength = GetFileSize (hFile, NULL) ;
     pBuffer =(unsigned char*) malloc (iFileLength + 2) ;

     ReadFile (hFile, pBuffer, iFileLength, &dwBytesRead, NULL) ;
     CloseHandle (hFile) ;
     pBuffer[iFileLength] = '\0' ;
     pBuffer[iFileLength + 1] = '\0' ;


     iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE ;

     if (IsTextUnicode (pBuffer, iFileLength, &iUniTest))
     {
          pText = pBuffer + 2 ;
          iFileLength -= 2 ;

          if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
          {
               for (i = 0 ; i < iFileLength / 2 ; i++)
               {
                    bySwap = ((BYTE *) pText) [2 * i] ;
                    ((BYTE *) pText) [2 * i] = ((BYTE *) pText) [2 * i + 1] ;
                    ((BYTE *) pText) [2 * i + 1] = bySwap ;
               }
          }

          pConv =(unsigned char*) malloc (iFileLength + 2) ;

#ifndef UNICODE
          WideCharToMultiByte (CP_ACP, 0, (PWSTR) pText, -1,(char*) pConv,
                               iFileLength + 2, NULL, NULL) ;

#else
          lstrcpy ((PTSTR) pConv, (PTSTR) pText) ;
#endif

     }
     else
     {
          pText = pBuffer ;

          pConv =(unsigned char*) malloc (2 * iFileLength + 2) ;

#ifdef UNICODE
          MultiByteToWideChar (CP_ACP, 0, pText, -1, (PTSTR) pConv,
                               iFileLength + 1) ;

#else
          lstrcpy ((PTSTR) pConv, (PTSTR) pText) ;
#endif
     }

     SetWindowText (hwndEdit, (PTSTR) pConv) ;
     free (pBuffer) ;
     free (pConv) ;

     return TRUE ;
}

BOOL PopFileWrite (HWND hwndEdit, PTSTR pstrFileName)
{
     DWORD  dwBytesWritten ;
     HANDLE hFile ;
     int    iLength ;
     PTSTR  pstrBuffer ;
     WORD   wByteOrderMark = 0xFEFF ;

     if (INVALID_HANDLE_VALUE ==
               (hFile = CreateFile (pstrFileName, GENERIC_WRITE, 0,
                                    NULL, CREATE_ALWAYS, 0, NULL)))
          return FALSE ;

     iLength = GetWindowTextLength (hwndEdit) ;
     pstrBuffer = (PTSTR) malloc ((iLength + 1) * sizeof (TCHAR)) ;

     if (!pstrBuffer)
     {
          CloseHandle (hFile) ;
          return FALSE ;
     }


#ifdef UNICODE
     WriteFile (hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL) ;
#endif

     GetWindowText (hwndEdit, pstrBuffer, iLength + 1) ;
     WriteFile (hFile, pstrBuffer, iLength * sizeof (TCHAR),
                &dwBytesWritten, NULL) ;

     if ((iLength * sizeof (TCHAR)) != (int) dwBytesWritten)
     {
          CloseHandle (hFile) ;
          free (pstrBuffer) ;
          return FALSE ;
     }

     CloseHandle (hFile) ;
     free (pstrBuffer) ;

     return TRUE ;
}

void DrawBorder(HDC hdc, int x, int y, int Width, int Height, int BorderWidth, int WTop)
{
	//WTopΪTRUEʱTopѡ�ױʣ�bottomΪ�ڱ�

	int i;
	HPEN  hpen;
	hpen=CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	if (WTop)
		SelectObject(hdc,GetStockObject(WHITE_PEN));
	else
		SelectObject(hdc,hpen);
	//��top,left��
	for (i=0; i<BorderWidth; i++)
	{
		MoveToEx(hdc,x+i, y+i,NULL);
		LineTo(hdc,x+Width-i, y+i);
		MoveToEx(hdc,x+i, y+i,NULL);
		LineTo(hdc,x+i, y+Height-i);
	}

	if (WTop)
		SelectObject(hdc,hpen);
	else
		SelectObject(hdc,GetStockObject(WHITE_PEN));

	//��bottom,right��
	for (i=0; i<BorderWidth; i++)
	{
		MoveToEx(hdc,x+Width-i, y+Height-i,NULL);
		LineTo(hdc,x+Width-i, y+i);
		MoveToEx(hdc,x+Width-i, y+Height-i,NULL);
		LineTo(hdc,x+i, y+Height-i);
	}

	DeleteObject(hpen);
}
//�����ڻص�����
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

     int         iSelect, iEnable ;
     static TCHAR     szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
     static string fileName=""; //�ļ�����¼
     switch (message)
     {
     case WM_CREATE:
          hwndEdit = CreateWindow (TEXT ("edit"), NULL,
                              WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                              0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                              ((LPCREATESTRUCT) lParam)->hInstance, NULL) ; //���������õ��Ӵ���
		  PopFileInitialize (hwnd) ;  //�ļ���Ϣ�ṹ�ĳ�ʼ��
          return 0 ;

     case WM_SETFOCUS:   //������ע�������
          SetFocus (hwndEdit) ;
          return 0 ;

     case WM_SIZE:  //���ı�������ڵĴ�С
          MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE) ;
          return 0 ;

     case WM_INITMENUPOPUP:  //�˵�����״̬ת��
          if (lParam == 1)
          {
			   if (!hDlgChild)
               {
				   EnableMenuItem ((HMENU) wParam,IDC_WORD,   MF_ENABLED) ;
				   EnableMenuItem ((HMENU) wParam,IDC_GRAMMAR,   MF_ENABLED) ;
				   EnableMenuItem ((HMENU) wParam,IDC_TREE,   MF_ENABLED) ;
               }
               else
               {
                    EnableMenuItem ((HMENU) wParam,IDC_WORD,  MF_GRAYED) ;
                    EnableMenuItem ((HMENU) wParam,IDC_GRAMMAR,  MF_GRAYED) ;
                    EnableMenuItem ((HMENU) wParam,IDC_TREE,  MF_GRAYED) ;
               }
               return 0 ;
          }
          break ;

	 case WM_COMMAND: //�յ��Ĳ˵�����Ϣ
		 switch(LOWORD(wParam))
          {
			 case IDC_OPEN:

                //���Դ��ļ�
                if (PopFileOpenDlg (hwnd, szFileName, szTitleName))
                {
                    if (!PopFileRead (hwndEdit, szFileName))
                    {
                         OkMessage (hwnd, TEXT ("�޷����ļ�%s!"),
                                    szTitleName) ;
                         szFileName[0]  = '\0' ;
                         szTitleName[0] = '\0' ;
                    }
                }
                //���ñ���
                DoCaption (hwnd, szTitleName) ;
                return 0 ;

             case IDC_WORD:
                //ѡ��ʷ�����
                if(szFileName[0]=='\0')
                {
                    MessageBox(hwnd,"����ļ�","����",MB_OK);
                    return 0;
                }
                fileName=szFileName;
                //�ʷ�������ʼi
                wordProject = new wordScanner(szFileName,hwnd);
                //wordProject = new wordScanner(szFileName,hwnd);
                wordProject->start();
                //���ɴʷ���������
                hDlgChild = CreateDialog(hInst,MAKEINTRESOURCE(IDC_WORD_DIALOG),hwnd, WordDlg);
                ShowWindow(hDlgChild,SW_SHOW);

                return 0;

             case IDC_GRAMMAR:
                 //�ʷ������Ƿ�ʵ�ִ���
                if(wordErrorState==true)
                {
                    MessageBox(hwnd,"�ʷ�����","����",MB_OK);
                    return 0;
                }
                //�Ƿ��Ѿ����дʷ�����
                if(fileName=="")
                {
                    MessageBox(hwnd,"�Ƚ��дʷ�����","����",MB_OK);
                    return 0;
                }
                //��ʼ�﷨����
                grammarProject = new GrammarAnalyzer(fileName.c_str(),hwnd);
                //grammarProject = new GrammarAnalyzer(fileName.c_str(),hwnd);
                grammarProject->start();

                //�����﷨��������


                hDlgChild = CreateDialog(hInst,MAKEINTRESOURCE(IDC_GRAMMAR_DIALOG),hwnd,GrammarDlg);
                ShowWindow(hDlgChild,SW_SHOW);
                return 0;
             case IDC_AUTHOR:
                 //��ʾ������Ա��Ϣ
                MessageBox(hwnd," 21172602 ¬��\n 21172603 ��謻�\n 21172617 ���ǳ�","������Ա",MB_OK);
                    return 0;
             case IDC_SAVE:
               if (szFileName[0])
               {
                    if (PopFileWrite (hwndEdit, szFileName)&&MessageBox(hwnd,"ȷ��Ҫ������",szFileName,MB_YESNO)==IDYES)
                         return 1 ;
                    else
                    {
                         OkMessage (hwnd, TEXT ("����д�ļ� %s"),
                                    szTitleName) ;
                         return 0 ;
                    }
               }
               else
               {
                   MessageBox(hwnd,"��δ���ļ�",NULL,MB_OK);
               }
               return 0;

             case IDC_TREE:
                 if(wordErrorState==true)
                {
                    MessageBox(hwnd,"�ʷ�����","����",MB_OK);
                    return 0;
                }
                //�Ƿ��Ѿ����дʷ�����
                if(fileName=="")
                {
                    MessageBox(hwnd,"�Ƚ��дʷ�����","����",MB_OK);
                    return 0;
                }
                //��ʼ�﷨����
                grammarProject = new GrammarAnalyzer(fileName.c_str(),hwnd);
                //grammarProject = new GrammarAnalyzer(fileName.c_str(),hwnd);
                grammarProject->start();
                if(grammarErrorState==true)
                {
                    MessageBox(hwnd,"�﷨�����д�","����",MB_OK);
                    return 0;
                }
                hDlgChild = CreateDialog(hInst,MAKEINTRESOURCE(IDC_TREE_DIALOG),hwnd,TreeDlg);
                ShowWindow(hDlgChild,SW_SHOW);
                return 0;
		 }
		 break;
     case WM_CLOSE:
         //�رմ���
          if (IDYES == AskConfirmation (hwnd))
               DestroyWindow (hwnd) ;
          return 0 ;

     case WM_QUERYENDSESSION:
         //ϵͳ�ر�
          if (IDYES == AskConfirmation (hwnd))
               return 1 ;
          else
               return 0 ;

     case WM_DESTROY:
         //ע������
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

BOOL CALLBACK WordDlg (HWND hwnd, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

	 RECT      rect;
   	 PAINTSTRUCT ps; //��ͼ��Ϣ�ṹ
	 HDC hdc;       //Ӳ����ʾ���
	 string str;
	 TEXTMETRIC  tm ;
	 TCHAR       szBuffer[10] ;
	 int i;
     static int  cxChar, cxCaps, cyChar, cyClient, pos=0 ,iVscrollPos=0;
     switch (message)
     {
     case WM_PAINT:

          hdc = GetDC (hwnd) ;
          //�����ַ���ʽ
          GetTextMetrics(hdc,&tm) ;
          cxChar = tm.tmAveCharWidth ;
          cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
          cyChar = tm.tmHeight + tm.tmExternalLeading ;

          ReleaseDC (hwnd, hdc) ;
          //���ù�����
          SetScrollRange (hwnd, SB_VERT, 0, wordProject->tokenList.num - 1, FALSE) ;
          SetScrollPos   (hwnd, SB_VERT, iVscrollPos, TRUE) ;

          //�Ƿ��﷨��������
          if(!wordErrorState)
          {
              hdc = BeginPaint (hwnd, &ps) ;
              int i,y;
              for (i = 0 ; i < wordProject->tokenList.List.size() ; i++)
              {
                   y = cyChar * (i- iVscrollPos) ;
                   SetBkMode(hdc, TRANSPARENT);
                   TextOut (hdc, 0, y,
                            wordProject->tokenList.List[i].name.c_str(),
                            lstrlen (wordProject->tokenList.List[i].name.c_str())) ;

                   TextOut (hdc, 15 * cxCaps, y,
                            wordProject->tokenList.List[i].type.c_str(),
                            lstrlen (wordProject->tokenList.List[i].type.c_str())) ;

                   SetTextAlign (hdc, TA_RIGHT | TA_TOP) ;

                   TextOut (hdc, 15 * cxCaps + 25 * cxChar, y, szBuffer,
                            wsprintf (szBuffer, TEXT ("%5d"),
                                 wordProject->tokenList.List[i].line)) ;

                   SetTextAlign (hdc, TA_LEFT | TA_TOP) ;
              }
              EndPaint (hwnd, &ps) ;
          }
          else
          {
              //������Ӵ����������ӡ������Ϣ
              hdc = BeginPaint (hwnd, &ps) ;
              int i,y;
              for (i = 0 ; i < wordProject->error.size() ; i++)
              {
                   y = cyChar * (i- iVscrollPos) ;
                   SetBkMode(hdc, TRANSPARENT);
                   TextOut (hdc, 0, y,
                            wordProject->error[i].c_str(),
                            lstrlen (wordProject->error[i].c_str())) ;
              }
              EndPaint (hwnd, &ps) ;
          }
          return TRUE ;

     case WM_VSCROLL:
        //�������Ĳ���
          switch (LOWORD(wParam))
          {
          case SB_LINEUP:
               iVscrollPos -= 1 ;
               break ;

          case SB_LINEDOWN:
               iVscrollPos += 1 ;
               break ;

          case SB_PAGEUP:
               iVscrollPos -= cyClient / cyChar ;
               break ;

          case SB_PAGEDOWN:
               iVscrollPos += cyClient / cyChar ;
               break ;

          case SB_THUMBPOSITION:
               iVscrollPos = HIWORD(wParam) ;
               break ;

          default :
               break ;
          }

          iVscrollPos = max (0, min (iVscrollPos, wordProject->tokenList.num - 1)) ;

          if (iVscrollPos != GetScrollPos (hwnd, SB_VERT))
          {
               SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE) ;
               InvalidateRect (hwnd, NULL, TRUE) ;
          }
          return 0 ;

    case WM_SIZE:
          cyClient = HIWORD (lParam) ;
          return 0 ;


    case WM_CLOSE :

          DestroyWindow (hwnd) ;
          cout<<"word over1"<<endl;
          delete(wordProject);
          cout<<"word over2"<<endl;
          hDlgChild = NULL ;
          cout<<"word over3"<<endl;
          return TRUE ;
     }
     return FALSE ;
}

//�﷨�����Ӵ��ڻص�����
BOOL CALLBACK GrammarDlg (HWND hwnd, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

	 RECT      rect;
   	 PAINTSTRUCT ps;
	 HDC hdc;
	 string str;
	 TEXTMETRIC  tm ;
	 TCHAR       szBuffer[10] ;
	 int i;
     static int  cxChar, cxCaps, cyChar, cyClient, pos=0 ,iVscrollPos=0;
     switch (message)
     {
     case WM_PAINT:

          hdc = GetDC (hwnd) ;
          GetTextMetrics(hdc,&tm) ;
          cxChar = tm.tmAveCharWidth ;
          cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
          cyChar = tm.tmHeight + tm.tmExternalLeading ;

          ReleaseDC (hwnd, hdc) ;

          SetScrollRange (hwnd, SB_VERT, 0, grammarProject->itemList.size() - 1, FALSE) ;
          SetScrollPos   (hwnd, SB_VERT, iVscrollPos, TRUE) ;

          hdc = BeginPaint (hwnd, &ps) ;
          int i,y;

          for (i = 0 ; i < grammarProject->itemList.size() ; i++)
          {
               y = cyChar * (i- iVscrollPos) ;
               SetBkMode(hdc, TRANSPARENT);
               TextOut (hdc, 0, y,
                        grammarProject->itemList[i].left.c_str(),
                        lstrlen (grammarProject->itemList[i].left.c_str())) ;

               TextOut (hdc, 15 * cxCaps, y,
                        grammarProject->itemList[i].oper.c_str(),
                        lstrlen (grammarProject->itemList[i].oper.c_str())) ;

               if(!isNum(grammarProject->itemList[i].right))
               {
                   TextOut (hdc, 15 * cxCaps + 25 * cxChar, y, grammarProject->itemList[i].right.c_str(),
                                 lstrlen(grammarProject->itemList[i].right.c_str())) ;
               }
               else if(grammarProject->itemList[i].right!="")
               {
                   TextOut (hdc, 15 * cxCaps + 55 * cxChar, y, szBuffer,
                            wsprintf (szBuffer, TEXT ("%5d"),
                                 toNum(grammarProject->itemList[i].right))) ;
               }

               SetTextAlign (hdc, TA_LEFT | TA_TOP) ;
          }
          EndPaint (hwnd, &ps) ;
          return TRUE ;

     case WM_VSCROLL:
          switch (LOWORD(wParam))
          {
          case SB_LINEUP:
               iVscrollPos -= 1 ;
               break ;

          case SB_LINEDOWN:
               iVscrollPos += 1 ;
               break ;

          case SB_PAGEUP:
               iVscrollPos -= cyClient / cyChar ;
               break ;

          case SB_PAGEDOWN:
               iVscrollPos += cyClient / cyChar ;
               break ;

          case SB_THUMBPOSITION:
               iVscrollPos = HIWORD(wParam) ;
               break ;

          default :
               break ;
          }

          iVscrollPos = max (0, min (iVscrollPos, (int)grammarProject->itemList.size() - 1)) ;

          if (iVscrollPos != GetScrollPos (hwnd, SB_VERT))
          {
               SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE) ;
               InvalidateRect (hwnd, NULL, TRUE) ;
          }
          return 0 ;

    case WM_SIZE:
          cyClient = HIWORD (lParam) ;
          return 0 ;


    case WM_CLOSE :
          DestroyWindow (hwnd) ;
          cout<<"grammar over"<<endl;
          delete(grammarProject);
          cout<<"over2"<<endl;
          hDlgChild = NULL ;
          cout<<"over3"<<endl;
          return TRUE ;

     }
     return FALSE ;
}
void dfs(HWND hwnd,Node* node,int depth,int width)
{
    if(node==NULL)
        return ;
    Node* tempNode = node;
    RECT Grid;
    Node* fatherNode = node->father;
    Node* tempSonNode;
    HPEN  hpen;
    HDC hdc;
    int high=fatherNode->son.size()-1;
	hpen=CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hdc=GetDC(hwnd);
	//cout<<node->name<<endl;
    if(fatherNode->son[0]==node)    //������
    {
        SetWindowOrgEx(hdc,(BLOCK_WIDTH+LINE)*hScroll,(BLOCK_HIGH+LINE)*(vScroll),NULL);
        DrawBorder(hdc,(BLOCK_WIDTH+LINE)*depth,(BLOCK_HIGH+LINE)*width,BLOCK_WIDTH,BLOCK_HIGH,1,1);
        Grid.left=(BLOCK_WIDTH+LINE)*depth;
        Grid.right=(BLOCK_WIDTH+LINE)*depth+BLOCK_WIDTH;
        Grid.top=(BLOCK_HIGH+LINE)*width;
        Grid.bottom=(BLOCK_HIGH+LINE)*width+BLOCK_HIGH;
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc,(node->name).c_str(),-1,&Grid,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        SelectObject(hdc,hpen);
        MoveToEx(hdc,(BLOCK_WIDTH+LINE)*(depth-1)+BLOCK_WIDTH,(BLOCK_HIGH+LINE)*width+LINE/2,NULL);
        LineTo(hdc,(BLOCK_WIDTH+LINE)*depth,(BLOCK_HIGH+LINE)*width+LINE/2);
        ReleaseDC (hwnd, hdc);
        DeleteObject(hpen);
        tempSonNode=node->getSon();
        dfs(hwnd,tempSonNode,depth+1,width);
    }

    while(fatherNode->curSon <= high)  //ȡ��һ���ֵܽڵ�
    {
        //cout<<"error"<<endl;
        hdc=GetDC(hwnd);
        maxWidth++;
        tempSonNode=fatherNode->getSon();
        SetWindowOrgEx(hdc,(BLOCK_WIDTH+LINE)*hScroll,(BLOCK_HIGH+LINE)*(vScroll),NULL);
        DrawBorder(hdc,(BLOCK_WIDTH+LINE)*depth,(BLOCK_HIGH+LINE)*maxWidth,BLOCK_WIDTH,BLOCK_HIGH,1,1);
        Grid.left=(BLOCK_WIDTH+LINE)*depth;
        Grid.right=(BLOCK_WIDTH+LINE)*depth+BLOCK_WIDTH;
        Grid.top=(BLOCK_HIGH+LINE)*maxWidth;
        Grid.bottom=(BLOCK_HIGH+LINE)*maxWidth+BLOCK_HIGH;
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc,(tempSonNode->name).c_str(),-1,&Grid,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        SelectObject(hdc,hpen);
        MoveToEx(hdc,(BLOCK_WIDTH+LINE)*depth+BLOCK_WIDTH/2,(BLOCK_HIGH+LINE)*width+BLOCK_HIGH,NULL);
        LineTo(hdc,(BLOCK_WIDTH+LINE)*depth+BLOCK_WIDTH/2,(BLOCK_HIGH+LINE)*maxWidth);
        ReleaseDC (hwnd, hdc);
        DeleteObject(hpen);

        width=maxWidth;
        dfs(hwnd,tempSonNode->getSon(),depth+1,maxWidth);
    }
}
BOOL CALLBACK TreeDlg (HWND hwnd, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

	 RECT      rect;
   	 PAINTSTRUCT ps;
   	 Node* a ;
	 HDC hdc;
	 string str;
	 TEXTMETRIC  tm ;
	 TCHAR       szBuffer[10] ;
	 Node* root = grammarProject->root;
	 RECT Grid;
	 int i,depth=grammarProject->maxDepth;
     static int  cxChar, cxCaps, cyChar, cyClient, pos=0 ,iVscrollPos=0,hVscrollPos=0;
     switch (message)
     {
     case WM_PAINT:
          hdc=GetDC(hwnd);
          SetWindowOrgEx(hdc,(BLOCK_WIDTH+LINE)*hVscrollPos,(BLOCK_HIGH+LINE)*(iVscrollPos),NULL);   //��������
          vScroll=iVscrollPos;
          hScroll=hVscrollPos;
          a=root->getSon();
          //cout<<depth<<endl;
          //cout<<"draw tree start"<<endl;
          DrawBorder(hdc,0,0,BLOCK_WIDTH,BLOCK_HIGH,1,1);  //���Ƴ�ʼ�ڵ�
          Grid.left=0;
          Grid.right=BLOCK_WIDTH;
          Grid.top=0;
          Grid.bottom=BLOCK_HIGH;
          SetBkMode(hdc, TRANSPARENT);
          DrawText(hdc,(a->name).c_str(),-1,&Grid,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

          dfs(hwnd,a,1,0);  //�ݹ鿪ʼ
          for(i=0;i<grammarProject->nodeList.size();i++)
              grammarProject->nodeList[i]->curSon=0;
          root=grammarProject->root;   //���³�ʼ��
          constMaxWidth=maxWidth;
          SetScrollRange (hwnd, SB_VERT, 0, maxWidth - 1, FALSE) ;
          SetScrollPos   (hwnd, SB_VERT, iVscrollPos, TRUE) ;
          SetScrollRange (hwnd, SB_HORZ, 0, grammarProject->maxDepth - 1, FALSE) ;
          SetScrollPos   (hwnd, SB_HORZ, hVscrollPos, TRUE) ;
          maxWidth=0;


          return TRUE ;


    case WM_SIZE:
          cyClient = HIWORD (lParam) ;
          return 0 ;

    case WM_VSCROLL:
      switch (LOWORD(wParam))
      {
          case SB_LINEUP:
               iVscrollPos -= 1 ;
               break ;

          case SB_LINEDOWN:
               iVscrollPos += 1 ;
               break ;

          case SB_THUMBPOSITION:
               iVscrollPos = HIWORD(wParam) ;
               break ;

          default :
               break ;
      }

      iVscrollPos = max (0, min (iVscrollPos, constMaxWidth - 1)) ;


      if (iVscrollPos != GetScrollPos (hwnd, SB_VERT))
      {
           SetScrollPos (hwnd, SB_VERT, iVscrollPos, TRUE) ;
           InvalidateRect (hwnd, NULL, TRUE) ;
      }
      return 0 ;
    case WM_HSCROLL:
      switch (LOWORD(wParam))
      {
          case SB_LINELEFT:
               hVscrollPos -= 1 ;
               break ;

          case SB_LINERIGHT:
               hVscrollPos += 1 ;
               break ;

          case SB_THUMBPOSITION:
               hVscrollPos = HIWORD(wParam) ;
               break ;

          default :
               break ;
      }

      hVscrollPos = max (0, min (hVscrollPos, grammarProject->maxDepth - 1)) ;


      if (hVscrollPos != GetScrollPos (hwnd, SB_HORZ))
      {
           SetScrollPos (hwnd, SB_HORZ, hVscrollPos, TRUE) ;
           InvalidateRect (hwnd, NULL, TRUE) ;
      }
      return 0 ;
    case WM_CLOSE :
          DestroyWindow (hwnd) ;
          cout<<"over"<<endl;
          delete(grammarProject);
          cout<<"over2"<<endl;
          hDlgChild = NULL ;
          cout<<"over3"<<endl;
          return TRUE ;

     }
     return FALSE ;
}

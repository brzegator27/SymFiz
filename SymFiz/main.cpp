#include <windows.h>
#include <winnt.h>
#include <commctrl.h>
//#include <iostream>
//#include <string.h>
#include <sstream>
#include <process.h>
#include <Windowsx.h>
#include <math.h>
#include "headerFileSymFiz.h"
//#include "Cialo.h"

using namespace std;


//UWAGA///////////////////////////////////////////////////////////////////////////////////////////////
//W przyszłości przekazywanie danych do odzielnych wątków będzi eodbywało się za pomocą struktóry ;)//
//////////////////////////////////////////////////////////////////////////////////////////////////////

struct DaneDoSymulacji
{
	double wysokosc;
	double przyspieszenie;
	HWND hWnd;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//DEKLARACJE///////////////////
LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HMENU createSymFizMenu();
HWND buttonsSymFiz(HWND hwndWindow, HINSTANCE hInstance);
int dlgWinUstawienia(HWND hWnd, HINSTANCE hInstance);
int dlgWinSFInformacje(HWND hWnd, HINSTANCE hInstance);
HWND buttonsUstawienia(HWND hwndWindow, HINSTANCE hInstance);
HWND buttonsSFInformacje(HWND hwndWindow, HINSTANCE hInstance);
LRESULT CALLBACK dlgWindowProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK dlgSFIWindowProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int spadekSwobodny(double wysokosc, double przyspieszenie, HWND hWnd);
int spadekSwobodny2(double wysokosc, double przyspieszenie, HWND hWnd);
int spadekSwobodny3(double wysokosc, double przyspieszenie, HWND hWnd);

//Próbna funkcja rzutu ukośnego wersja pierwsza. Dopracować parametry funkcji. ;)
int rzutUkosny1(double przyspieszenie, double vPoczatkowe, double katAlfa, int x, int y);

//int spadekSwobodny(daneDoSymulacji* dane);
int wypiszDaneCiala(double szybkosc, double wysokosc, double czas, int wsp1x, int wsp2y, HDC hdc);
int rysCialo(int wsp1x, int wsp2y, int promienKola, HDC hdc);
int antyRysCialo(int wsp1x, int wsp2y, int promienKola, HDC hdc);
int rysPodzialki1(HDC hdc);
double charToDouble(char *charText);
char doubleToChar(double *doubleValue, const char *buffor);
char *append_char ( const char *s, const char c );
int rysowanieWszystkiegoSymFiz(HWND hWnd, HDC myHDC, RECT myRECT);
int rysowanieWszystkiegoUstawienia(HWND hWnd, HDC myHDC, RECT myRECT);
int rysowanieWszystkiegoInformacje(HWND hWnd, HDC myHDC, RECT myRECT);
HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText);
HWND tworzenieTrackBarCialo(HWND hDlg, int IDofTrackBar, UINT iMin, UINT iMax, int x, int y);

//Funkcje, które są nowym wątkiem:
void _cdecl spadekSwobodnyNowyWatek(void* daneNowyWatek);
void _cdecl rzutUkosnyNowyWatek(void* daneNowyWatek);

//DEFINICJE////////////////////
char* windowClassName = "SymFiz Application";
char* dlgWinCNUstawienia = "SymFiz Ustawienia";
char* dlgWinCNInformacje = "SymFiz Informacje";

//Zmienna, która przechowóje wartość mówiącą o promieniu rysowanego koła - ciala:
int promienKolaWSymulacji = 20;
//Kopia zmiennej promienKolaWSymulacji, która jest inicjalizowana podczas przetarzania wiadomośc WM_CREATE okna "Ustawienia"
//Jeśli wyłączenie nastąpiło przez naciśnięcie przycisku "Anuluj" jest ona przypisywana do zmiennej promienKolaWSymulacji
int promienKolaWSymulacjiKopia = promienKolaWSymulacji;

//Promień koła - wartość domyślna:
int WD_promienKolaWSymulacji = 20;

//Promien kola - po to, aby nie zmieniac zmiennej promienKolaWSymulacji podczas wykonywania symulacji, 
//ponieważ "brzydko to wygląda"
int promienKolaUstawienia = promienKolaWSymulacji;

//Dane do spadku swobodnego:
double wysokosc = 0;
double przyspieszenie = 0;

//Dane do rzutu ukosnego plus przyspieszenie z spadku swobodnego:
double szybkoscPoczatkowa = 0;
double katAlfa = 0;
double polozenieX = 0;
double polozenieY = 0;

//Dane po to,a by symulacja rzutu ukosnego zmiescila sie w "Polu", czyli, aby nie byla rysowana poza wyznaczonym obszarem:
int wysokoscPola = 380;
int szerokoscPola = 380;

//Dane mowiace o tym gdzie znajduje sie "pole" - wspolzedne lewego dolnego rogu
int polePolozenieX = 70;
int polePolozenieY = 440;

//W tych dwóch zmiennych przechowywujemy wspolżędne położenia ciała po animacji, tak, aby można je było odrysować
//jeśli fragment okna zostanie zasłonięty itp.
int wspCialaX = 250;
int wspCialaY = 60;

//Zmienne do funkcji wypiszDaneCiala();
double wysokoscCiala = 0;
double szybkoscCiala = 0;
double czasCiala = 0;
const int polozenieDanychX = 280;
const int polozenieDanychY = 110;
//Z funkcji spadekSwobodny2();
double wysokoscTerazniejsza1 = wysokosc;

char parametr1[300];
char parametr2[300];
char parametr3[300];
char parametr4[300];
char charTextPrzyspieszenie[300]; 
char charTextWysokosc[300];

//Czy okno: "Ustawienia jest"
bool isDlgWin = false;

//Mówi, czy okno "Ustawienia" było już otworzone chociaż raz - jeśli tak to nie wysyłamy poraz kolejny wiadomości BN_SETFOCUS 
//do przycisku "Rozpocznij sy..."
bool czyUstWindByloOtwarte = false;

//Mówi, czy nastąpiła jakaś zmiana w danych w Combobox w oknie "Ustawienia"
bool czyNastapilaZmiana = false;

//Mówi, czy ramka ma być czarna, czy szara. Jeśli szara to false, jeśli czarna to true
bool czyPierwszyRaz = true;

//Mówi, czy symulacja fizyczna jest właśnie w trakcie:
bool czyTrwaSymulacja = false;

//Zmienne, w których jest przechowywana informacja o tym, gdzie jest początkowe położenie ciala w symulacji:
int polozeniePoczatkoweX = 250, polozeniePoczatkoweY = 60;

//Zmienne, w których jest przechowywana obecna pozycja ciała:
int terazXOdswierz = polozeniePoczatkoweX, terazYOdswierz = polozeniePoczatkoweY;

//Zmienna, ktora mowi, jaki rodzaj symulacji ma zostac wykonany:
//0)spadek swobodny
//1)rzut ukosny
int jakaSymulacja = 0;

HWND glowneOkno = 0;
HWND dlgWindowHandle = 0;

HWND buttonRozpoczecieSymG = 0;
HWND buttonUstwieniaG = 0;

HWND systemLink1 = 0;

//double

//MENU ID
#define ID_ZAKONCZ 1
#define ID_SFINFORMACJE 2

//BUTTONS, COMBOBOX ID
#define ID_USTAWIENIA 10
#define ID_ZATWIERDZ 11
#define ID_INFORMACJEOK 12
#define ID_COMBOBOXUSTAWIENIA1 13
#define ID_COMBOBOXUSTAWIENIA2 14
#define ID_STATICCONTROL1 15
#define ID_STATICCONTROL2 16
#define ID_STATICCONTROL3 17
#define ID_SYSTEMLINK1 18
#define ID_ROZPOCZSYMULACJI 19
#define ID_ANULUJ 20
#define ID_DODAJ1 21
#define ID_DODAJ2 22
#define ID_GROUPBOX1 23
#define ID_TRACKBAR1 24
#define ID_GROUPBOX2 25
#define ID_USTAWIENIADOMYLNE1 26
#define ID_POMOCUSTAWIENIA1 27
#define ID_POMOCUSTAWIENIA2 28
#define ID_COMBOBOXSYMFIZ1 29
#define ID_EDITBOXUSTAWIENIA1 30
#define ID_EDITBOXUSTAWIENIA2 31
#define ID_EDITBOXUSTAWIENIA3 32
#define ID_EDITBOXUSTAWIENIA4 33
#define ID_BUTTONUSTAWIENIAPRZERWIJ 34
//#define ID_PROGRESBARUSTAWIENIA1 20
//#define ID_COMANDLINKINFORMACJE1 21

//DIALOG WINDOW HINSTANCE
#define HI_USTAWIENIA NULL
#define HI_SYMFIZINFORMACJE NULL
#define HI_SFINFORMACJE NULL

//CZCIONKI
//HFONT myFont1 = 0;

	HFONT myFont1 = CreateFont(
	17,
	0,
	0,
	0,
	FW_DONTCARE,
	FALSE,
	FALSE,
	FALSE,
	EASTEUROPE_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE,
	"Segoe UI");

	//HFONT myFont2 = 0;

	HFONT myFont2 = CreateFont(
	15,
	0,
	0,
	0,
	FW_DONTCARE,
	FALSE,
	FALSE,
	FALSE,
	EASTEUROPE_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE,
	"Segoe UI");

	HFONT myFont2Kursywa = CreateFont(
	15,
	0,
	0,
	0,
	FW_DONTCARE,
	TRUE,
	FALSE,
	FALSE,
	EASTEUROPE_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE,
	"Segoe UI");

	HFONT myFont3 = CreateFont(
	15,
	0,
	0,
	0,
	FW_DONTCARE,
	FALSE,
	FALSE,
	FALSE,
	EASTEUROPE_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE,
	"Segoe UI");



//WINMAIN/////////////////////
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//Cialo mojecialo();

	WNDCLASSEX wndclassex = {0};

	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;
	wndclassex.lpfnWndProc = windowProcedure;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = LoadIcon(0, (LPCTSTR)IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(0, (LPCTSTR)IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;
	wndclassex.lpszMenuName = 0;
	wndclassex.lpszClassName = windowClassName;
	wndclassex.hIcon = 0;

	RegisterClassEx(&wndclassex);

	RECT windowSize = {0, 0, 800, 550};
	
	AdjustWindowRectEx(&windowSize, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE, 0);

	HWND windowHandle = 0;

	windowHandle = CreateWindowEx(
		0,
		windowClassName,
		"SymFiz Application 2.0 beta",
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		100,
		60,
		windowSize.right - windowSize.left,
		windowSize.bottom - windowSize.top,
		0,
		0,
		hInstance,
		0);

	glowneOkno = windowHandle;

	//HMENU menuSymFiz = 0;
	//menuSymFiz = createSymFizMenu();
	const HMENU menuSymFiz = createSymFizMenu();

	SetMenu(windowHandle, menuSymFiz);

	int odpowiedzTakNie = MessageBox(NULL,"Korzystając z programu bierzesz całkowitą odpowiedzialność za błędne symulacje i mogące powodować to(tj. błędne symulacje) różne skutki. Jeśli naciśniesz przycisk: \"Tak\" zgadzasz się z powyższymi sformułowaniami. W przypadku naciśnięcia innego przycisku program zamknie się automatycznie.", "Informacja", MB_ICONINFORMATION | MB_YESNO);

	if(odpowiedzTakNie == IDNO)
	{
		DestroyWindow(windowHandle);
	}
	else
	{
		ShowWindow(windowHandle, SW_NORMAL);
	}

	UpdateWindow(windowHandle);

	//HDC myHDC = 0;
	//myHDC = GetDC(windowHandle);
	//
	//	SelectObject(myHDC, (HGDIOBJ)myFont2);
	//	SetBkColor(myHDC, RGB(0, 0, 0));
	//	
	//	RECT t1RECT = {10, 10, 510, 510};
	//	RECT t2RECT = {20, 20, 500, 500};

	//	FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(GRAY_BRUSH));
	//	FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

	//	//wypiszDaneCiala(2, 2, 2, 200, 200, myHDC);

	//	rysPodzialki1(myHDC);

	//ReleaseDC(windowHandle, myHDC);

	//////////////////BUTTONS
	HWND hwndButtonUstawienia = buttonsSymFiz(windowHandle, hInstance);
	SetFocus(hwndButtonUstawienia);

	/*LPARAM lParam = 0;
	WPARAM wParam = 0;

	lParam = MAKELPARAM(TRUE, GetDlgItem(windowHandle, ID_USTAWIENIA));
	wParam = NULL;
	
	SendDlgItemMessage(windowHandle, ID_USTAWIENIA, WM_SETFOCUS, wParam, lParam);*/

	/*int odpowiedzTakNie = MessageBox(NULL,"Korzystając z programu bierzesz całkowitą odpowiedzialnoś za błędne symulacje i mogące powodować to(tj. błędne symulacje) różne skutki. Jeśli naciśniesz przycisk: \"Tak\" zgadzasz się z powyższymi sformułowaniami. W przypadku naciśnięcia innego przycisku program zamknie się autoamtycznie.", "Informacja", MB_ICONINFORMATION | MB_YESNO);

	if(odpowiedzTakNie == IDNO)DestroyWindow(windowHandle);*/
  
	MSG msg;

	for(;;)
	{
		if(GetMessage(&msg, 0, 0, 0) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
		case WM_CREATE:
			{
				HDC myHDC = 0;
				myHDC = GetDC(hWnd);
				
					RECT myRECT = {0};
					GetWindowRect(hWnd,&myRECT);

					rysowanieWszystkiegoSymFiz(hWnd, myHDC, myRECT);

				DeleteDC(myHDC);
				return 0;
			}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_COMMAND:
			if(LOWORD(wParam) == ID_COMBOBOXSYMFIZ1 && HIWORD(wParam) == CBN_SELENDOK)
			{
				jakaSymulacja = ComboBox_GetCurSel(GetDlgItem(hWnd, ID_COMBOBOXSYMFIZ1));

				return 0;
			}

			if(LOWORD(wParam) == ID_USTAWIENIA && HIWORD(wParam) == BN_CLICKED)
			{
				if(!isDlgWin)
				{
					EnableWindow(GetDlgItem(glowneOkno, ID_USTAWIENIA), FALSE);
					dlgWinUstawienia(hWnd, (HINSTANCE)HI_USTAWIENIA);     //dlgWindowHandle = dlgWinUstawienia(glowneOkno, (HINSTANCE)HI_USTAWIENIA);
				}
				//To już nie jest potrzebne - najprawdopodobniej
				/*else
				{
					int messageID = MessageBox(NULL,"Okno jest już otworzone!", "Informacja", MB_ICONINFORMATION | MB_OK);
					SetFocus(dlgWindowHandle);
				}*/
				return TRUE;
			}
			
			if(LOWORD(wParam) == ID_ROZPOCZSYMULACJI && HIWORD(wParam) == BN_CLICKED)
			{
				//Ladowanie kursorow:
				HCURSOR waitCursor = LoadCursor(0, IDC_WAIT);
				HCURSOR arrowCursor = LoadCursor(0, IDC_ARROW);

				bool czyWszystkoDoSpadkuSwobodnego = false;
				bool czyWszystkoDoRzutuUkosnego = false;
				bool czyWszystkieDaneGotowe = false;

				czyWszystkoDoSpadkuSwobodnego = (wysokosc == 0 || przyspieszenie == 0);
				/*przyspieszenie = 10;
				szybkoscPoczatkowa = 120;
				katAlfa = 60;*/
				bool skaldnik1 = false;
				bool skaldnik2 = false;
				bool skaldnik3 = false;
				skaldnik1 = przyspieszenie == 0;
				skaldnik2 = szybkoscPoczatkowa == 0;
				skaldnik3 = katAlfa == 0;
				czyWszystkoDoRzutuUkosnego = (skaldnik1 || skaldnik2 || skaldnik3);

				if(jakaSymulacja == 0)
				{
					czyWszystkieDaneGotowe = czyWszystkoDoSpadkuSwobodnego;
				}
				else
				{
					czyWszystkieDaneGotowe = czyWszystkoDoRzutuUkosnego;
				}

				//Ustawienie kursoru:
				if(czyWszystkieDaneGotowe)
				{
					MessageBox(NULL,"Przed rozpoczęciem symulacji fizycznej \nnależy ustalić dane wtym celu skorzystaj \nz okna ustawień. Okno można otworzyć \nprzez naciśnięcie przycisku: \"Ustawienia\".", "Informacja", MB_ICONINFORMATION | MB_OK);
				}
				else
				{
					//Zmiana przycisku:
					EnableWindow(buttonRozpoczecieSymG, FALSE);

					MessageBox(NULL,"Rozpoczęta symulacja.", "Informacja", MB_ICONINFORMATION | MB_OK);// Można w następnej wersji zrobić tak, aby uzytkownik mógł ustalić, czy okno ma być wyświetlane następnym razem. ;)
					//UWAGA nie wiem, czy ma byc ujeta ponizsza linijka w komentarz ;)
					//czyPierwszyRaz = false;
					
					SetCursor(waitCursor);

					switch(jakaSymulacja)
					{
						case 0:
							{
								_beginthread(spadekSwobodnyNowyWatek, 0, (void*)"0");
							}
						break;

						case 1:
							{
								_beginthread(rzutUkosnyNowyWatek, 0, (void*)"0");
							}
						break;

						default:
							{
								return DefWindowProc(hWnd, message, wParam, lParam);
							}
						break;
					}

					SetCursor(arrowCursor);

					//To(kod poniżej) zmieniamy w funkcji nowego wątku ;)
					//EnableWindow(buttonRozpoczecieSymG, TRUE);
				}

				return TRUE;
			}
			
			if(LOWORD(wParam) == ID_ZAKONCZ && HIWORD(wParam) == BN_CLICKED)
			{
				int odpowiedz = MessageBox(NULL,"Czy napewno chcesz zamknąć program?", "Informacja", MB_ICONINFORMATION | MB_YESNO);
				
				if(odpowiedz == IDYES)
				{
					SendMessage(hWnd,WM_CLOSE,0,0);
				}

				return 0;
			}

			if(LOWORD(wParam) == ID_SFINFORMACJE && HIWORD(wParam) == BN_CLICKED)
			{
				dlgWinSFInformacje(hWnd, (HINSTANCE)HI_SFINFORMACJE);
				
				return 0;
			}

		case WM_PAINT:
			PAINTSTRUCT myStruct = {0};
			HDC myHDC = 0;

			myHDC = BeginPaint(hWnd, &myStruct); 

				RECT jakasRECT = {0};
				rysowanieWszystkiegoSymFiz(hWnd, myHDC, myStruct.rcPaint);
				
				////ShowWindow(buttonRozpoczecieSymG, SW_NORMAL);
				//UpdateWindow(buttonRozpoczecieSymG);

				////ShowWindow(buttonUstwieniaG, SW_NORMAL);
				//UpdateWindow(buttonUstwieniaG);

			EndPaint(hWnd, &myStruct);

		return 0;
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK dlgWindowProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{	
		case WM_CREATE:
			isDlgWin = true;
			promienKolaWSymulacjiKopia = promienKolaWSymulacji;
			return 0;

		case WM_CLOSE:
			isDlgWin = false;
			DestroyWindow(hDlg);
			return 0;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				EnableWindow(GetDlgItem(glowneOkno, ID_USTAWIENIA), TRUE);
				HDC hendleOfDCMainWindow = 0;
			
				hendleOfDCMainWindow = GetDC(glowneOkno);
				
					RECT mainWindowRECT = {0};
					GetWindowRect(glowneOkno, &mainWindowRECT);

					if(!czyTrwaSymulacja)rysowanieWszystkiegoSymFiz(glowneOkno, hendleOfDCMainWindow, mainWindowRECT);

				DeleteDC(hendleOfDCMainWindow);

				isDlgWin = false;
				return 0;
			}

		case WM_HSCROLL:
		{
			HWND hTrackBar1 = GetDlgItem(hDlg, ID_TRACKBAR1);
			if(hTrackBar1 == (HWND)lParam)
			{
				//W zmiennej tymczasowyPromienKola przechowywujemy promien kola na potrzeby funkcji antyRysCialo:
				int tymczasowyPromienKola = promienKolaWSymulacjiKopia;
				//Pobieramy wartość z TaskBara'a:
				promienKolaWSymulacjiKopia = SendMessage(hTrackBar1, TBM_GETPOS, 0, 0);

				HDC ustawieniaHDC = 0;
				ustawieniaHDC = GetDC(hDlg);

					//Zamalowywujemy wcześniejsze koło i rysujemy nowe:
					antyRysCialo(326, 215, tymczasowyPromienKola, ustawieniaHDC);
					
					RECT rectZamaluj = {287, 176, 365, 254};//{290, 251, 179, 326};//{290, 179, 251, 326};
					FillRect(ustawieniaHDC, &rectZamaluj, (HBRUSH)GetStockObject(WHITE_BRUSH));

					rysCialo(326, 215, promienKolaWSymulacjiKopia, ustawieniaHDC);

				DeleteDC(ustawieniaHDC);
			}
		}
		break;

		case WM_COMMAND:
			if(LOWORD(wParam) == ID_ZATWIERDZ && HIWORD(wParam) == BN_CLICKED)
			{
				/*BOOL czyDobrze1 = false;
				BOOL czyZnakMinus1 = false;
				BOOL czyDobrze2 = false;
				BOOL czyZnakMinus2 = false;*/

				//charTextPrzyspieszenie[0] = "/0"; charTextWysokosc[0] = "/0"; 


				///////////////////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////TEN TEKST MOŻE BYĆ PRZYDATNY/////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////////////
				//GetDlgItemText(hDlg, ID_COMBOBOXUSTAWIENIA1, charTextPrzyspieszenie, 300);
				//GetDlgItemText(hDlg, ID_COMBOBOXUSTAWIENIA2, charTextWysokosc, 300);

				////MessageBox(NULL, charTextPrzyspieszenie, "Dane pobrane!!!", MB_ICONINFORMATION | MB_OK);

				//przyspieszenie = charToDouble(charTextPrzyspieszenie);
				//wysokosc = charToDouble(charTextWysokosc);


				//przyspieszenie = (double)GetDlgItemInt(hDlg, ID_COMBOBOXUSTAWIENIA1, &czyDobrze1, czyZnakMinus1);
				//wysokosc = (double)GetDlgItemInt(hDlg, ID_COMBOBOXUSTAWIENIA2, &czyDobrze2, czyZnakMinus2);

				//char przyspieszenie2[] =(char[]) przyspieszenie;
				//int wysokosc2 = (int)wysokosc;

				//if(!czyDobrze1 || !czyDobrze2) MessageBox(NULL,"Dane nie pobrane!!!", "Informacja!", MB_ICONINFORMATION | MB_OK);
				//else 
				//{
				//	MessageBox(NULL,"Dane pobrane!!!", "Informacja!", MB_ICONINFORMATION | MB_OK);
					//MessageBox(NULL,(LPCSTR)przyspieszenie2, "Informacja!", MB_ICONINFORMATION | MB_OK);
				//}

				//long indexWybranegoElementu1 = 0;
				//long indexWybranegoElementu2 = 0;

				//indexWybranegoElementu1 = SendDlgItemMessage(hDlg, ID_COMBOBOXUSTAWIENIA1, CB_GETCURSEL, 0, 0);//ComboBox_GetCurSel(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1));//SendDlgItemMessage(hDlg, ID_COMBOBOXUSTAWIENIA1, CB_GETCURSEL, 0, 0);
				//indexWybranegoElementu2 = SendDlgItemMessage(hDlg, ID_COMBOBOXUSTAWIENIA2, CB_GETCURSEL, 0, 0);

				//if(indexWybranegoElementu1 == CB_ERR)
				//{
				//	GetDlgItemText(hDlg, ID_COMBOBOXUSTAWIENIA1, charTextPrzyspieszenie, sizeof(charTextPrzyspieszenie));
				//	
				//	//MessageBox(hDlg, "!!!", "CB_ERR 1", MB_ICONINFORMATION | MB_OK);
				//	przyspieszenie = charToDouble(charTextPrzyspieszenie);
				//}
				//else
				//{
				//	//przyspieszenie = ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1), indexWybranegoElementu1);
				//	przyspieszenie = SendDlgItemMessage(hDlg, ID_COMBOBOXUSTAWIENIA1, CB_GETITEMDATA, 0, 0);
				//}

				//if(indexWybranegoElementu2 == CB_ERR)
				//{
				//	GetDlgItemText(hDlg, ID_COMBOBOXUSTAWIENIA2, charTextWysokosc, 300);

				//	//MessageBox(hDlg, "!!!", "CB_ERR 2", MB_ICONINFORMATION | MB_OK);
				//	wysokosc = charToDouble(charTextWysokosc);
				//}
				//else
				//{
				//	//wysokosc = ComboBox_GetItemData(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA2), indexWybranegoElementu1);
				//	wysokosc = SendDlgItemMessage(hDlg, ID_COMBOBOXUSTAWIENIA2, CB_GETITEMDATA, 0, 0);
				//}

				//Edit_GetText(GetElementById
				GetDlgItemText(hDlg, ID_EDITBOXUSTAWIENIA1, parametr1, 300);
				przyspieszenie = charToDouble(parametr1);
				//parametr1[0] = (char)"\0";
				GetDlgItemText(hDlg, ID_EDITBOXUSTAWIENIA2, parametr2, 300);
				wysokosc = charToDouble(parametr2);
				GetDlgItemText(hDlg, ID_EDITBOXUSTAWIENIA3, parametr3, 300);
				szybkoscPoczatkowa = charToDouble(parametr3);
				GetDlgItemText(hDlg, ID_EDITBOXUSTAWIENIA4, parametr4, 300);
				katAlfa = charToDouble(parametr4);
				
				
				promienKolaWSymulacji = promienKolaWSymulacjiKopia;

				SendMessage(hDlg,WM_CLOSE,0,0);
			}
			
			if(LOWORD(wParam) == ID_ANULUJ && HIWORD(wParam) == BN_CLICKED)
			{
				SendMessage(hDlg,WM_CLOSE,0,0);
				//promienKolaWSymulacji = promienKolaWSymulacjiKopia;
			}

			if(LOWORD(wParam) == ID_USTAWIENIADOMYLNE1 && HIWORD(wParam) == BN_CLICKED)
			{
				HDC ustawieniaHDC1 = 0;
				ustawieniaHDC1 = GetDC(hDlg);

					antyRysCialo(326, 215, promienKolaWSymulacjiKopia, ustawieniaHDC1);

				DeleteDC(ustawieniaHDC1);
				
				promienKolaWSymulacjiKopia = WD_promienKolaWSymulacji;

				HWND hTrackBar1 = GetDlgItem(hDlg, ID_TRACKBAR1);
				SendMessage(hTrackBar1, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)promienKolaWSymulacjiKopia);

				HDC ustawieniaHDC2 = 0;
				ustawieniaHDC2 = GetDC(hDlg);

					rysCialo(326, 215, promienKolaWSymulacjiKopia, ustawieniaHDC2);

				DeleteDC(ustawieniaHDC2);
			}

			//if((LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA1 || LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA2) && HIWORD(wParam) == CBN_SETFOCUS)
			//{
			//	/*char tekst[1] = "";

			//	int czyPosiadaDane = ComboBox_GetText(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1), (LPSTR)tekst, 1);

			//	if(czyPosiadaDane == NULL) MessageBox(hDlg, "Kontrolka nie posiada dnych!!! ;)", "Kontrolka nie posiada dnych!!! ;)", MB_ICONINFORMATION | MB_OK);*/

			//	if(LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA1)
			//	{
			//		//Zmieniamy czcinkę na inną
			//		SendMessage(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1), WM_SETFONT, (WPARAM)myFont2, TRUE);
			//	}

			//	if(LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA2)
			//	{
			//		//Zmieniamy czcinkę na inną
			//		SendMessage(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA2), WM_SETFONT, (WPARAM)myFont2, TRUE);
			//	}
			//}

			//if((LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA1 || LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA2) && HIWORD(wParam) == CBN_KILLFOCUS)
			//{
			//	/*char tekst[1] = "";

			//	int czyPosiadaDane = ComboBox_GetText(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1), (LPSTR)tekst, 1);

			//	if(czyPosiadaDane == NULL) MessageBox(hDlg, "Kontrolka nie posiada dnych!!! ;)", "Kontrolka nie posiada dnych!!! ;)", MB_ICONINFORMATION | MB_OK);*/

			//	if(LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA1)
			//	{
			//		//Zmieniamy czcinkę na inną
			//		SendMessage(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA1), WM_SETFONT, (WPARAM)myFont2Kursywa, TRUE);
			//	}

			//	if(LOWORD(wParam) == ID_COMBOBOXUSTAWIENIA2)
			//	{
			//		//Zmieniamy czcinkę na inną
			//		SendMessage(GetDlgItem(hDlg, ID_COMBOBOXUSTAWIENIA2), WM_SETFONT, (WPARAM)myFont2Kursywa, TRUE);
			//	}
			//}

			return TRUE;

		case WM_PAINT:
			PAINTSTRUCT myStruct = {0};
			HDC myHDC = 0;

			myHDC = BeginPaint(hDlg, &myStruct); 

				RECT jakasRECT = {0};
				rysowanieWszystkiegoUstawienia(hDlg, myHDC, myStruct.rcPaint);
				
				//ShowWindow(buttonRozpoczecieSymG, SW_NORMAL);
				SetFocus(GetDlgItem(hDlg, ID_ZATWIERDZ));
				UpdateWindow(GetDlgItem(hDlg, ID_ZATWIERDZ));

				//ShowWindow(buttonUstwieniaG, SW_NORMAL);
				UpdateWindow(GetDlgItem(hDlg, ID_USTAWIENIA));

			EndPaint(hDlg, &myStruct);

		return 0;
	}

	return DefWindowProc(hDlg, message, wParam, lParam);
}

HMENU createSymFizMenu()
{
	HMENU menuHandle = 0;
	HMENU menuHPlik = 0;
	HMENU menuHPomoc = 0;

	menuHandle = CreateMenu();
	menuHPlik = CreatePopupMenu();
	menuHPomoc = CreatePopupMenu();

	AppendMenu(menuHPlik, MF_STRING, ID_ZAKONCZ, "Zakończ");
	AppendMenu(menuHPomoc, MF_STRING, ID_SFINFORMACJE, "SymFiz - informacje");

	AppendMenu(menuHandle, MF_STRING | MF_POPUP, (UINT)menuHPlik, "Plik");
	AppendMenu(menuHandle, MF_STRING | MF_POPUP, (UINT)menuHPomoc, "Pomoc");

	return (HMENU)menuHandle;
}

HWND buttonsSymFiz(HWND hwndWindow, HINSTANCE hInstance)
{
	HWND hwndButton = CreateWindow( 				
    "Button",
	"Ustawienia",		
    BS_DEFPUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD ^ BS_CENTER,
    600,     
    50,        
    150,        
    30,        
    hwndWindow,
    (HMENU)ID_USTAWIENIA, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);  

	SendMessage(hwndButton, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton, SW_NORMAL);
	UpdateWindow(hwndButton);
	

	//ToolTip dla przycisku "Ustwienia" to taki dymek ;)
	HWND hwndToolTip = CreateToolTip(ID_USTAWIENIA, hwndWindow, "Otwiera okno dialogowe, w którym ustala się parametry symulacji.");
	//Zmienia rozmiar ToolTipu - zwiększa jego szerokość, a także sprawia, że jest "wiloliniowy"
	SendMessage(hwndToolTip, TTM_SETMAXTIPWIDTH, 0, 200);

	buttonUstwieniaG = hwndButton;

	HWND hwndButton2 = CreateWindow( 				
    "Button",
	"Rozpocznij symulacje fizyczną",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD ^ BS_MULTILINE,
    600,     
    90,        
    150,        
    70,        
    hwndWindow,
    (HMENU)ID_ROZPOCZSYMULACJI, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton2, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton2, SW_NORMAL);
	UpdateWindow(hwndButton2);

	buttonRozpoczecieSymG = hwndButton2;

	SetFocus(hwndButton);

	HWND hwndButton3 = CreateWindow( 				
    "Button",
	"Natychmiast przerwij symulacje!!!",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD ^ BS_MULTILINE,
    600,     
    170,        
    150,        
    50,       
    hwndWindow,
    (HMENU)ID_BUTTONUSTAWIENIAPRZERWIJ, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);  

	SendMessage(hwndButton3, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton3, SW_NORMAL);
	UpdateWindow(hwndButton3);
	

	//ToolTip dla przycisku "Ustwienia" to taki dymek ;)
	HWND hwndToolTip2 = CreateToolTip(ID_BUTTONUSTAWIENIAPRZERWIJ, hwndWindow, "Natychmiast przerywa symulacje.");
	//Zmienia rozmiar ToolTipu - zwiększa jego szerokość, a także sprawia, że jest "wiloliniowy"
	SendMessage(hwndToolTip2, TTM_SETMAXTIPWIDTH, 0, 200);

	HWND hwndComboBox1 = CreateWindow( 				
    "ComboBox",
	0,		
    CBS_DROPDOWNLIST ^ WS_VISIBLE ^ WS_CHILD,
    600,     
    230,        
    150,        
    30,        
    hwndWindow,
    (HMENU)ID_COMBOBOXSYMFIZ1, 
    hInstance,
	0); 

	char lancuch1[] = "Spadek swobodny";
	char lancuch2[] = "Rzut ukośny";
	
	long index1 = 0;
	long index2 = 0;

	index1 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXSYMFIZ1, CB_ADDSTRING, 0, (LPARAM)lancuch1);
	index2 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXSYMFIZ1, CB_ADDSTRING, 0, (LPARAM)lancuch2);

	SendMessage(hwndComboBox1, WM_SETFONT, (WPARAM)myFont2, TRUE);

	//Wybieramy element o indeksie przechowywanym w zmiennej: index1 - jest wybierany przez nas:
	ComboBox_SetCurSel(hwndComboBox1, index1);

	//"Wyłączamy" kontrolkę:
	//ComboBox_Enable(hwndComboBox1, FALSE);

	ShowWindow(hwndComboBox1, SW_NORMAL);
	UpdateWindow(hwndComboBox1);

	return hwndButton;
}


int dlgWinUstawienia(HWND hWnd, HINSTANCE hInstance)
{
	WNDCLASSEX wndclassex = {0};

	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;
	wndclassex.lpfnWndProc = dlgWindowProcedure;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = LoadIcon(0, (LPCTSTR)IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(0, (LPCTSTR)IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;
	wndclassex.lpszMenuName = 0;
	wndclassex.lpszClassName = dlgWinCNUstawienia;
	wndclassex.hIcon = 0;

	RegisterClassEx(&wndclassex);

	RECT windowSize = {0, 0, 800, 500};
	
	AdjustWindowRectEx(&windowSize, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE, 0);

	//HWND windowDlgHandle = 0;

	dlgWindowHandle = CreateWindowEx(
		0,
		dlgWinCNUstawienia,
		"SymFiz - ustawienia",
		WS_OVERLAPPED ^ WS_CAPTION ^ WS_POPUP ^ WS_CHILD,
		100,
		60,
		400,
		400,
		hWnd,
		0,
		hInstance,
		0);

	ShowWindow(dlgWindowHandle, SW_NORMAL);
	UpdateWindow(dlgWindowHandle);

	buttonsUstawienia(dlgWindowHandle, (HINSTANCE)123);

	//To est już robione w funkcji buttonsUstawienia:
	/*HDC ustawieniaHDC = 0;
	ustawieniaHDC = GetDC(dlgWindowHandle);
					RECT rectZamaluj = {287, 176, 365, 254};
					FillRect(ustawieniaHDC, &rectZamaluj, (HBRUSH)GetStockObject(WHITE_BRUSH));
	DeleteDC(ustawieniaHDC);*/


	/*HWND hwndButton = CreateWindow( 				
    "Button",
	"Zatwierdź",		
    BS_FLAT ^ BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    290,     
    340,        
    100,        
    30,        
    windowDlgHandle,
    (HMENU)ID_ZATWIERDZ, 
    hInstance,
	0);      

	ShowWindow(hwndButton, SW_NORMAL);
	UpdateWindow(hwndButton);
	*/
	//dlgWindowHandle = windowDlgHandle;
	
	MSG msg;

	for(;;)
	{
		if(GetMessage(&msg, 0, 0, 0) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;
	}

	return (int)msg.wParam;

	//return windowDlgHandle;
}

HWND buttonsUstawienia(HWND hwndWindow, HINSTANCE hInstance)
{
	HWND hwndButton1 = CreateWindow( 				
    "Button",
	"Zatwierdź",		
    BS_DEFPUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    290,     
    340,        
    100,        
    30,        
    hwndWindow,
    (HMENU)ID_ZATWIERDZ, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton1, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton1, SW_NORMAL);
	UpdateWindow(hwndButton1);

	HWND hwndButton2 = CreateWindow( 				
    "Button",
	"Anuluj",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    180,     
    340,        
    100,        
    30,        
    hwndWindow,
    (HMENU)ID_ANULUJ, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton2, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton2, SW_NORMAL);
	UpdateWindow(hwndButton2);

	/*HWND hwndButton3 = CreateWindow( 				
    "Button",
	"+/-",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    320,     
    49,        
    26,        
    24,        
    hwndWindow,
    (HMENU)ID_DODAJ1, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton3, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton3, SW_NORMAL);
	UpdateWindow(hwndButton3);

	EnableWindow(hwndButton3, FALSE);

	HWND hwndButton4 = CreateWindow( 				
    "Button",
	"+/-",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    320,     
    99,        
    26,        
    24,        
    hwndWindow,
    (HMENU)ID_DODAJ2, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton4, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton4, SW_NORMAL);
	UpdateWindow(hwndButton4);

	EnableWindow(hwndButton4, FALSE);


	HWND hwndButton5 = CreateWindow( 				
    "Button",
	"?",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    355,     
    99,        
    15,        
    24,        
    hwndWindow,
    (HMENU)ID_POMOCUSTAWIENIA1, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton5, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton5, SW_NORMAL);
	UpdateWindow(hwndButton5);

	EnableWindow(hwndButton5, FALSE);

	HWND hwndButton6 = CreateWindow( 				
    "Button",
	"?",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    355,     
    49,        
    15,        
    24,        
    hwndWindow,
    (HMENU)ID_POMOCUSTAWIENIA2, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton6, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndButton6, SW_NORMAL);
	UpdateWindow(hwndButton6);

	EnableWindow(hwndButton6, FALSE);*/


	HWND hwndButton7 = CreateWindow( 				
    "Button",
	"Wróć do ustawień domyślnych(średnica: 20px)",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    10,     
    225,        
    260,        
    25,        
    hwndWindow,
    (HMENU)ID_USTAWIENIADOMYLNE1, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndButton7, WM_SETFONT, (WPARAM)myFont3, TRUE);
	ShowWindow(hwndButton7, SW_NORMAL);
	UpdateWindow(hwndButton7);


	//EnableWindow(hwndButton1, FALSE);
	//EnableWindow(hwndButton1, TRUE);

	//SendMessage(hwndWindow, WM_NEXTDLGCTL, (WPARAM)hwndButton1, TRUE);
	//SetDialogFocus(
	//SetFocus(hwndButton1);
	//SetActiveWindow(hwndButton1);

	//HWND hwndComboBox1 = CreateWindow( 				
 //   "ComboBox",
	//0,		
 //   CBS_DROPDOWN ^ WS_VISIBLE ^ WS_CHILD,
 //   10,     
 //   50,        
 //   300,        
 //   0,        
 //   hwndWindow,
 //   (HMENU)ID_COMBOBOXUSTAWIENIA1, 
 //   hInstance,
	//0); 

	//char lancuch1[] = "Przyspieszenie ziemskie____________________9.81m/s2";
	//char lancuch2[] = "Przyspieszenie księżycowe_________________1.622m/s2";
	//char lancuch3[] = "Przyśpieszenie marsjańskie_________________3.69m/s2";
	//char lancuch4[] = "Standard1__________________________________100m/s2";
	//char lancuch5[] = "Standard2__________________________________1000m/s2";

	//long index1 = 0;
	//long index2 = 0;
	//long index3 = 0;
	//long index4 = 0;
	//long index5 = 0;

	//index1 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA1, CB_ADDSTRING, 0, (LPARAM)lancuch1);
	////Wiążemy z danym elementem 32-bitową wartość:
	//ComboBox_SetItemData(hwndComboBox1, index1, 10);

	//index2 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA1, CB_ADDSTRING, 0, (LPARAM)lancuch2);
	//ComboBox_SetItemData(hwndComboBox1, index2, 2);

	//index3 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA1, CB_ADDSTRING, 0, (LPARAM)lancuch3);
	//ComboBox_SetItemData(hwndComboBox1, index3, 4);

	//index4 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA1, CB_ADDSTRING, 0, (LPARAM)lancuch4);
	//ComboBox_SetItemData(hwndComboBox1, index4, 100);

	//index5 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA1, CB_ADDSTRING, 0, (LPARAM)lancuch5);
	//ComboBox_SetItemData(hwndComboBox1, index5, 1000);

	//SetDlgItemText(hwndWindow, ID_COMBOBOXUSTAWIENIA1, charTextPrzyspieszenie);

	//SendMessage(hwndComboBox1, WM_SETFONT, (WPARAM)myFont2Kursywa, TRUE);
	//
	////Dodaje do Combo Box'a szary tekst, który jest wyświetlany w tejże kontrolce do chwili, 
	////gdy użytkownik nie kliknie na kontrolkę:
	//ComboBox_SetCueBannerText(hwndComboBox1, L"Tutaj podaj przyśpieszenie");

	//ShowWindow(hwndComboBox1, SW_NORMAL);
	//UpdateWindow(hwndComboBox1);



	//HWND hwndComboBox2 = CreateWindow( 				
 //   "ComboBox",
	//0,		
 //   CBS_DROPDOWN ^ WS_VISIBLE ^ WS_CHILD,
 //   10,     
 //   100,        
 //   300,        
 //   0,        
 //   hwndWindow,
 //   (HMENU)ID_COMBOBOXUSTAWIENIA2, 
 //   hInstance,
	//0); 

	//char lancuch2_1[] = "Wysokość Pałacu Kultury i Nauki______________230.68m";
	//char lancuch2_2[] = "Wysokość wieży Eiffel__________________________324m";
	//char lancuch2_3[] = "Wysokość Burdż Chalif_________________________828m";
	//char lancuch2_4[] = "Standard1______________________________________100m";
	//char lancuch2_5[] = "Standard2______________________________________1000m";

	//long index2_0 = 0;
	//long index2_1 = 0;
	//long index2_2 = 0;
	//long index2_3 = 0;
	//long index2_4 = 0;

	//index2_0 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA2, CB_ADDSTRING, 0, (LPARAM)lancuch2_1);
	//ComboBox_SetItemData(hwndComboBox2, index2_0, 230);

	//index2_1 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA2, CB_ADDSTRING, 0, (LPARAM)lancuch2_2);
	//ComboBox_SetItemData(hwndComboBox2, index2_1, 324);

	//index2_2 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA2, CB_ADDSTRING, 0, (LPARAM)lancuch2_3);
	//ComboBox_SetItemData(hwndComboBox2, index2_2, 828);

	//index2_3 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA2, CB_ADDSTRING, 0, (LPARAM)lancuch2_4);
	//ComboBox_SetItemData(hwndComboBox2, index2_3, 100);

	//index2_4 = SendDlgItemMessage(hwndWindow, ID_COMBOBOXUSTAWIENIA2, CB_ADDSTRING, 0, (LPARAM)lancuch2_5);
	//ComboBox_SetItemData(hwndComboBox2, index2_4, 1000);

	//SetDlgItemText(hwndWindow, ID_COMBOBOXUSTAWIENIA2, charTextWysokosc);

	//SendMessage(hwndComboBox2, WM_SETFONT, (WPARAM)myFont2Kursywa, TRUE);

	////Dodaje do Combo Box'a szary tekst, który jest wyświetlany w tejże kontrolce do chwili, 
	////gdy użytkownik nie kliknie na kontrolkę:
	//ComboBox_SetCueBannerText(hwndComboBox2, L"Tutaj podaj wysokość");

	//ShowWindow(hwndComboBox2, SW_NORMAL);
	//UpdateWindow(hwndComboBox2);

	HWND hwndEditBox1 = CreateWindowEx( 				
    WS_EX_CLIENTEDGE,
	"edit",
	0,
    WS_CHILD | WS_VISIBLE | ES_RIGHT,
    10,     
    50,        
    150,        
    22,        
    hwndWindow,
    (HMENU)ID_EDITBOXUSTAWIENIA1, 
    hInstance,
	0);

	//Edit_SetCueBannerText(hwndWindow, "Wysokość");

	SendMessage(hwndEditBox1, WM_SETFONT, (WPARAM)myFont2, TRUE);

	SetDlgItemText(hwndWindow, ID_EDITBOXUSTAWIENIA1, parametr1);
	ShowWindow(hwndEditBox1, SW_NORMAL);
	UpdateWindow(hwndEditBox1);


	HWND hwndEditBox2 = CreateWindowEx( 				
    WS_EX_CLIENTEDGE,
	"edit",
	0,
    WS_CHILD | WS_VISIBLE | ES_RIGHT,
    10,     
    100,        
    150,        
    22,        
    hwndWindow,
    (HMENU)ID_EDITBOXUSTAWIENIA2, 
    hInstance,
	0);
	
	//Edit_SetCueBannerText(hwndWindow, "Przyśpieszenie");

	SendMessage(hwndEditBox2, WM_SETFONT, (WPARAM)myFont2, TRUE);

	SetDlgItemText(hwndWindow, ID_EDITBOXUSTAWIENIA2, parametr2);
	ShowWindow(hwndEditBox2, SW_NORMAL);
	UpdateWindow(hwndEditBox2);


	HWND hwndEditBox3 = CreateWindowEx( 				
    WS_EX_CLIENTEDGE,
	"edit",
	0,
    WS_CHILD | WS_VISIBLE | ES_RIGHT,
    180,     
    50,        
    150,        
    22,        
    hwndWindow,
    (HMENU)ID_EDITBOXUSTAWIENIA3, 
    hInstance,
	0);
	
	//Edit_SetCueBannerText(hwndWindow, "Szybkość początkowa");

	SendMessage(hwndEditBox3, WM_SETFONT, (WPARAM)myFont2, TRUE);

	SetDlgItemText(hwndWindow, ID_EDITBOXUSTAWIENIA3, parametr3);
	ShowWindow(hwndEditBox3, SW_NORMAL);
	UpdateWindow(hwndEditBox3);


	HWND hwndEditBox4 = CreateWindowEx( 				
    WS_EX_CLIENTEDGE,
	"edit",
	0,
    WS_CHILD | WS_VISIBLE | ES_RIGHT,
    180,     
    100,        
    150,        
    22,        
    hwndWindow,
    (HMENU)ID_EDITBOXUSTAWIENIA4, 
    hInstance,
	0);
	
	//Edit_SetCueBannerTextFocused(hwndWindow, "Kąt α", FALSE);

	SendMessage(hwndEditBox4, WM_SETFONT, (WPARAM)myFont2, TRUE);

	SetDlgItemText(hwndWindow, ID_EDITBOXUSTAWIENIA4, parametr4);
	ShowWindow(hwndEditBox4, SW_NORMAL);
	UpdateWindow(hwndEditBox4);
	//DRAWTEXT:
	/*
	HDC myHDC = 0;
	myHDC = GetDC(hwndWindow);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkMode(myHDC, TRANSPARENT);
		
		RECT t1RECT = {10, 10, 300, 200};
		RECT t2RECT = {10, 80, 300, 200};

		DrawText(myHDC, "Przyspieszenie:\0", -1, &t1RECT, DT_LEFT);
		DrawText(myHDC, "Wysokość:\0", -1, &t2RECT, DT_LEFT);

	ReleaseDC(hwndWindow, myHDC);
	*/
	/*
	HWND hwndStaticControl1 = CreateWindow( 				
    "Static",
	"Przyspieszenie:",		
    SS_SIMPLE ^ WS_CHILD,
    10,     
    10,        
    300,        
    200,        
    hwndWindow,
    (HMENU)ID_STATICCONTROL1, 
    hInstance,
	0); 

	SendMessage(hwndStaticControl1, WM_SETFONT, (WPARAM)myFont2, TRUE);
	ShowWindow(hwndStaticControl1, SW_NORMAL);
	UpdateWindow(hwndStaticControl1);

	HWND hwndStaticControl2 = CreateWindow( 				
    "Static",
	"Wysokość:",		
    SS_SIMPLE ^ WS_CHILD,
    10,     
    80,        
    300,        
    200,        
    hwndWindow,
    (HMENU)ID_STATICCONTROL2, 
    hInstance,
	0); 

	SendMessage(hwndStaticControl2, WM_SETFONT, (WPARAM)myFont2, TRUE);
	ShowWindow(hwndStaticControl2, SW_NORMAL);
	UpdateWindow(hwndStaticControl2);

	*/	

	/*HWND hwndProgressBar1 = CreateWindowEx(
	0,
	PROGRESS_CLASS,
    0,
	WS_CHILD | WS_VISIBLE,
    100,
	100,
	200,
	30,
    hwndWindow,
	(HMENU) ID_PROGRESBARUSTAWIENIA1,
	hInstance,
	NULL);
	
	ShowWindow(hwndProgressBar1, SW_NORMAL);
	UpdateWindow(hwndProgressBar1);

	SendMessage(hwndProgressBar1, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
    SendMessage(hwndProgressBar1, PBM_SETSTEP, (WPARAM) 1, 0);
	
	int i = 80;
	while(i)
	{
		i--;
		SendMessage(hwndProgressBar1, PBM_STEPIT, 0, 0);
	}*/
	
	HWND hwndGroupBox1 = CreateWindow( 				
    "Button",
	"Ustawienia początkowe ciała",		
    BS_GROUPBOX ^ WS_VISIBLE ^ WS_CHILD,
    4,     
    4,        
    386,        
    150,        
    hwndWindow,
    (HMENU)ID_GROUPBOX1, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndGroupBox1, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndGroupBox1, SW_NORMAL);
	UpdateWindow(hwndGroupBox1);

	HWND hwndGroupBox2 = CreateWindow( 				
    "Button",
	"Ustawienia rysowania ciała",		
    BS_GROUPBOX ^ WS_VISIBLE ^ WS_CHILD,
    4,     
    160,        
    386,        
    332 - 160,        
    hwndWindow,
    (HMENU)ID_GROUPBOX2, 
    (HINSTANCE)GetWindowLong(hwndWindow, GWL_HINSTANCE),
	0);      

	SendMessage(hwndGroupBox2, WM_SETFONT, (WPARAM)myFont1, TRUE);
	ShowWindow(hwndGroupBox2, SW_NORMAL);
	UpdateWindow(hwndGroupBox2);
	
	SetFocus(hwndButton1);

	HDC ustawieniaHDC = 0;
	ustawieniaHDC = GetDC(hwndWindow);

		RECT ustawieniaRECT = {0};
		ustawieniaRECT.top = 0;
		ustawieniaRECT.left = 0;
		ustawieniaRECT.bottom = 350;
		ustawieniaRECT.right = 200;

		rysowanieWszystkiegoUstawienia(hwndWindow, ustawieniaHDC, ustawieniaRECT);

		//Przy okazji rysujemy także białą "podkładkę" pod czerwone koło:
		RECT rectZamaluj = {287, 176, 365, 254};
		FillRect(ustawieniaHDC, &rectZamaluj, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysCialo(326, 215, promienKolaWSymulacji, ustawieniaHDC);

	DeleteDC(ustawieniaHDC);

	tworzenieTrackBarCialo(hwndWindow, ID_TRACKBAR1, 6, 35, 10, 175);

	return hwndButton1;
}

int dlgWinSFInformacje(HWND hWnd, HINSTANCE hInstance)
{
	WNDCLASSEX wndclassex = {0};

	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;
	wndclassex.lpfnWndProc = dlgSFIWindowProcedure;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = LoadIcon(0, (LPCTSTR)IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(0, (LPCTSTR)IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH) COLOR_BTNSHADOW;
	wndclassex.lpszMenuName = 0;
	wndclassex.lpszClassName = dlgWinCNInformacje;
	wndclassex.hIcon = 0;

	RegisterClassEx(&wndclassex);

	RECT windowSize = {0, 0, 800, 500};
	
	AdjustWindowRectEx(&windowSize, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, FALSE, 0);

	HWND windowDlgHandle = 0;

	windowDlgHandle = CreateWindowEx(
		0,
		dlgWinCNInformacje,
		"SymFiz - Informacje",
		WS_OVERLAPPED ^ WS_CAPTION ^ WS_POPUP ^ WS_CHILD,
		300,
		200,
		350,
		250,
		hWnd,
		0,
		hInstance,
		0);

	ShowWindow(windowDlgHandle, SW_NORMAL);
	UpdateWindow(windowDlgHandle);

	buttonsSFInformacje(windowDlgHandle, (HINSTANCE) 1212);

	//dlgWindowHandle = windowDlgHandle;

	MSG msg;

	for(;;)
	{
		if(GetMessage(&msg, 0, 0, 0) != 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT) break;
	}

	return (int)msg.wParam;

	//return windowDlgHandle;
}

HWND buttonsSFInformacje(HWND hwndWindow, HINSTANCE hInstance)
{
	HWND hwndButton = CreateWindow( 				
    "Button",
	"OK",		
    BS_PUSHBUTTON ^ WS_VISIBLE ^ WS_CHILD,
    290,     
    190,        
    50,        
    30,        
    hwndWindow,
    (HMENU)ID_INFORMACJEOK, 
    hInstance,
	0);      

	SendMessage(hwndButton, WM_SETFONT, (WPARAM)myFont2, TRUE);
	ShowWindow(hwndButton, SW_NORMAL);
	UpdateWindow(hwndButton);


	HWND hwndStaticControl3 = CreateWindow( 				
    "Static",
	"Program stworzony przez brzegatora27.\n\nUWAGA:\nJest to wersja niestabilna i może zachowywac się niestabilnie.\n\nWszelkie prawa zastrzeżone!!!",		
    SS_EDITCONTROL ^ WS_CHILD,
    10,     
    10,        
    226,        
    119,        
    hwndWindow,
    (HMENU)ID_STATICCONTROL3, 
    hInstance,
	0); 

	SendMessage(hwndStaticControl3, WM_SETFONT, (WPARAM)myFont3, TRUE);
	ShowWindow(hwndStaticControl3, SW_NORMAL);
	UpdateWindow(hwndStaticControl3);

	/*systemLink1 = CreateWindowEx(
	0, 
	"SysLink",
	"Zajrzyj na <A HREF=\"http://www.interia.pl/\">mojego bloga</A> :)", 
	WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
	10, 
	129, 
	150, 
	40, 
	hwndWindow, 
	0, 
	hInstance, 
	0);*/
	
	SendMessage(systemLink1, WM_SETFONT, (WPARAM)myFont3, TRUE);
	ShowWindow(systemLink1, SW_NORMAL);
	UpdateWindow(systemLink1);


	/*HWND hwndComandLink1 = CreateWindow( 				
    "Button",
	"Jakiaś informacja. :)",		
    BS_COMMANDLINK ^ WS_VISIBLE ^ WS_CHILD,
    10,     
    10,        
    50,        
    30,        
    hwndWindow,
    (HMENU)ID_COMANDLINKINFORMACJE1, 
    hInstance,
	0);      

	SetWindowText(hwndComandLink1, "Jakiś tekst.");

	ShowWindow(hwndComandLink1, SW_NORMAL);
	UpdateWindow(hwndComandLink1);*/

	return hwndButton;
}

LRESULT CALLBACK dlgSFIWindowProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CLOSE:
			DestroyWindow(hDlg);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_COMMAND:
			if(LOWORD(wParam) == ID_INFORMACJEOK && HIWORD(wParam) == BN_CLICKED)
			{
				SendMessage(hDlg,WM_CLOSE,0,0);

				return TRUE;
			}

		case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
			case NM_CLICK:
			case NM_RETURN:
			{
				PNMLINK pNMLink = (PNMLINK)lParam;
				LITEM item = pNMLink->item;
				if ((((LPNMHDR)lParam)->hwndFrom == systemLink1) && (item.iLink == 0))
				{
					ShellExecute(0, "open", "http://interia.pl/", "", "", SW_SHOWNORMAL);
				}
				break;
			}
		}
		break;
	}

	return DefWindowProc(hDlg, message, wParam, lParam);
}


int spadekSwobodny(double wysokosc, double przyspieszenie, HWND hWnd)
{
	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		const int przelicznik = 1000;

		rysCialo(250, 60, 15, myHDC);
		//antyRysCialo(250, 60, myHDC);

		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		const int wysokoscSpadaniaNaEkraniePix = 380;
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		const double przelicznikWysRzeczNaPix = wysokosc / wysokoscSpadaniaNaEkraniePix;
		
		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = 250, terazY = 60;
		const int gdziePoczatekY = 60;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		//Zmienna przechowujaca wartosc o ile przesunac cialo:
		int oIlePrzesunac = 0;
		double przyspieszenieTerazniejsze = 0;
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) nie będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		double wysokoscTerazniejsza1 = wysokosc;
		double wysokoscTerazniejsza2 = 0;
		LARGE_INTEGER czasSpadaniaCiala = {0};

		QueryPerformanceCounter(&stanLicznika1);

		while(!czySkonczone)
		{	
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			//if((stanLicznika2.QuadPart - stanLicznika1.QuadPart) / rozdzielczoscZegara.QuadPart > 0.001)
			//{
				roznicaCzasow.QuadPart = stanLicznika2.QuadPart - stanLicznika1.QuadPart;
				QueryPerformanceCounter(&stanLicznika1);
				czasSpadaniaCiala.QuadPart = roznicaCzasow.QuadPart / rozdzielczoscZegara.QuadPart;
				//czasSpadaniaCiala;

			

			przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			wysokoscTerazniejsza1 += przyspieszenieTerazniejsze * czasSpadaniaCiala.QuadPart;
			wysokoscTerazniejsza2 += przyspieszenieTerazniejsze * czasSpadaniaCiala.QuadPart;

			for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
			{
				wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
				oIlePrzesunac++;
			}

			if(oIlePrzesunac)
			{
				antyRysCialo(terazX, terazY, 15, myHDC);
				for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
				{
					terazY++;
				}
				rysPodzialki1(myHDC);
				rysCialo(terazX, terazY, 15, myHDC);
			}

			//}
			if(terazY >= wysokoscSpadaniaNaEkraniePix + gdziePoczatekY) czySkonczone = true;
			czyPierwszyRaz = false;
		}

	ReleaseDC(hWnd, myHDC);

	return 0;
}

int spadekSwobodny2(double wysokosc, double przyspieszenie, HWND hWnd)
{
	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		//Przelicznik
		const int przelicznik = 1;

		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = 250, terazY = 60;
		terazXOdswierz = 250, terazYOdswierz = 60;

		rysCialo(terazX, terazY, 15, myHDC);
		//antyRysCialo(250, 60, myHDC);

		//Poniżej definicja zmiennych, które będą używane do obliczania czasu
		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		double rozdzielczoscZegara2 = (double)rozdzielczoscZegara.QuadPart;
		double roznicaCzasow2 = (double)roznicaCzasow.QuadPart;

		//zmienna przechowuje wartość, która jest równa odległości jaką "przemierza" ciało na ekranie
		const int wysokoscSpadaniaNaEkraniePix = 380;
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		const double przelicznikWysRzeczNaPix = wysokosc / wysokoscSpadaniaNaEkraniePix;
		
		const int gdziePoczatekY = 60;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		czasCiala = 0;

		//Zmienna przechowujaca wartosc o ile przesunac cialo na ekranie:
		int oIlePrzesunac = 0;
		//Zmienna przechowująca wartość szybkości ciała
		double szybkoscTerazniejsza = 0;
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		//double wysokoscTerazniejsza1 = wysokosc;  //Zdefiniowana jako zmienna globalna
		double wysokoscTerazniejsza2 = 0;
		//Zmienna przechowywująca czas spadania ciała:
		LARGE_INTEGER czasSpadaniaCiala = {0};
		double czasSpadaniaCiala2 = (double) czasSpadaniaCiala.QuadPart;
		//Pobranie czasu:
		QueryPerformanceCounter(&stanLicznika1);

		/////////////////////////////////////////////////////////////////////////////
		//wypiszDaneCiala(2, 2, 2, 2, 2, myHDC);
		/////////////////////////////////////////////////////////////////////////////
		while(!czySkonczone)
		{	
			//pobranie czasu
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			if(stanLicznika2.QuadPart - stanLicznika1.QuadPart > rozdzielczoscZegara.QuadPart / 100000)
			{
				//obliczanie różnicy czasów
				roznicaCzasow2 = (double)(stanLicznika2.QuadPart - stanLicznika1.QuadPart);
				//QueryPerformanceCounter(&stanLicznika1);
				QueryPerformanceCounter(&stanLicznika1);
				
				czasSpadaniaCiala2 = roznicaCzasow2 / rozdzielczoscZegara2;
				czasCiala += czasSpadaniaCiala2; 
				//czasSpadaniaCiala.QuadPart /= 1000;
				//czasSpadaniaCiala;
				//Obliczenia zmiennych
			
				wysokoscTerazniejsza1 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				wysokoscTerazniejsza2 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				szybkoscTerazniejsza += przyspieszenie * czasSpadaniaCiala2;
				szybkoscCiala = szybkoscTerazniejsza;

				//wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);
				
				//wysokoscTerazniejsza1 =(double)(przyspieszenie * (double)czasSpadaniaCiala.QuadPart * (double)czasSpadaniaCiala.QuadPart / 2);
				//wysokoscTerazniejsza2 =przyspieszenie * czasSpadaniaCiala.QuadPart * czasSpadaniaCiala.QuadPart / 2;
			

				//przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			
				//jeśli wysokośc rzeczywista jest większa od przelicznika pixeli na ekranie to wykonywana jest instrukacja
				for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
				{
					wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
					oIlePrzesunac++;
				}
					
				if(oIlePrzesunac)
				{
					//zamazuje poprzednio narysowane ciało - chwilowo "wyłączona"
					antyRysCialo(terazX, terazY, promienKolaWSymulacji, myHDC);
					for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
					{
						terazY++;
						terazYOdswierz++;
					}
					//rysuje podziałke:
					rysPodzialki1(myHDC);
					//rysuje ciało w podanych współżędnych
					rysCialo(terazX, terazY, promienKolaWSymulacji, myHDC);
				}

				//}
				//Sprawdzamy, czy ciało już jest "na ziemi" - jeśli tak to pętla się kończy 
				if(terazY >= wysokoscSpadaniaNaEkraniePix + gdziePoczatekY) czySkonczone = true;
				//czyPierwszyRaz = false;

			}
			
		}

		wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);

	ReleaseDC(hWnd, myHDC);

	return 0;
}

int spadekSwobodny3(double wysokosc, double przyspieszenie, HWND hWnd)
{
	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		//Przelicznik
		const int przelicznik = 1000;

		rysCialo(250, 60, 15, myHDC);
		//antyRysCialo(250, 60, myHDC);

		//Poniżej definicja zmiennych, które będą używane do obliczania czasu
		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		double rozdzielczoscZegara2 = (double)rozdzielczoscZegara.QuadPart;
		double roznicaCzasow2 = (double)roznicaCzasow.QuadPart;

		//zmienna przechowuje wartość, która jest równa odległości jaką "przemierza" ciało na ekranie
		const int wysokoscSpadaniaNaEkraniePix = 380;
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		const double przelicznikWysRzeczNaPix = wysokosc / wysokoscSpadaniaNaEkraniePix;
		
		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = 250, terazY = 60;
		const int gdziePoczatekY = 60;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		//Zmienna przechowujaca wartosc o ile przesunac cialo na ekranie:
		int oIlePrzesunac = 0;
		//Zmienna przechowująca wartość szybkości ciała
		double szybkoscTerazniejsza = 0;
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) nie będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		double wysokoscTerazniejsza1 = wysokosc;
		double wysokoscTerazniejsza2 = 0;
		//Zmienna przechowywująca czas spadania ciała:
		LARGE_INTEGER czasSpadaniaCiala = {0};
		double czasSpadaniaCiala2 = (double) czasSpadaniaCiala.QuadPart;
		//Pobranie czasu:
		QueryPerformanceCounter(&stanLicznika1);

		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		while(!czySkonczone)
		{	
			//pobranie czasu
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			if(stanLicznika2.QuadPart - stanLicznika1.QuadPart > rozdzielczoscZegara.QuadPart / 1000)
			{
			//obliczanie różnicy czasów
			roznicaCzasow.QuadPart = stanLicznika2.QuadPart - stanLicznika1.QuadPart;
			//QueryPerformanceCounter(&stanLicznika1);
			czasSpadaniaCiala.QuadPart = roznicaCzasow.QuadPart / rozdzielczoscZegara.QuadPart;
			//czasSpadaniaCiala.QuadPart /= 1000;
			//czasSpadaniaCiala;
			//Obliczenia zmiennych
			szybkoscTerazniejsza += przyspieszenie * czasSpadaniaCiala.QuadPart;
			wysokoscTerazniejsza1 += szybkoscTerazniejsza * czasSpadaniaCiala.QuadPart;
			wysokoscTerazniejsza2 += szybkoscTerazniejsza * czasSpadaniaCiala.QuadPart;
			
			
			
			//wysokoscTerazniejsza1 =(double)(przyspieszenie * (double)czasSpadaniaCiala.QuadPart * (double)czasSpadaniaCiala.QuadPart / 2);
			//wysokoscTerazniejsza2 =przyspieszenie * czasSpadaniaCiala.QuadPart * czasSpadaniaCiala.QuadPart / 2;
			

			//przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			
			//jeśli wysokośc rzeczywista jest większa od przelicznika pixeli na ekranie to wykonywana jest instrukacja
			for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
			{
				wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
				oIlePrzesunac++;
			}

			if(oIlePrzesunac)
			{
				//zamazuje poprzednio narysowane ciało - chwilowo "wyłączona"
				//antyRysCialo(terazX, terazY, myHDC);
				for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
				{
					terazY++;
				}
				//rysuje podziałke:
				rysPodzialki1(myHDC);
				//rysuje ciało w podanych współżędnych
				rysCialo(terazX, terazY, 15, myHDC);
			}

			//}
			//Sprawdzamy, czy ciało już jest "na ziemi" - jeśli tak to pętla się kończy 
			if(terazY >= wysokoscSpadaniaNaEkraniePix + gdziePoczatekY) czySkonczone = true;
			//czyPierwszyRaz = false;
			
			QueryPerformanceCounter(&stanLicznika1);
			}
			
		}

	ReleaseDC(hWnd, myHDC);

	return 0;
}

int rysCialo(int wsp1x, int wsp2y, int promienKola, HDC hdc)
{
	HGDIOBJ Obj_0;
	HGDIOBJ Obj_1;
	/*HBRUSH Obj_Brush1;
	HPEN Obj_Pen1;*/
	//HPEN czerwonePioro = 0;
	//HPEN czarnePioro = 0;
	HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN czerownePioro = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	POINT punktKolo[5] = {
	wsp1x, wsp2y - 3,
	wsp1x, wsp2y + 3,
	wsp1x, wsp2y,
	wsp1x - 3, wsp2y,
	wsp1x + 4, wsp2y
	};

	Obj_0 = SelectObject(hdc, czerownePioro);
	Obj_1 = SelectObject(hdc, nullBrush);
	Ellipse(hdc, wsp1x + promienKola, wsp2y + promienKola, wsp1x - promienKola, wsp2y - promienKola);
	Polyline(hdc, punktKolo, 5);
	SelectObject(hdc, Obj_0);
	SelectObject(hdc, Obj_1);
	DeleteObject(nullBrush);
	DeleteObject(czerownePioro);

	return 0;
}


int antyRysCialo(int wsp1x, int wsp2y, int promienKola, HDC hdc)
{
	HGDIOBJ Obj_0;
	HGDIOBJ Obj_1;
	//HPEN czerwonePioro = 0;
	//HPEN czarnePioro = 0;
	HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HPEN bialePioro = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

	POINT punktKolo[5] = {
	wsp1x, wsp2y - 3,
	wsp1x, wsp2y + 3,
	wsp1x, wsp2y,
	wsp1x - 3, wsp2y,
	wsp1x + 4, wsp2y
	};

	Obj_0 = SelectObject(hdc, bialePioro);
	Obj_1 = SelectObject(hdc, nullBrush);
	Ellipse(hdc, wsp1x + promienKola, wsp2y + promienKola, wsp1x - promienKola, wsp2y - promienKola);
	Polyline(hdc, punktKolo, 5);
	SelectObject(hdc, Obj_0);
	SelectObject(hdc, Obj_1);
	DeleteObject(Obj_0);
	DeleteObject(Obj_1);

	return 0;
}


int rysPodzialki1(HDC hdc)
{
	HGDIOBJ Obj_0;

	POINT linePoints[4] = {
	70, 60,
	70, 480,
	70, 440,
	450, 440
	};

	Polyline(hdc, linePoints, 4);

	HBRUSH myBrush = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
	Obj_0 = SelectObject(hdc, myBrush);
	SetBkMode(hdc, TRANSPARENT);

	HRGN myRegion = CreateRectRgn(70, 440, 450, 480);
	SelectClipRgn(hdc, myRegion);
	//HGDIOBJ myRegionObject = SelectObject(hdc, myRegion);

	Rectangle(hdc, 69, 439, 451, 481);

	SelectClipRgn(hdc, 0);
	SelectObject(hdc, Obj_0);
	DeleteObject(myRegion);
	DeleteObject(myBrush);

	return 0;
}


int wypiszDaneCiala(double szybkosc, double wysokosc, double czas, int wsp1x, int wsp2y, HDC hdc)
{
	HGDIOBJ Obj_0;
	/*HGDIOBJ Obj_1;
	HGDIOBJ Obj_2;*/

		Obj_0 = SelectObject(hdc, (HGDIOBJ)myFont2);
		//SetBkMode(hdc, TRANSPARENT);
		
		SetTextColor(hdc, RGB(0, 0, 0));

		TEXTMETRIC textMetrics = {0};
		GetTextMetrics(hdc, &textMetrics);
		int wysokoscTekstu = textMetrics.tmHeight;

		int czyTekstWyswietlony = 0;
		
		RECT t1RECT = {550, 110, 480, 140};
		RECT t2RECT = {280, 140, 170, 480};
		//SetRect(&t2RECT, 280, 140, 280, 170);
		RECT t3RECT = {280, 170, 200, 480};
		//RECT t1RECT = {280, 110, 110 + wysokoscTekstu, 480};
		//RECT t2RECT = {280, 115 + wysokoscTekstu, + wysokoscTekstu, 480};
		//RECT t3RECT = {280, 120 + wysokoscTekstu + wysokoscTekstu, 120 + wysokoscTekstu + wysokoscTekstu, 480};

		char dane1[] = "szybkosc\0";//+(char)szybkosc;
		char dane1Szybkosc[256] = {"\0"};
		char dane1Calosc[300] = {"\0"};
		dane1Szybkosc[0] = (const char)48;
		doubleToChar(&szybkosc, dane1Szybkosc);

		sprintf(dane1Calosc, "v = %0.3f m/s\0\0\0\0\0\0\0\0\0\0\0\0\0\0", szybkosc);

		char dane2[] = "szybkosc\0";//+(char)szybkosc;
		char dane2Szybkosc[256] = {"\0"};
		char dane2Calosc[300] = {"\0"};
		dane2Szybkosc[0] = (const char)48;
		doubleToChar(&wysokosc, dane2Szybkosc);

		sprintf(dane2Calosc, "h = %.3f m\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", wysokosc);

		char dane3[] = "szybkosc\0";//+(char)szybkosc;
		char dane3Szybkosc[256] = {"\0"};
		char dane3Calosc[300] = {"\0"};
		dane3Szybkosc[0] = (const char)48;
		doubleToChar(&czas, dane3Szybkosc);

		sprintf(dane3Calosc, "t = %.3f s\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", czas);
		//append_char((const char)&dane1Calosc, dane1Szybkosc);
		//dane1Calosc = dane1;
		//dane1Calosc += dane1Szybkosc;
		//char dane2[] = (char)szybkosc;

		//OMT - odstep między tekstem:
		int OMT = 20;

		/*HBRUSH nullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		HPEN czerownePioro = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN czarnePioro = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));*/


		SetBkMode(hdc, OPAQUE);
		//SelectClipRgn(hdc, 0);
		//RECT rectZamaluj = {250, 180, 250, 180 + 80};
		//RECT rectZamaluj = {250, 250, 180, 180 + 80};
		/*Obj_1 = SelectObject(hdc, czerownePioro);
		Obj_2 = SelectObject(hdc, nullBrush);*/
		//czyTekstWyswietlony = FillRect(hdc, &rectZamaluj, (HBRUSH)GetStockObject(WHITE_BRUSH));
		/*SelectObject(hdc, whiteBrush);
		SelectObject(hdc, czarnePioro);*/

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//if(czyTekstWyswietlony == FALSE)MessageBox(NULL,"Tekst nie może być wyświetlony!", "Informacja", MB_ICONINFORMATION | MB_OK | MB_HELP);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Zamalowywujemy poprzednie dane:
		RECT rectZamalujPoprzednieDane = {300, 180, 500, (180 + OMT * 3 + 90)};
		FillRect(hdc, &rectZamalujPoprzednieDane, (HBRUSH)GetStockObject(WHITE_BRUSH));

		//Wypisujemy dane:
		SelectObject(hdc, (HGDIOBJ)myFont2);
		SetBkMode(hdc, TRANSPARENT);
		
		/*RECT t1RECT_WypiszDane = {300, (180 + OMT), 300, (180 + OMT + 90)};
		RECT t2RECT_WypiszDane = {300, (180 + OMT * 2), 300, (180 + OMT * 2 + 90)};
		RECT t3RECT_WypiszDane = {300, (180 + OMT * 3), 470, (180 + OMT * 3 + 90)};*/
		
		RECT t1RECT_WypiszDane = {300, 180, 500, (180 + OMT + 90)};
		RECT t2RECT_WypiszDane = {300, 180 + OMT, 500, (180 + OMT * 2 + 90)};
		RECT t3RECT_WypiszDane = {300, 180 + OMT * 2, 500, (180 + OMT * 3 + 90)};

		DrawText(hdc, dane1Calosc, -1, &t1RECT_WypiszDane, DT_LEFT);
		DrawText(hdc, dane2Calosc, -1, &t2RECT_WypiszDane, DT_LEFT);
		DrawText(hdc, dane3Calosc, -1, &t3RECT_WypiszDane, DT_LEFT);

		/*SetBkMode(hdc,TRANSPARENT);
		TextOut(hdc, 300, 180 + OMT, dane1Calosc, sizeof(dane1Calosc));
		TextOut(hdc, 300, 180 + OMT * 2, dane2Calosc, sizeof(dane2Calosc));
		TextOut(hdc, 300, 180 + OMT * 3, dane3Calosc, sizeof(dane3Calosc));*/

		SelectObject(hdc, Obj_0);
		/*SetBkMode(hdc, OPAQUE);
		
		czyTekstWyswietlony = DrawText(hdc, "szybkosc", -1, &t1RECT, DT_TOP);
		czyTekstWyswietlony = DrawText(hdc, "wysokosc", -1, &t2RECT, DT_LEFT);
		czyTekstWyswietlony = DrawText(hdc, "czas", -1, &t3RECT, DT_LEFT);

		if(!czyTekstWyswietlony)MessageBox(NULL,"Tekst nie może być wyświetlony!", "Informacja", MB_ICONINFORMATION | MB_OK | MB_HELP);*/
	
	return 0;
}


double charToDouble(char *charText)
{
	stringstream stringText (charText);
	double result = 0;
	stringText >> result;
	
	return result;
}

char doubleToChar(double *doubleValue, const char *buffor)
{	
	
	stringstream stringStreamText (buffor);
	string stringText;

	stringStreamText << doubleValue;
	stringText = stringStreamText.str();
	//strcpy(buffor, stringText);
	buffor = stringText.c_str();

	//bufor = "123.123123";

	//strcpy(buffor, stringText);
	//buffor = (char)stringText.str;

	char niePotrzebne[] = "Niepotrzebne bo urzywamy wskaznikow ;)";
	double result = 0;
	//stringText >> result;
	
	return niePotrzebne[0];
}

char *append_char ( const char *s, const char c )
{
	size_t len = strlen ( s );
	char *ret = new char[len + 2];

	strcpy ( ret, s );
	ret[len] = c;
	ret[len + 1] = '\0';

	return ret;
}

int rysowanieWszystkiegoSymFiz(HWND hWnd, HDC myHDC, RECT myRECT)
{
	HGDIOBJ Obj_0;
	//HDC myHDC = 0;
	//myHDC = GetDC(hWnd);
	
		Obj_0 = SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		/*if(czyPierwszyRaz)FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(GRAY_BRUSH));
		else FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));*/

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysCialo(wspCialaX, wspCialaY, promienKolaWSymulacji, myHDC);
		rysPodzialki1(myHDC);
		wypiszDaneCiala(szybkoscCiala, wysokoscCiala, wysokoscTerazniejsza1, polozenieDanychX, polozenieDanychY, myHDC);

		SelectObject(myHDC, Obj_0);

	//ReleaseDC(hWnd, myHDC);

	return 0;
}

int rysowanieWszystkiegoUstawienia(HWND hWnd, HDC myHDC, RECT myRECT)
{
	//HDC myHDC = 0;
	//myHDC = GetDC(hWnd);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkMode(myHDC, TRANSPARENT);
		
		RECT t1RECT = {10, 34, 300, 224};
		RECT t2RECT = {10, 84, 300, 204};
		RECT t3RECT = {180, 34, 470, 224};
		RECT t4RECT = {180, 84, 470, 204};

		DrawText(myHDC, "Przyspieszenie   [m/s^2]:\0", -1, &t1RECT, DT_LEFT);
		DrawText(myHDC, "Wysokość   [m]:\0", -1, &t2RECT, DT_LEFT);
		DrawText(myHDC, "Prędkość początkowa   [m/s]:\0", -1, &t3RECT, DT_LEFT);
		DrawText(myHDC, "Kąt(alfa)   [stopnie]:\0", -1, &t4RECT, DT_LEFT);

	//ReleaseDC(hWnd, myHDC);

	return 0;
}

int rysowanieWszystkiegoInformacje(HWND hWnd, HDC myHDC, RECT myRECT)
{
	return 0;
}

HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText)
{
    if (!toolID || !hDlg || !pszText)
    {
        return FALSE;
    }
    // Get the window of the tool.
    HWND hwndTool = GetDlgItem(hDlg, toolID);
    
    // Create the tooltip. g_hInst is the global instance handle.
    HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                              WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              hDlg, NULL, 
                              NULL, NULL);
    
   if (!hwndTool || !hwndTip)
   {
       return (HWND)NULL;
   }                              
                              
    // Associate the tooltip with the tool.
    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hDlg;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.uId = (UINT_PTR)hwndTool;
    toolInfo.lpszText = pszText;
    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    return hwndTip;
}

void _cdecl spadekSwobodnyNowyWatek(void* daneNowyWatek)
{
	czyTrwaSymulacja = true;

	//Aby podczas symulacji rozmiar ciała się nie zmieniał w przyszlosci można coś takiego wprowadzić:
	int promienKolaStaly = promienKolaWSymulacji;

	//Zmienna po to, aby 
	//int promienKolaWczesniejszy = promienKolaWSymulacji;

	/*HWND hWnd = daneNowyWatek.hWnd;
	double wysokosc = daneNowyWatek.wysokosc;
	double przyspieszenie = daneNowyWatek.przyspieszenie;*/

	HWND hWnd = glowneOkno;
	double wysokosc1 = wysokosc;
	double przyspieszenie1 = przyspieszenie;

	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		//SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		HBRUSH myBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		HBRUSH myWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

		FillRect(myHDC, &t1RECT, myBlackBrush);
		FillRect(myHDC, &t2RECT, myWhiteBrush);

		DeleteObject(myBlackBrush);
		DeleteObject(myWhiteBrush);

		antyRysCialo(wspCialaX, wspCialaY, promienKolaWSymulacji, myHDC);

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		//Przelicznik
		const int przelicznik = 1;

		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = polozeniePoczatkoweX, terazY = polozeniePoczatkoweY;
		terazXOdswierz = polozeniePoczatkoweX, terazYOdswierz = polozeniePoczatkoweY;

		rysCialo(terazX, terazY, promienKolaWSymulacji, myHDC);

		//Po to, aby dobrze zamalowywać ciało jeśli zmienia się jego wielkość, ogólnie do użytku w całym programie:
		wspCialaX = terazX;
		//antyRysCialo(250, 60, myHDC);

		//Poniżej definicja zmiennych, które będą używane do obliczania czasu
		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		double rozdzielczoscZegara2 = (double)rozdzielczoscZegara.QuadPart;
		double roznicaCzasow2 = (double)roznicaCzasow.QuadPart;

		//zmienna przechowuje wartość, która jest równa odległości jaką "przemierza" ciało na ekranie
		const int wysokoscSpadaniaNaEkraniePix = 380;
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		const double przelicznikWysRzeczNaPix = wysokosc1 / wysokoscSpadaniaNaEkraniePix;
		
		const int gdziePoczatekY = polozeniePoczatkoweY;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		czasCiala = 0;

		//Zmienna przechowujaca wartosc o ile przesunac cialo na ekranie:
		int oIlePrzesunac = 0;
		//Zmienna przechowująca wartość szybkości ciała
		double szybkoscTerazniejsza = 0;
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		//double wysokoscTerazniejsza1 = wysokosc;  //Zdefiniowana jako zmienna globalna
		double wysokoscTerazniejsza2 = 0;
		double wysokoscTerazniejsza3 = wysokosc1;
		//Zmienna przechowywująca czas spadania ciała:
		LARGE_INTEGER czasSpadaniaCiala = {0};
		double czasSpadaniaCiala2 = (double) czasSpadaniaCiala.QuadPart;
		//Pobranie czasu:
		QueryPerformanceCounter(&stanLicznika1);

		//Zmienna, ktora mowi nam ile razy zostala wykonana pentla:
		int ileRazyWywolana = 0;

		//Zmienna, po to, aby wiedzieć który raz jest wywoływana pętla rysująca cilo, 
		//jeśli mniej od 4 to nie wypisujemy danych jeśli jest rowna 4 to wypisujemy i zerujemy zmienną, może to zapobiegnie "miganiu" tych danych:
		int ktoryRaz = 0;

		/////////////////////////////////////////////////////////////////////////////
		//wypiszDaneCiala(2, 2, 2, 2, 2, myHDC);
		/////////////////////////////////////////////////////////////////////////////
		while(!czySkonczone)
		{	
			//pobranie czasu
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			if(stanLicznika2.QuadPart - stanLicznika1.QuadPart > rozdzielczoscZegara.QuadPart / 100000)
			{
				//obliczanie różnicy czasów
				roznicaCzasow2 = (double)(stanLicznika2.QuadPart - stanLicznika1.QuadPart);
				//QueryPerformanceCounter(&stanLicznika1);
				QueryPerformanceCounter(&stanLicznika1);
				
				czasSpadaniaCiala2 = roznicaCzasow2 / rozdzielczoscZegara2;
				czasCiala += czasSpadaniaCiala2; 
				//czasSpadaniaCiala.QuadPart /= 1000;
				//czasSpadaniaCiala;
				//Obliczenia zmiennych
			
				wysokoscTerazniejsza1 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				wysokoscTerazniejsza3 -= szybkoscTerazniejsza * czasSpadaniaCiala2;
				wysokoscTerazniejsza2 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				szybkoscTerazniejsza += przyspieszenie1 * czasSpadaniaCiala2;
				szybkoscCiala = szybkoscTerazniejsza;

				if(ktoryRaz == 1000)
				{
					wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza3, czasCiala, 2, 2, myHDC);
					ktoryRaz = 0;
				}
				ktoryRaz++;

				//wysokoscTerazniejsza1 =(double)(przyspieszenie * (double)czasSpadaniaCiala.QuadPart * (double)czasSpadaniaCiala.QuadPart / 2);
				//wysokoscTerazniejsza2 =przyspieszenie * czasSpadaniaCiala.QuadPart * czasSpadaniaCiala.QuadPart / 2;
			

				//przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			
				//jeśli wysokośc rzeczywista jest większa od przelicznika pixeli na ekranie to wykonywana jest instrukacja
				for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
				{
					wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
					oIlePrzesunac++;
				}
					
				if(oIlePrzesunac)
				{
					//zamazuje poprzednio narysowane ciało - chwilowo "wyłączona"
					antyRysCialo(terazX, terazY, promienKolaStaly, myHDC);
					for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
					{
						terazY++;
						terazYOdswierz++;
					}
					//rysuje podziałke:
					rysPodzialki1(myHDC);
					//wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);
					//rysuje ciało w podanych współżędnych
					rysCialo(terazX, terazY, promienKolaStaly, myHDC);

					wspCialaY = terazY;
				}

				ileRazyWywolana++;

				//}
				//Sprawdzamy, czy ciało już jest "na ziemi" - jeśli tak to pętla się kończy 
				if(terazY >= wysokoscSpadaniaNaEkraniePix + gdziePoczatekY) czySkonczone = true;
				//czyPierwszyRaz = false;

			}
			
		}

		wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);

	ReleaseDC(hWnd, myHDC);

	ileRazyWywolana++;

	//Włączmy spowrotem przycisk: "Rozpocznij symulacje fizyczną"
	HWND handleOfButton = GetDlgItem(hWnd, ID_ROZPOCZSYMULACJI);
	EnableWindow(handleOfButton, TRUE);

	//wspCialaX = ;

	czyTrwaSymulacja = false;

	//MessageBox(hWnd, (LPCSTR)ileRazyWywolana, "Ile razy wywołana:", MB_ICONINFORMATION | MB_OK);

	_endthread();
}

HWND tworzenieTrackBarCialo(HWND hDlg, int IDofTrackBar, UINT iMin, UINT iMax, int x, int y)
{
	/*InitCommonControls();*/ 

	HWND hwndTrackBar = 0;
	HWND hwndStatic1 = 0;
	HWND hwndStatic2 = 0;

	const int widthOfStatic = 65;
	const int heightOfStatic = 30;

	#define ID_STATICBUDDY1 100
	#define ID_STATICBUDDY2 101

	hwndTrackBar = CreateWindowEx(
		0,
		TRACKBAR_CLASS,
		"TrackBar",
		WS_CHILD | WS_VISIBLE | TBS_HORZ,
		x + widthOfStatic, y + heightOfStatic / 2,
		125, 25,
		hDlg,
		(HMENU)IDofTrackBar,
		(HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE),
		NULL);
	
	hwndStatic1 = CreateWindowEx(
		0,
		"STATIC",
		"Mniejsze\n(min: 6px)",
		SS_CENTER | WS_CHILD | WS_VISIBLE,
		0, 0,
		widthOfStatic, heightOfStatic,
		hDlg,
		(HMENU)ID_STATICBUDDY1,
		(HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE),
		NULL);

	SendMessage(hwndStatic1, WM_SETFONT, (WPARAM)myFont3, TRUE);

	hwndStatic2 = CreateWindowEx(
		0,
		"STATIC",
		"Większe\n(max: 35px)",
		SS_CENTER | WS_CHILD | WS_VISIBLE,
		0, 0,
		widthOfStatic, heightOfStatic,
		hDlg,
		(HMENU)ID_STATICBUDDY2,
		(HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE),
		NULL);

	SendMessage(hwndStatic2, WM_SETFONT, (WPARAM)myFont3, TRUE);

	SendMessage(hwndTrackBar, TBM_SETBUDDY, (WPARAM)TRUE, (LPARAM)hwndStatic1);
	SendMessage(hwndTrackBar, TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)hwndStatic2);

	SendMessage(hwndTrackBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM) MAKELONG(iMin, iMax));
	SendMessage(hwndTrackBar, TBM_SETPAGESIZE, 0, (LPARAM)1);

	SendMessage(hwndTrackBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)promienKolaWSymulacji);

		//SendMessage(hwndTrackBar, TBM_SETRANGE, 
 //       (WPARAM) TRUE,                   // redraw flag 
 //       (LPARAM) MAKELONG(iMin, iMax));  // min. & max. positions 

 //   SendMessage(hwndTrackBar, TBM_SETPAGESIZE, 
 //       0, (LPARAM) 4);                  // new page size 


 //   SendMessage(hwndTrackBar, TBM_SETSEL, 
 //       (WPARAM) FALSE,                  // redraw flag 
 //       (LPARAM) MAKELONG(iSelMin, iSelMax)); 

 //   SendMessage(hwndTrackBar, TBM_SETPOS, 
 //       (WPARAM) TRUE,                   // redraw flag 
 //       (LPARAM) iSelMin); 

	return hwndTrackBar;
}

int rzutUkosny1(double przyspieszenie, double vPoczatkowe, double katAlfa, int x, int y)
{
	czyTrwaSymulacja = true;

	//Aby podczas symulacji rozmiar ciała się nie zmieniał w przyszlosci można coś takiego wprowadzić:
	int promienKolaStaly = promienKolaWSymulacji;

	//Zmienna po to, aby 
	//int promienKolaWczesniejszy = promienKolaWSymulacji;

	/*HWND hWnd = daneNowyWatek.hWnd;
	double wysokosc = daneNowyWatek.wysokosc;
	double przyspieszenie = daneNowyWatek.przyspieszenie;*/

	HWND hWnd = glowneOkno;
	double wysokosc1 = wysokosc;
	double przyspieszenie1 = przyspieszenie;

	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		//Przelicznik
		const int przelicznik = 1;

		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = 250, terazY = 60;
		terazXOdswierz = 250, terazYOdswierz = 60;

		rysCialo(terazX, terazY, promienKolaWSymulacji, myHDC);
		//antyRysCialo(250, 60, myHDC);

		//Poniżej definicja zmiennych, które będą używane do obliczania czasu
		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		double rozdzielczoscZegara2 = (double)rozdzielczoscZegara.QuadPart;
		double roznicaCzasow2 = (double)roznicaCzasow.QuadPart;

		//zmienna przechowuje wartość, która jest równa odległości jaką "przemierza" ciało na ekranie
		const int wysokoscSpadaniaNaEkraniePix = 380;
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		const double przelicznikWysRzeczNaPix = wysokosc1 / wysokoscSpadaniaNaEkraniePix;
		
		const int gdziePoczatekY = 60;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		czasCiala = 0;

		//Zmienna przechowujaca wartosc o ile przesunac cialo na ekranie:
		int oIlePrzesunac = 0;
		//Zmienna przechowująca wartość szybkości ciała
		double szybkoscTerazniejsza = 0;
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		//double wysokoscTerazniejsza1 = wysokosc;  //Zdefiniowana jako zmienna globalna
		double wysokoscTerazniejsza2 = 0;
		//Zmienna przechowywująca czas spadania ciała:
		LARGE_INTEGER czasSpadaniaCiala = {0};
		double czasSpadaniaCiala2 = (double) czasSpadaniaCiala.QuadPart;
		//Pobranie czasu:
		QueryPerformanceCounter(&stanLicznika1);

		/////////////////////////////////////////////////////////////////////////////
		//wypiszDaneCiala(2, 2, 2, 2, 2, myHDC);
		/////////////////////////////////////////////////////////////////////////////
		while(!czySkonczone)
		{	
			//pobranie czasu
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			if(stanLicznika2.QuadPart - stanLicznika1.QuadPart > rozdzielczoscZegara.QuadPart / 100000)
			{
				//obliczanie różnicy czasów
				roznicaCzasow2 = (double)(stanLicznika2.QuadPart - stanLicznika1.QuadPart);
				//QueryPerformanceCounter(&stanLicznika1);
				QueryPerformanceCounter(&stanLicznika1);
				
				czasSpadaniaCiala2 = roznicaCzasow2 / rozdzielczoscZegara2;
				czasCiala += czasSpadaniaCiala2; 
				//czasSpadaniaCiala.QuadPart /= 1000;
				//czasSpadaniaCiala;
				//Obliczenia zmiennych
			
				wysokoscTerazniejsza1 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				wysokoscTerazniejsza2 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				szybkoscTerazniejsza += przyspieszenie1 * czasSpadaniaCiala2;
				szybkoscCiala = szybkoscTerazniejsza;

				//wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);
				
				//wysokoscTerazniejsza1 =(double)(przyspieszenie * (double)czasSpadaniaCiala.QuadPart * (double)czasSpadaniaCiala.QuadPart / 2);
				//wysokoscTerazniejsza2 =przyspieszenie * czasSpadaniaCiala.QuadPart * czasSpadaniaCiala.QuadPart / 2;
			

				//przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			
				//jeśli wysokośc rzeczywista jest większa od przelicznika pixeli na ekranie to wykonywana jest instrukacja
				for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
				{
					wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
					oIlePrzesunac++;
				}
					
				if(oIlePrzesunac)
				{
					//zamazuje poprzednio narysowane ciało - chwilowo "wyłączona"
					antyRysCialo(terazX, terazY, promienKolaStaly, myHDC);
					for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
					{
						terazY++;
						terazYOdswierz++;
					}
					//rysuje podziałke:
					rysPodzialki1(myHDC);
					//rysuje ciało w podanych współżędnych
					rysCialo(terazX, terazY, promienKolaStaly, myHDC);

					wspCialaY = terazY;
				}

				//}
				//Sprawdzamy, czy ciało już jest "na ziemi" - jeśli tak to pętla się kończy 
				if(terazY >= wysokoscSpadaniaNaEkraniePix + gdziePoczatekY) czySkonczone = true;
				//czyPierwszyRaz = false;

			}
			
		}

		wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);

	ReleaseDC(hWnd, myHDC);

	//Włączmy spowrotem przycisk: "Rozpocznij symulacje fizyczną"
	HWND handleOfButton = GetDlgItem(hWnd, ID_ROZPOCZSYMULACJI);
	EnableWindow(handleOfButton, TRUE);

	//wspCialaX = ;

	czyTrwaSymulacja = false;

	return 0;

}


void _cdecl rzutUkosnyNowyWatek(void* daneNowyWatek)
{
	czyTrwaSymulacja = true;

	//Aby podczas symulacji rozmiar ciała się nie zmieniał w przyszlosci można coś takiego wprowadzić:
	int promienKolaStaly = promienKolaWSymulacji;

	//Zmienna po to, aby 
	//int promienKolaWczesniejszy = promienKolaWSymulacji;

	/*HWND hWnd = daneNowyWatek.hWnd;
	double wysokosc = daneNowyWatek.wysokosc;
	double przyspieszenie = daneNowyWatek.przyspieszenie;*/

	HWND hWnd = glowneOkno;
	double przyspieszenie1 = przyspieszenie;
	double szybkoscPoczatkowa1 = szybkoscPoczatkowa;
	double katAlfa1 = katAlfa;
	double polozenieX1Rzeczywiste = polozenieX;
	double polozenieY1Rzeczywiste = polozenieY;
	double polozenieX1 = polozenieX;
	double polozenieY1 = polozenieY;
	
	double wysokosc1 = wysokosc;

	int wysokoscPola1 = wysokoscPola;
	int szerokoscPola1 = szerokoscPola;

	HDC myHDC = 0;
	myHDC = GetDC(hWnd);
	
		HGDIOBJ Obj_0;
		//HGDIOBJ Obj_1;

		Obj_0 = SelectObject(myHDC, (HGDIOBJ)myFont2);
		SetBkColor(myHDC, RGB(0, 0, 0));
		
		RECT t1RECT = {10, 10, 510, 510};
		RECT t2RECT = {20, 20, 500, 500};

		FillRect(myHDC, &t1RECT, (HBRUSH)GetStockObject(BLACK_BRUSH));
		FillRect(myHDC, &t2RECT, (HBRUSH)GetStockObject(WHITE_BRUSH));

		SelectObject(myHDC, Obj_0);

		antyRysCialo(wspCialaX, wspCialaY, promienKolaWSymulacji, myHDC);

		rysPodzialki1(myHDC);

		bool czySkonczone = false;
		bool czyPierwszyRaz = true;

		//Przelicznik
		const int przelicznik = 1;

		//Zmienne, w ktorych beda przechowywane wspolzedne terazniejsze i wczesniejsze:
		int terazX = polePolozenieX, terazY = polePolozenieY;
		terazXOdswierz = polozeniePoczatkoweX, terazYOdswierz = polozeniePoczatkoweY;

		rysCialo(terazX, terazY, promienKolaWSymulacji, myHDC);
		//antyRysCialo(250, 60, myHDC);

		//Poniżej definicja zmiennych, które będą używane do obliczania czasu
		LARGE_INTEGER rozdzielczoscZegara = {0};
		LARGE_INTEGER stanLicznika1 = {0};
		LARGE_INTEGER stanLicznika2 = {0};
		LARGE_INTEGER roznicaCzasow = {0};
		QueryPerformanceFrequency(&rozdzielczoscZegara);
		rozdzielczoscZegara.QuadPart /= przelicznik;

		double rozdzielczoscZegara2 = 0;
		double roznicaCzasow2 = 0;

		rozdzielczoscZegara2 /= 0;

		rozdzielczoscZegara2 = (double)rozdzielczoscZegara.QuadPart;
		roznicaCzasow2 = (double)roznicaCzasow.QuadPart;
		
		//zmienna przechowuje wartość, która jest równa odległości jaką "przemierza" ciało na ekranie
		const int wysokoscSpadaniaNaEkraniePix = 380;

		//Na potrzeby obliczeń definiujemy PI:
		#define PI 3.14159265

		//Na potrzeby obliczeń definiujemy przelicznik radianów(rad) na stopnie:
		#define RAD 57.29577951

		//Zmienna, w której będzie przechowywana wartość mówiąca 
		//o zasięgu rzutu - na potrzeby przelicznika wysokosci na pixele:
		//int rzut_Zasieng = 0;
		
		int rzut_Zasieng = (int)((szybkoscPoczatkowa1 * szybkoscPoczatkowa1 / przyspieszenie1) * (sin(2 * katAlfa1 * PI / 180)));// * RAD));
		
		//Zmienna, w której będzie przechowywana wartość mówiąca 
		//o maxymalnej wysokości, którą osiągnie ciało - na potrzeby przelicznika wysokosci na pixele:
		int rzut_WysokoscMax = 0;

		rzut_WysokoscMax = (int)((szybkoscPoczatkowa1 * szybkoscPoczatkowa1) * (sin(katAlfa1 * PI / 180) * sin(katAlfa1 * PI / 180) / (2 * przyspieszenie1)));//* RAD) * (sin(katAlfa1 * PI / 180)) / przyspieszenie1));
		
		//Przelicznik wysokosci rzeczywistej na pixele na ekranie:
		double przelicznikWysRzeczNaPix;

		//Przelicznik wysokosci rzeczywistej na pixele na ekranie - inicjalizacja:
		/*if(rzut_Zasieng > rzut_WysokoscMax)
		{
			if(wysokoscPola > szerokoscPola)
			{
				przelicznikWysRzeczNaPix = rzut_WysokoscMax / wysokoscPola;
			}
			else
			{
				przelicznikWysRzeczNaPix = rzut_WysokoscMax / szerokoscPola;
			}
		}
		else
		{
			if(wysokoscPola > szerokoscPola)
			{
				przelicznikWysRzeczNaPix = rzut_Zasieng / wysokoscPola;
			}
			else
			{
				przelicznikWysRzeczNaPix = rzut_Zasieng / szerokoscPola;
			}
		}*/
		if(rzut_Zasieng > rzut_WysokoscMax)
		{
			if(wysokoscPola > szerokoscPola)
			{
				przelicznikWysRzeczNaPix = rzut_Zasieng / szerokoscPola;
			}
			else
			{
				przelicznikWysRzeczNaPix = rzut_Zasieng / wysokoscPola;
			}
		}
		else
		{
			if(wysokoscPola > szerokoscPola)
			{
				przelicznikWysRzeczNaPix = rzut_WysokoscMax / szerokoscPola;
			}
			else
			{
				przelicznikWysRzeczNaPix = rzut_WysokoscMax / wysokoscPola;
			}
		}
	
		int gdziePoczatekX = polePolozenieX;
		int gdziePoczatekY = polePolozenieY;
		//int wczesniejX = 0, wczesniejY = 0;  //Tych nie trzeba bo jest lepsze rozwiązanie, czyli bez nich.

		czasCiala = 0;
		//Zmienne przechowujace wartosci, ktore mowia o poprzednim polozeniu ciala:
		/*int polozeniePixeleX_poprzednie = 0; //Nie potrzebne ;)
		int polozeniePixeleY_poprzednie = 0;
		*/

		//Zmienne przechowujace wartosc o ile przesunac cialo na ekranie:
		int oIlePrzesunac = 0;///////////////////////////////////////////////////////Po przeksztalceniu kodu bedzie juz nie potrzebna ;)
		int polozeniePixeleX = polePolozenieX;
		int polozeniePixeleY = polePolozenieY;
		//Zmienna przechowująca wartość szybkości ciała
		double szybkoscTerazniejsza = 0;///////////////////////////////////////////////////////Po przeksztalceniu kodu bedzie juz nie potrzebna ;)
		double szybkoscTerazniejszaX = 0;
		double szybkoscTerazniejszaY = 0;

		szybkoscTerazniejszaX = szybkoscPoczatkowa1 * cos(katAlfa1 * PI / 180);
		szybkoscTerazniejszaY = szybkoscPoczatkowa1 * sin(katAlfa1 * PI / 180);
		
		//przyspieszenie /= przelicznik;
		//Dwie zmienne na wysokosc: 1) będzie tylko się zmniejszała, 2) będzie się zwiększała i czasami zmniejszała.
		//double wysokoscTerazniejsza1 = wysokosc;  //Zdefiniowana jako zmienna globalna
		double wysokoscTerazniejsza2 = 0;///////////////////////////////////////////////////////Po przeksztalceniu kodu bedzie juz nie potrzebna ;)
		double polozenieTerazniejszeX = 0;
		double polozenieTerazniejszeY = 0;
		//Zmienne, na wcześniejsze dane do rysowania ciała, dla funkcji anytyRysCialo(), 
		//bo musi byc wywołana po obliczeniach nowego położenia, aby zredukować miganie:
		int polozeniePixeleX_tymczasowa = 0;
		int polozeniePixeleY_tymczasowa = 0;
		//Zmienna przechowywująca czas spadania ciała:
		LARGE_INTEGER czasSpadaniaCiala = {0};
		double czasSpadaniaCiala2 = (double) czasSpadaniaCiala.QuadPart;
		//Pobranie czasu:
		QueryPerformanceCounter(&stanLicznika1);

		////Informuje użytkownika ile potrwa symulacja:
		//double czasRzutu = 0;
		//czasRzutu = 2 * szybkoscPoczatkowa1 * sin(katAlfa1 * PI / 180) / przyspieszenie1;
		//char czasRzutu_Char[300];// = "\0";
		//doubleToChar(&czasRzutu, &czasRzutu_Char[0]);

		//MessageBox(glowneOkno, czasRzutu_Char, "Czas trwania symulacji:", MB_ICONINFORMATION | MB_YESNO);

		//Sprawdzamy ile razy została wywołana pentla:
		int ileRazyWywolana = 0;
		/////////////////////////////////////////////////////////////////////////////
		//wypiszDaneCiala(2, 2, 2, 2, 2, myHDC);
		/////////////////////////////////////////////////////////////////////////////
		while(!czySkonczone)
		{	
			//pobranie czasu
			QueryPerformanceCounter(&stanLicznika2);
			
			//if(czyPierwszyRaz)QueryPerformanceCounter(&stanLicznika1);

			if(stanLicznika2.QuadPart - stanLicznika1.QuadPart > rozdzielczoscZegara.QuadPart / 100)
			{
				//obliczanie różnicy czasów
				roznicaCzasow2 = (double)(stanLicznika2.QuadPart - stanLicznika1.QuadPart);
				//QueryPerformanceCounter(&stanLicznika1);
				QueryPerformanceCounter(&stanLicznika1);
				
				czasSpadaniaCiala2 = roznicaCzasow2 / rozdzielczoscZegara2;
				czasCiala += czasSpadaniaCiala2; 
				//czasSpadaniaCiala.QuadPart /= 1000;
				//czasSpadaniaCiala;
				//Obliczenia zmiennych

				//Obliczamy nowe poloznie ciala:
				polozenieX1Rzeczywiste = szybkoscPoczatkowa1 * czasCiala * (cos(katAlfa1 * PI / 180));// * RAD);
				polozenieY1Rzeczywiste = szybkoscPoczatkowa1 * czasCiala * (sin(katAlfa1 * PI / 180)) - przyspieszenie1 / 2 * (czasCiala * czasCiala);// * RAD) - przyspieszenie1 / 2 * czasCiala * czasCiala;
				
				//Obliczmy szybkosci ciala:
				//szybkoscTerazniejszaX = ; //Jej nie obliczmy bo jest stala
				szybkoscTerazniejszaY = szybkoscPoczatkowa1 * sin(katAlfa1 * PI / 180) - przyspieszenie1 * czasCiala;
				
				//Odległość pozioma przebyta w poziomie po czasie t:
				polozenieTerazniejszeX = szybkoscTerazniejszaX * czasCiala;

				//Wysokość na jakiej znajduje się ciało po czasie t:
				polozenieTerazniejszeY = szybkoscTerazniejszaY * czasCiala - (przyspieszenie1 / 2) * (czasCiala * czasCiala);
				
				//Przypisanie do dwóch zmiennych polożenia na potrzeby funkcji antyRysCialo():
				polozeniePixeleX_tymczasowa = polozeniePixeleX;
				polozeniePixeleY_tymczasowa = polozeniePixeleY;

				//Obliczmy nowe polozenie ciala(jednostak pixele):
				polozeniePixeleX = polePolozenieX;
				polozeniePixeleY = polePolozenieY;

				polozeniePixeleX += (int)(polozenieTerazniejszeX / przelicznikWysRzeczNaPix + 0.5);
				polozeniePixeleY -= (int)(polozenieTerazniejszeY / przelicznikWysRzeczNaPix + 0.5);

				//polozeniePixeleY *= 3;

				antyRysCialo(polozeniePixeleX_tymczasowa, polozeniePixeleY_tymczasowa, promienKolaStaly, myHDC);

				rysPodzialki1(myHDC);

				/*terazX = polozeniePixeleX;
				terazY = polozeniePixeleY;*/

				rysCialo(polozeniePixeleX, polozeniePixeleY, promienKolaStaly, myHDC);

				/*
				wysokoscTerazniejsza1 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				wysokoscTerazniejsza2 += szybkoscTerazniejsza * czasSpadaniaCiala2;
				szybkoscTerazniejsza += przyspieszenie1 * czasSpadaniaCiala2;
				szybkoscCiala = szybkoscTerazniejsza;
				*/

				//wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);
				
				//wysokoscTerazniejsza1 =(double)(przyspieszenie * (double)czasSpadaniaCiala.QuadPart * (double)czasSpadaniaCiala.QuadPart / 2);
				//wysokoscTerazniejsza2 =przyspieszenie * czasSpadaniaCiala.QuadPart * czasSpadaniaCiala.QuadPart / 2;
			

				//przyspieszenieTerazniejsze += przyspieszenie * czasSpadaniaCiala.QuadPart;
			
				//jeśli wysokośc rzeczywista jest większa od przelicznika pixeli na ekranie to wykonywana jest instrukacja
				/*for( ;wysokoscTerazniejsza2 > przelicznikWysRzeczNaPix; )
				{
					wysokoscTerazniejsza2 -= przelicznikWysRzeczNaPix;
					oIlePrzesunac++;
				}
					
				if(oIlePrzesunac)
				{
					//zamazuje poprzednio narysowane ciało - chwilowo "wyłączona"
					antyRysCialo(terazX, terazY, promienKolaStaly, myHDC);
					for( ;oIlePrzesunac > 0 && terazY < wysokoscSpadaniaNaEkraniePix + gdziePoczatekY; oIlePrzesunac--)
					{
						terazY++;
						terazYOdswierz++;
					}
					//rysuje podziałke:
					rysPodzialki1(myHDC);
					//rysuje ciało w podanych współżędnych
					rysCialo(terazX, terazY, promienKolaStaly, myHDC);

					wspCialaY = terazY;
				}
				*/
				//}

				ileRazyWywolana++;

				//Sprawdzamy, czy ciało już jest "na ziemi" - jeśli tak to pętla się kończy
				if(polozeniePixeleY >= polePolozenieY && polozenieX1Rzeczywiste >= (rzut_Zasieng / 3)) czySkonczone = true;
				//if(polozeniePixeleY >= polePolozenieY && polozenieX1Rzeczywiste >= (rzut_Zasieng / 3)) czySkonczone = true;
				//if(polozeniePixeleY >= polePolozenieY && polozeniePixeleX * przelicznikWysRzeczNaPix >= rzut_Zasieng) czySkonczone = true;
				//czyPierwszyRaz = false;

			}
			
		}

		wspCialaX = polozeniePixeleX; 
		wspCialaY = polozeniePixeleY;

		///////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		//wypiszDaneCiala(szybkoscCiala, wysokoscTerazniejsza2, czasCiala, 2, 2, myHDC);///
		///////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////

	ReleaseDC(hWnd, myHDC);

	//MessageBox(hWnd, (LPCSTR)ileRazyWywolana, "Ile razy wywołana:", MB_ICONINFORMATION | MB_OK);

	//Włączamy spowrotem przycisk: "Rozpocznij symulacje fizyczną"
	HWND handleOfButton = GetDlgItem(hWnd, ID_ROZPOCZSYMULACJI);
	EnableWindow(handleOfButton, TRUE);

	//wspCialaX = ;

	czyTrwaSymulacja = false;

	_endthread();
}
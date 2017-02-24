/****************************************************************************
 Modul:   		N_VISUAL.C

 Aufgabe:    	EnthÑlt Funktionen zur Visualisierung

 Funktionen:  Visualisierung
****************************************************************************/


#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "ndl_visu.h"
#include "neuromen.h"
#include "math.h"

/****************************************************************************
 Funktion:   		Visualisierung

 Aufgabe:       Visualisierungsdialogbox darstellen

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Visualisierung(	HWND			hDlg,
																unsigned	message,
																WORD			wParam,
																LONG			lParam)
{
HDC 					hDC;
PAINTSTRUCT 	ps;
int 					i,c;

static	int 	x=0;
static	int 	y=0;
static	int 	z=0;
static	int 	nOffX1;
static	int		nOffX2;
static  int		nOffY1;
static	int		nOffY2;
static	int		nOffZ1;
static	int		nOffZ2;

static	char 	Daten[50];
static  char	Grip[60];
static	char  Koordinatenx[10]="0";
static  char  Koordinateny[10]="0";
static	char  Koordinatenz[10]="0";


	switch (message){
		case WM_INITDIALOG:
				 Koordinatenx[0]='\0';
				 Koordinateny[0]='\0';
				 Koordinatenz[0]='\0';
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case	IDOK:
					 case IDCANCEL:
								PostMessage(hDlg,WM_SYSCOMMAND,SC_CLOSE,0L);
								return (TRUE);
					 case ID10_AKTUELL:
								Koordinatenx[0]='\0';
								Koordinateny[0]='\0';
								Koordinatenz[0]='\0';
								HoleRoboterPosition((LPSTR)Daten,(LPSTR)Grip);
								i=0;
								c=0;
								while(Daten[i]!=',')
								{
									Koordinatenx[c] = Daten[i];
									i++;
									c++;
								}
								Koordinatenx[c] = '\0';
								c=0;
								i++;
								while(Daten[i]!=',')
								{
									Koordinateny[c] = Daten[i];
									i++;
									c++;
								}
								Koordinateny[c] = '\0';
								c=0;
								i++;
								while(Daten[i]!=',')
								{
									Koordinatenz[c] = Daten[i];
									i++;
									c++;
								}
								Koordinatenz[c] = '\0';
								wsprintf((LPSTR)dummy,(LPSTR)"%s,%s,%s",(LPSTR)Koordinatenx,
									(LPSTR)Koordinateny,(LPSTR)Koordinatenz);
								SetDlgItemText(hDC,ID10_DATEN,(LPSTR)dummy);
								InvalidateRect(hDlg,NULL,TRUE);
								SendMessage(hDlg,WM_PAINT,0,0L);
								break;
				 }
				 break;
		case WM_PAINT:
								hDC = BeginPaint(hDlg, &ps);
								SetMapMode(hDC,MM_ANISOTROPIC);
								SetWindowExt(hDC,1,1);
								SetViewportExt(hDC,1,1);
								SetWindowOrg(hDC,1,1);
								SetViewportOrg(hDC,0,0);
								SelectObject(hDC, hRotPen1);
								MoveTo(hDC,5,120);
								LineTo(hDC,130,120);
								MoveTo(hDC,70,60);
								LineTo(hDC,70,180);

								MoveTo(hDC,145,120);
								LineTo(hDC,270,120);
								MoveTo(hDC,210,60);               // z-Achse
								LineTo(hDC,210,180);
																									// K-System y-z
								MoveTo(hDC,285,120);              // y-Achse
								LineTo(hDC,405,120);
								MoveTo(hDC,350,60);               // z-Achse
								LineTo(hDC,350,180);

								MoveTo(hDC,125,115);
								LineTo(hDC,130,120);              // Pfeil x
								MoveTo(hDC,125,125);
								LineTo(hDC,130,120);

								MoveTo(hDC,65,65);                // Pfeil y
								LineTo(hDC,70,60);
								MoveTo(hDC,75,65);
								LineTo(hDC,70,60);

								MoveTo(hDC,265,115);
								LineTo(hDC,270,120);              // Pfeil x
								MoveTo(hDC,265,125);
								LineTo(hDC,270,120);

								MoveTo(hDC,205,65);
								LineTo(hDC,210,60);               // Pfeil z
								MoveTo(hDC,215,65);
								LineTo(hDC,210,60);

								MoveTo(hDC,400,115);
								LineTo(hDC,405,120);
								MoveTo(hDC,400,125);              // Pfeil y
								LineTo(hDC,405,120);

								MoveTo(hDC,345,65);
								LineTo(hDC,350,60);               // Pfeil z
								MoveTo(hDC,355,65);
								LineTo(hDC,350,60);
																									// Koord._Text
								SelectObject(hDC,hSchwarzPen1);
								TextOut (hDC,125,125,(LPSTR)"x",1);
								TextOut (hDC,75,65,(LPSTR)"y",1);
								TextOut (hDC,266,126,(LPSTR)"x",1);
								TextOut (hDC,215,65,(LPSTR)"z",1);
								TextOut (hDC,401,126,(LPSTR)"y",1);
								TextOut (hDC,355,65,(LPSTR)"z",1);
								x=atoi(Koordinatenx);
								y=atoi(Koordinateny);
								z=atoi(Koordinatenz);
								nOffX1=70;                        // OFFSET fuer ....
								nOffY1=120;                       // Koordinatensystem
								nOffX2=210;
								nOffZ1=120;
								nOffY2=350;
								nOffZ2=120;
								x=x/15;
								y=y/15;
								z=z/15;
								y=y*(-1);
								z=z*(-1);
								SelectObject(hDC,hSchwarzPen1);
								SelectObject(hDC,hSchwarzBrush1);
								Ellipse(hDC,x+nOffX1-2,y+nOffY1-2,x+nOffX1+2,y+nOffY1+2);
								Ellipse(hDC,x+nOffX2-2,z+nOffZ1-2,x+nOffX2+2,z+nOffZ1+2);
								y=y*(-1);
								Ellipse(hDC,y+nOffY2-2,z+nOffZ2-2,y+nOffY2+2,z+nOffZ2+2);
								EndPaint(hDlg,&ps);
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								DestroyWindow(hDlgVisualisierung);
								FreeProcInstance(lpVisualisierung);
								bVisualisierung = FALSE;
								hDlgVisualisierung = NULL;
								CheckMenuItem(hMainMenu,IDM_VISUALISIERUNG,MF_UNCHECKED);
								return TRUE;
					 default:
								return FALSE;
				 }
				 break;
	}
	return(FALSE);
}//Visualisierung

/****************************************************************************
 Modul:      	N_ROBINI.C

 Aufgabe:  		Initialisiert den Roboter und enthÑlt Kommunikationsfunktionen

 Funktionen:  RobInit
							SendeComm1
							Sende
							HoleRoboterPosition
							LadeGrundposition
							Gelenkdemo
****************************************************************************/

#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "ndl_rob.h"
#include "stdio.h"
#include "math.h"

/****************************************************************************
 Funktion:  		RobInit

 Aufgabe:       Initialisiert den Roboter

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/


BOOL FAR PASCAL RobInit(HWND			hDlg,
												unsigned	message,
												WORD			wParam,
												LONG			lParam)
{
char							szSetting[] = "COM1: 9600,E,7,2";
COMSTAT						CommStatus;
static 	BOOL		  bNTSenden;

switch (message){
		case WM_INITDIALOG:
				 bNTSenden=FALSE;
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDCANCEL:
								bMoveMasterInit=FALSE;
								EndDialog(hDlg, TRUE);
								return(TRUE);
					 case ID19_NT:
								if(bNTSenden)
								{
									CheckDlgButton(hDlg,wParam,0);
									bNTSenden=FALSE;
								}
								else
								{
									CheckDlgButton(hDlg,wParam,1);
									bNTSenden=TRUE;
								}
								break;
					 case IDOK:
								SetDlgItemText(hDlg,ID19_TEXT1,
									(LPSTR)"Bitte warten Sie einen Moment!");
								if (BuildCommDCB(szSetting,&CommDCB)!=0)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Schnittstellenfehler aufgetreten",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									EndDialog(hDlg,TRUE);
								}
								CommDCB.fOutxDsrFlow=1;
								CommDCB.fOutxCtsFlow=1;
								CloseComm(nCid);
								if ((nCid = OpenComm((LPSTR)"COM1",128,128))<0)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Schnittstellenfehler aufgetreten",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									EndDialog(hDlg,TRUE);
									return(TRUE);
								}
								if (SetCommState(&CommDCB) != 0)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Schnittstellenfehler aufgetreten",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									EndDialog(hDlg,TRUE);
									return(TRUE);
								}
								FlushComm(nCid,0);
								FlushComm(nCid,1);
								if(bNTSenden)
									SendeComm1(nCid,"NT\n");
								wsprintf((LPSTR)dummy,(LPSTR)"%i",roboterspeed);
								SP((LPSTR)dummy);
								SendeComm1(nCid,"TL 132\n");
								SendeComm1(nCid,"NW\n");
								if(!LadeGrundposition())
								{
									MessageBox(GetFocus(),
										(LPSTR)"Schnittstellenfehler aufgetreten",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									EndDialog(hDlg,TRUE);
									return(TRUE);
								}
								SendeComm1(nCid,"MO 999,O\n");
								if(GetCommError(nCid,&CommStatus)>0)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Schnittstellenfehler aufgetreten",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									EndDialog(hDlg,TRUE);
									return(TRUE);
								}
								bMoveMasterInit=TRUE;
								EndDialog(hDlg, TRUE);
								return(TRUE);
				 }
	}
	return (FALSE);
}//RobInit

/****************************************************************************
 Funktion:			SendeComm1

 Aufgabe:       Sendet ein Befehl zum Roboter mit nachfolgender FehlerprÅfung

 Parameter:     nCid						: Handle der Schnittstelle
								*befehlsstring	: Zeiger auf Zeichenkette die den Befehl
																	enthÑlt

 RÅckgabewert:  int	TRUE wenn erfolgreich

****************************************************************************/


int SendeComm1(HANDLE	nCid,char *befehlsstring)
{
char		error[5];
int			lenerror;
COMSTAT	CommStatus;
HCURSOR	hSaveCursor;
char 		Sender[50];
int			nSender;

	hSaveCursor = SetCursor(hMaster);
	Sende(nCid,befehlsstring);
	lenerror = 0;
	FlushComm(nCid,1);
	Sende(nCid,"ER\n");
	error[0]='\0';
	do
	{
		nSender = ReadComm(nCid,(LPSTR)Sender,5);
		Sender[nSender]='\0';
		lstrcat((LPSTR)error,(LPSTR)Sender);
		lenerror+=nSender;
	}while(Sender[nSender-1]!='\n');
	error[lenerror-1]='\0';
	if(error[0]=='1')
	{
		Sende(nCid,"RS\n");
		MessageBox(GetFocus(),(LPSTR)"Starten Sie das System komplett neu",
			(LPSTR)"Hardware Error",MB_OK|MB_ICONHAND|MB_SYSTEMMODAL);
	}
	if(error[0]=='2')
	{
		Sende(nCid,"RS\n");
		MessageBox(GetFocus(),(LPSTR)befehlsstring,
			(LPSTR)"Software Error",MB_OK|MB_ICONHAND|MB_SYSTEMMODAL);
	}
	SetCursor(hSaveCursor);
	return(TRUE);
}//SendComm1

/****************************************************************************
 Funktion:       Sende

 Aufgabe:        öbernimmt DatenÅbertragung zum Movemaster

 Parameter: nCid         		: Handle von COM1
						*befehlsstring	: Zeiger auf String der den Befehl enthÑlt

 RÅckgabewert: int   negativ wenn Fehler aufgetreten
****************************************************************************/


int Sende(HANDLE	nCid,char *befehlsstring)
{
COMSTAT	CommStatus;
LONG		i;
LONG		time1;
LONG		time2;
LONG		x;
	if(GetCommError(nCid,&CommStatus)!=0)
		return(FALSE);
	do
	{
		GetCommError(nCid,&CommStatus);
	}while(CommStatus.cbOutQue!=0);
	x = 1;
	time1 = GetTickCount();
	do
	{
		time2 = GetTickCount();
	}while((time2-time1)<x);
	if(WriteComm(nCid,befehlsstring,min(lstrlen(befehlsstring),
			128-CommStatus.cbOutQue))<0)
	{
		MessageBox(GetFocus(),(LPSTR)"Fehler beim Schreiben auf COM1",
			(LPSTR)"WriteComm",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return(FALSE);
	}
	return(TRUE);
}//Sende


/****************************************************************************
 Funktion:    	HoleRoboterPosition

 Aufgabe:       Holt die aktuellen Roboterkoordinaten und gibt diese in where
								und grid zurÅck

 Parameter:     where : Zeiger auf String der genug Platz zur VerfÅgung
												stellt
								grid  : dito

 RÅckgabewert:  -
****************************************************************************/

void HoleRoboterPosition(LPSTR where,LPSTR grid)
{
int		lenwhere=0;
int   gelesen=0;
char	string[50];

	Sende(nCid,"WH\n");
	where[0]='\0';
	do
	{
		gelesen = ReadComm(nCid,(LPSTR)string,50);
		string[gelesen]='\0';
		lstrcat((LPSTR)where,(LPSTR)string);
		lenwhere+=gelesen;
	}while(string[gelesen-1]!='\n');
	where[lenwhere-4]='\0';
	where[lenwhere-2]='\0';
	grid[0]='G';
	lstrcpy((LPSTR)&grid[1],(LPSTR)&where[lenwhere-3]);
}//HoleRoboterPosition

/****************************************************************************
 Funktion:  		LadeGrundposition

 Aufgabe:       LÑd die Systemposition die in NEUROROB.POS stehen und sendet
								sie zum Movemaster

 Parameter:     -

 RÅckgabewert:	BOOL					TRUE wenn erfolgreich
****************************************************************************/

BOOL LadeGrundposition()
{
FILE *FilePointer;
char Befehl[50];
char Befehl1[50];
char FileName[100]="";

	FilePointer=fopen("NEUROROB.POS","rt");
	if(!FilePointer)
	{
		MessageBox(GetFocus(),"Datei konnte nicht geoeffnet werden",
			"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return (FALSE);
	}
	ndummy=0;
	do
	{
		if(fgets(dummy,90,FilePointer)==NULL)
			break;
		dummy[strlen(dummy)-1]='\0';
		if(fgets(dummy2,90,FilePointer)==NULL)
			break;
		dummy2[strlen(dummy2)-1]='\0';
		wsprintf((LPSTR)Befehl1,(LPSTR)"%s,%s",(LPSTR)dummy,(LPSTR)dummy2);
		wsprintf((LPSTR)Befehl,(LPSTR)"PD %s",(LPSTR)Befehl1);
		lstrcat((LPSTR)Befehl,(LPSTR)"\n");
		SendeComm1(nCid,Befehl);
	}while(TRUE);
	fclose(FilePointer);
	return TRUE;
}//LadeGrundPosition

/****************************************************************************
 Funktion:  		Gelenkdemo

 Aufgabe:       FÅhrt eine Gelenkarmdemo fÅr den Roboter durch

 Parameter:

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

void Gelenkdemo(void)
{
float rad,droll,x1,y1;
int  dx,dy,a,i,k,r,z,x,y,we;
char Demo[50];

	SP ("10");
	PD ("600,0,400,200,-90,90,C");
	PD ("601,399.5,-654.7,393.4,-0.6,90,C");
	PD ("602,456.4,315.8,458.9,35.6,90,C");
	MO ("600,C");
	SP ("20");
	DS ("0,0,-100");
	GO ;
	GC ;
	MJ ("0,0,0,90,0");
	DS ("0,0,-100");
	HE ("699");
	MT ("699,-50,C");
	HE ("699");
	MT ("699,100,C");
	MO ("602,C");
	MT ("602,200,C");
	HE ("699");
	MT ("699,-200,C");
	MJ ("-100,0,0,0,0");
	HE ("699");
	MT ("699,200,C");
	HE ("699");
	MT ("699,-200,C");
	MO ("601,C");
	MJ ("-280,0,0,0,0");
	MJ ("0,-25,0,0,0");
	MJ ("280,60,0,0,0");
	MJ ("0,-35,0,0,0");
	MJ ("0,100,0,100,0");
	MJ ("-280,0,0,0,0");
	MO ("601,C");
	MJ ("-150,90,0,-90,0");
	MJ ("0,0,-90,180,90");
	MJ ("0,0,90,-180,-90");
	MJ ("0,-90,-90,0,0");
	MJ ("150,0,90,90,0");
	MO ("600,C");

	MessageBox(GetFocus(),(LPSTR)"Kreis",(LPSTR)"DEMO",MB_OK|MB_SYSTEMMODAL);
	PD ("10,-7,450,35,-90,90,C");
	MO ("10,C");
	a=10;
	dx=-7;
	dy=340;
	r=300-dy;
	for(i=1;i<=30;i++)
	{
		a++;
		rad=3*i*3.14159/45;
		y1=(cos(rad)*r+dy);
		x1=(sin(rad)*r+dx);
		droll=180*atan(x/y)/3.14159;
		we=(int)(90+droll);
		y=(int)y1;
		x=(int)x1;

		wsprintf(Demo,"%d, %d, %d,35,-90,90,C",a,x,y);
		PD (Demo);
	}
	MC ("10,40,C");
}//GelenkDemo








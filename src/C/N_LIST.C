/****************************************************************************
 Modul:     	N_LIST.C

 Aufgabe:     Bearbeitet den Hauptmenuepunkt "Befehlsliste"

 Funktionen:  Befehlsliste
							Befehlseingabe
****************************************************************************/

#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "ndl_list.h"
#include "ndl_visu.h"
#include "neuromen.h"
#include "ndl_edit.h"
#include <stdio.h>

#define	 ID7_ADD 799  					//Sendet die Befehlseingabe

BOOL FAR PASCAL Befehlseingabe(	HWND,unsigned,WORD,LONG);

BOOL 		bBefehlseingabe = FALSE;
BOOL		bListeSpeichern = FALSE;
FARPROC	lpBefehlseingabe;
HWND		hDlgBefehlseingabe;


/****************************************************************************
 Funktion:  		Befehlsliste

 Aufgabe:       Darstellen und bearbeiten der Befehlsliste-Dialogbox

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	msg		 : Typ der Nachricht
	WORD			wParam : Nachrichtenabh„ngiger Wert
	LONG			lParam :        "               "

 Rckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Befehlsliste(HWND hDlg,unsigned msg,WORD wParam,LONG lParam)
{
FILE		*FilePointer;
FARPROC	lpProcBefehlseingabe;
char		robpos[40];
char		befehl[40];
char		grid[6];
char    Listenbefehl[60];
int			buffer[1001];
int		  nAbbruch;
DWORD		select;
DWORD		ldummy;

	switch(msg){
		case WM_INITDIALOG:
				 if(bRepeatmodus)
					 CheckDlgButton(hDlg,ID3_REPEAT,(WORD) 1);
				 if(bEinzelschrittmodus)
					 CheckDlgButton(hDlg,ID3_SINGLESTEP,(WORD) 1);
				 bBefehlseingabe = FALSE;
				 bListeSpeichern = FALSE;
				 return(TRUE);
		case WM_COMMAND:
				 switch (wParam){
					 case IDOK:
								PostMessage(hDlg,WM_SYSCOMMAND,SC_CLOSE,0L);
								break;
					 case ID3_ADD:
								HoleRoboterPosition((LPSTR)robpos,(LPSTR)grid);
								befehl[0]='\0';
								lstrcat((LPSTR)befehl,(LPSTR)"MP ");
								lstrcat((LPSTR)befehl,(LPSTR)robpos);
								ndummy=SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_ADDSTRING,
									0,(DWORD)(LPSTR)befehl);
								if((ndummy==LB_ERR)||(ndummy==LB_ERRSPACE))
									MessageBox(GetFocus(),(LPSTR)"Fehler beim Hinzufuegen",
										(LPSTR)"Liste",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								ndummy=SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_ADDSTRING,
									0,(DWORD)(LPSTR)grid);
								if((ndummy==LB_ERR)||(ndummy==LB_ERRSPACE))
									MessageBox(GetFocus(),(LPSTR)"Fehler beim Hinzufuegen",
										(LPSTR)"Liste",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								bListeSpeichern = TRUE;
								break;
					 case ID7_ADD:		//Kommt vom editfenster
								ndummy=SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_ADDSTRING,
									0,(DWORD)(LPSTR)lParam);
								bListeSpeichern = TRUE;
								break;
					 case ID3_COMMAND:
								if(!bBefehlseingabe)
								{
									lpBefehlseingabe = MakeProcInstance(Befehlseingabe,hInst);
									hDlgBefehlseingabe = CreateDialog(hInst,"BEFEHLSEINGABE",
										hDlg,lpBefehlseingabe);
									CheckDlgButton(hDlg,ID3_COMMAND,(WORD) 1);
									bBefehlseingabe = TRUE;
								}
								else
								{
									DestroyWindow(hDlgBefehlseingabe);
									FreeProcInstance(lpBefehlseingabe);
									bBefehlseingabe = FALSE;
									CheckDlgButton(hDlg,ID3_COMMAND,(WORD) 0);
									bRepeatmodus = TRUE;
									hDlgBefehlseingabe = NULL;
								}
								break;
					 case ID3_DEL:
								if(MessageBox(GetFocus(),(LPSTR)"Wirklich loeschen",
									(LPSTR)"",MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL)==IDYES)
								{
									while((select = SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,
														LB_GETSELITEMS,100,(DWORD)buffer))!=0)
									{
											if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,
												LB_DELETESTRING,(WORD)buffer[0],0L)==LB_ERR)
														MessageBox(GetFocus(),(LPSTR)"Fehler beim L”schen",
															(LPSTR)"Liste",
															MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									}
								}
								bListeSpeichern = TRUE;
								break;
					 case ID3_SELECTALL:
								SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_SETSEL,1,-1L);
								break;
					 case ID3_UNSELECTALL:
								SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_SETSEL,0,-1L);
								break;
					 case ID3_LOAD:
								if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_GETCOUNT,0,0L)
									&&bListeSpeichern)
									SendMessage(hDlg,WM_COMMAND,ID3_SAVE,0L);
								strcpy(DefExt,".TXT");
								szFileCaption = (LPSTR)"Befehlsliste laden";
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hDlg,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden) ;
								FilePointer=fopen(FileName,"rt");
								if(!FilePointer)
								{
									MessageBox(GetFocus(),"Datei konnte nicht geoeffnet werden",
										"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									return (FALSE);
								}
								ndummy=0;
								do
								{
									if (fgets(dummy, 90 , FilePointer) == NULL)
										break;
									dummy[strlen(dummy)-1] = '\0';
									++ndummy;
									ndummy=SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,
										LB_ADDSTRING,0,(DWORD)(LPSTR)dummy);
									if((ndummy==LB_ERR)||(ndummy==LB_ERRSPACE))
										MessageBox(GetFocus(),(LPSTR)
											"Fehler beim Laden einer Liste",
											(LPSTR)"Liste",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								} while (TRUE);
								fclose(FilePointer);
								bListeSpeichern = FALSE;
								break;
					 case ID3_SAVE:
								SendMessage(hDlg,WM_COMMAND,ID3_SELECTALL,0L);
								if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_GETCOUNT,0,0L)
									==0)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Befehlsliste leer",
										(LPSTR)"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									break;
								}
								strcpy(DefExt,".TXT");
								szFileCaption = (LPSTR)"Befehlsliste speichern";
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hDlg,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden) ;
								FilePointer=fopen(FileName,"wt");
								if(!FilePointer)
								{
									MessageBox(GetFocus(),"Datei konnte nicht geoeffnet werden",
										"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									return (FALSE);
								}
								select = SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,
													LB_GETSELITEMS,1000,(DWORD)buffer);
								if(select>0L)
								{

									for(ldummy=0;ldummy<select;++ldummy)
									{
										if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_GETTEXT,
											(WORD)buffer[(int)ldummy],(DWORD)dummy)==LB_ERR)
												MessageBox(GetFocus(),(LPSTR)"Fehler beim Textholen",
													(LPSTR)"Liste",
													MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
										lstrcat((LPSTR)dummy,(LPSTR)"\n");
										fputs(dummy,FilePointer);
									}
								}
								fclose(FilePointer);
								bListeSpeichern = FALSE;
								break;
					 case ID3_DO:
									select = SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,
														LB_GETSELITEMS,1001,(DWORD)buffer);
									if(select == 1001)
									{
										MessageBox(hDlg,(LPSTR)"Max. 1000 Befehle",NULL,
											MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
										break;
									}
									if(select ==  0)
										MessageBox(GetFocus(),
											(LPSTR)"Es wurden noch keine Befehle markiert",
											(LPSTR)"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									if(select>0L)
									{

										for(ldummy=0;ldummy<select;++ldummy)
										{
											if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_GETTEXT,
												(WORD)buffer[(int)ldummy],(DWORD)Listenbefehl)==LB_ERR)
												MessageBox(GetFocus(),(LPSTR)"Fehler beim Textholen",
												(LPSTR)"Liste",
													MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
											if(bEinzelschrittmodus)
											{
												nAbbruch=MessageBox(GetFocus(),(LPSTR)Listenbefehl,
													(LPSTR)"Sende Befehl",MB_YESNOCANCEL|
													MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
												if(nAbbruch!=IDYES)
													break;
											}
											lstrcat((LPSTR)Listenbefehl,(LPSTR)"\n");
											SendeComm1(nCid,Listenbefehl);
											if(GetAsyncKeyState(VK_RBUTTON)==0x8000)
												break;
											SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_SETTOPINDEX,
												(WORD)buffer[(int)ldummy],0L);
											if(bVisualisierung)
												SendMessage(hDlgVisualisierung,WM_COMMAND,
													ID10_AKTUELL,0L);
										}
									}
								if((nAbbruch!=IDCANCEL)&&bRepeatmodus&&
									(GetAsyncKeyState(VK_RBUTTON)!=0x8000))
										PostMessage(hDlg,WM_COMMAND,ID3_DO,0L);
								break;
					 case ID3_REPEAT:
								if(bRepeatmodus == FALSE)
								{
									CheckDlgButton(hDlg,ID3_REPEAT,(WORD) 1);
									bRepeatmodus = TRUE;
								}
								else
								{
									CheckDlgButton(hDlg,ID3_REPEAT,(WORD) 0);
									bRepeatmodus = FALSE;
								}
								break;
					 case ID3_SINGLESTEP:
								if(bEinzelschrittmodus == FALSE)
								{
									CheckDlgButton(hDlg,ID3_SINGLESTEP,(WORD) 1);
									bEinzelschrittmodus = TRUE;
								}
								else
								{
									CheckDlgButton(hDlg,ID3_SINGLESTEP,(WORD) 0);
									bEinzelschrittmodus = FALSE;
								}
								break;
					 default:
								return FALSE;
				 }
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								if(SendDlgItemMessage(hDlg,ID3_BEFEHLSLISTE,LB_GETCOUNT,0,0L)
									&&bListeSpeichern)
									SendMessage(hDlg,WM_COMMAND,ID3_SAVE,0L);
								DestroyWindow(hDlgBefehlsliste);
								FreeProcInstance(lpBefehlsliste);
								bBefehlsliste = FALSE;
								hDlgBefehlsliste = NULL;
								CheckMenuItem(hMainMenu,IDM_BEFEHLSLISTE,MF_UNCHECKED);
								return TRUE;
					 default:
								return FALSE;
				 }
				 break;
		default:
				 return FALSE;
	}
	return FALSE;
}//BefehlsListe

/****************************************************************************
 Funktion:  		Befehlseingabe

 Aufgabe:       Befehlseingabedialogbox darstellen

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : Nachrichtenabh„ngiger Wert
	LONG			lParam :        "               "

 Rckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Befehlseingabe(	HWND			hDlg,
																unsigned	message,
																WORD			wParam,
																LONG			lParam)
{
	switch (message){
		case WM_INITDIALOG:
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDOK:
								SendDlgItemMessage(hDlg,ID7_EDIT,WM_GETTEXT,40,(DWORD)dummy);
								SendMessage(hDlgBefehlsliste,WM_COMMAND,ID7_ADD,
									(DWORD)(LPSTR)dummy);
								SendDlgItemMessage(hDlg,ID7_EDIT,WM_SETTEXT,0,(LPSTR)"");
								SetFocus(GetDlgItem(hDlg,ID7_EDIT));
								return(TRUE);
					 case IDCANCEL:
								SendMessage(hDlgBefehlseingabe,WM_SYSCOMMAND,SC_CLOSE,0L);
								return (TRUE);
				 }
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								DestroyWindow(hDlgBefehlseingabe);
								FreeProcInstance(lpBefehlseingabe);
								bBefehlseingabe = FALSE;
								hDlgBefehlseingabe = NULL;
								CheckDlgButton(hDlgBefehlsliste,ID3_COMMAND,(WORD) 0);
								return TRUE;
					 default:
								return FALSE;
				 }
				 break;
		default:
				 return FALSE;
	}
	return FALSE;
}//BefehlsEingabe


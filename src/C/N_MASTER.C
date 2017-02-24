/****************************************************************************
 Modul:   		N_MASTER.C

 Aufgabe:    	Macht einige Roboterbefehle fÅr 'C' verfÅgbar
							Genaue ErlÑuterung der Aufgaben im Handbuch des Roboters

 Funktionen:  MO
							DS
							DW
							GC
							GP
							GO
							HE
							MA
							MC
							MJ
							MP
							MS
							MP
							NT
							NW
							PD
							RS
							SD
							SP

****************************************************************************/


#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"


void MO (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MO %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MO

void DS (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"DS %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//DS

void DW (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"DW %s\n",Param);
	SendeComm1(nCid,Befehl);
}//DW

void GC (void)    // Schlie·t die Hand
{
	SendeComm1(nCid,"GC\n");
}//GC

void GP (LPSTR Param)    // Kraft der Hand
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"GP %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//GP

void GO (void)    // ôffnet die Hand
{
	SendeComm1(nCid,"GO\n");
}//GO

void HE (LPSTR Param)    // Position wird gespeichert
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"HE %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//HE

void MA (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MA %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MA

void MC (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MC %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MC

void MJ (LPSTR Param)    // WinkelÑnderung
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MJ %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MJ

void MP (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MP %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MP

void MS (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MS %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MS

void MT (LPSTR Param)    // Position wird angefahren
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"MT %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//MT

void NT (void)    // Nestposition
{
	SendeComm1(nCid,"NT\n");
}//NT

void NW (void)    // NEW
{
	SendeComm1(nCid,"NW\n");
}//NW

void PD (LPSTR Param)    // Position wird gespeichert
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"PD %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//PD

void RS (LPSTR Param)    // RESET
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"RS %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//RS

void SD (LPSTR Param)    // Speed TOOL_CENTER POINT
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"SD %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//SD

void SP (LPSTR Param)    // SPEED Roboter
{
char Befehl[50];

	wsprintf((LPSTR)Befehl,(LPSTR)"SP %s\n",(LPSTR)Param);
	SendeComm1(nCid,Befehl);
}//SP


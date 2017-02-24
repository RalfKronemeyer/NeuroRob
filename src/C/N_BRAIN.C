/****************************************************************************
 Modul:   		N_BRAIN.C

 Aufgabe:    	Dieses Modul Åbernimmt die Bilderkennung mittels eines
							neuronalen Netzes

 Funktionen:  VerknuepfeNetz
							SetzeEingangsVektor
							SetzeAusgangsVektor
							Sigmoid
							SigmoidStrich
							BerechneAusgang
							HoleEingangsVektor
							SetzeEingangsTrainingsVektor
							ErstelleMinMaxVektoren
							MaxOfTraining
							MinOfTraining
							TransformiereEingangsVektor
							NeuronenMaximum
							Backpropagation
							Brain
							ZeichneBrain
							LadeNeuronalesNetz
							SpeicherNeuronalesNetz
							NeuesNetz
							NeuronenDaten
							TeileDaten

****************************************************************************/


#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "neurorob.h"
#include "neuromen.h"
#include "n_extern.h"
#include "brainmen.h"
#include "ndl_nnet.h"
#include "ndl_ndat.h"
#include "ndl_tdat.h"
#include "ndl_test.h"
#include "math.h"


#define ERKENNUNGSSCHWELLE 0.98
#define NETZ_HOEHE 32
#define NETZ_BREITE 4
#define ZUFALL ((double)(rand())/60000.0)
#define ROBOTERRANDOM (((double)rand()/32767.0)*2.0-1.0)
#define ROBOTERRANDOMP ((int)(((double)rand())/((double)RAND_MAX)*355))
#define TETACONST (1.0)
#define ABSTANDGROSS 40
#define ABSTANDMITTEL 21
#define ABSTANDKLEIN 15
#define GROESSEGROSS 20
#define GROESSEMITTEL 12
#define GROESSEKLEIN 8
#define HIGH 60
#define MIDDLE 500
#define LOW 3000
#define HIGHDURCHLAEUFE 1000
#define MIDDLEDURCHLAEUFE 300
#define LOWDURCHLAEUFE 1
#define XORGOFFSET (-280)
#define YORGOFFSET (-40)
#define HIDDENMAX NETZ_HOEHE
#define HIDDENPAGE 5
#define TESTMAX 1000
#define TESTPAGE 100
#define MERKMALEMAX NETZ_HOEHE
#define TEILEMAX 16
#define LAYERMAX (NETZ_BREITE-2)
#define	SAMPLEMAX 8
#define SAMPLEANZAHL (SAMPLEMAX*TEILEMAX) //muss ein Divisor von 65536 sein !
#define TEILE 0
#define LAYER 1
#define SAMPLE 2
#define KEIN 0
#define TRAINING 1
#define KOMPLETT 2


typedef	struct eingaenge
				{
					double	huge *x;
					double	w;
				}EINGAENGE;

typedef struct neuron
				{
					EINGAENGE	huge xin[NETZ_HOEHE];
					double		teta;
					double		p;
					double		ausgang;
					double		delta;
					int				xpos1;
					int				ypos1;
					int				xpos2;
					int				ypos2;
				}NEURON;

int						Netzform[NETZ_BREITE]={0,3,3,3};

NEURON	huge 	Netz[NETZ_HOEHE][NETZ_BREITE];

double	huge 	EingangsTrainingsVektor[NETZ_HOEHE][SAMPLEMAX*TEILEMAX];
double	huge 	AusgangsTrainingsVektor[NETZ_HOEHE][SAMPLEMAX*TEILEMAX];

double	huge 	EingangsVektor[NETZ_HOEHE];
double	huge 	AusgangsVektor[NETZ_HOEHE];

double	huge 	Maximum[NETZ_HOEHE];
double	huge 	Minimum[NETZ_HOEHE];

HPEN					hBluePen;
HPEN					hBlackPen;
HPEN					hGreenPen;

HBRUSH				hOldBrush;
HBRUSH				hRedBrush[256];
HBRUSH				hGreenBrush;
HBRUSH				hBlueBrush;

RECT					rClientRect;

int 					scrollhmax = 700;
int						scrollvmax = 1700;
int						hpage			 = 100;
int						vpage			 = 100;
int						balkenmerker;
int						nDurchlaeufe			= MIDDLEDURCHLAEUFE;
int						nPrio							= MIDDLE;
int						nHiddenlayer 			= 1;
int						nTeile			 			= 2;
int						nSampleProTeil		= 1;
int						nNeuronenAbstand 	= ABSTANDMITTEL;
int						nNeuronenGroesse 	= GROESSEMITTEL;
int						nXoffset	 				= 20;
int						nLayerMax				 	= 3;
int						nAktuelli;               //
int						nAktuellj;               // Selektiertes Neuron
int						nHorzAuswahl;
int						nHorzmax[3]				={TEILEMAX,LAYERMAX,SAMPLEMAX};
int						nHorzPage[3]			={3,1,3};
int 					nRoboterEinsatz		= KEIN;
int						nTimerz           = 0;

static int		nSelect;


long					lPropagations			= 0L;


BOOL					bMerkmal[11];
BOOL					bStart 						= FALSE;
BOOL					bNurEllipsen 			= FALSE;
BOOL					bZeigeTraining		= FALSE;
BOOL					bBrainIcon				= TRUE;
BOOL					bRoboterEinsatz		= FALSE;
BOOL					bErkennung				= FALSE;
BOOL					bTestphase				= FALSE;

FARPROC				lpTeileDaten;

char					TeileGroesse[TEILEMAX+1][12];
char					TeileFach[TEILEMAX+1][12];
char 					TeileHoehe[TEILEMAX+1][12];
char					TeileName[TEILEMAX+1][21]={	"Teil 1",
																					"Teil 2",
																					"Teil 3",
																					"Teil 4",
																					"Teil 5",
																					"Teil 6",
																					"Teil 7",
																					"Teil 8",
																					"Teil 9",
																					"Teil 10",
																					"Teil 11",
																					"Teil 12",
																					"Teil 13",
																					"Teil 14",
																					"Teil 15",
																					"Teil 16"};

char				MerkmalName[MERKMALEMAX+1][50];
char 				NetzName[128];

char				MajorAng[20];


/****************************************************************************
 Funktion: 			VerknuepfeNetz

 Aufgabe:				Diese Funktion verbindet die einzelnen Neuronen zu einem Netz

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/


void	VerknuepfeNetz(void)
{
int	nHoehe;
int	nBreite;
int	nNeuroneingaenge=0;

	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
	{
		(Netz[nHoehe][0].xin[nNeuroneingaenge]).x =
			(double huge *)&EingangsVektor[nHoehe];
		(Netz[nHoehe][0].xin[nNeuroneingaenge]).w = ZUFALL;
		Netz[nHoehe][0].teta = TETACONST;
	}
	for(nBreite=1;nBreite<nHiddenlayer+2;++nBreite)
		for(nHoehe=0;nHoehe<Netzform[nBreite];++nHoehe)
			for(nNeuroneingaenge=0;nNeuroneingaenge<Netzform[nBreite-1];
						++nNeuroneingaenge)
			{
				(Netz[nHoehe][nBreite].xin[nNeuroneingaenge]).x =
					(double huge *)&(Netz[nNeuroneingaenge][nBreite-1].ausgang);
				(Netz[nHoehe][nBreite].xin[nNeuroneingaenge]).w = ZUFALL;
				Netz[nHoehe][nBreite].teta = TETACONST;
			}
}//VerknuepfeNetz

/****************************************************************************
 Funktion:      SetzeEingangsVektor

 Aufgabe:       Setzt den EingangsVektor einem EingangsTrainingsVektor gleich

 Parameter:			sample: bestimmmt den zu setzenden EingangsTrainingsVektor

 RÅckgabewert:  -

****************************************************************************/

void SetzeEingangsVektor(int sample)
{
int	z;
	for(z=0;z<Netzform[0];++z)
		EingangsVektor[z]=EingangsTrainingsVektor[z][sample];
	TransformiereEingangsVektor();
}//SetzeEingangsVektor

/****************************************************************************
 Funktion:      SetzeAusgangsVektor

 Aufgabe:       Setzt den AusgangsVektor einem AusgangsTrainingsVektor gleich

 Parameter:     sample: bestimmmt den zu setzenden AusgangsTrainingsVektor

 RÅckgabewert:  -

****************************************************************************/

void SetzeAusgangsVektor(int sample)
{
int	z;
	for(z=0;z<Netzform[nHiddenlayer+1];++z)
		AusgangsVektor[z]=AusgangsTrainingsVektor[z][sample];
}//SetzeAusgangsVektor

/****************************************************************************
 Funktion:      Sigmoid

 Aufgabe:       Berechnet die Sigmoide-Funktion

 Parameter:			p: Argument fÅr die Funktion

 RÅckgabewert:  double: Funktionswert der Sigmoiden

****************************************************************************/

double	Sigmoid(double p)
{
	return(1.0/(1.0+exp(-p)));
}//Sigmoid

/****************************************************************************
 Funktion:   		SigmoidStrich

 Aufgabe:     	Berechnet die Abgleitete Funktion der Sigmoiden

 Parameter:     p: Argument fÅr die Funktion

 RÅckgabewert:  double: Funktionswert der Abgeleiteten Funktion

****************************************************************************/

double 	SigmoidStrich(double p)
{
static	double	dDummy;

	dDummy = Sigmoid(p);
	dDummy *= dDummy;
	return(exp(-p)*dDummy);
}//SigmoidStrich

/****************************************************************************
 Funktion:      BerechneAusgang

 Aufgabe:       Berechnet den Ausgangslayer von dem im Moment angelegten
								EingangsVektors des neuronalen Netzes

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/

void BerechneAusgang(void)
{
double	p;
int			nHoehe;
int			nBreite;
int			nNeuroneingaenge;

	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
		Netz[nHoehe][0].ausgang = EingangsVektor[nHoehe];
	for(nBreite=1;nBreite<nHiddenlayer+2;++nBreite)
		for(nHoehe=0;nHoehe<Netzform[nBreite];++nHoehe)
		{
			p=0.0;
			for(nNeuroneingaenge=0;nNeuroneingaenge<Netzform[nBreite-1];
					++nNeuroneingaenge)
				p += *(Netz[nHoehe][nBreite].xin[nNeuroneingaenge].x) *
						 Netz[nHoehe][nBreite].xin[nNeuroneingaenge].w;
			p-=Netz[nHoehe][nBreite].teta;
			Netz[nHoehe][nBreite].p = p;
			Netz[nHoehe][nBreite].ausgang = Sigmoid(p);
		}
}//BerechneAusgang

/****************************************************************************
 Funktion:      HoleEingangsVektor

 Aufgabe:     	Holt die Merkmale von der Sivips-Anlage und selektiert
								die fÅr die Erkennung relevanten Merkmale

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/

void HoleEingangsVektor(void)
{
int			Grauwerte[64];
char 		Grauwert[15];
char  	Merkmal[20];
char  	copy[40];
int 		i,j,k,l;
double 	x;

	k=0;
	for(l=0;l<10;++l)
	{
		SendeComm2(nCid2,"A");
		HoleVideomatZeile((LPSTR)Merkmal);
		if(bMerkmal[l]==TRUE)
		{
				x=atof(Merkmal);
				EingangsVektor[k]=x;
				++k;
		}
	}
	x=0;
	for(i=0;i<16;++i)
	{
		x=0;
		for(j=0;j<4;++j)
		{
			SendeComm2(nCid2,"A");
			HoleVideomatZeile((LPSTR)Grauwert);
			x+=atof(Grauwert);
		}
		EingangsVektor[i+k]=x;
	}
	SendeComm2(nCid2,"A");
	HoleVideomatZeile((LPSTR)copy);
	i=0;
	while(copy[i]==' ')
		++i;
	lstrcpy((LPSTR)MajorAng,(LPSTR)&(copy[i]));

}//HoleEingangsVektor

/****************************************************************************
 Funktion:      SetzeEingangsTrainingsVektor

 Aufgabe:       Speichert den aktuellen EingangsVektor im EingangsTrainings-
								Vektor

 Parameter:     sample: bestimmt in welchem EingangsTrainingsVektor der
												EingangsVektor gespeichert werden soll

 RÅckgabewert:	-

****************************************************************************/

void SetzeEingangsTrainingsVektor(int sample)
{
int	z;
	for(z=0;z<Netzform[0];++z)
		EingangsTrainingsVektor[z][sample]=EingangsVektor[z];
}//SetzeEingangsTrainingsVektor

/****************************************************************************
 Funktion:      ErstelleMinMaxVektoren

 Aufgabe:       Erstellt die zur Transformation notwendigen Umrechnungs-
								vektoren

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/

void ErstelleMinMaxVektoren(void)
{
int			z;
	for(z=0;z<Netzform[0];++z)
	{
		Maximum[z] = MaxOfTraining(z);
		Minimum[z] = MinOfTraining(z,Maximum[z]);
	}
}//ErstelleMinMaxVektoren

/****************************************************************************
 Funktion:      MaxOfTraining

 Aufgabe:       Bestimmt den Maximalwert eines Eingangs in den Trainings-
								vektoren

 Parameter:     nEingang: Bestimmt den zu untersuchenden Eingang

 RÅckgabewert:  -

****************************************************************************/

double MaxOfTraining(int nEingang)
{
double 	dMaximum=-1E300;
int     z;
	for(z=0;z<nTeile*nSampleProTeil;++z)
		dMaximum = max(dMaximum,EingangsTrainingsVektor[nEingang][z]);
	return(dMaximum);
}//MaxOfTraining

/****************************************************************************
 Funktion:      MinOfTraining

 Aufgabe:       Bestimmt den Minimalwert eines Eingangs in den Trainings-
								vektoren

 Parameter:     nEingang: Bestimmt den zu untersuchenden Eingang

 RÅckgabewert:  -

****************************************************************************/

double MinOfTraining(int nEingang,double dMinimum)
{
int			z;
	for(z=0;z<nTeile*nSampleProTeil;++z)
		dMinimum = min(dMinimum,EingangsTrainingsVektor[nEingang][z]);
	return(dMinimum);
}//MinOfTraining

/****************************************************************************
 Funktion:      TransformiereEingangsVektor

 Aufgabe:       Transformiert den EingangsVektor mit Hilfe der MinMaxVektoren
								auf einen Werteberech zwischen 1/11 und 1

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/

void TransformiereEingangsVektor(void)
{
int z;
	for(z=0;z<Netzform[0];++z)
	{
		if((Maximum[z]-Minimum[z])>0.0)
		{
			if(EingangsVektor[z]>Maximum[z])
				EingangsVektor[z]=1.0;
			else if (EingangsVektor[z]<Minimum[z])
				EingangsVektor[z]=0.1;
			else
				EingangsVektor[z]=(((EingangsVektor[z]-Minimum[z])/
													(Maximum[z]-Minimum[z]))+0.1)/1.1;
		}
		else
			EingangsVektor[z]=0.1;
	}
}//TransformiereEingangsVektor

/****************************************************************************
 Funktion:      NeuronenMaximum

 Aufgabe:       Bestimmt das Neuron im Ausgangslayer welches das hîchste
								Potential hat

 Parameter:     -

 RÅckgabewert:  int: Bestimmt die Nummer des Neuron

****************************************************************************/

int NeuronenMaximum(void)
{
int 		i;
int  		nAusgangsNeuron;
double 	Merker;

	nAusgangsNeuron = -1;
	Merker=0.0;
	for(i=0;i<Netzform[nHiddenlayer+1];++i)
	{
		if(Netz[i][nHiddenlayer+1].ausgang>Merker)
		{
			Merker = Netz[i][nHiddenlayer+1].ausgang;
			nAusgangsNeuron=i;
		}
	}
	return(nAusgangsNeuron);
}//NeuronenMaximum

/****************************************************************************
 Funktion:      Backpropagation

 Aufgabe:       Berechnet die Gewichtsanpassung der Neuronenverbindungen

 Parameter:     -

 RÅckgabewert:  -

****************************************************************************/

void Backpropagation(void)
{
int			i,j,k;
double	delta;

	for(i=0;i<Netzform[nHiddenlayer+1];++i)
		Netz[i][nHiddenlayer+1].delta =
			(Netz[i][nHiddenlayer+1].ausgang-AusgangsVektor[i])*
			SigmoidStrich(Netz[i][nHiddenlayer+1].p);
	for(i=nHiddenlayer;i>0;--i)
		for(j=0;j<Netzform[i];++j)
		{
			delta = 0.0;
			for(k=0;k<Netzform[i+1];++k)
				delta += Netz[k][i+1].delta * Netz[k][i+1].xin[j].w;
			delta *= SigmoidStrich(Netz[j][i].p);
			Netz[j][i].delta = delta;
		}
	for(i=1;i<nHiddenlayer+2;++i)
		for(j=0;j<Netzform[i];++j)
			for(k=0;k<Netzform[i-1];++k)
				Netz[j][i].xin[k].w -= Netz[j][i].delta * Netz[k][i-1].ausgang;
}//BackPropagation

/****************************************************************************
 Funktion:      Brain

 Aufgabe:       Callbackfunktion von Brain. Hier wird das Brain-Menu
								abgearbeitet

 Parameter:
	HWND			hWnd   : Fensterhandle
	unsigned 	msg		 : Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	long		wird von Windows verwaltet
****************************************************************************/


long FAR PASCAL Brain(HWND hWnd,unsigned msg,WORD wParam,LONG lParam)

{
FARPROC 		lpProcNeuesNetz;
FARPROC 		lpProcTestNetz;
FARPROC 		lpNeuronenDaten;
int					i,j,k,x,y,z,xDreh,yDreh,nDrehWinkel;
int					nBreite;
int					nHoehe;
int 				nNeuroneneingaenge;
int 				time1,time2;
HDC					hDC;
FILE				*FilePointer;
LONG				lTimer;
double 			alpha;

	switch(msg){
		case WM_COMMAND:
				 switch (wParam){
					 case IDM9_NEU:
								lpProcNeuesNetz = MakeProcInstance(NeuesNetz,hInst);
								DialogBox(hInst,"NEUESNETZ",hWnd,lpProcNeuesNetz);
								FreeProcInstance(lpProcNeuesNetz);
								bSpeichernAbfrage=TRUE;
								BringWindowToTop(hWnd);
								break;
					 case IDM9_TEST:
								if(bTestphase)
								{
									CheckMenuItem(hBrainMenu,IDM9_TEST,MF_UNCHECKED);
									bTestphase = FALSE;
								}
								else
								{
									CheckMenuItem(hBrainMenu,IDM9_TEST,MF_CHECKED);
									bTestphase = TRUE;
								}
								break;
					 case IDM9_RESET:
								if(MessageBox(GetFocus(),
									(LPSTR)"Soll das neuronale Netz\n wirklich alles vergessen",
									(LPSTR)"",MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL)==IDYES)
										VerknuepfeNetz();
								InvalidateRect(hWnd,NULL,TRUE);
								break;
					 case IDM9_ENDE:
								if(bStart)
									SendMessage(hWnd,WM_COMMAND,IDM9_START,0L);
								ShowWindow(hWndBrain,SW_HIDE);
								bBrain = FALSE;
								CheckMenuItem(hMainMenu,IDM_NEUINIT,MF_UNCHECKED);
								break;
					 case IDM9_START:
								if(bStart)
								{
									CheckMenuItem(hBrainMenu,IDM9_START,MF_UNCHECKED);
									bStart = FALSE;
									KillTimer(hWnd,1);
								}
								else
								{
									CheckMenuItem(hBrainMenu,IDM9_START,MF_CHECKED);
									bStart = TRUE;
									lPropagations=0L;
									SetTimer(hWnd,1,nPrio,NULL);
								}
								bSpeichernAbfrage=TRUE;
								break;
					 case IDM9_ANZEIGEN:
								if(bZeigeTraining)
								{
									CheckMenuItem(hBrainMenu,IDM9_ANZEIGEN,MF_UNCHECKED);
									bZeigeTraining = FALSE;
								}
								else
								{
									CheckMenuItem(hBrainMenu,IDM9_ANZEIGEN,MF_CHECKED);
									bZeigeTraining = TRUE;
								}
								break;
					 case IDM9_PRIOHIGH:
								nPrio = HIGH;
								nDurchlaeufe = HIGHDURCHLAEUFE;
								CheckMenuItem(hBrainMenu,IDM9_PRIOHIGH,MF_CHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOMIDDLE,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOLOW,MF_UNCHECKED);
								break;
					 case IDM9_PRIOMIDDLE:
								nPrio = MIDDLE;
								nDurchlaeufe = MIDDLEDURCHLAEUFE;
								CheckMenuItem(hBrainMenu,IDM9_PRIOHIGH,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOMIDDLE,MF_CHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOLOW,MF_UNCHECKED);
								break;
					 case IDM9_PRIOLOW:
								nPrio = LOW;
								nDurchlaeufe = LOWDURCHLAEUFE;
								CheckMenuItem(hBrainMenu,IDM9_PRIOHIGH,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOMIDDLE,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_PRIOLOW,MF_CHECKED);
								break;
					 case IDM9_GROSS:
								nNeuronenAbstand = ABSTANDGROSS;
								nNeuronenGroesse = GROESSEGROSS;
								CheckMenuItem(hBrainMenu,IDM9_GROSS,MF_CHECKED);
								CheckMenuItem(hBrainMenu,IDM9_MITTEL,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_KLEIN,MF_UNCHECKED);
								InvalidateRect(hWnd,NULL,TRUE);
								break;
					 case IDM9_MITTEL:
								nNeuronenAbstand = ABSTANDMITTEL;
								nNeuronenGroesse = GROESSEMITTEL;
								CheckMenuItem(hBrainMenu,IDM9_GROSS,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_MITTEL,MF_CHECKED);
								CheckMenuItem(hBrainMenu,IDM9_KLEIN,MF_UNCHECKED);
								InvalidateRect(hWnd,NULL,TRUE);
								break;
					 case IDM9_KLEIN:
								nNeuronenAbstand = ABSTANDKLEIN;
								nNeuronenGroesse = GROESSEKLEIN;
								CheckMenuItem(hBrainMenu,IDM9_GROSS,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_MITTEL,MF_UNCHECKED);
								CheckMenuItem(hBrainMenu,IDM9_KLEIN,MF_CHECKED);
								InvalidateRect(hWnd,NULL,TRUE);
								break;
					 case IDM9_ALS:
								szFileCaption = (LPSTR)"Neuronales Netz speichern";
								strcpy(DefExt,".NET");
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hWnd,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden);
								lstrcpy((LPSTR)NetzName,(LPSTR)FileName);
					 case IDM9_SPEICHERN:
								lstrcpy((LPSTR)FileName,(LPSTR)NetzName);
								if(SpeicherNeuronalesNetz())
								{
									bSpeichern = TRUE;
									bSpeichernAbfrage = FALSE;
								}
								break;
					 case IDM9_LADEN:
								if(LadeNeuronalesNetz())
								{
									lstrcpy((LPSTR)NetzName,(LPSTR)FileName);
									bNetzdefine = TRUE;
									bSpeichern=TRUE;
									SetzeEingangsVektor(0);
									BerechneAusgang();
									InvalidateRect(hWndBrain,NULL,TRUE);
									bSpeichernAbfrage=FALSE;
								}
								break;
					 case IDM9_ERKENNUNG:
								if(bErkennung)
								{
									CheckMenuItem(hBrainMenu,IDM9_ERKENNUNG,MF_UNCHECKED);
									bErkennung = FALSE;
									KillTimer(hWnd,1);
								}
								else
								{
									CheckMenuItem(hBrainMenu,IDM9_ERKENNUNG,MF_CHECKED);
									bErkennung = TRUE;
									SetTimer(hWnd,1,1,NULL);
								}
								break;
					 case IDM9_ROBOTEREINSATZ:
								if(bRoboterEinsatz)
								{
									CheckMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,MF_UNCHECKED);
									bRoboterEinsatz = FALSE;
								}
								else
								{
									CheckMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,MF_CHECKED);
									bRoboterEinsatz = TRUE;
								}
								break;
					 case IDM9_EDITTEILEDATEN:
								lpTeileDaten = MakeProcInstance(TeileDaten,hInst);
								DialogBox(hInst,"TEILEDATEN",hWnd,lpTeileDaten);
								FreeProcInstance(lpTeileDaten);
								bSpeichernAbfrage=TRUE;
								break;
				 }
				 break;
		case WM_TIMER:
				 KillTimer(hWnd,1);
				 if(bStart)
				 {
					 ++nSelect;
					 lTimer=GetTickCount();
					 do
					 {
						 SetzeEingangsVektor(nTimerz);
						 BerechneAusgang();
						 SetzeAusgangsVektor(nTimerz);
						 Backpropagation();
						 ++lPropagations;
						 ++nTimerz;
						 if(nTimerz==nSampleProTeil*nTeile)
							 nTimerz=0;
					 }while((GetTickCount()-lTimer)<nDurchlaeufe);
					 SetzeEingangsVektor(nSelect%(nTeile*nSampleProTeil));
					 BerechneAusgang();
					 if(bZeigeTraining && !IsIconic(hWnd))
					 {
						 InvalidateRect(hWnd,NULL,FALSE);
						 SendMessage(hWnd,WM_PAINT,0,0L);
					 }
					 SetTimer(hWnd,1,nPrio,NULL);
					 break;
				 }
				 if(bErkennung)
				 {
					 if(bRoboterEinsatz && nRoboterEinsatz==KOMPLETT)
					 {
						 SendeComm2(nCid2,"T");
						 HoleVideomatZeile((LPSTR)dummy2);
						 if(dummy2[0]=='N')
						 {
							 SetTimer(hWnd,1,1,NULL);
							 break;
						 }
					 }
					 HoleEingangsVektor();
					 TransformiereEingangsVektor();
					 BerechneAusgang();
					 InvalidateRect(hWndBrain,NULL,TRUE);
					 SendMessage(hWndBrain,WM_PAINT,0,0L);
					 if(bRoboterEinsatz && nRoboterEinsatz==KOMPLETT)
					 {
						 HolePartSchwerpunkt();
						 alpha=atof(MajorAng);
						 nDrehWinkel=(int)((-alpha)*180.0/M_PI+90);
						 while(nDrehWinkel>3)
							 nDrehWinkel-=180;
						 while(nDrehWinkel<-177)
							 nDrehWinkel+=180;
						 MO("996");
						 wsprintf((LPSTR)dummy,(LPSTR)"0,0,0,0,%i",nDrehWinkel);
						 MJ((LPSTR)dummy);
						 xDreh=(int)(xUrsprung-xPart);
						 yDreh=(int)(yUrsprung-yPart);
						 wsprintf((LPSTR)dummy,(LPSTR)"%i,%i,%i",xDreh,yDreh,
							 atoi(TeileHoehe[NeuronenMaximum()])+30);
						 AbsolutRelativ();
						 SendeComm1(nCid,dummy);
						 wsprintf((LPSTR)dummy,(LPSTR)"%i,%i,%i",xDreh,yDreh,
							 atoi(TeileHoehe[NeuronenMaximum()]));
						 AbsolutRelativ();
						 if(!bTestphase)
							 SendeComm1(nCid,dummy);
						 GC();
						 MO((LPSTR)"999,C");
						 wsprintf((LPSTR)dummy,(LPSTR)"%s,C",
							 (LPSTR)TeileFach[NeuronenMaximum()]);
						 MO((LPSTR)dummy);
						 GO();
					 }
					 SetTimer(hWnd,1,1,NULL);
				 }
				 break;
		case WM_CREATE:
				 hGehirnBitmap = LoadBitmap(hInst,"GEHIRNBILD");
				 SetScrollRange(hWnd,SB_VERT,0,scrollvmax,0);
				 SetScrollRange(hWnd,SB_HORZ,0,scrollhmax,0);
				 for(i=0;i<256;++i)
					 hRedBrush[i] = CreateSolidBrush(RGB(i,0,0));
				 hBluePen   = CreatePen(PS_SOLID,1,RGB(0,0,255));
				 hGreenPen  = CreatePen(PS_SOLID,1,RGB(0,255,0));
				 hGreenBrush= CreateSolidBrush(RGB(0,255,0));
				 hBlueBrush = CreateSolidBrush(RGB(0,0,255));
				 hBlackPen  = CreatePen(PS_SOLID,1,RGB(0,0,0));
				 for(ndummy=0;ndummy<TEILEMAX;++ndummy)
					 lstrcpy((LPSTR)TeileHoehe[ndummy],(LPSTR)"200");
				 break;
		case WM_SIZE:
				 GetClientRect(hWndBrain,(LPRECT)&rClientRect);
				 hpage = rClientRect.right;
				 vpage = rClientRect.bottom;
				 break;
		case WM_PAINT:
				 ZeichneBrain(hWnd);
				 break;
		case WM_LBUTTONDBLCLK:
				 if(!bStart)
				 {
					 x = LOWORD(lParam)+GetScrollPos(hWnd,SB_HORZ)+XORGOFFSET;
					 y = HIWORD(lParam)+GetScrollPos(hWnd,SB_VERT)+YORGOFFSET;
					 for(i=0;i<nHiddenlayer+2;++i)
						 for(j=0;j<Netzform[i];++j)
						 {
							 if(Netz[j][i].xpos1<x &&
									Netz[j][i].xpos2>x &&
									Netz[j][i].ypos1<y &&
									Netz[j][i].ypos2>y)
							 {
								 nAktuelli=i;
								 nAktuellj=j;
								 lpNeuronenDaten = MakeProcInstance(NeuronenDaten, hInst);
								 DialogBox(hInst,"NEURONENDATEN",hWnd,lpNeuronenDaten);
								 FreeProcInstance(lpNeuronenDaten);
							 }
						 }
					 }
					 else
					 {
						 MessageBox(GetFocus(),
							(LPSTR)"Waehrend des Trainings keine Neuronendaten",
							NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
					 }
					 break;
		case WM_HSCROLL:
				 balkenmerker = GetScrollPos(hWnd,SB_HORZ);
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<scrollhmax)
								++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>0)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<scrollhmax-hpage)
									balkenmerker+=hpage;
								else
									balkenmerker=scrollhmax;
								break;
					 case SB_PAGEUP:
								if(balkenmerker>hpage)
									balkenmerker-=hpage;
								else
									balkenmerker=0;
								break;
					 case SB_BOTTOM:
								balkenmerker=0;
								break;
					 case SB_TOP:
								balkenmerker=scrollhmax;
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(hWnd,SB_HORZ,balkenmerker,TRUE);
				 InvalidateRect(hWnd,NULL,TRUE);
				 break;
		case WM_VSCROLL:
				 balkenmerker = GetScrollPos(hWnd,SB_VERT);
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<scrollvmax)
								++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>0)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<scrollvmax-vpage)
									balkenmerker+=vpage;
								else
									balkenmerker=scrollvmax;
								break;
					 case SB_PAGEUP:
								if(balkenmerker>vpage)
									balkenmerker-=vpage;
								else
									balkenmerker=0;
								break;
					 case SB_BOTTOM:
								balkenmerker=0;
								break;
					 case SB_TOP:
								balkenmerker=scrollvmax;
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(hWnd,SB_VERT,balkenmerker,TRUE);
				 InvalidateRect(hWnd,NULL,TRUE);
				 break;
		case WM_INITMENU:
				 EnableMenuItem(hBrainMenu,IDM9_NEU,      			MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_SPEICHERN,			MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_ALS,						MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_LADEN,					MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_START,					MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_TEST,						MF_ENABLED);
				 EnableMenuItem(hBrainMenu,IDM9_RESET,					MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_ERKENNUNG,			MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,	MF_GRAYED);
				 EnableMenuItem(hBrainMenu,IDM9_EDITTEILEDATEN,	MF_GRAYED);
				 if(!bStart && !bErkennung)
				 {
					 EnableMenuItem(hBrainMenu,IDM9_EDITTEILEDATEN,MF_ENABLED);
					 EnableMenuItem(hBrainMenu,IDM9_NEU,  MF_ENABLED); //weg damit
					 EnableMenuItem(hBrainMenu,IDM9_LADEN,MF_ENABLED);
					 if(bNetzdefine)
					 {
						 EnableMenuItem(hBrainMenu,IDM9_ALS,MF_ENABLED);
						 if(bSpeichern)
							 EnableMenuItem(hBrainMenu,IDM9_SPEICHERN,MF_ENABLED);
						 EnableMenuItem(hBrainMenu,IDM9_START,MF_ENABLED);
						 EnableMenuItem(hBrainMenu,IDM9_RESET,MF_ENABLED);
						 if(bVideomatInit)
						 {
							 EnableMenuItem(hBrainMenu,IDM9_NEU,  				 MF_ENABLED);
							 EnableMenuItem(hBrainMenu,IDM9_ERKENNUNG,		 MF_ENABLED);
						 }
						 if(bMoveMasterInit&&nRoboterEinsatz==KOMPLETT)
							 EnableMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,MF_ENABLED);
					 }
				 }
				 if(bStart && !bErkennung)
					 EnableMenuItem(hBrainMenu,IDM9_START,MF_ENABLED);
				 if(!bStart && bErkennung)
				 {
					 EnableMenuItem(hBrainMenu,IDM9_ERKENNUNG,MF_ENABLED);
					 if(bMoveMasterInit && nRoboterEinsatz==KOMPLETT)
						 EnableMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,	MF_ENABLED);
				 }
				 break;
		case WM_DESTROY:
				 for(i=0;i<256;++i)
					 DeleteObject(hRedBrush[i]);
				 DeleteObject(hGehirnBitmap);
				 DeleteObject(hBlueBrush);
				 DeleteObject(hGreenBrush);
				 DeleteObject(hBluePen);
				 DeleteObject(hBlackPen);
				 DeleteObject(hGreenPen);
				 if(bStart)
					 SendMessage(hWndBrain,WM_COMMAND,IDM9_START,0L);
				 if(bErkennung)
					 SendMessage(hWndBrain,WM_COMMAND,IDM9_ERKENNUNG,0L);
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								if(bStart)
									SendMessage(hWndBrain,WM_COMMAND,IDM9_START,0L);
								if(bErkennung)
									SendMessage(hWndBrain,WM_COMMAND,IDM9_ERKENNUNG,0L);
								PostMessage(hWndNeurorob,WM_COMMAND,IDM_NEUINIT,0L);
								break;
					 default:
								return (DefWindowProc(hWnd, msg, wParam, lParam));
				 }
				 break;
		default:
				 return (DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return (NULL);
}//Brain

/****************************************************************************
 Funktion:      ZeichneBrain

 Aufgabe:       öbernimmt die Darstellung des neuronalen Netzes

 Parameter:     hWnd: Handle des Fensters in dem gezeichnet werden soll

 RÅckgabewert:  -

****************************************************************************/


void ZeichneBrain(HWND hWnd)
{
HDC					hDC;
HDC					hMemoryDC;
PAINTSTRUCT	ps;
int					i,j,k;

	hDC= BeginPaint(hWnd,&ps);
	if(bNetzdefine)
	{
		SetMapMode(hDC,MM_ANISOTROPIC);
		SetWindowExt(hDC,1,1);
		SetViewportExt(hDC,1,1);
		SetWindowOrg(hDC,GetScrollPos(hWnd,SB_HORZ)+XORGOFFSET,
			GetScrollPos(hWnd,SB_VERT)+YORGOFFSET);
		SelectObject(hDC,hBluePen);
		if(bStart)
		{
			wsprintf((LPSTR)dummy,"Trainingsanzahl : %li    ",lPropagations);
			TextOut(hDC,XORGOFFSET+10,-20,(LPSTR)dummy,lstrlen((LPSTR)dummy));

		}
		for(i=0;i<nHiddenlayer+1;++i)
			for(j=0;j<Netzform[i];++j)
			{
				for(k=0;k<Netzform[i+1];++k)
				{
					MoveTo(hDC,i*nNeuronenAbstand+nNeuronenGroesse/2+nXoffset,
						(j+1)*nNeuronenAbstand+nNeuronenGroesse/2+
						(nLayerMax/2-Netzform[i]/2)*nNeuronenAbstand);
					LineTo(hDC,(i+1)*nNeuronenAbstand+nNeuronenGroesse/2+nXoffset,
						(k+1)*nNeuronenAbstand+nNeuronenGroesse/2+
						(nLayerMax/2-Netzform[i+1]/2)*nNeuronenAbstand);
				}
				if(i==0)
				{
					MoveTo(hDC,XORGOFFSET+10,
						(j+1)*nNeuronenAbstand+nNeuronenGroesse/2+
						(nLayerMax/2-Netzform[i]/2)*nNeuronenAbstand);
					LineTo(hDC,i*nNeuronenAbstand+nNeuronenGroesse/2+nXoffset,
						(nLayerMax/2-Netzform[i]/2)*nNeuronenAbstand+
						(j+1)*nNeuronenAbstand+nNeuronenGroesse/2);
				}
			}
		SelectObject(hDC,hBlackPen);
		for(i=0;i<nHiddenlayer+2;++i)
			for(j=1;j<Netzform[i]+1;++j)
			{
				Netz[j-1][i].xpos1=i*nNeuronenAbstand+nXoffset;
				Netz[j-1][i].ypos1=j*nNeuronenAbstand+
					(nLayerMax/2-Netzform[i]/2)*nNeuronenAbstand;
				Netz[j-1][i].xpos2=i*nNeuronenAbstand+nNeuronenGroesse+nXoffset;
				Netz[j-1][i].ypos2=j*nNeuronenAbstand+nNeuronenGroesse+
					(nLayerMax/2-Netzform[i]/2)*nNeuronenAbstand;
				SelectObject(hDC,hRedBrush[min((int)fabs((Netz[j-1][i].ausgang*255)),
					255)]);
				Ellipse(hDC,Netz[j-1][i].xpos1,Netz[j-1][i].ypos1,
					Netz[j-1][i].xpos2,Netz[j-1][i].ypos2);
				if(i==nHiddenlayer+1)
				{
					lstrcpy((LPSTR)dummy,(LPSTR)TeileName[j-1]);
					if((AusgangsTrainingsVektor[j-1][nSelect%(nSampleProTeil*nTeile)]
						==1)&&bStart)
						lstrcat((LPSTR)dummy,(LPSTR)"<-");
					else
						lstrcat((LPSTR)dummy,(LPSTR)"   ");
					if(NeuronenMaximum()==j-1)
					{
						SetTextColor(hDC,RGB(0,255,0));
						TextOut(hDC,Netz[j-1][i].xpos1+nNeuronenGroesse+5,
							Netz[j-1][i].ypos1,(LPSTR)dummy,lstrlen((LPSTR)dummy));
						SetTextColor(hDC,RGB(0,0,0));
					}
					else
						TextOut(hDC,Netz[j-1][i].xpos1+nNeuronenGroesse+5,
							Netz[j-1][i].ypos1,(LPSTR)dummy,lstrlen((LPSTR)dummy));
					if((AusgangsTrainingsVektor[j-1][nSelect%(nSampleProTeil*nTeile)]==1)
							&&bStart)
					{
						wsprintf((LPSTR)dummy,
							"Sample Nr.: %i      Soll: %s                   ",
							(nSelect%nSampleProTeil)+1,(LPSTR)TeileName[j-1]);
						TextOut(hDC,0,-20,(LPSTR)dummy,lstrlen((LPSTR)dummy));
					}
				}
				if(i==0)
				{
					sprintf((LPSTR)dummy,(LPSTR)"%10.9lf %s",Netz[j-1][i].ausgang,
						(LPSTR)MerkmalName[j-1]);
					TextOut(hDC,XORGOFFSET+10,
						Netz[j-1][i].ypos1,(LPSTR)dummy,lstrlen((LPSTR)dummy));
				}
			}
	}
	else
	{
		SetWindowOrg(hDC,GetScrollPos(hWnd,SB_HORZ),
			GetScrollPos(hWnd,SB_VERT)+YORGOFFSET);
		hMemoryDC = CreateCompatibleDC(hDC);
		hOldBitmap = SelectObject(hMemoryDC,hGehirnBitmap);
		if(hOldBitmap)
		{
			BitBlt (hDC,20,0,500,408,hMemoryDC,0,0,SRCCOPY);
			SelectObject(hMemoryDC,hOldBitmap);
		}
		DeleteDC(hMemoryDC);
	}
	EndPaint(hWnd,&ps);
}//ZeichneBrain

/****************************************************************************
 Funktion:      LadeNeuronalesNetz

 Aufgabe:       LÑd ein neuronales Netz

 Parameter:     -

 RÅckgabewert:  BOOL: TRUE wenn gelungen

****************************************************************************/


BOOL LadeNeuronalesNetz(void)
{
int 		i,j,k;
int 		nHoehe,nBreite,nNeuroneneingaenge;
FILE  	*FilePointer;
HCURSOR	hSaveCursor;

	if(bNetzdefine&&bSpeichernAbfrage)
	{
		if(MessageBox(GetFocus(),
			(LPSTR)"Moechten Sie das Netz speichern",(LPSTR)"",
			MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL)==IDYES)
		{
			if(bSpeichern)
				SendMessage(hWndBrain,WM_COMMAND,IDM9_SPEICHERN,0L);
			else
				SendMessage(hWndBrain,WM_COMMAND,IDM9_ALS,0L);
		}
	}
	szFileCaption = (LPSTR)"Neuronales Netz laden";
	strcpy(DefExt,".NET");
	lpProcLaden = MakeProcInstance(DateiLaden, hInst);
	if(!DialogBox(hInst,"FILEOPERATION",hWndBrain,lpProcLaden))
		return FALSE;
	FreeProcInstance(lpProcLaden) ;
	FilePointer=fopen(FileName,"rt");
	if(!FilePointer)
	{
		MessageBox(GetFocus(),(LPSTR)"Datei konnte nicht geoeffnet werden",
			(LPSTR)"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return (FALSE);
	}
	fgets(dummy, 90 , FilePointer);
	dummy[strlen(dummy)-1]='\0';
	if(lstrcmp((LPSTR)dummy,(LPSTR)VERSION)!=0)
	{
		wsprintf((LPSTR)dummy,(LPSTR)"Gewaehlte Datei ist nicht vom Typ\n%s",
			(LPSTR)VERSION);
		MessageBox(GetFocus(),(LPSTR)dummy,(LPSTR)"Fehler",
			MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return FALSE;
	}
	hSaveCursor = SetCursor(hHourGlass);
	fgets(dummy, 90 , FilePointer);
	fgets(dummy, 90 , FilePointer);
	nRoboterEinsatz=atoi(dummy);
	fgets(dummy, 90 , FilePointer);
	fgets(dummy, 90 , FilePointer);
	nHiddenlayer=atoi(dummy);
	fgets(dummy, 90 , FilePointer);
	for(i=0;i<nHiddenlayer+2;++i)
	{
		fgets(dummy, 90 , FilePointer);
		dummy[strlen(dummy)-1]='\0';
		Netzform[i]=atoi(dummy);
	}
	VerknuepfeNetz();
	nLayerMax=0;
	for(i=0;i<nHiddenlayer+2;++i)
		nLayerMax = max(nLayerMax,Netzform[i]);
	for(i=0;i<Netzform[0];++i)
	{
		fgets(dummy, 90 , FilePointer);
		dummy[strlen(dummy)-1]='\0';
		strcpy(MerkmalName[i],dummy);
	}
	for(i=0;i<Netzform[nHiddenlayer+1];++i)
	{
		fgets(dummy, 90 , FilePointer);
		dummy[strlen(dummy)-1]='\0';
		strcpy(TeileName[i],dummy);
		fgets(dummy,90,FilePointer);
		dummy[strlen(dummy)-1]='\0';
		strcpy(TeileGroesse[i],dummy);
		fgets(dummy,90,FilePointer);
		dummy[strlen(dummy)-1]='\0';
		strcpy(TeileFach[i],dummy);
		fgets(dummy,90,FilePointer);
		dummy[strlen(dummy)-1]='\0';
		strcpy(TeileHoehe[i],dummy);
	}
	fgets(dummy,90,FilePointer);
	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
	{
		fgets(dummy,90,FilePointer);
		dummy[strlen(dummy)-1]='\0';
		Maximum[nHoehe]=atof(dummy);
	}
	fgets(dummy,90,FilePointer);
	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
	{
		fgets(dummy,90,FilePointer);
			dummy[strlen(dummy)-1]='\0';
		Minimum[nHoehe]=atof(dummy);
	}
	for(nBreite=1;nBreite<nHiddenlayer+2;++nBreite)
		for(nHoehe=0;nHoehe<Netzform[nBreite];++nHoehe)
		{
			fgets(dummy,90,FilePointer);
			for(nNeuroneneingaenge=0;nNeuroneneingaenge<
				Netzform[nBreite-1];++nNeuroneneingaenge)
			{
				fgets(dummy,90,FilePointer);
				dummy[strlen(dummy)-1]='\0';
				(Netz[nHoehe][nBreite].xin[nNeuroneneingaenge]).w=atof(dummy);
			}
		}
	fgets(dummy,90,FilePointer);
	for(i=0;i<11;++i)
	{
		fgets(dummy,90,FilePointer);
		bMerkmal[i]=atoi(dummy);
	}
	fgets(dummy,90,FilePointer);
	fgets(dummy,90,FilePointer);
	nTeile=atoi(dummy);
	fgets(dummy,90,FilePointer);
	fgets(dummy,90,FilePointer);
	nSampleProTeil=atoi(dummy);
	for(i=0;i<nTeile*nSampleProTeil;++i)
	{
		fgets(dummy,90,FilePointer);
		fgets(dummy,90,FilePointer);
		for(j=0;j<nLayerMax;++j)
		{
			fgets(dummy,90,FilePointer);
			EingangsTrainingsVektor[j][i]=atof(dummy);
		}
		fgets(dummy,90,FilePointer);
		for(j=0;j<nLayerMax;++j)
		{
			fgets(dummy,90,FilePointer);
			AusgangsTrainingsVektor[j][i]=atof(dummy);
		}
	}
	fclose(FilePointer);
	SetCursor(hSaveCursor);
	return TRUE;
}//LadeNeuronalesNetz

/****************************************************************************
 Funktion:      SpeicherNeuronalesNetz

 Aufgabe:				speichert ein neuronales Netz

 Parameter:			-

 RÅckgabewert:  BOOL: TRUE wenn gelungen

****************************************************************************/

BOOL SpeicherNeuronalesNetz(void)
{
int 		i,j,k;
int 		nHoehe,nBreite,nNeuroneneingaenge;
FILE  	*FilePointer;
HCURSOR	hSaveCursor;


	ErstelleMinMaxVektoren();
	FilePointer=fopen(FileName,"wt");
	if(!FilePointer)
	{
		MessageBox(GetFocus(),"Datei konnte nicht geoeffnet werden",
			"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return (FALSE);
	}
	hSaveCursor = SetCursor(hHourGlass);
	wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)VERSION);
	fputs(dummy,FilePointer);
	fputs("Robotereinsatz:\n",FilePointer);
	wsprintf((LPSTR)dummy,(LPSTR)"%i\n",nRoboterEinsatz);
	fputs(dummy,FilePointer);
	fputs("nHiddenlayer :\n",FilePointer);
	wsprintf((LPSTR)dummy,(LPSTR)"%i\n",nHiddenlayer);
	fputs(dummy,FilePointer);
	fputs("Netzform:\n",FilePointer);
	for(i=0;i<nHiddenlayer+2;++i)
	{
		wsprintf((LPSTR)dummy,(LPSTR)"%i\n",Netzform[i]);
		fputs(dummy,FilePointer);
	}
	for(i=0;i<Netzform[0];++i)
	{
		wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)MerkmalName[i]);
		fputs(dummy,FilePointer);
	}
	for(i=0;i<Netzform[nHiddenlayer+1];++i)
	{
		wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)TeileName[i]);
		fputs(dummy,FilePointer);
		wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)TeileGroesse[i]);
		fputs(dummy,FilePointer);
		wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)TeileFach[i]);
		fputs(dummy,FilePointer);
		wsprintf((LPSTR)dummy,(LPSTR)"%s\n",(LPSTR)TeileHoehe[i]);
		fputs(dummy,FilePointer);
	}
	fputs("MaximumVektor:\n",FilePointer);
	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
	{
		sprintf(dummy,"%26.18lf\n",Maximum[nHoehe]);
		fputs(dummy,FilePointer);
	}
	fputs("MinimumVektor:\n",FilePointer);
	for(nHoehe=0;nHoehe<Netzform[0];++nHoehe)
	{
		sprintf(dummy,"%26.18lf\n",Minimum[nHoehe]);
		fputs(dummy,FilePointer);
	}
	for(nBreite=1;nBreite<nHiddenlayer+2;++nBreite)
		for(nHoehe=0;nHoehe<Netzform[nBreite];++nHoehe)
		{
			wsprintf((LPSTR)dummy,(LPSTR)"Breite:%i Hoehe:%i\n",nBreite,nHoehe);
			fputs(dummy,FilePointer);
			for(nNeuroneneingaenge=0;nNeuroneneingaenge<
				Netzform[nBreite-1];++nNeuroneneingaenge)
			{
				sprintf(dummy,"%26.18lf\n",
				(Netz[nHoehe][nBreite].xin[nNeuroneneingaenge]).w);
				fputs(dummy,FilePointer);
			}
		}
	fputs("Merkmale:\n",FilePointer);
	for(i=0;i<11;++i)
		fprintf(FilePointer,(LPSTR)"%i\n",bMerkmal[i]);
	fputs("nTeile :\n",FilePointer);
	wsprintf((LPSTR)dummy,(LPSTR)"%i\n",nTeile);
	fputs(dummy,FilePointer);
	fputs("nSampleProTeil :\n",FilePointer);
	wsprintf((LPSTR)dummy,(LPSTR)"%i\n",nSampleProTeil);
	fputs(dummy,FilePointer);
	for(i=0;i<nTeile;++i)
		for(k=0;k<nSampleProTeil;++k)
		{
			fprintf(FilePointer,(LPSTR)"Teil %i Sample %i\n",i,k);
			fprintf(FilePointer,(LPSTR)"Eingang:\n");
			for(j=0;j<nLayerMax;++j)
				fprintf(FilePointer,(LPSTR)"%20.16lf\n",
					EingangsTrainingsVektor[j][i*nSampleProTeil+k]);
			fprintf(FilePointer,(LPSTR)"Ausgang:\n");
			for(j=0;j<nLayerMax;++j)
				fprintf(FilePointer,(LPSTR)"%20.16lf\n",
					AusgangsTrainingsVektor[j][i*nSampleProTeil+k]);
		}
	fclose(FilePointer);
	SetCursor(hSaveCursor);
	return TRUE;
}//SpeicherNeuronalesNetz

/****************************************************************************
 Funktion:  		NeuesNetz

 Aufgabe:       Erfragt die Daten des neuen Netzes, das angelegt werden soll

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/


BOOL FAR PASCAL NeuesNetz(HWND			hDlg,
													unsigned	message,
													WORD			wParam,
													LONG			lParam)
{
int		i,j;
BOOL  back;

	switch (message){
		case WM_INITDIALOG:
				 if(bNetzdefine&&bSpeichernAbfrage)
				 {
					 back=MessageBox(GetFocus(),
						 (LPSTR)"Moechten Sie das Netz speichern",(LPSTR)"",
						 MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL);
					 if(back==IDYES)
					 {
						 if(bSpeichern)
							 SendMessage(hWndBrain,WM_COMMAND,IDM9_SPEICHERN,0L);
						 else
							 SendMessage(hWndBrain,WM_COMMAND,IDM9_ALS,0L);
					 }
				 }
				 SetScrollRange(GetDlgItem(hDlg,ID11_TEILE),SB_CTL,1,
					 nHorzmax[TEILE],0);
				 SetScrollPos(GetDlgItem(hDlg,ID11_TEILE),SB_CTL,2,1);
				 SetScrollRange(GetDlgItem(hDlg,ID11_LAYER),SB_CTL,1,
					 nHorzmax[LAYER],0);
				 SetScrollPos(GetDlgItem(hDlg,ID11_LAYER),SB_CTL,1,1);
				 SetScrollRange(GetDlgItem(hDlg,ID11_SAMPLE),SB_CTL,1,
					 nHorzmax[SAMPLE],0);
				 SetScrollPos(GetDlgItem(hDlg,ID11_SAMPLE),SB_CTL,1,1);
				 for(i=1;i<NETZ_BREITE-1;++i)
				 {
					ShowScrollBar(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,FALSE);
					SetWindowText(GetDlgItem(hDlg,ID11_T0+i),(LPSTR)"");
				 }
				 SetWindowText(GetDlgItem(hDlg,ID11_T0),(LPSTR)"3");
				 for(i=0;i<NETZ_BREITE-1;++i)
				 {
					 SetScrollRange(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,1,HIDDENMAX,0);
					 SetScrollPos(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,3,1);
				 }
				 for(i=0;i<11;++i)
					 bMerkmal[i] = FALSE;
				 for(i=0;i<MERKMALEMAX;++i)
					 MerkmalName[i][0]='\0';
				 for(i=1;i<NETZ_BREITE;++i)
					 Netzform[i]=3;
				 for(i=0;i<TEILEMAX;++i)
				 {
					lstrcpy((LPSTR)TeileGroesse[i],(LPSTR)"20");
					lstrcpy((LPSTR)TeileFach[i],(LPSTR)"999");
				 }
				 nHiddenlayer=1;
				 nSampleProTeil = 1;
				 Netzform[nHiddenlayer+1]=2;
				 nTeile = 2;
				 bNetzdefine	= FALSE;
				 Netzform[0] 	= 0;
				 CheckRadioButton(hDlg,ID11_ROBOTER_KEIN,ID11_ROBOTER_KOMPLETT,
					 ID11_ROBOTER_KEIN);
				 nRoboterEinsatz = KEIN;
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDOK:
								nLayerMax=0;
								for(i=0;i<nHiddenlayer+2;++i)
									nLayerMax = max(nLayerMax,Netzform[i]);
								if(Netzform[0]>0)
								{
									bNetzdefine = TRUE;
									VerknuepfeNetz();
									BerechneAusgang();
									bSpeichern = FALSE;
								}
								InvalidateRect(hWndBrain,NULL,TRUE);
								for(i=0;i<SAMPLEANZAHL;++i)
									for(j=0;j<NETZ_HOEHE;++j)
									{
										AusgangsTrainingsVektor[j][i]=0;
										EingangsTrainingsVektor[j][i]=0;
									}
								EndDialog(hDlg,TRUE);
								if(!bSteuerung&&nRoboterEinsatz!=KEIN)
								{
									SendMessage(hWndNeurorob,WM_COMMAND,IDM_STEUERUNG,0L);
									BringWindowToTop(hDlgSteuerung);
								}
								lpTeileDaten = MakeProcInstance(TeileDaten,hInst);
								DialogBox(hInst,"TEILEDATEN",hDlg,lpTeileDaten);
								FreeProcInstance(lpTeileDaten);
								return (TRUE);
					 case IDCANCEL:
								bNetzdefine = FALSE;
								bSpeichern = FALSE;
								InvalidateRect(hWndBrain,NULL,TRUE);
								EndDialog(hDlg,FALSE);
								return (TRUE);
								break;
					 case ID11_ROBOTER_TRAINING:
					 case ID11_ROBOTER_KOMPLETT:
								if(!bMoveMasterInit)
									break;
					 case ID11_ROBOTER_KEIN:
								CheckRadioButton(hDlg,ID11_ROBOTER_KEIN,ID11_ROBOTER_KOMPLETT,
									wParam);
								nRoboterEinsatz = wParam-ID11_ROBOTER_KEIN;
								break;
					 case ID11_TOTALAREA:
					 case ID11_MAJOR:
					 case ID11_MINOR:
					 case ID11_NHOLES:
					 case ID11_PERIMETER:
					 case ID11_RMAX:
					 case ID11_RMIN:
					 case ID11_AVRAD:
					 case ID11_LAENGE:
					 case ID11_BREITE:
					 case ID11_GRAUWERTE:
								if(bMerkmal[wParam-ID11_TOTALAREA] == FALSE)
								{
									CheckDlgButton(hDlg,wParam,(WORD) 1);
									bMerkmal[wParam-ID11_TOTALAREA] = TRUE;
									++Netzform[0];
									if(wParam==ID11_GRAUWERTE)
										Netzform[0]+=15;
								}
								else
								{
									CheckDlgButton(hDlg,wParam,(WORD) 0);
									bMerkmal[wParam-ID11_TOTALAREA] = FALSE;
									--Netzform[0];
									if(wParam==ID11_GRAUWERTE)
										Netzform[0]-=15;
								}
								j=0;
								for(i=0;i<11;++i)
								{
									if(bMerkmal[i])
									{
										ndummy=GetDlgItemText(hDlg,ID11_TOTALAREA+i,
											(LPSTR)dummy,30);
										dummy[ndummy]='\0';
										wsprintf((LPSTR)MerkmalName[j],(LPSTR)"%s ",
											(LPSTR)dummy);
										++j;
									}
								}
								return (TRUE);
					 case ID11_TEILEDATEN:
								lpTeileDaten = MakeProcInstance(TeileDaten,hInst);
								DialogBox(hInst,"TEILEDATEN",hDlg,lpTeileDaten);
								FreeProcInstance(lpTeileDaten);
								break;
				 }
				 return TRUE;
		case WM_VSCROLL:
				 balkenmerker = GetScrollPos(HIWORD(lParam),SB_CTL);
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<HIDDENMAX)
								++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<HIDDENMAX-HIDDENPAGE)
									balkenmerker+=HIDDENPAGE;
								else
									balkenmerker=HIDDENMAX;
								break;
					 case SB_PAGEUP:
								if(balkenmerker>HIDDENPAGE+1)
									balkenmerker-=HIDDENPAGE;
								else
									balkenmerker=1;
								break;
					 case SB_BOTTOM:
								balkenmerker=1;
								break;
					 case SB_TOP:
								balkenmerker=HIDDENMAX;
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,TRUE);
				 itoa(balkenmerker,dummy,10);
				 SetWindowText(GetDlgItem(hDlg,GetDlgCtrlID(HIWORD(lParam))+10)
					,(LPSTR)dummy);
				 Netzform[GetDlgCtrlID(HIWORD(lParam))-ID11_S0+1] = balkenmerker;
				 return TRUE;
		case WM_HSCROLL:
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID11_TEILE))
					 nHorzAuswahl = TEILE;
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID11_LAYER))
					 nHorzAuswahl = LAYER;
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID11_SAMPLE))
					 nHorzAuswahl = SAMPLE;
				 balkenmerker = GetScrollPos(HIWORD(lParam),SB_CTL);
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<nHorzmax[nHorzAuswahl])
								++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<nHorzmax[nHorzAuswahl]-nHorzPage[nHorzAuswahl])
									balkenmerker+=nHorzPage[nHorzAuswahl];
								else
									balkenmerker=nHorzmax[nHorzAuswahl];
								break;
					 case SB_PAGEUP:
								if(balkenmerker>nHorzPage[nHorzAuswahl])
									balkenmerker-=nHorzPage[nHorzAuswahl];
								else
									balkenmerker=1;
								break;
					 case SB_BOTTOM:
								balkenmerker=1;
								break;
					 case SB_TOP:
								balkenmerker=nHorzmax[nHorzAuswahl];
								break;
					 case SB_THUMBPOSITION:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,TRUE);
				 if (nHorzAuswahl == TEILE)
				 {
					 wsprintf((LPSTR)dummy,(LPSTR)"Anzahl der Teile : %i",balkenmerker);
					 SetWindowText(GetDlgItem(hDlg,ID11_TEILETEXT),(LPSTR)dummy);
					 nTeile = balkenmerker;
					 Netzform[nHiddenlayer+1] = nTeile;
				 }
				 if (nHorzAuswahl == LAYER)
				 {
					 wsprintf((LPSTR)dummy,(LPSTR)"Hiddenlayer: %i",
						balkenmerker);
					 nHiddenlayer = balkenmerker;
					 SetWindowText(GetDlgItem(hDlg,ID11_LAYERTEXT),(LPSTR)dummy);
					 for(i=0;i<nHiddenlayer;++i)
					 {
						 ShowScrollBar(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,TRUE);
						 Netzform[i+1]=GetScrollPos(GetDlgItem(hDlg,ID11_S0+i),SB_CTL);
						 wsprintf((LPSTR)dummy,(LPSTR)"%i",Netzform[i+1]);
						 SetWindowText(GetDlgItem(hDlg,ID11_T0+i),(LPSTR)dummy);
					 }
					 Netzform[nHiddenlayer+1] = nTeile;
					 for(i=nHiddenlayer;i<NETZ_BREITE-1;++i)
					 {
						 ShowScrollBar(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,FALSE);
						 SetScrollPos(GetDlgItem(hDlg,ID11_S0+i),SB_CTL,3,0);
						 SetWindowText(GetDlgItem(hDlg,ID11_T0+i),(LPSTR)"");
					 }
				 }
				 if (nHorzAuswahl == SAMPLE)
				 {
					 wsprintf((LPSTR)dummy,(LPSTR)"Sample pro Teil: %i",balkenmerker);
					 SetWindowText(GetDlgItem(hDlg,ID11_SAMPLETEXT),(LPSTR)dummy);
					 nSampleProTeil = balkenmerker;
				 }
				 return TRUE;
	}
	return FALSE;
}//NeuesNetz

/****************************************************************************
 Funktion:  		NeuronenDaten

 Aufgabe:       Wird bei Doppelklick auf Neuron aufgerufen und gibt Auskunft
								Åber die internen Neuronendaten

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/


BOOL FAR PASCAL NeuronenDaten(HWND			hDlg,
															unsigned	message,
															WORD			wParam,
															LONG			lParam)

{
int					k;
double			dDummy;
static  int oldbalkenmerker;

	switch (message){
		case WM_INITDIALOG:
				 if(nAktuelli>0)
					 SetScrollRange(GetDlgItem(hDlg,ID13_SCROLLBAR),SB_CTL,1,
						 Netzform[nAktuelli-1],0);
				 else
					 SetScrollRange(GetDlgItem(hDlg,ID13_SCROLLBAR),SB_CTL,1,1,0);
				 SetScrollPos(GetDlgItem(hDlg,ID13_SCROLLBAR),SB_CTL,1,1);
				 SetDlgItemText(hDlg,ID13_WTEXT,(LPSTR)"W1:");
				 sprintf(dummy,"%lf",Netz[nAktuellj][nAktuelli].xin[0].w);
				 SetDlgItemText(hDlg,ID13_WEDIT,(LPSTR)dummy);
				 sprintf(dummy,"%lf",Netz[nAktuellj][nAktuelli].ausgang);
				 SetDlgItemText(hDlg,ID13_AUSGANG,(LPSTR)dummy);
				 sprintf(dummy,"%lf",Netz[nAktuellj][nAktuelli].p);
				 SetDlgItemText(hDlg,ID13_POTENTIAL,(LPSTR)dummy);
				 oldbalkenmerker = 1;
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam)
				 {
					 case IDCANCEL:
					 case IDOK:
								if(nAktuelli>0)
								{
										GetDlgItemText(hDlg,ID13_WEDIT,(LPSTR)dummy,25);
										dDummy=atof((LPSTR)dummy);
										Netz[nAktuellj][nAktuelli].xin[oldbalkenmerker-1].w=dDummy;
								}
								else
								{
									GetDlgItemText(hDlg,ID13_WEDIT,(LPSTR)dummy,25);
									dDummy=atof((LPSTR)dummy);
									Netz[nAktuellj][nAktuelli].xin[0].w = dDummy;
								}
								EndDialog(hDlg, TRUE);
								return (TRUE);
					}
					break;
		case WM_VSCROLL:
				 balkenmerker = GetScrollPos(HIWORD(lParam),SB_CTL);
				 GetDlgItemText(hDlg,ID13_WEDIT,
					(LPSTR)dummy,20);
				 dDummy = atof((LPSTR)dummy);
				 Netz[nAktuellj][nAktuelli].xin[oldbalkenmerker-1].w=dDummy;
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<Netzform[nAktuelli-1])
									++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<Netzform[nAktuelli-1])
									++balkenmerker;
								break;
					 case SB_PAGEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_BOTTOM:
								balkenmerker=1;
								break;
					 case SB_TOP:
								balkenmerker=Netzform[nAktuelli-1];
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,TRUE);
				 wsprintf((LPSTR)dummy,(LPSTR)"W%i:",balkenmerker);
				 SetDlgItemText(hDlg,ID13_WTEXT,(LPSTR)dummy);
				 sprintf(dummy,"%lf",Netz[nAktuellj][nAktuelli].xin[balkenmerker-1].w);
				 SetDlgItemText(hDlg,ID13_WEDIT,(LPSTR)dummy);
				 oldbalkenmerker = balkenmerker;
				 break;
	}
	return (FALSE);
}//NeuronenDaten

/****************************************************************************
 Funktion:  		TeileDaten

 Aufgabe:       In dieser DialogBox werden die spezifischen Daten der Teile
								erfragt

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/


BOOL FAR PASCAL TeileDaten(HWND			hDlg,
													 unsigned	message,
													 WORD			wParam,
													 LONG			lParam)

{
int		i,j,k;
int 	xZufall;
int 	yZufall;
int 	pZufall;
char 	OldPosition[50];

static int oldbalkenmerker;
char Position[60];

	switch (message){
		case WM_INITDIALOG:
				 SetScrollRange(GetDlgItem(hDlg,ID15_SCROLLTEILE),SB_CTL,1,nTeile,0);
				 SetScrollPos(GetDlgItem(hDlg,ID15_SCROLLTEILE),SB_CTL,1,1);
				 SetScrollRange(GetDlgItem(hDlg,ID15_SCROLLSAMPLE),SB_CTL,1,
					nSampleProTeil,0);
				 SetScrollPos(GetDlgItem(hDlg,ID15_SCROLLSAMPLE),SB_CTL,1,1);
				 SetDlgItemText(hDlg,ID15_TEILENUMMER,(LPSTR)"Teilenummer: 1");
				 SetDlgItemText(hDlg,ID15_SAMPLENUMMER,(LPSTR)"Samplenummer: 1");
				 SetDlgItemText(hDlg,ID15_NAME,(LPSTR)TeileName[0]);
				 SetDlgItemText(hDlg,ID15_GROESSE,(LPSTR)TeileGroesse[0]);
				 SetDlgItemText(hDlg,ID15_FACHNUMMER,(LPSTR)TeileFach[0]);
				 oldbalkenmerker=1;
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam)
				 {
					 case IDCANCEL:
					 case IDOK:
								GetDlgItemText(hDlg,ID15_NAME,
									(LPSTR)TeileName[oldbalkenmerker-1],20);
								GetDlgItemText(hDlg,ID15_GROESSE,
									(LPSTR)TeileGroesse[oldbalkenmerker-1],9);
								GetDlgItemText(hDlg,ID15_FACHNUMMER,
									(LPSTR)TeileFach[oldbalkenmerker-1],9);
								ndummy=atoi(TeileFach[oldbalkenmerker-1]);
								if(ndummy<0 || ndummy>999)
									lstrcpy((LPSTR)TeileFach[oldbalkenmerker-1],(LPSTR)"999");
								InvalidateRect(hWndBrain,NULL,TRUE);
								PostMessage(hWndBrain,WM_PAINT,0,0L);
								bSpeichernAbfrage=TRUE;
								ErstelleMinMaxVektoren();
								EndDialog(hDlg, TRUE);
								return (TRUE);
					 case ID15_SAMPLEAUFNEHMEN:
								if(bVideomatInit)
								{
									i=GetScrollPos(GetDlgItem(hDlg,ID15_SCROLLTEILE),
										SB_CTL)-1;
									j=GetScrollPos(GetDlgItem(hDlg,ID15_SCROLLSAMPLE),
										SB_CTL)-1;
									HoleEingangsVektor();
									SetzeEingangsTrainingsVektor(i*nSampleProTeil+j);
									AusgangsTrainingsVektor[i][i*nSampleProTeil+j]=1;
									ErstelleMinMaxVektoren();
									TransformiereEingangsVektor();
									SendMessage(hDlg,WM_HSCROLL,SB_LINEDOWN,
										MAKELONG(0,GetDlgItem(hDlg,ID15_SCROLLSAMPLE)));
								}
								else
									MessageBox(GetFocus(),
										(LPSTR)"Videomat ist noch nicht initialisiert",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);

								break;
					 case ID15_GREIFHOEHE:
								HoleRoboterPosition((LPSTR)Position,(LPSTR)dummy);
								ndummy=0;
								while(Position[ndummy]!=',')
									++ndummy;
									++ndummy;
								while(Position[ndummy]!=',')
									++ndummy;
								++ndummy;
								strcpy(dummy,&(Position[ndummy]));
								ndummy=0;
								while(dummy[ndummy]!=',')
									++ndummy;
								dummy[ndummy]='\0';
								i=GetScrollPos(GetDlgItem(hDlg,ID15_SCROLLTEILE),
									SB_CTL)-1;
								lstrcpy((LPSTR)TeileHoehe[i],(LPSTR)dummy);
								break;
					 case ID15_STARTEROBOTER:
								if(bVideomatInit)
								{
									if(nRoboterEinsatz==KEIN)
										MessageBox(hDlg,
											(LPSTR)"Robotereinsatz wurde nicht angewÑhlt",
											NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									else
									{
										if(bMoveMasterInit)
										{
											i=GetScrollPos(GetDlgItem(hDlg,ID15_SCROLLTEILE),
													SB_CTL)-1;
											MO("999");
											wsprintf((LPSTR)dummy,(LPSTR)"0,400,%i",
												atoi(TeileHoehe[i]));
											AbsolutRelativ();
											SendeComm1(nCid,dummy);
											GC();
											MO("998");
											MO("997");
											for(j=GetScrollPos(GetDlgItem(hDlg,ID15_SCROLLSAMPLE),
														SB_CTL)-1;j<nSampleProTeil;++j)
											{
												xZufall=ROBOTERRANDOM*
													(200-2*atoi(TeileGroesse[i]))/2+xCent;
												yZufall=ROBOTERRANDOM*
													(260-2*atoi(TeileGroesse[i]))/2+yCent;
												pZufall=ROBOTERRANDOMP;
												if(j==0)
												{
													xZufall=xCent;
													yZufall=yCent;
													pZufall=0;
												}
												wsprintf((LPSTR)dummy,(LPSTR)"0,0,0,0,%i",-pZufall);
												MJ((LPSTR)dummy);
												wsprintf((LPSTR)dummy,(LPSTR)"%i,%i,%i",
													xZufall,yZufall,atoi(TeileHoehe[i]));
												AbsolutRelativ();
												SendeComm1(nCid,dummy);
												HE("995");
												GO();
												wsprintf((LPSTR)dummy,(LPSTR)"%i,%i,%i",
													xZufall,yZufall,atoi(TeileHoehe[i])+40);
												AbsolutRelativ();
												SendeComm1(nCid,dummy);
												HE("994");
												MO("993");
												HoleEingangsVektor();
												SetzeEingangsTrainingsVektor(i*nSampleProTeil+j);
												AusgangsTrainingsVektor[i][i*nSampleProTeil+j]=1;
												ErstelleMinMaxVektoren();
												TransformiereEingangsVektor();
												HolePartSchwerpunkt();
												wsprintf((LPSTR)dummy,(LPSTR)"Samplenummer: %i",j+1);
												MO("994,O");
												MO("995,O");
												GC();
												MO("997");
												SendMessage(hDlg,WM_HSCROLL,SB_LINEDOWN,
													MAKELONG(0,GetDlgItem(hDlg,ID15_SCROLLSAMPLE)));
											}
											SendMessage(hDlg,WM_HSCROLL,SB_BOTTOM,
												MAKELONG(0,GetDlgItem(hDlg,ID15_SCROLLSAMPLE)));
											MO("998");
											wsprintf((LPSTR)dummy,(LPSTR)"0,0,%i",
												-200+atoi(TeileHoehe[i]));
											DS((LPSTR)dummy);
											GO();
											MO("999");
											SendMessage(hDlg,WM_HSCROLL,SB_LINEDOWN,
												MAKELONG(0,GetDlgItem(hDlg,ID15_SCROLLTEILE)));
										}
										else
											MessageBox(GetFocus(),
												(LPSTR)"Movemaster ist noch nicht initialisiert",
												NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									}
								}
								else
									MessageBox(GetFocus(),
										(LPSTR)"Videomat ist noch nicht initialisiert",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);

								break;
				 }
				 break;
		case WM_HSCROLL:
				 balkenmerker = GetScrollPos(HIWORD(lParam),SB_CTL);
				 GetDlgItemText(hDlg,ID15_NAME,
					(LPSTR)TeileName[oldbalkenmerker-1],20);
				 GetDlgItemText(hDlg,ID15_GROESSE,
					(LPSTR)TeileGroesse[oldbalkenmerker-1],9);
				 GetDlgItemText(hDlg,ID15_FACHNUMMER,
					 (LPSTR)TeileFach[oldbalkenmerker-1],9);
				 ndummy=atoi(TeileFach[oldbalkenmerker-1]);
				 if(ndummy<0 || ndummy>999)
					 lstrcpy((LPSTR)TeileFach[oldbalkenmerker-1],(LPSTR)"999");
				 if(HIWORD(lParam)==GetDlgItem(hDlg,ID15_SCROLLTEILE))
				 {
					 ndummy=nTeile;
				 }
				 else
				 {
					 ndummy=nSampleProTeil;
				 }
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<ndummy)
									++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<ndummy)
									++balkenmerker;
								break;
					 case SB_PAGEUP:
								if(balkenmerker>1)
									--balkenmerker;
								break;
					 case SB_BOTTOM:
								balkenmerker=1;
								break;
					 case SB_TOP:
								balkenmerker=ndummy;
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 if(HIWORD(lParam)==GetDlgItem(hDlg,ID15_SCROLLTEILE))
				 {
					 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,TRUE);
					 wsprintf((LPSTR)dummy,(LPSTR)"Teilenummer: %i",balkenmerker);
					 SetDlgItemText(hDlg,ID15_TEILENUMMER,(LPSTR)dummy);
					 SetDlgItemText(hDlg,ID15_NAME,(LPSTR)TeileName[balkenmerker-1]);
					 SetDlgItemText(hDlg,ID15_GROESSE,
						 (LPSTR)TeileGroesse[balkenmerker-1]);
					 SetDlgItemText(hDlg,ID15_FACHNUMMER,
						 (LPSTR)TeileFach[balkenmerker-1]);
					 oldbalkenmerker = balkenmerker;
				 }
				 else
				 {
					 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,TRUE);
					 wsprintf((LPSTR)dummy,(LPSTR)"Samplenummer: %i",balkenmerker);
					 SetDlgItemText(hDlg,ID15_SAMPLENUMMER,(LPSTR)dummy);
				 }
				 break;
	}
	return (FALSE);
}//TeileDaten

/****************************************************************************
 Funktion:        AbsolutRelativ

 Aufgabe:         berechnet den String dummy von absoluten Roboterkoordinaten
									in relative um

 Parameter:

 RÅckgabewert:

****************************************************************************/

void AbsolutRelativ(void)
{
char hilf[50];
char hilf2[50];
int  i,j;
int  xa,ya,za;
int  xh,yh,zh;

	i=0;
	j=0;
	lstrcpy((LPSTR)hilf,(LPSTR)dummy);
	while(hilf[i]!=',')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	xa=atoi(hilf2);
	++i;
	j=0;
	while(hilf[i]!=',')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	ya=atoi(hilf2);
	++i;
	j=0;
	while(hilf[i]!='\0')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	za=atoi(hilf2);
	HoleRoboterPosition((LPSTR)hilf,(LPSTR)hilf2);
	i=0;
	j=0;
	while(hilf[i]!=',')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	xh=atoi(hilf2);
	++i;
	j=0;
	while(hilf[i]!=',')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	yh=atoi(hilf2);
	++i;
	j=0;
	while(hilf[i]!='\0')
	{
		hilf2[j]=hilf[i];
		++i;
		++j;
	}
	hilf2[j]='\0';
	zh=atoi(hilf2);
	wsprintf((LPSTR)dummy,(LPSTR)"DS %i,%i,%i\n",xa-xh,ya-yh,za-zh);
}//AbsolutRelativ

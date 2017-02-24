
#define NEUROROBPFAD "D:\\DIPLOM\\"
#define VERSION "Neurorob Version 1.0"


//  Funktionsprototypen

int PASCAL 			WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL 						InitApplication(HANDLE);
BOOL 						InitInstance(HANDLE, int);
long FAR PASCAL	Brain(HWND, unsigned, WORD, LONG);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Befehlsliste(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Steuerung(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Visualisierung(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL NeuesNetz(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL RobInit(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL VideoSteuerung(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL DateiLaden (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Positionsana (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL NeuronenDaten (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL TeileDaten (HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL Face (HWND, unsigned, WORD, LONG);
int							Sende(HANDLE,char *);
int							SendeComm1(HANDLE,char *);
int             SendeComm2(HANDLE,char *);
void            Gelenkdemo(void);
void						HoleVideomatZeile(LPSTR);
void						ZeichneBrain(HWND);
void						Anzeige(int);
BOOL       			LadeGrundposition(VOID);
VOID						HolePartSchwerpunkt(VOID);
void						AbsolutRelativ(void);


//*************************************************************

void						VerknuepfeNetz(void);
double					Sigmoid(double);
double					SigmoidStrich(double);
void						BerechneAusgang(void);
void						Backpropagation(void);
void						SetzeAusgangsVektor(int);
void						SetzeEingangsVektor(int);
void						HoleEingangsVektor(void);
void						SetzeEingangsTrainingsVektor(int);
int 						NeuronenMaximum(void);
void						ErstelleMinMaxVektoren(void);
double					MaxOfTraining(int);
double					MinOfTraining(int,double);
void						TransformiereEingangsVektor(void);
BOOL						LadeNeuronalesNetz(void);
BOOL						SpeicherNeuronalesNetz(void);

//*************************************************************

void MO(LPSTR Param);      // Funktions_Prototypen fÅr
void DS(LPSTR Param);
void DW(LPSTR Param);      // MOVEMASTER-BEFEHLE
void GC(void);
void GP(LPSTR Param);

void GO(void);
void HE(LPSTR Param);
void MA(LPSTR Param);
void MC(LPSTR Param);
void MJ(LPSTR Param);

void MP(LPSTR Param);
void MS(LPSTR Param);
void MT(LPSTR Param);
void NT(void);
void NW(void);
void PD(LPSTR Param);
void PR(LPSTR Param);
void RS(LPSTR Param);
void SD(LPSTR Param);
void SP(LPSTR Param);

//*****************************************************************

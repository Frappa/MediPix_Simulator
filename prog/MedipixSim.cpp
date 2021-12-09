#include "TROOT.h"
#include "TRint.h"

#include "PhysList.hh"
#include "DetConstr.hh"
#include "AnalysisManager.hh"
#include "PrimGenAction.hh"
//#include "StackingAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"

#include "globals.hh"
#include "G4String.hh"

#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

//#include "G4RunManager.hh"



using std::string;
using std::stringstream;
using std::ifstream;


void usage();


int main(int argc, char **argv)
{
	// switches
	int c = 0;

	stringstream hStream;
	
	bool bInteractive = false;
	bool bVisualize = false;
	bool bVrmlVisualize = false;
	bool bOpenGlVisualize = false;
	bool bUseGui = false;
	
	bool bPreinitMacroFile = false;
	bool bVisMacroFile = false;
	bool bMacroFile = false;
	G4String hPreinitMacroFileName, hMacroFileName, hVisMacroFileName, hOutFileName;
	int iNbEventsToSimulate = 0;

	// parse switches
	while((c = getopt(argc,argv,"p:m:o:n:v:iGh")) != -1)
	{
		switch(c)
		{
			case 'p':
				bPreinitMacroFile = true;
				hPreinitMacroFileName = optarg;
				break;
				
			case 'm':
				bMacroFile = true;
				hMacroFileName = optarg;
				break;
				
			case 'o':
				hOutFileName = optarg;
				break;
				
			case 'i':
				bInteractive = true;
				break;
				
			case 'n':
				hStream.clear(); hStream.str("");
				hStream << optarg;
				iNbEventsToSimulate = atoi(hStream.str().c_str());
				break;
				
			case 'v':
				bVisMacroFile = true;
				hVisMacroFileName = optarg;
				break;
				
			case 'G':
				bUseGui=true;
				bInteractive=true;
				break;
			
			case 'h':
				usage();
				return 0;
				
			default:
				usage();
		}
	}
	
	
	// create the run manager
	ROOT::EnableThreadSafety();
	
	
#ifdef G4MULTITHREADED
	G4MTRunManager* pRunManager = new G4MTRunManager;
	pRunManager->SetNumberOfThreads(4);
#else
	G4RunManager *pRunManager = new G4RunManager;
#endif
	 
	
	//G4RunManager *pRunManager = new G4RunManager;
	
	PhysListMediPix *pPhysList = new PhysListMediPix;
	DetConstrMediPix *pDetGeom = new DetConstrMediPix;
	
	
	// set user-defined initialization classes
	pRunManager->SetUserInitialization(pDetGeom);
	pRunManager->SetUserInitialization(pPhysList);
	
	
	

	// create the primary generator action
	PrimGenAction *pPrimaryGeneratorAction = new PrimGenAction;

	// create an analysis manager object
	AnalysisManager *pAnalysisManager = new AnalysisManager(pPrimaryGeneratorAction);
	
	
	// set user-defined action classes
	pRunManager->SetUserAction(pPrimaryGeneratorAction);
	//pRunManager->SetUserAction(new StackingAction(pAnalysisManager));
	pRunManager->SetUserAction(new RunAction(pAnalysisManager));
	pRunManager->SetUserAction(new EventAction(pAnalysisManager));
	pRunManager->SetUserAction(new TrackAct(pAnalysisManager));
	pRunManager->SetUserAction(new SteppingAction(pAnalysisManager));
	
	
	
	G4UImanager* pUImanager = G4UImanager::GetUIpointer();
	G4UIsession * pUIsession = nullptr;
	G4UIExecutive* ui = nullptr;
	G4VisManager* pVisManager = nullptr;
	
	
	if(bPreinitMacroFile){
		G4String hCommand = "/control/execute " + hPreinitMacroFileName;
		pUImanager->ApplyCommand(hCommand);
	}
	
	//Initialize the RunManager
	pRunManager->Initialize();
	
	if(bInteractive){
		// Visualization Manager
		pVisManager = new G4VisExecutive;
		pVisManager->Initialize();
		
		//Let G4UIExecutive guess what is the best available UI
		ui = new G4UIExecutive(1,argv);
		if (ui->IsGUI() && bUseGui ){
			if(bVisMacroFile){
				G4String hCommand = "/control/execute " + hVisMacroFileName;
				pUImanager->ApplyCommand(hCommand);
			}
			if(bMacroFile)
			{
				G4String hCommand = "/control/execute " + hMacroFileName;
				pUImanager->ApplyCommand(hCommand);
			}
			ui->SessionStart();
			delete ui;
		}else{
			pUIsession = new G4UIterminal(new G4UItcsh);
			if(bMacroFile)
			{
				G4String hCommand = "/control/execute " + hMacroFileName;
				pUImanager->ApplyCommand(hCommand);
			}
			pUIsession->SessionStart();
			delete pUIsession;
		}
		
	}else{
		if(bMacroFile)
		{
			G4String hCommand = "/control/execute " + hMacroFileName;
			pUImanager->ApplyCommand(hCommand);
		}
		if(!hOutFileName.empty()) pAnalysisManager->SetDataFilename(hOutFileName);
			
		if(iNbEventsToSimulate){
			pAnalysisManager->SetNbEventsToSimulate(iNbEventsToSimulate);
			hStream.clear(); hStream.str("");
			hStream << "/run/beamOn " << iNbEventsToSimulate;
			pUImanager->ApplyCommand(hStream.str());
		}
	}
	
	
	//delete pAnalysisManager;
	if(pVisManager) delete pVisManager;
	delete pRunManager;
	return 0;
}


void usage()
{
	G4cout << "\nUsage:" << G4endl;
	G4cout << "MedipixSim [-p preinit.mac] [-m macro.mac] [-o output.root] [-n nevents]" << G4endl;
	G4cout << "MedipixSim -G [-p preinit.mac]\n"<< G4endl;
	exit(0);
}


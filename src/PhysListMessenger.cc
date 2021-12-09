
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysListMessenger.hh"
#include "PhysList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"


////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------//
////////////////////////////////////////////////////////////////////

PhysListMessenger::PhysListMessenger(PhysListMediPix* pPhys)
:fPhysList(pPhys)
{
	fPhysDir = new G4UIdirectory("/medipix/physics/");
	fPhysDir->SetGuidance("Physics control for the MediPix detector.");
	
	fGammaCutCmd = new G4UIcmdWithADoubleAndUnit("/medipix/physics/setGCut",this);
	fGammaCutCmd->SetGuidance("Set gamma cut.");
	fGammaCutCmd->SetParameterName("Gcut",false);
	fGammaCutCmd->SetUnitCategory("Length");
	fGammaCutCmd->SetRange("Gcut>0.0");
	fGammaCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fElectCutCmd = new G4UIcmdWithADoubleAndUnit("/medipix/physics/setECut",this);
	fElectCutCmd->SetGuidance("Set electron cut.");
	fElectCutCmd->SetParameterName("Ecut",false);
	fElectCutCmd->SetUnitCategory("Length");
	fElectCutCmd->SetRange("Ecut>0.0");
	fElectCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fProtoCutCmd = new G4UIcmdWithADoubleAndUnit("/medipix/physics/setPCut",this);
	fProtoCutCmd->SetGuidance("Set positron cut.");
	fProtoCutCmd->SetParameterName("Pcut",false);
	fProtoCutCmd->SetUnitCategory("Length");
	fProtoCutCmd->SetRange("Pcut>0.0");
	fProtoCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fAllCutCmd = new G4UIcmdWithADoubleAndUnit("/medipix/physics/setCuts",this);
	fAllCutCmd->SetGuidance("Set cut for all.");
	fAllCutCmd->SetParameterName("cut",false);
	fAllCutCmd->SetUnitCategory("Length");
	fAllCutCmd->SetRange("cut>0.0");
	fAllCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	
	fMCutCmd = new G4UIcmdWithADoubleAndUnit("/medipix/physics/setDetectorCuts",this);
	fMCutCmd->SetGuidance("Set cuts for the Detector");
	fMCutCmd->SetParameterName("Ecut",false);
	fMCutCmd->SetUnitCategory("Length");
	fMCutCmd->SetRange("Ecut>0.0");
	fMCutCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
	/*
	fPListCmd = new G4UIcmdWithAString("/medipix/physics/SelectPhysics",this);
	fPListCmd->SetGuidance("Select modula physics list.");
	fPListCmd->SetParameterName("PList",false);
	fPListCmd->AvailableForStates(G4State_PreInit);
	*/
	
}


////////////////////////////////////////////////////////////////////

PhysListMessenger::~PhysListMessenger()
{
	if(fPhysDir) delete fPhysDir;
	if(fGammaCutCmd) delete fGammaCutCmd;
	if(fElectCutCmd) delete fElectCutCmd;
	if(fProtoCutCmd) delete fProtoCutCmd;
	if(fAllCutCmd) delete fAllCutCmd;
	if(fMCutCmd) delete fMCutCmd;
	//if(fPListCmd) delete fPListCmd;
}


////////////////////////////////////////////////////////////////////

void PhysListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue)
{
	if( command == fGammaCutCmd ) fPhysList->SetCutForGamma(fGammaCutCmd->GetNewDoubleValue(newValue));
	
	if( command == fElectCutCmd ) fPhysList->SetCutForElectron(fElectCutCmd->GetNewDoubleValue(newValue));
	
	if( command == fProtoCutCmd ) fPhysList->SetCutForPositron(fProtoCutCmd->GetNewDoubleValue(newValue));
	
	if( command == fAllCutCmd ){
		G4double cut = fAllCutCmd->GetNewDoubleValue(newValue);
		fPhysList->SetCutForGamma(cut);
		fPhysList->SetCutForElectron(cut);
		fPhysList->SetCutForPositron(cut);
	}
	
	//if( command == fECutCmd ) fPhysList->SetTargetCut(fECutCmd->GetNewDoubleValue(newValue));
	
	if( command == fMCutCmd ) fPhysList->SetDetectorCut(fMCutCmd->GetNewDoubleValue(newValue));

  //if( command == fPListCmd ) fPhysList->SelectPhysicsList(newValue);
}

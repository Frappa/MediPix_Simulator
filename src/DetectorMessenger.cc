#include "DetectorMessenger.hh"
#include "DetConstr.hh"

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4ParticleTable.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithABool.hh>
#include <G4Tokenizer.hh>
#include <G4ios.hh>

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>




///////////////////////////////////////////////////////

MediPixDetMessenger::MediPixDetMessenger(DetConstrMediPix *pDetector)
:fDetector(pDetector)
{
	std::stringstream txt;
	
	fDetectorDir = new G4UIdirectory("/medipix/detector/");
	fDetectorDir->SetGuidance("MediPix detector geometry properties control.");
	
	fAllumThicknCmd = new G4UIcmdWithADoubleAndUnit("/medipix/detector/setAllumThick",this);
	fAllumThicknCmd->SetGuidance("Set the thickness of the MediPix alluminium layer.");
	fAllumThicknCmd->SetParameterName("AllumThick",false);
	fAllumThicknCmd->AvailableForStates(G4State_PreInit);
	fAllumThicknCmd->SetDefaultUnit("mm");
	fAllumThicknCmd->SetUnitCandidates("mm cm");
	
	fDetConstrVerb = new G4UIcmdWithAnInteger("/medipix/detector/verbosity", this);
	fDetConstrVerb->SetGuidance("Set the verbosity for the detector constructor.");
	fDetConstrVerb->SetParameterName("DetVerb",false);
	fDetConstrVerb->AvailableForStates(G4State_PreInit, G4State_Idle);
	txt.str(""); txt << "DetVerb>=0 && DetVerb<=" << (G4int)DetVerbosity::kDebug;
	fDetConstrVerb->SetRange( txt.str().c_str() );
	
	fStepTrackLimCmd = new G4UIcommand("/medipix/detector/setMaxTrackStep", this);
	fStepTrackLimCmd->SetGuidance("Set the maximum tracking step length for a given logical volume.");
	fStepTrackLimCmd->SetGuidance("usage: /medipix/detector/setMaxTrackStep LogVol step unit");
	fStepTrackLimCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	G4UIparameter *param;
	param = new G4UIparameter("LogVol", 's', false);
	fStepTrackLimCmd->SetParameter(param);
	param = new G4UIparameter("Step", 'd', false);
	fStepTrackLimCmd->SetParameter(param);
	param = new G4UIparameter("unit", 's', false);
	fStepTrackLimCmd->SetParameter(param);
}


///////////////////////////////////////////////////////

MediPixDetMessenger::~MediPixDetMessenger()
{
	if(fAllumThicknCmd) delete fAllumThicknCmd;
	if(fDetConstrVerb) delete fDetConstrVerb;
	if(fStepTrackLimCmd) delete fStepTrackLimCmd;
}


///////////////////////////////////////////////////////

void MediPixDetMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	if(pUIcommand == fDetConstrVerb){
		G4cout << "\nInfo --> MediPixDetMessenger::SetNewValue: called command \"fDetConstrVerb\"" << G4endl;
		G4cout << "Setting verbosity to " << hNewValue << G4endl;
		fDetector->SetVerbosity( (DetVerbosity)StoI(hNewValue) );
		return;
	}
	
	if(pUIcommand == fAllumThicknCmd){
		G4cout << "\nInfo --> MediPixDetMessenger::SetNewValue: called command \"fAllumThicknCmd\"" << G4endl;
		G4cout << "Setting alluminium thickness to " << hNewValue << G4endl;
		fDetector->SetAllumThickness( fAllumThicknCmd->GetNewDoubleValue(hNewValue) );
		return;
	}
	
	if(pUIcommand == fStepTrackLimCmd){
		G4cout << "\nInfo --> MediPixDetMessenger::SetNewValue: called command \"fStepTrackLimCmd\"" << G4endl;
		
		ProcessTrackLimCmd(fStepTrackLimCmd, hNewValue);
		return;
	}
	
	G4cerr << "\nERROR --> MediPixDetMessenger::SetNewValue: command not recognized." << G4endl;
}

void MediPixDetMessenger::ProcessTrackLimCmd(const G4UIcommand *cmd, const G4String& newValues)
{
	G4Tokenizer next(newValues);

	// check argument
	G4String LogVol = next();
	G4double stepLim = StoD(next());
	G4String unit = next();
	
	G4cout << "Setting track length limit for <" << LogVol << "> logical volume to " << stepLim << " " << unit << "." << G4endl;
	
	fDetector->SetStepTrackLimit(LogVol, stepLim*cmd->ValueOf(unit.c_str()));
}
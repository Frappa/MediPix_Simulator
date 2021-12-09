#include "AnalysisMessenger.hh"
#include "AnalysisManager.hh"

#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>


AnalysisMessenger::AnalysisMessenger(AnalysisManager *pAnManager):
fAnManager(pAnManager)
{ 
	std::stringstream txt; txt.str("");
	
	
	fAnalysisDir = new G4UIdirectory("/medipix/analysis/");
	fAnalysisDir->SetGuidance("MediPix analysis manager settings control.");
	
	
	fVerboseCmd = new G4UIcmdWithAnInteger("/medipix/analysis/verbose",this);
	fVerboseCmd->SetGuidance("Set verbosity of the analysis manager.");
	txt.str(""); txt << " Default " << (int)AnalysisVerbosity::kInfo << ".";
	fVerboseCmd->SetGuidance(txt.str().c_str());
	fVerboseCmd->SetParameterName("Verb", false);
	fVerboseCmd->SetDefaultValue((int)AnalysisVerbosity::kInfo);
	txt.str(""); txt << "Verb>=" << (int)AnalysisVerbosity::kSilent << " && Verb<=" << (int)AnalysisVerbosity::kDebug;
	fVerboseCmd->SetRange( txt.str().c_str() );
	fVerboseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fPrintModuloCmd = new G4UIcmdWithAnInteger("/medipix/analysis/PrintModulo",this);
	fPrintModuloCmd->SetGuidance("Prints the start of event every \"PrMod\"");
	fPrintModuloCmd->SetParameterName("PrMod", false);
	fPrintModuloCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fStepDebugCmd = new G4UIcmdWithABool("/medipix/analysis/stepsDebug", this);
	fStepDebugCmd->SetGuidance("Activates debugging controls and messages at every step (super heavy and super slow).");
	fStepDebugCmd->SetParameterName("DebugSteps", false);
	fStepDebugCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fSaveDataCmd = new G4UIcmdWithAnInteger("/medipix/analysis/SaveData", this);
	fSaveDataCmd->SetGuidance("Control for data tree saving: 0 = not save; 1 = standard analysis variables (default); 2 extended tracking info for debugging (slow and heavy)");
	fSaveDataCmd->SetParameterName("Save", false);
	fSaveDataCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fFileNameCmd = new G4UIcmdWithAString("/medipix/analysis/FileName",this);
	fFileNameCmd->SetGuidance("Set the file name where data tree will be saved.");
	fFileNameCmd->SetParameterName("filename", false);
	fFileNameCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	// Define sensitive volumes
	fDefSDCmd = new G4UIcmdWithAString("/medipix/analysis/DefSD", this);
	fDefSDCmd->SetGuidance("Defines a list of physical volume as sensitive (NULL to unset).");
	fDefSDCmd->SetGuidance("DefOptSD: sensdet VolName1 VolName2 ...");
	fDefSDCmd->SetParameterName("VolName", true, true);
	fDefSDCmd->SetDefaultValue("NULL");
	fDefSDCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	
	
	// Autoflush and autosave for data tree
	fAutoFlushCmd = new G4UIcmdWithAnInteger("/medipix/analysis/SetAutoFlush",this);
	fAutoFlushCmd->SetGuidance("Autoflush settings of the data TTree (see ROOT reference guide for more).");
	fAutoFlushCmd->SetParameterName("AutoFlush", false, false);
	fAutoFlushCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fAutoSaveCmd = new G4UIcmdWithAnInteger("/medipix/analysis/SetAutoSave",this);
	fAutoSaveCmd->SetGuidance("Autosave settings of the data TTree (see ROOT reference guide for more).");
	fAutoSaveCmd->SetParameterName("AutoSave", false, false);
	fAutoSaveCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	fRandSeedCmd = new G4UIcmdWithAnInteger("/medipix/analysis/SetRandSeed",this);
	fRandSeedCmd->SetGuidance("Manual set of the random seed. 0 is from machine time (default).");
	fRandSeedCmd->SetParameterName("RandSeed", true);
	fRandSeedCmd->SetDefaultValue(0);
	fRandSeedCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}


AnalysisMessenger::~AnalysisMessenger()
{
	if(fVerboseCmd) delete fVerboseCmd;
	if(fPrintModuloCmd) delete fPrintModuloCmd;
	if(fStepDebugCmd) delete fStepDebugCmd;
	if(fSaveDataCmd) delete fSaveDataCmd;
	if(fFileNameCmd) delete fFileNameCmd;
	if(fDefSDCmd) delete fDefSDCmd;
	
	if(fAnalysisDir) delete fAnalysisDir;
	if(fAutoSaveCmd) delete fAutoSaveCmd;
	if(fAutoFlushCmd) delete fAutoFlushCmd;
	if(fRandSeedCmd) delete fRandSeedCmd;
}


void AnalysisMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	
	if(pUIcommand == fVerboseCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fVerboseCmd. Setting verbosity to " << fVerboseCmd->GetNewIntValue(hNewValue) << G4endl;
		fAnManager->SetVerbosity((AnalysisVerbosity)fVerboseCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fPrintModuloCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fPrintModuloCmd" << G4endl;
		fAnManager->SetPrintModulo(fPrintModuloCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fStepDebugCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fVerboseCmd. Setting the analysis manager steps debug flag to: " << G4UIcommand::ConvertToString(fStepDebugCmd->GetNewBoolValue(hNewValue)) << G4endl;
		fAnManager->SetStepsDebug(fStepDebugCmd->GetNewBoolValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fSaveDataCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fSaveDataCmd. Setting the analysis manager saving flag to: " << fSaveDataCmd->ConvertToString(fSaveDataCmd->GetNewIntValue(hNewValue)) << G4endl;
		fAnManager->SetSaveData((DatasaveLevel)fSaveDataCmd->GetNewIntValue(hNewValue));
		return;
	}
	
	if(pUIcommand == fFileNameCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fFileNameCmd. Setting the tree file name to: " << hNewValue << G4endl;
		if(fAnManager->GetSaveLevel()>DatasaveLevel::kOff){
			fAnManager->SetDataFilename(hNewValue);
		}
		return;
	}
	
	if(pUIcommand == fDefSDCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fDefSDCmd. Setting the sensitive volumes to: " << hNewValue << G4endl;
		fAnManager->DefineSensDet(hNewValue);
		return;
	}
	
	
	if(pUIcommand == fAutoFlushCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fAutoFlushCmd. Setting the data TTree autoflush to " << hNewValue << G4endl;
		fAnManager->SetAutoFlush( std::stoll(hNewValue) );
		return;
	}
	
	if(pUIcommand == fAutoSaveCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fAutoSaveCmd. Setting the data TTree autosave to " << hNewValue << G4endl;
		fAnManager->SetAutoSave( std::stoll(hNewValue) );
		return;
	}
	
	if(pUIcommand == fRandSeedCmd){
		G4cout << "\nInfo --> AnalysisMessenger::SetNewValue: called command fRandSeedCmd. Setting the random seed to " << hNewValue << G4endl;
		fAnManager->SetRanSeed( std::stoi(hNewValue) );
		return;
	}
	
	
	G4cout << "\nERROR ---> AnalysisMessenger::SetNewValue: not recognized command!" << G4endl;

}



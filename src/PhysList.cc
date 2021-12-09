/**
 * AUTHOR: 
 * CONTACT: 
 * FIRST SUBMISSION: 12-28-2010
 *
 * REVISION:
 *
 * mm-dd-yyyy, What is changed, Whoami
 *
 * 12-28-2010, the switching from Geant4.9.2 to Geant4.9.3
 *             is finished starting from e+, Xiang
 */

// ---------------------------------------------------------------------------

#include "PhysListMessenger.hh"
#include "PhysList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
//#include "PhysListHadron.hh"
#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"

#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4DecayPhysics.hh"

#include "G4StepLimiterPhysics.hh"

#include "G4ProductionCuts.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessVector.hh"

#include "G4ParticleWithCuts.hh"
#include "G4UserLimits.hh"
#include "G4ios.hh"

#include "G4SystemOfUnits.hh"

#include <iomanip>




////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------//
////////////////////////////////////////////////////////////////////


//G4ThreadLocal PhysVerbosity PhysListMediPix::fVerboseLevel = PhysVerbosity::kSilent;

//const G4double PhysListMediPix::fDefaultCutValue = 1*mm;


PhysListMediPix::PhysListMediPix():
G4VModularPhysicsList(),
fMessenger(nullptr),
fVerboseLevel(PhysVerbosity::kInfo),
fDefaultCutValue(1.*mm),
fCutForGamma(fDefaultCutValue),
fCutForElectron(fDefaultCutValue),
fCutForPositron(fDefaultCutValue), 
fEmPhysicsList(nullptr),
fRaddecayList(nullptr),
fParticleList(nullptr),
fHadPhysicsList(nullptr),
fNhadcomp(0),
fDetectorCuts(nullptr),
fStepLimiter(nullptr)
{
	G4LossTableManager::Instance();
	defaultCutValue =1.*mm;
	
	fMessenger = new PhysListMessenger(this);
	
	SetVerboseLevel((G4int)PhysVerbosity::kInfo);
	
	//default physics
	fParticleList = new G4DecayPhysics();
	
	
	//default physics
	fRaddecayList = new G4RadioactiveDecayPhysics();
	
	
	//Hadronic physics list builders
	//Add additional physics for hadrons
	fHadronPhys.push_back( new G4EmExtraPhysics((G4int)fVerboseLevel) );
	fHadronPhys.push_back( new G4HadronElasticPhysicsHP((G4int)fVerboseLevel) );
	fHadronPhys.push_back( new G4StoppingPhysics((G4int)fVerboseLevel) );
    fHadronPhys.push_back( new G4IonBinaryCascadePhysics((G4int)fVerboseLevel) );
    fHadronPhys.push_back( new G4NeutronTrackingCut((G4int)fVerboseLevel));
   
    fHadPhysicsList = new G4HadronPhysicsQGSP_BIC_HP((G4int)fVerboseLevel);
	
	// EM physics
	//fEmPhysicsList = new G4EmStandardPhysics();
	fEmPhysicsList = new G4EmStandardPhysics_option4((G4int)fVerboseLevel);
	
	fStepLimiter = new G4StepLimiterPhysics;
	auto oldverb = verboseLevel;
	verboseLevel = 3;
	this->G4VModularPhysicsList::RegisterPhysics(fStepLimiter);
	verboseLevel = oldverb;
}


////////////////////////////////////////////////////////////////////

PhysListMediPix::~PhysListMediPix()
{
	if(fMessenger) delete fMessenger;
	if(fRaddecayList) delete fRaddecayList;
	if(fEmPhysicsList) delete fEmPhysicsList;
	if(fHadPhysicsList) delete fHadPhysicsList;
	if(fHadronPhys.size()>0) {
		for(auto& hadBuil: fHadronPhys) {
			if(hadBuil) delete hadBuil;
		}
	}
}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::ConstructParticle()
{
	fParticleList->ConstructParticle();
}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::ConstructProcess()
{
	AddTransportation();
	
	fStepLimiter->ConstructProcess();
	
	// em
	fEmPhysicsList->ConstructProcess();
	
	// decays
	fParticleList->ConstructProcess();
	fRaddecayList->ConstructProcess();
	
	// had
	if(fHadronPhys.size()>0){
		for(auto& hadBuil: fHadronPhys) {
			hadBuil->ConstructProcess();
		}
	}
	
	if(fHadPhysicsList) fHadPhysicsList->ConstructProcess();
	
	
	
	
	if(fVerboseLevel>=PhysVerbosity::kDetails)
		G4cout << "Detail --> PhysListMediPix::ConstructProcess is done" << G4endl;

}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::SetCuts()
{
	SetCutValue(fCutForGamma, "gamma");
	SetCutValue(fCutForElectron, "e-");
	SetCutValue(fCutForPositron, "e+");
	if(fVerboseLevel>=PhysVerbosity::kInfo){
		G4cout << "\nInfo --> PhysListMediPix::SetCuts: Global cuts are set" << G4endl;
	}
	
	//if( !fTargetCuts ) SetTargetCut(fCutForElectron);
	//G4Region* region = (G4RegionStore::GetInstance())->GetRegion("Target");
	//region->SetProductionCuts(fTargetCuts);
	//if(fVerboseLevel>=PhysVerbosity::kInfo)
	//	G4cout << "Target cuts are set" << G4endl;
	
	if( !fDetectorCuts ){
		this->SetDetectorCut(fCutForElectron);
	}
	
	G4Region *region = (G4RegionStore::GetInstance())->GetRegion("Detector");
	if(region){
		region->SetProductionCuts(fDetectorCuts);
		if(fVerboseLevel>=PhysVerbosity::kInfo){
			G4cout << "Info --> PhysListMediPix::SetCuts: \"Detector\" region cuts are set." << G4endl;
		}
	}
	
	if (fVerboseLevel>=PhysVerbosity::kDetails) DumpCutValuesTable();
}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::SetCutForGamma(G4double cut)
{
	fCutForGamma = cut;
	SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::SetCutForElectron(G4double cut)
{
  fCutForElectron = cut;
  SetParticleCuts(fCutForElectron, G4Electron::Electron());
}


////////////////////////////////////////////////////////////////////

void PhysListMediPix::SetCutForPositron(G4double cut)
{
  fCutForPositron = cut;
  SetParticleCuts(fCutForPositron, G4Positron::Positron());
}


////////////////////////////////////////////////////////////////////
/*
void PhysListMediPix::SetTargetCut(G4double cut)
{
  if( !fTargetCuts ) fTargetCuts = new G4ProductionCuts();

  fTargetCuts->SetProductionCut(cut, idxG4GammaCut);
  fTargetCuts->SetProductionCut(cut, idxG4ElectronCut);
  fTargetCuts->SetProductionCut(cut, idxG4PositronCut);

}
*/

////////////////////////////////////////////////////////////////////

void PhysListMediPix::SetDetectorCut(G4double cut)
{
  if( !fDetectorCuts ) fDetectorCuts = new G4ProductionCuts();
  
  //The idxG4***Cut are enumerator values defined in the G4ProductionCuts.hh header
  
  fDetectorCuts->SetProductionCut(cut, idxG4GammaCut);
  fDetectorCuts->SetProductionCut(cut, idxG4ElectronCut);
  fDetectorCuts->SetProductionCut(cut, idxG4PositronCut);
}
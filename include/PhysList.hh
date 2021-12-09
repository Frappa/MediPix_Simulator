#ifndef __PHYS_LIST_HH__
#define __PHYS_LIST_HH__

#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "globals.hh"

#include <vector>


////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------//
////////////////////////////////////////////////////////////////////



class G4VPhysicsConstructor;
class PhysicsListMessenger;
class G4ProductionCuts;
class PhysListMessenger;
class G4StepLimiterPhysics;

enum class PhysVerbosity{kSilent, kInfo, kDetails, kDebug};


class PhysListMediPix: public G4VModularPhysicsList
{
	PhysListMessenger* fMessenger;
	
	//All the static declaration here are made for speed and low memory consumption in addition they are thread safe using the G4 internal functionalities
	//Note: All this static members are not shared between different threads but shared between different instances of the class in the same thread
	PhysVerbosity fVerboseLevel;
	
	const G4double fDefaultCutValue;
	G4double fCutForGamma;
	G4double fCutForElectron;
	G4double fCutForPositron;
	
	G4VPhysicsConstructor*  fEmPhysicsList;
	G4VPhysicsConstructor*  fRaddecayList;
	G4VPhysicsConstructor*  fParticleList;
	G4VPhysicsConstructor*  fHadPhysicsList;
	
	std::vector<G4VPhysicsConstructor*>  fHadronPhys;
	G4int fNhadcomp;
	
	G4ProductionCuts* fDetectorCuts;
	
	G4StepLimiterPhysics *fStepLimiter;
	
	//Do not allow to have the assign operator and the copy constructor
	PhysListMediPix & operator=(const PhysListMediPix &right);
	PhysListMediPix(const PhysListMediPix&);
	
	
	
protected:
	
	
public:
	PhysListMediPix();
	virtual ~PhysListMediPix();
	
	virtual void ConstructParticle();
	virtual void ConstructProcess();
	virtual void SetCuts();
	
	void SetCutForGamma(G4double);
	void SetCutForElectron(G4double);
	void SetCutForPositron(G4double);
	
	void SelectPhysicsList(const G4String& name){;}; //Dummy for the moment
	
	
	void SetTargetCut(G4double val){;}; //Dummy for the moment
	void SetDetectorCut(G4double val);
	
	//for the Messenger
	void SetVerbose(PhysVerbosity verb){fVerboseLevel=verb;};

};

#endif


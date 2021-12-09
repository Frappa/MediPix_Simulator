#include "EventAction.hh"


#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrackStatus.hh"
#include "G4StepStatus.hh"
#include "G4VProcess.hh"



EventAction::EventAction(AnalysisManager *pAnalysisManager):
fAnalysisManager(pAnalysisManager)
	{;}

EventAction::~EventAction()
	{;}

void EventAction::BeginOfEventAction(const G4Event *pEvent)
{
	if(fAnalysisManager) fAnalysisManager->BeginOfEvent(pEvent);
	/*
	if(fPmtHitsCollectionID == -1){
		G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
		fPmtHitsCollectionID = pSDManager->GetCollectionID("PmtHitsCollection");
	}
	*/
}

void EventAction::EndOfEventAction(const G4Event *pEvent)
{
	if(fAnalysisManager) fAnalysisManager->EndOfEvent(pEvent);
	/*
	G4HCofThisEvent* pHCofThisEvent = pEvent->GetHCofThisEvent();
	Xurich2PmtHitsCollection* pPmtHitsCollection = NULL;

	G4int iNbPmtHits = 0;

	if(pHCofThisEvent){
		if(fPmtHitsCollectionID != -1){
			pPmtHitsCollection = (Xurich2PmtHitsCollection *)(pHCofThisEvent->GetHC(fPmtHitsCollectionID));
			if(pPmtHitsCollection){
				iNbPmtHits = pPmtHitsCollection->entries();
			}
		}
	}
	G4cout << "\nTotal number of PMT hits: " << iNbPmtHits << G4endl;
	*/
}



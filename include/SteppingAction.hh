#ifndef __STEPPING_ACTION_HH__
#define __STEPPING_ACTION_HH__

#include <globals.hh>
#include <G4UserSteppingAction.hh>


class AnalysisManager;

class SteppingAction: public G4UserSteppingAction
{
public:
	SteppingAction(AnalysisManager *pAnalysisManager=0);
	virtual ~SteppingAction();
	
	void UserSteppingAction(const G4Step* aStep);

private:
	AnalysisManager *fAnalysisManager;
};

#endif


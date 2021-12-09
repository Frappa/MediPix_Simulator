#ifndef __RUN_ACTION_HH__
#define __RUN_ACTION_HH__


#include <G4UserRunAction.hh>

class G4Run;

class AnalysisManagerOptPh;

class RunAction: public G4UserRunAction
{
public:
	RunAction(AnalysisManager *pAnalysisManager=NULL);
	~RunAction();

public:
	void BeginOfRunAction(const G4Run *pRun);
	void EndOfRunAction(const G4Run *pRun);
	
	

private:
	AnalysisManager *fAnalysisManager;
};

#endif


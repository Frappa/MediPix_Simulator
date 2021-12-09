#ifndef __DET_MESSANGER_HH__
#define __DET_MESSANGER_HH__

#include <G4UImessenger.hh>
#include <globals.hh>


class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class DetConstrMediPix;


class MediPixDetMessenger: public G4UImessenger
{
public:
	MediPixDetMessenger(DetConstrMediPix *pDetector);
	virtual ~MediPixDetMessenger();

	void SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue);
	void ProcessTrackLimCmd(const G4UIcommand *cmd, const G4String& newValues);

private:
	DetConstrMediPix* fDetector;
	
	G4UIdirectory *fDetectorDir, *fMediPixDetDir;
	
	G4UIcmdWithAnInteger *fDetConstrVerb;
	
	G4UIcmdWithADoubleAndUnit *fAllumThicknCmd;
	
	G4UIcommand *fStepTrackLimCmd;
	
};

#endif


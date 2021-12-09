#ifndef __DET_CONSTR_HH__
#define __DET_CONSTR_HH__

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

#include "G4OpticalSurface.hh"
#include "G4GDMLParser.hh"
#include "G4Material.hh"

#include <vector>
#include <map>

using std::vector;
using std::map;
using std::string;


class G4String;
class G4Colour;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4SubtractionSolid;
class MediPixDetMessenger;


enum class DetVerbosity{kSilent, kInfo, kDetails, kDebug};

class DetConstrMediPix: public G4VUserDetectorConstruction
{
	MediPixDetMessenger *fMediPixMess;
	
	G4VPhysicalVolume *fWorld;
	
	DetVerbosity fVerbose;
	
	G4double fAllumLayerThick; //Thickness of the alluminium layer
	
	const G4MaterialTable *fMatTab;
	
	std::map<G4String, G4double> fGeomDefs;
	
	
	//Methods used internally. Mostly by the constructor
	void GeomDefinitions();
	
	G4LogicalVolume* BuildMediPix();
	
	G4LogicalVolume* BuildAllumLayer();
	
	void BuildGeometry();
	
public:
	
	DetConstrMediPix();
	virtual ~DetConstrMediPix(){;};
	
	inline G4VPhysicalVolume* Construct(){return fWorld;};
	
	
	inline void SetAllumThickness(G4double val){fAllumLayerThick=val;};
	inline const G4VPhysicalVolume* GetWorldVolume() const {return fWorld;};
	void SetStepTrackLimit(const G4String& log_vol, const G4double step_lim);
	
	inline void SetVerbosity(DetVerbosity verb){fVerbose = verb;};
};



#endif

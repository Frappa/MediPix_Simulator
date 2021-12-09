#include "DetConstr.hh"
#include "DetectorMessenger.hh"
//#include "SensitiveDetectors.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4Ellipsoid.hh"
#include "G4ExtrudedSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"


#include <vector>
#include <map>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;
using std::map;
using std::stringstream;
using std::max;
using std::ifstream;
using std::ofstream;






/////////////////////////////////////////////////////////////////////////////////////////
DetConstrMediPix::DetConstrMediPix():
fMediPixMess(nullptr),
fWorld(nullptr),
fMatTab(nullptr),
fAllumLayerThick(0.0),
fVerbose(DetVerbosity::kInfo)
{
	fMediPixMess = new MediPixDetMessenger(this);
	
	GeomDefinitions();
	
	BuildGeometry();
}



/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrMediPix::GeomDefinitions(){
	
	//World dimensions
	fGeomDefs["world_X"] = 10.0*cm;
	fGeomDefs["world_Y"] = 10.0*cm;
	fGeomDefs["world_Z"] = 10.0*cm;
	
	//Medipix dimension and shift
	fGeomDefs["medipix_X"] = 10.0*mm;
	fGeomDefs["medipix_Y"] = 10.0*mm;
	fGeomDefs["medipix_Z"] = 0.3*mm;
}


/////////////////////////////////////////////////////////////////////////////////////////
void DetConstrMediPix::BuildGeometry()
{
	//Construct the world volume (primary volume of the hierarchy)
	
	G4Material *world_Mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
	if(!world_Mat){
		G4cerr << "\nERROR --> DetConstrMediPix::Construct: Could not find or build the \"G4_AIR\" material." << G4endl;
		G4Exception("DetConstrMediPix::Construct","DetConstr001",FatalException,"The \"world_Mat\" is null!");
	}
	
	G4Box *world_Solid = new G4Box("world_Solid", fGeomDefs["world_X"]/2., fGeomDefs["world_Y"]/2., fGeomDefs["world_Z"]/2.);
	
	G4LogicalVolume *world_LogVol = new G4LogicalVolume(world_Solid, world_Mat, "world_LogVol");
	
	fWorld = new G4PVPlacement(nullptr, G4ThreeVector(0.,0., 0), world_LogVol,"world_PhysVol", nullptr, false, 0);
	if(fVerbose>=DetVerbosity::kDetails) G4cout << "\nDetail --> DetConstrMediPix::Construct: Build physical volume \""<< fWorld->GetName() << "\"" << G4endl;
	
	G4VisAttributes* world_Vis = new G4VisAttributes(true, G4Color::Grey());
	world_Vis->SetForceWireframe(true);
	world_LogVol->SetVisAttributes(world_Vis);
	
	
	//Construct and place the silicon detector
	G4LogicalVolume *medipix_LogVol = BuildMediPix();
	
	//Add to a region for finer production cuts
	G4Region* detector_Region = new G4Region("Detector");
	medipix_LogVol->SetRegion(detector_Region);
	detector_Region->AddRootLogicalVolume(medipix_LogVol);
	
	//Translation vector of the medipix w/r to its mother volume
	G4ThreeVector medipix_tlate(0.,0., -fGeomDefs["world_Z"]/2.+fGeomDefs["medipix_Z"]/2);
	
	G4VPhysicalVolume *pPV_tmp = new G4PVPlacement(nullptr, medipix_tlate, medipix_LogVol, "medipix_PhysVol", world_LogVol, false, 0);
	if(fVerbose>=DetVerbosity::kDetails) G4cout << "\nDetail --> DetConstrMediPix::Construct: Build physical volume \""<< pPV_tmp->GetName() << "\"" << G4endl;
	
	
	
	//Define the alluminium layer
	if(fAllumLayerThick<=0.0){
		fMatTab = G4Material::GetMaterialTable();
		return;
	}
	
	G4LogicalVolume *allumlayer_LogVol = BuildAllumLayer();
	
	//Translation vector of the alluminium layer w/r to its mother volume
	G4ThreeVector allumlayer_tlate(0.,0., -fGeomDefs["world_Z"]/2. + fGeomDefs["medipix_Z"] + fAllumLayerThick/2.);
	
	pPV_tmp = new G4PVPlacement(nullptr, allumlayer_tlate, allumlayer_LogVol, "allumlayer_PhysVol", world_LogVol, false, 0);
	if(fVerbose>=DetVerbosity::kDetails) G4cout << "\nDetail --> DetConstrMediPix::Construct: Build physical volume \""<< pPV_tmp->GetName() << "\"" << G4endl;
	
	fMatTab = G4Material::GetMaterialTable();
	
	return;
	
}



/////////////////////////////////////////////////////////////////////////////////////////
G4LogicalVolume* DetConstrMediPix::BuildMediPix()
{
	
	if(fVerbose>=DetVerbosity::kDebug){
		G4cout << "Debug --> DetConstrMediPix::BuildMediPix: Entering the function." << G4endl;
	}
	
	
	G4Material *medipix_Mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
	
	G4Box *medipix_Solid = new G4Box("medipix_Solid", fGeomDefs["medipix_X"]/2., fGeomDefs["medipix_Y"]/2., fGeomDefs["medipix_Z"]/2.);
	
	G4LogicalVolume* medipix_LogVol= new G4LogicalVolume(medipix_Solid, medipix_Mat, "medipix_LogVol");
	
	if(fVerbose>=DetVerbosity::kDebug){
		G4cout << "Debug --> DetConstrMediPix::BuildMediPix(): MediPix logical volume buit! Exiting the function." << G4endl;
	}
	
	//HARDCODED, ADD A UI COMMAND IN THE MESSENGER!!!
	//G4double maxStep = 0.1*fGeomDefs["medipix_Z"];
	//medipix_LogVol->SetUserLimits(new G4UserLimits(maxStep));
	
	G4VisAttributes* medipix_Vis = new G4VisAttributes(true, G4Color::Red());
	medipix_Vis->SetForceSolid(true);
	medipix_LogVol->SetVisAttributes(medipix_Vis);
	
	return medipix_LogVol;
}


/////////////////////////////////////////////////////////////////////////////////////////
G4LogicalVolume* DetConstrMediPix::BuildAllumLayer()
{
	
	if(fVerbose>=DetVerbosity::kDebug){
		G4cout << "Debug --> DetConstrMediPix::BuildAllumLayer: Entering the function." << G4endl;
	}
	
	
	G4Material *allumlayer_Mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
	
	G4Box *allumlayer_Solid = new G4Box("allumlayer_Solid", fGeomDefs["medipix_X"]/2., fGeomDefs["medipix_Y"]/2., fAllumLayerThick/2.);
	
	G4LogicalVolume* allumlayer_LogVol= new G4LogicalVolume(allumlayer_Solid, allumlayer_Mat, "allumlayer_LogVol");
	
	if(fVerbose>=DetVerbosity::kDebug){
		G4cout << "Debug --> DetConstrMediPix::BuildAllumLayer(): Alluminium layer buit! Exiting the function." << G4endl;
	}
	
	G4VisAttributes* allumlayer_Vis = new G4VisAttributes(true, G4Color::Blue());
	allumlayer_Vis->SetForceSolid(true);
	allumlayer_LogVol->SetVisAttributes(allumlayer_LogVol);
	
	return allumlayer_LogVol;
}


void DetConstrMediPix::SetStepTrackLimit(const G4String& log_vol, const G4double step_lim)
{
	G4LogicalVolume *logVol = G4LogicalVolumeStore::GetInstance()->GetVolume(log_vol, false);
	
	
	if(!logVol){
		G4cerr << "\nERROR --> DetConstrMediPix::SetStepTrackLimit: could not find the logical volume <" << log_vol << ">." << G4endl;
		return;
	}
	
	if(logVol->GetUserLimits()) delete logVol->GetUserLimits(); //This is a bit dangerous but the operation below can produce memory leaks.
	
	logVol->SetUserLimits(new G4UserLimits(step_lim));
	
}
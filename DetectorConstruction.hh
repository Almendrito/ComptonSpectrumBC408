#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4String.hh"
#include "globals.hh"
#include <vector>

class G4GenericMessenger;
class G4VPhysicalVolume;

// -----------------------------------------------------------------------------
// DetectorConstruction
//
// Construye un centellador plastico BC404 (base PVT) cilindrico, con carcasa de
// Al y un stub de PMT opcionales, frente a una fuente gamma puntual.
//
// Ademas centraliza la configuracion de la FUENTE (energias, pesos, cascada,
// distancia, tipo de haz). Esto es deliberado: en modo multihilo la
// PrimaryGeneratorAction se clona por hilo, asi que guardar los parametros aqui
// (instancia unica compartida) y leerlos desde el generador evita duplicar
// mensajeros G4GenericMessenger por hilo. Es el mismo patron usado en
// NeutronAlAttenuation.
// -----------------------------------------------------------------------------
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;

    // --- Configuracion de la fuente (la lee PrimaryGeneratorAction) ---
    const std::vector<G4double>& GetSourceEnergies() const { return fEnergies; }
    const std::vector<G4double>& GetSourceWeights()  const { return fWeights; }
    G4bool   GetCascade()        const { return fCascade; }
    G4double GetSourceDistance() const { return fSourceDistance; }
    G4String GetBeamType()       const { return fBeamType; }
    G4String GetSourceLabel()    const { return fSourceLabel; }

    // --- Geometria (la usa el generador para ubicar la fuente en el eje) ---
    G4double GetScintHalfZ()  const { return fScintHalfZ; }
    G4double GetScintRadius() const { return fScintRadius; }

private:
    void DefineCommands();
    void SetPreset(const G4String& name);   // /gun/preset co60|cs137|na22
    void SetSingleEnergy(G4double e);        // /gun/energy
    void UpdateGeometry();                    // /det/update

    G4GenericMessenger* fGeoMessenger = nullptr;
    G4GenericMessenger* fSrcMessenger = nullptr;

    // Geometria (por defecto: cilindro 2"x2" = r 2.54 cm, largo 5.08 cm)
    G4double fScintRadius;
    G4double fScintHalfZ;
    G4bool   fBuildPMT;
    G4bool   fBuildHousing;
    G4double fHousingWall;      // grosor de la pared lateral de Al (cilindro)
    G4double fHousingFront;     // grosor de la tapa frontal de Al (hacia la fuente)

    // Fuente (Co-60 por defecto)
    std::vector<G4double> fEnergies;
    std::vector<G4double> fWeights;
    G4bool   fCascade;          // true: todas las lineas por evento; false: muestrea una
    G4double fSourceDistance;   // fuente puntual -> cara frontal del centellador
    G4String fBeamType;         // "pencil" | "cone" | "iso"
    G4String fSourceLabel;      // etiqueta para el nombre de archivo de salida
};

#endif

#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class DetectorConstruction;
class G4Run;

// -----------------------------------------------------------------------------
// RunAction
//
// Define el ntuple y el histograma del espectro, abre/cierra el archivo ROOT
// (nombrado segun la etiqueta de la fuente) y mezcla los ntuples de los hilos.
// -----------------------------------------------------------------------------
class RunAction : public G4UserRunAction
{
public:
    explicit RunAction(DetectorConstruction* det);
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

private:
    DetectorConstruction* fDet = nullptr;
};

#endif

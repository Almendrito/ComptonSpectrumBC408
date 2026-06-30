#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4Gamma.hh"

SteppingAction::SteppingAction(EventAction* ea)
: fEventAction(ea)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    const auto* preVol = step->GetPreStepPoint()->GetPhysicalVolume();
    if (!preVol || preVol->GetName() != "Scintillator") return;

    // Energia depositada en el centellador (cualquier particula del step).
    const G4double edep = step->GetTotalEnergyDeposit();
    if (edep > 0.) fEventAction->AddEdep(edep);

    // Diagnostico: contar las interacciones del foton en el centellador.
    // Con EM option4 el proceso sigue llamandose "compt"/"phot" (solo cambia el modelo).
    if (step->GetTrack()->GetParticleDefinition() == G4Gamma::Definition()) {
        const auto* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
        if (proc) {
            const G4String& pname = proc->GetProcessName();
            if      (pname == "compt") fEventAction->AddCompton();
            else if (pname == "phot")  fEventAction->AddPhoto();
        }
    }
}

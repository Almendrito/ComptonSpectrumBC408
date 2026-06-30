#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"

// -----------------------------------------------------------------------------
// EventAction
//
// Acumula por evento la energia depositada en el centellador (la suma de todos
// los steps dentro del volumen "Scintillator", reportada por SteppingAction) y
// los contadores de diagnostico. Al final del evento, si hubo deposito, llena
// una fila del ntuple y el histograma.
// -----------------------------------------------------------------------------
class EventAction : public G4UserEventAction
{
public:
    EventAction() = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event*) override;
    void EndOfEventAction(const G4Event*) override;

    void AddEdep(G4double e) { fEdep += e; }
    void AddCompton()        { ++fNCompton; }
    void AddPhoto()          { ++fNPhoto; }

private:
    G4double fEdep     = 0.;
    G4int    fNCompton = 0;
    G4int    fNPhoto   = 0;
};

#endif

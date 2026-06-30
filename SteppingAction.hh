#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"

class EventAction;

// -----------------------------------------------------------------------------
// SteppingAction
//
// En cada step dentro del volumen "Scintillator" suma la energia depositada al
// EventAction y, si el track es un gamma, cuenta la interaccion (compt / phot).
// -----------------------------------------------------------------------------
class SteppingAction : public G4UserSteppingAction
{
public:
    explicit SteppingAction(EventAction* ea);
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step*) override;

private:
    EventAction* fEventAction = nullptr;
};

#endif

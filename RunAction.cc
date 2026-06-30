#include "RunAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4AnalysisManager.hh"

RunAction::RunAction(DetectorConstruction* det)
: fDet(det)
{
    auto* am = G4AnalysisManager::Instance();
    am->SetDefaultFileType("root");
    am->SetNtupleMerging(true);   // mezcla los ntuples de todos los hilos en un archivo
    am->SetVerboseLevel(1);

    // -------------------------------------------------------------------------
    // Ntuple: una fila por evento con deposito > 0 en el centellador.
    //
    // Edep_keV  : energia depositada en el centellador (la observable clave).
    // E0_keV    : energia primaria total del evento (linea muestreada, o suma en cascada).
    // NCompton  : numero de dispersiones Compton del foton en el centellador.
    // NPhoto    : numero de absorciones fotoelectricas (~0 en plastico de Z bajo).
    // FullAbs   : 1 si Edep ~ E0 (1%), absorcion casi total del evento.
    // -------------------------------------------------------------------------
    am->CreateNtuple("ComptonSpectrum", "Espectro Compton en BC404");
    am->CreateNtupleIColumn("EventID");    // 0
    am->CreateNtupleDColumn("Edep_keV");   // 1
    am->CreateNtupleDColumn("E0_keV");     // 2
    am->CreateNtupleIColumn("NCompton");   // 3
    am->CreateNtupleIColumn("NPhoto");     // 4
    am->CreateNtupleIColumn("FullAbs");    // 5
    am->FinishNtuple(0);

    // Histograma de conveniencia (espectro "verdadero", sin resolucion).
    // 0..2600 keV cubre incluso la suma en cascada de Co-60 (2505 keV).
    am->CreateH1("Edep", "Energia depositada;E_{dep} [keV];Cuentas", 1300, 0., 2600.);
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto* am = G4AnalysisManager::Instance();
    am->OpenFile("ComptonSpectrum_" + fDet->GetSourceLabel() + ".root");
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    auto* am = G4AnalysisManager::Instance();
    am->Write();
    am->CloseFile();

    if (IsMaster()) {
        G4cout << "\n====================================================="
               << "\n  Espectro Compton BC404 - simulacion terminada"
               << "\n  Fuente        : " << fDet->GetSourceLabel()
               << "\n  Eventos       : " << run->GetNumberOfEvent()
               << "\n  Archivo ROOT  : ComptonSpectrum_" << fDet->GetSourceLabel() << ".root"
               << "\n=====================================================\n"
               << G4endl;
    }
}

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4ParticleGun.hh"
#include "G4Gamma.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"

#include <cmath>
#include <numeric>

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det)
: fDet(det)
{
    fGun = new G4ParticleGun(1);
    fGun->SetParticleDefinition(G4Gamma::Definition());
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fGun;
}

// Direccion del gamma segun el modo de haz. srcZ < 0 es la z de la fuente.
G4ThreeVector PrimaryGeneratorAction::SampleDirection(const G4String& beamType,
                                                      G4double srcZ) const
{
    if (beamType == "iso") {
        const G4double cosT = 1. - 2. * G4UniformRand();
        const G4double sinT = std::sqrt(1. - cosT * cosT);
        const G4double phi  = twopi * G4UniformRand();
        return {sinT * std::cos(phi), sinT * std::sin(phi), cosT};
    }
    if (beamType == "cone") {
        // Cono que subtiende la cara frontal del centellador (semiangulo atan(R/d)).
        const G4double R = fDet->GetScintRadius();
        const G4double d = -srcZ - fDet->GetScintHalfZ();   // distancia a la cara frontal
        const G4double cosMax = d / std::sqrt(d * d + R * R);
        const G4double cosT = 1. - G4UniformRand() * (1. - cosMax);
        const G4double sinT = std::sqrt(1. - cosT * cosT);
        const G4double phi  = twopi * G4UniformRand();
        return {sinT * std::cos(phi), sinT * std::sin(phi), cosT};
    }
    // "pencil": haz axial
    return {0., 0., 1.};
}

void PrimaryGeneratorAction::FireGamma(G4Event* event, G4double energy,
                                       const G4ThreeVector& pos, const G4ThreeVector& dir)
{
    fGun->SetParticleEnergy(energy);
    fGun->SetParticlePosition(pos);
    fGun->SetParticleMomentumDirection(dir);
    fGun->GeneratePrimaryVertex(event);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    const auto& energies = fDet->GetSourceEnergies();
    const auto& weights  = fDet->GetSourceWeights();
    if (energies.empty()) return;

    const G4double srcZ = -(fDet->GetScintHalfZ() + fDet->GetSourceDistance());
    const G4ThreeVector pos(0., 0., srcZ);
    const G4String beam = fDet->GetBeamType();

    if (fDet->GetCascade()) {
        // Cascada: todas las lineas en el mismo evento (direcciones independientes
        // si el haz es cone/iso). Reproduce el summing por coincidencia.
        for (G4double e : energies) FireGamma(event, e, pos, SampleDirection(beam, srcZ));
        return;
    }

    // Muestrea una sola linea segun los pesos.
    const G4double wsum = std::accumulate(weights.begin(), weights.end(), 0.0);
    const G4double r = G4UniformRand() * wsum;
    G4double acc = 0.0;
    std::size_t idx = energies.size() - 1;
    for (std::size_t i = 0; i < weights.size(); ++i) {
        acc += weights[i];
        if (r <= acc) { idx = i; break; }
    }
    FireGamma(event, energies[idx], pos, SampleDirection(beam, srcZ));
}

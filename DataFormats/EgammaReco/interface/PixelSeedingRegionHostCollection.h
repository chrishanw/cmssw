#ifndef DataFormats_EgammaReco_interface_PixelSeedingHostCollection_h
#define DataFormats_EgammaReco_interface_PixelSeedingHostCollection_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/EgammaReco/interface/PixelSeedingRegionSoA.h"
#include "DataFormats/EgammaReco/interface/PixelSeedingRegionsSoA.h"
#include "DataFormats/EgammaReco/interface/PixelSeedingTrackingHitsSoA.h"

namespace reco {

  // Egamma pixel seeding SoAs in host memory
  using PixelSeedingRegionHostCollection = PortableHostCollection<PixelSeedingRegionSoA>;
  using PixelSeedingRegionsHostCollection = PortableHostCollection<PixelSeedingRegionsSoA>;
  using PixelSeedingTrackingHitsSoAHostCollection = PortableHostCollection<PixelSeedingTrackingHitsSoA>;
}  // namespace reco

#endif
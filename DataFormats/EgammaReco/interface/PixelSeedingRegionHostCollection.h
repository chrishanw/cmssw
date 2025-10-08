#ifndef DataFormats_EgammaReco_interface_PixelSeedingHostCollection_h
#define DataFormats_EgammaReco_interface_PixelSeedingHostCollection_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/EgammaReco/interface/PixelSeedingRegionSoA.h"

namespace reco {

  // Egamma pixel seeding SoAs in host memory
  using PixelSeedingRegionHostCollection = PortableHostCollection<PixelSeedingRegionSoA>;
}  // namespace reco

#endif
#ifndef DataFormats_EgammaReco_interface_alpaka_PixelSeedingDeviceCollection_h
#define DataFormats_EgammaReco_interface_alpaka_PixelSeedingDeviceCollection_h

#include <Eigen/Core>
#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

#include "DataFormats/EgammaReco/interface/PixelSeedingRegionSoA.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  namespace reco {
    using namespace ::reco;
    using PixelSeedingRegionDeviceCollection = PortableCollection<PixelSeedingRegionSoA>;
  }  // namespace reco
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // DataFormats_EgammaReco_interface_alpaka_PixelSeedingDeviceCollection_h
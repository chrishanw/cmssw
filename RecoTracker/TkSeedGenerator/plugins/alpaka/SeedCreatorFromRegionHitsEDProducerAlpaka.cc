#include <Eigen/Core>

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDGetToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/Event.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EventSetup.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/EgammaReco/interface/PixelSeedingRegionHostCollection.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/Math/interface/AlgebraicROOTObjects.h"
#include "DataFormats/TrackerRecHit2D/interface/BaseTrackerRecHit.h"

#include "RecoTracker/TkHitPairs/interface/RegionsSeedingHitSets.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingHitSet.h"
#include "RecoTracker/TkTrackingRegions/interface/RectangularEtaPhiTrackingRegion.h"

#include "DataFormats/Math/interface/AlgebraicROOTObjects.h"

#include <algorithm>

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class SeedCreatorFromRegionHitsEDProducerAlpaka : public stream::EDProducer<> {
  public:
    SeedCreatorFromRegionHitsEDProducerAlpaka(const edm::ParameterSet& iConfig);
    ~SeedCreatorFromRegionHitsEDProducerAlpaka() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    void produce(device::Event&, device::EventSetup const&) override;

  private:
    edm::EDGetTokenT<RegionsSeedingHitSets> seedingHitSetsToken_;
    device::EDPutToken<TrajectorySeedCollection> trajectorySeedCollectionToken_;
  };

  SeedCreatorFromRegionHitsEDProducerAlpaka::SeedCreatorFromRegionHitsEDProducerAlpaka(
      const edm::ParameterSet& iConfig)
      : seedingHitSetsToken_(consumes<RegionsSeedingHitSets>(iConfig.getParameter<edm::InputTag>("seedingHitSets"))) {
  }

  void SeedCreatorFromRegionHitsEDProducerAlpaka::fillDescriptions(
      edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<edm::InputTag>("seedingHitSets", edm::InputTag("hitPairEDProducer"));

    descriptions.addWithDefaultLabel(desc);
  }

  void SeedCreatorFromRegionHitsEDProducerAlpaka::produce(device::Event& iEvent, const device::EventSetup& iSetup) {
    const auto hseedingHitSets = iEvent.getHandle(seedingHitSetsToken_);
    // type of seedingHitSets is RegionsSeedingHitSets
    const auto& seedingHitSets = *hseedingHitSets;

    auto seeds = std::make_unique<TrajectorySeedCollection>();
    seeds->reserve(seedingHitSets.size());

    std::vector<reco::PixelSeedingRegionHostCollection*> psrhcs;
    psrhcs.reserve(seedingHitSets.size());

    // type of regionSeedingHitSets is ihd::RegionLayerSets<SeedingHitSet>
    for (const auto& regionSeedingHitSets : seedingHitSets) {
      const size_t nMultipletsInRegion = std::distance(regionSeedingHitSets.begin(), regionSeedingHitSets.end());
      const int multipletSize = regionSeedingHitSets.begin()->size();
      // Should be dealing with either doublets or triplets at this point
      assert(multipletSize == 2 or multipletSize == 3);
      const bool isTriplet = (multipletSize == 3);

      // stack host memory in the queue
      reco::PixelSeedingRegionHostCollection psrhc(nMultipletsInRegion, iEvent.queue());
      psrhc.view().isTriplet() = isTriplet;
      psrhc.view().size() = nMultipletsInRegion;

      // set default values of 0, 
      // TODO: need to check whether this is reasonable or not
      psrhc.view().ptMin() = 0.0;
      psrhc.view().vertexRBound() = 0.0;
      psrhc.view().vertexZBound() = 0.0;
      psrhc.view().tanLambdaRangeMin() = 0.0;
      psrhc.view().tanLambdaRangeMax() = 0.0;
      psrhc.view().phiMarginLeft() = 0.0;
      psrhc.view().phiMarginRight() = 0.0;
      psrhc.view().directionX() = 0.0;
      psrhc.view().directionY() = 0.0;
      psrhc.view().directionZ() = 0.0;
      psrhc.view().vertexPosX() = 0.0;
      psrhc.view().vertexPosY() = 0.0;
      psrhc.view().vertexPosZ() = 0.0;

      const TrackingRegion& region = regionSeedingHitSets.region();

      if (region.direction().x() != 0 /* &&
          forceKinematicWithRegionDirection_*/)  // a direction was given, check if it is an etaPhi region
      {
        const RectangularEtaPhiTrackingRegion* etaPhiRegion = dynamic_cast<const RectangularEtaPhiTrackingRegion*>(&region);
        if (etaPhiRegion) {
          psrhc.view().ptMin() = region.ptMin();
          psrhc.view().vertexRBound() = region.originRBound();  // assume equal cxx cyy
          psrhc.view().vertexZBound() = region.originZBound();
          psrhc.view().tanLambdaRangeMin() = etaPhiRegion->tanLambdaRange().first;
          psrhc.view().tanLambdaRangeMax() = etaPhiRegion->tanLambdaRange().second;
          psrhc.view().phiMarginLeft() = etaPhiRegion->phiMargin().left();
          psrhc.view().phiMarginRight() = etaPhiRegion->phiMargin().right();
          const GlobalVector& direction = region.direction() / region.direction().mag();
          psrhc.view().directionX() = direction.x();
          psrhc.view().directionY() = direction.y();
          psrhc.view().directionZ() = direction.z();
          const GlobalPoint& vertexPos = region.origin();
          psrhc.view().vertexPosX() = vertexPos.x();
          psrhc.view().vertexPosY() = vertexPos.y();
          psrhc.view().vertexPosZ() = vertexPos.z();
        }
      }

      uint i = 0; 
      for (const SeedingHitSet& hits : regionSeedingHitSets) {

        const auto toEigen = [](const auto& mat) {
          using Matrix4d = Eigen::Matrix<double, 4, 4>;
          const Matrix4d out{{mat.cxx(), mat.cyx(), mat.czx(), mat.ctx()},
                             {mat.cyx(), mat.cyy(), mat.czy(), mat.cty()},
                             {mat.czx(), mat.czy(), mat.czz(), mat.ctz()},
                             {mat.ctx(), mat.cty(), mat.ctz(), mat.ctt()}};
          return out;
        };

        SeedingHitSet::ConstRecHitPointer tth0 = hits[0];
        const auto& locPos0 = tth0->localPosition();
        const auto& locPosErr0 = tth0->localPositionError();
        const auto& globPos0 = tth0->globalPosition();
        psrhc.view()[i].hit0Detid() = tth0->rawId();
        psrhc.view()[i].hit0LocalPos() = Eigen::Vector3d(locPos0.x(), locPos0.y(), locPos0.z());
        psrhc.view()[i].hit0LocalPosError() = Eigen::Vector3d(locPosErr0.xx(), locPosErr0.xy(), locPosErr0.yy());
        psrhc.view()[i].hit0GlobalPos() = Eigen::Vector3d(globPos0.x(), globPos0.y(), globPos0.z());
         psrhc.view()[i].hit0GlobalPosError() = toEigen(tth0->globalPositionError());

        SeedingHitSet::ConstRecHitPointer tth1 = hits[1];
        const auto& locPos1 = tth1->localPosition();
        const auto& locPosErr1 = tth1->localPositionError();
        const auto& globPos1 = tth1->globalPosition();
        psrhc.view()[i].hit1Detid() = tth1->rawId();
        psrhc.view()[i].hit1LocalPos() = Eigen::Vector3d(locPos1.x(), locPos1.y(), locPos1.z());
        psrhc.view()[i].hit1LocalPosError() = Eigen::Vector3d(locPosErr1.xx(), locPosErr1.xy(), locPosErr1.yy());
        psrhc.view()[i].hit1GlobalPos() = Eigen::Vector3d(globPos1.x(), globPos1.y(), globPos1.z());
        psrhc.view()[i].hit1GlobalPosError() = toEigen(tth1->globalPositionError());

        if (isTriplet) {
          SeedingHitSet::ConstRecHitPointer tth2 = hits[2];
          const auto& locPos2 = tth2->localPosition();
          const auto& locPosErr2 = tth2->localPositionError();
          const auto& globPos2 = tth2->globalPosition();
          psrhc.view()[i].hit2Detid() = tth2->rawId();
          psrhc.view()[i].hit2LocalPos() = Eigen::Vector3d(locPos2.x(), locPos2.y(), locPos2.z());
          psrhc.view()[i].hit2LocalPosError() = Eigen::Vector3d(locPosErr2.xx(), locPosErr2.xy(), locPosErr2.yy());
          psrhc.view()[i].hit2GlobalPos() = Eigen::Vector3d(globPos2.x(), globPos2.y(), globPos2.z());
          psrhc.view()[i].hit2GlobalPosError() = toEigen(tth2->globalPositionError());
        } else {
          psrhc.view()[i].hit2Detid() = 0;
          psrhc.view()[i].hit2LocalPos().setZero();
          psrhc.view()[i].hit2LocalPosError().setZero();
          psrhc.view()[i].hit2GlobalPos().setZero();
          psrhc.view()[i].hit2GlobalPosError().setZero();
        }
        i++;
      }

      psrhcs.push_back(&psrhc);
    }

    // seeds->shrink_to_fit();
    // iEvent.put(std::move(seeds));
  }
} // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(SeedCreatorFromRegionHitsEDProducerAlpaka);

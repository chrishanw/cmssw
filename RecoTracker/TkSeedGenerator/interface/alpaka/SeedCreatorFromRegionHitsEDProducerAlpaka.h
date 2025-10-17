#ifndef RecoTracker_TkSeedGenerator_interface_alpaka_SeedCreatorFromRegionHitsEDProducerAlpaka_H
#define RecoTracker_TkSeedGenerator_interface_alpaka_SeedCreatorFromRegionHitsEDProducerAlpaka_H

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
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

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class SeedCreatorFromRegionHitsEDProducerAlpaka : public global::EDProducer<> {
  public:
    SeedCreatorFromRegionHitsEDProducerAlpaka(const edm::ParameterSet& iConfig);
    ~SeedCreatorFromRegionHitsEDProducerAlpaka() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  private:
    edm::EDGetTokenT<RegionsSeedingHitSets> seedingHitSetsToken_;
  };

  SeedCreatorFromRegionHitsEDProducerAlpaka::SeedCreatorFromRegionHitsEDProducerAlpaka(
      const edm::ParameterSet& iConfig)
      : seedingHitSetsToken_(consumes<RegionsSeedingHitSets>(iConfig.getParameter<edm::InputTag>("seedingHitSets")))
    edm::ConsumesCollector iC = consumesCollector();

    produces<TrajectorySeedCollection>();
  }

  void SeedCreatorFromRegionHitsEDProducerAlpaka::fillDescriptions(
      edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<edm::InputTag>("seedingHitSets", edm::InputTag("hitPairEDProducer"));

    auto label = std::string("seedCreatorFromRegion") + T_SeedCreator::fillDescriptionsLabel() + "EDProducer";
    descriptions.add(label, desc);
  }

  void SeedCreatorFromRegionHitsEDProducerAlpaka::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
    edm::Handle<RegionsSeedingHitSets> hseedingHitSets;
    iEvent.getByToken(seedingHitSetsToken_, hseedingHitSets);
    const auto& seedingHitSets = *hseedingHitSets;

    auto seeds = std::make_unique<TrajectorySeedCollection>();
    seeds->reserve(seedingHitSets.size());

    for (const auto& regionSeedingHitSets : seedingHitSets) {
      const TrackingRegion& region = regionSeedingHitSets.region();
      
      const size_t nMultipletsInRegion = regionSeedingHitSets.size();
      const int multipletSize = regionSeedingHitSets.at(0).size();
      // Should be dealing with either doublets or triplets at this point
      assert(multipletSize == 2 or multipletSize == 3);
      const bool isTriplet = (multipletSize == 3);

      // stack host memory in the queue
      PixelSeedingRegionHostCollection psrhc(nMultipletsInRegion, event.queue());
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


      if (region.direction().x() != 0 &&
          forceKinematicWithRegionDirection_)  // a direction was given, check if it is an etaPhi region
      {
        const RectangularEtaPhiTrackingRegion etaPhiRegion = dynamic_cast<const RectangularEtaPhiTrackingRegion>(region);
        if (etaPhiRegion) {
          psrhc.view().ptMin() = region.ptMin();
          psrhc.view().vertexRBound() = region.originRBound();  // assume equal cxx cyy
          psrhc.view().vertexZBound() = region.originZBound();
          psrhc.view().tanLambdaRangeMin() = etaPhiRegion.tanLambdaRange().first;
          psrhc.view().tanLambdaRangeMax() = etaPhiRegion.tanLambdaRange().second;
          psrhc.view().phiMarginLeft() = etaPhiRegion.phiMargin().left();
          psrhc.view().phiMarginRight() = etaPhiRegion.phiMargin().right();
          const GlobalVector& direction = region.direction() / region.direction().mag();
          psrhc.view().directionX() = direction.X();
          psrhc.view().directionY() = direction.Y();
          psrhc.view().directionZ() = direction.Z();
          const GlobalPoint& vertexPos = region.origin();
          psrhc.view().vertexPosX() = vertexPos.X();
          psrhc.view().vertexPosY() = vertexPos.Y();
          psrhc.view().vertexPosZ() = vertexPos.Z();
        }
      }

      uint i = 0; 
      for (const SeedingHitSet& hits : regionSeedingHitSets) {
        SeedingHitSet::ConstRecHitPointer tth0 = hits[0];
        SeedingHitSet::ConstRecHitPointer tth1 = hits[1];

        psrhc.view().hit0Detid()[i] = tth0->rawId();
        psrhc.view().hit0LocalPos()[i] = tth0->localPosition();
        psrhc.view().hit0LocalPosError()[i] = tth0->localPositionError();
        psrhc.view().hit0GlobalPos()[i] = tth0->globalPosition();
        psrhc.view().hit0GlobalPosError()[i] = tth0->globalPositionError();
        psrhc.view().hit1Detid()[i] = tth1->rawId();
        psrhc.view().hit1LocalPos()[i] = tth1->localPosition();
        psrhc.view().hit1LocalPosError()[i] = tth0->localPositionError();
        psrhc.view().hit1GlobalPos()[i] = tth1->globalPosition();
        psrhc.view().hit1GlobalPosError()[i] = tth1->globalPositionError();
        if (isTriplet) {
          SeedingHitSet::ConstRecHitPointer tth2 = hits[2];
          psrhc.view().hit2Detid()[i] = tth2->rawId();
          psrhc.view().hit2LocalPos()[i] = tth2->localPosition();
          psrhc.view().hit2LocalPosError()[i] = tth2->localPositionError();
          psrhc.view().hit2GlobalPos()[i] = tth2->globalPosition();
          psrhc.view().hit2GlobalPosError()[i] = tth2->globalPositionError();
        } else {
          psrhc.view().hit2Detid()[i] = 0;
          psrhc.view().hit2LocalPos()[i] = {0.0, 0.0, 0.0};
          psrhc.view().hit2LocalPosError()[i] = {0.0, 0.0, 0.0};
          psrhc.view().hit2GlobalPos()[i] = {0.0, 0.0, 0.0};
          psrhc.view().hit2GlobalPosError()[i] = AlgebraicSymMatrix44();
            // {{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}};
        }
        i++;
      }

    }

    seeds->shrink_to_fit();
    iEvent.put(std::move(seeds));
  }
} // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif

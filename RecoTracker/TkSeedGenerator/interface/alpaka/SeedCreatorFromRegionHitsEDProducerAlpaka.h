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

#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"

#include "RecoTracker/TkHitPairs/interface/RegionsSeedingHitSets.h"


namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class SeedCreatorFromRegionHitsEDProducerAlpaka : public global::EDProducer<> {
  public:
    SeedCreatorFromRegionHitsEDProducerAlpaka(const edm::ParameterSet& iConfig);
    ~SeedCreatorFromRegionHitsEDProducerAlpaka() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  private:
    edm::EDGetTokenT<RegionsSeedingHitSets> seedingHitSetsToken_;
    std::unique_ptr<SeedComparitor> comparitor_;
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

    }

    seeds->shrink_to_fit();
    iEvent.put(std::move(seeds));
  }
} // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif

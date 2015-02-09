#include "FWCore/Framework/interface/one/EDProducerBase.h"

#include "EventFilter/L1TRawToDigi/interface/Packer.h"
#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "EventFilter/L1TRawToDigi/interface/PackingSetup.h"

#include "CaloCollections.h"
#include "CaloTokens.h"

namespace l1t {
   namespace stage1 {
      class CaloSetup : public PackingSetup {
         public:
            virtual std::unique_ptr<PackerTokens> registerConsumes(const edm::ParameterSet& cfg, edm::ConsumesCollector& cc) override {
               return std::unique_ptr<PackerTokens>(new CaloTokens(cfg, cc));
            };

            virtual PackerMap getPackers(int fed, int fw) override {
               PackerMap res;

               res[{1, 0x100D}] = {
                  PackerFactory::get()->make("stage1::IsoEGammaPacker"),
                  PackerFactory::get()->make("stage1::NonIsoEGammaPacker"),
                  PackerFactory::get()->make("stage1::CentralJetPacker"),
                  PackerFactory::get()->make("stage1::ForwardJetPacker"),
                  PackerFactory::get()->make("stage1::TauPacker"),
                  PackerFactory::get()->make("stage1::IsoTauPacker"),
                  PackerFactory::get()->make("stage1::EtSumPacker"),
                  PackerFactory::get()->make("stage1::MissEtPacker"),
                  PackerFactory::get()->make("stage1::CaloSpareHFPacker"),
                  PackerFactory::get()->make("stage1::MissHtPacker"),
                  PackerFactory::get()->make("stage1::RCTEmRegionPacker"),      
               };
               res[{1, 0x100E}] = {
                  PackerFactory::get()->make("stage1::RCTEmRegionPacker"),      
               };

               return res;
            };

            virtual void registerProducts(edm::one::EDProducerBase& prod) override {
               prod.produces<L1CaloEmCollection>();
               prod.produces<CaloSpareBxCollection>("HFBitCounts");
               prod.produces<CaloSpareBxCollection>("HFRingSums");
               prod.produces<L1CaloRegionCollection>();
               prod.produces<CaloTowerBxCollection>();
               prod.produces<EGammaBxCollection>();
               prod.produces<EtSumBxCollection>();
               prod.produces<JetBxCollection>();
               prod.produces<TauBxCollection>("rlxTaus");
               prod.produces<TauBxCollection>("isoTaus");
            };

            virtual std::unique_ptr<UnpackerCollections> getCollections(edm::Event& e) override {
               return std::unique_ptr<UnpackerCollections>(new CaloCollections(e));
            };

            virtual UnpackerMap getUnpackers(int fed, int board, int amc, int fw) override {
               UnpackerMap res;

               auto cjet_unp = UnpackerFactory::get()->make("stage1::CentralJetUnpacker");
               auto fjet_unp = UnpackerFactory::get()->make("stage1::ForwardJetUnpacker");
               auto iegamma_unp = UnpackerFactory::get()->make("stage1::IsoEGammaUnpacker");
               auto niegamma_unp = UnpackerFactory::get()->make("stage1::NonIsoEGammaUnpacker");
               auto etsum_unp = UnpackerFactory::get()->make("stage1::EtSumUnpacker");
               auto missetsum_unp = UnpackerFactory::get()->make("stage1::MissEtUnpacker");
               //auto tau_unp = UnpackerFactory::get()->make("stage1::TauUnpacker");
               //auto isotau_unp = UnpackerFactory::get()->make("stage1::IsoTauUnpacker");
               auto calospare_unp = UnpackerFactory::get()->make("stage1::CaloSpareHFUnpacker");
               auto misshtsum_unp = UnpackerFactory::get()->make("stage1::MissHtUnpacker");

                if (fed == 1352) {
                  auto rctRegion_unp = UnpackerFactory::get()->make("stage1::RCTRegionUnpacker");
                  auto rctEm_unp = UnpackerFactory::get()->make("stage1::RCTEmUnpacker");
                  
                  if(board == 4109){  

                    res[93] = cjet_unp;
                    res[95] = cjet_unp;
                    res[97] = fjet_unp;
                    res[99] = fjet_unp;
                    res[101] = niegamma_unp;
                    res[103] = niegamma_unp;
                    res[105] = iegamma_unp;
                    res[107] = iegamma_unp;
                    res[109] = etsum_unp;
                    res[111] = missetsum_unp;
                    res[113] = calospare_unp;
                    res[115] = misshtsum_unp;

                  }
                  for (int m=0;m<36;m++) {
                    if (board == 4109) {
                      res[m*2] = rctRegion_unp;
                    }
                    else if (board == 4110) {
                      res[m*2] = rctEm_unp;
                    }
                  }
               }
               return res;
            };
      };
   }
}

DEFINE_L1T_PACKING_SETUP(l1t::stage1::CaloSetup);

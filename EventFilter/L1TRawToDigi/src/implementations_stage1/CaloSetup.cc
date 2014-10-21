#include "FWCore/Framework/interface/one/EDProducerBase.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"
#include "EventFilter/L1TRawToDigi/interface/UnpackerProvider.h"

#include "CaloCollections.h"

namespace l1t {
  namespace stage1 {
   class CaloSetup : public UnpackerProvider {
      public:
         CaloSetup(edm::one::EDProducerBase& prod) : UnpackerProvider(prod) {
            prod.produces<JetBxCollection>("stage1");
         };

         virtual std::unique_ptr<UnpackerCollections> getCollections(edm::Event& e) override {
            return std::unique_ptr<UnpackerCollections>(new CaloCollections(e));
         };

         virtual UnpackerMap getUnpackers(int fed, int amc, int fw) override {

            auto jet_unp = UnpackerFactory::get()->make("stage1::JetUnpacker");

            UnpackerMap res;
            res[73] = jet_unp;

            return res;
         };
   };
  }
}

DEFINE_L1T_UNPACKER_PROVIDER(l1t::stage1::CaloSetup);

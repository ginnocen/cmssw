#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "DataFormats/L1CaloTrigger/interface/L1CaloEmCand.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegion.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegionDetId.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"

#include "CaloCollections.h"

namespace l1t {
  namespace stage1 {
    class RCTUnpacker : public Unpacker {
      public:
        virtual bool unpack(const Block& block, UnpackerCollections *coll) override;
      private:
        unsigned int counter_ = 0;
    };
  }
}

// Implementation

namespace l1t {
  namespace stage1 {
    bool RCTUnpacker::unpack(const Block& block, UnpackerCollections *coll){

      LogDebug("L1T") << "Block ID  = " << block.header().getID() << " size = " << block.header().getSize();

      int nBX = int(ceil(block.header().getSize() / 2.)); 

      // Find the first and last BXs
      int firstBX = -(ceil((double)nBX/2.)-1);
      int lastBX;
      if (nBX % 2 == 0) {
        lastBX = ceil((double)nBX/2.)+1;
      } else {
        lastBX = ceil((double)nBX/2.);
      }

      auto ems_ = static_cast<CaloCollections*>(coll)->getCTEMCands();
      auto regs_ = static_cast<CaloCollections*>(coll)->getCTRegions();
      ems_->setBXRange(firstBX, lastBX);
      regs_->setBXRange(firstBX, lastBX);

      LogDebug("L1T") << "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX;

      // Initialise index
      int unsigned i = 0;

      for (int bx=firstBX; bx<lastBX; bx++){

        unsigned int crate=-1;
        bool even;

        RCTInfoFactory rctInfoFactory;
        RCTInfo myrctinfo;

        rctInfoFactory.decodeCapturedLinkID(block.header().getID(), crate, counter_, even);

        std::vector <uint32_t> uint;
        uint.reserve(6);

        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);

        if (odd) {

          rctInfoFactory->produceEven(uint,myrctinfo);

          for(int j = 0; j < 4; j++) {

            L1CaloEmCand em = L1CaloEmCand(rctInfo.neRank[j], 
                rctInfo.neRegn[j], 
                rctInfo.neCard[j], 
                rctInfo.crateID, 
                false);

            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > *p4 =
              new ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >();

            //l1t::CaloStage1Cluster cluster;
            CaloEmCand EmCand(*p4,
                (int) em.rank(),
                (int) em.regionId().ieta(),
                (int) em.regionId().iphi(),
                0);

            res1_->push_back( bx, EmCand);
            
           //etc etc 
           //etc etc 

          }
        }
        else(){

          rctInfoFactory->produceOdd(uint,myrctinfo);
           //etc etc 
           //etc etc 

        }
      }

      return true;

    }
  }
}

DEFINE_L1T_UNPACKER(l1t::stage1::RCTUnpacker);

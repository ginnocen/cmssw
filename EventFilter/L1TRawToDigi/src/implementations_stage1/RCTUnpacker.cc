#include "FWCore/Framework/interface/MakerMacros.h"
#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"
#include "EventFilter/L1TRawToDigi/interface/RCTInfo.hh"
#include "EventFilter/L1TRawToDigi/interface/RCTInfoFactory.hh"

#include "DataFormats/L1CaloTrigger/interface/L1CaloEmCand.h"
#include "DataFormats/L1TCalorimeter/interface/CaloEmCand.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegion.h"
#include "DataFormats/L1TCalorimeter/interface/CaloRegion.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegionDetId.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"


#include <iostream>
#include <fstream>

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

      int nBX = int(ceil(block.header().getSize() / 6.)); 

      // Find the first and last BXs
      int firstBX = -(ceil((double)nBX/2.)-1);
      int lastBX;
      if (nBX % 2 == 0) {
        lastBX = ceil((double)nBX/2.)+1;
      } else {
        lastBX = ceil((double)nBX/2.);
      }
    
        auto resRCTRegions_ = static_cast<CaloCollections*>(coll)->getCaloRegions();
        resRCTRegions_->setBXRange(std::min(firstBX, resRCTRegions_->getFirstBX()), std::max(lastBX, resRCTRegions_->getLastBX()));

        auto resRCTEMCands_ = static_cast<CaloCollections*>(coll)->getCaloEmCands();
        resRCTEMCands_->setBXRange(std::min(firstBX, resRCTEMCands_->getFirstBX()), std::max(lastBX, resRCTEMCands_->getLastBX()));

      // Initialise index
      int unsigned i = 0;

      for (int bx=firstBX; bx<lastBX; bx++){

        unsigned int crate;
        bool even=0;

        RCTInfoFactory rctInfoFactory;
        std::vector<RCTInfo> rctInfoArray;
        std::vector <uint32_t> uint;
        uint.reserve(6);

        int mp7link=(int)(block.header().getID()/2);
        int indexfromMP7tooRSC[36]={0,1,18,19,16,17,34,35,2,3,20,21,14,15,32,33,4,5,22,23,12,13,30,31,6,7,24,25,10,11,28,29,8,9,26,27};

        int oRSClink=indexfromMP7tooRSC[mp7link];
        crate=(int)(oRSClink/2);
        if (oRSClink%2==0) even=true;
        else even=false;

        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);

        rctInfoFactory.produce(uint,uint,rctInfoArray);
        rctInfoFactory.setRCTInfoCrateID(rctInfoArray, crate);
        RCTInfo rctInfo=rctInfoArray.at(0);

        LogDebug("L1T")<<"--------------- mp7 link ="<<mp7link<<"RCT crate id="<<crate<<", RCT crate even="<<even<<std::endl;

        if(!even) {

          for(int j = 0; j < 4; j++) {

            LogDebug("L1T") <<"index="<<j<<", neRank="<<rctInfo.neRank[j]<<", neRegn="<<rctInfo.neRegn[j]<<", neCard="<<rctInfo.neCard[j]<<std::endl;
            L1CaloEmCand em = L1CaloEmCand(rctInfo.neRank[j],rctInfo.neRegn[j],rctInfo.neCard[j],rctInfo.crateID,false);
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > *p4 =new ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >();
            CaloEmCand EmCand(*p4,(int) em.rank(),(int) em.regionId().ieta(),(int) em.regionId().iphi(),0);
            resRCTEMCands_->push_back(bx,EmCand);
          }

          for(int j = 0; j < 4; j++) {

            LogDebug("L1T") <<"index="<<j<<", ieRank="<<rctInfo.ieRank[j]<<", neRegn="<<rctInfo.ieRegn[j]<<", neCard="<<rctInfo.ieCard[j]<<std::endl;
            L1CaloEmCand em = L1CaloEmCand(rctInfo.ieRank[j],rctInfo.ieRegn[j],rctInfo.ieCard[j],rctInfo.crateID,true);
            ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > *p4 =new ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >();
            CaloEmCand EmCand(*p4,(int) em.rank(),(int) em.regionId().ieta(),(int) em.regionId().iphi(),0);
            resRCTEMCands_->push_back(bx,EmCand);
          }

          for(int j = 0; j < 2; j++) {
            for(int k = 0; k < 4; k++) {
              LogDebug("L1T") <<"region HF ="<<j<<", card="<<k<<", rgnEt="<<rctInfo.hfEt[j][k]<<std::endl;
              L1CaloRegion rgn = L1CaloRegion(rctInfo.hfEt[j][k], 0,  rctInfo.crateID , (j * 2 +  k));
              ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > *p4 =new ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >();
              CaloRegion region(*p4,0.,0.,(int) rgn.et(),(int) rgn.id().ieta(),(int) rgn.id().iphi(),0.,0.);
              resRCTRegions_->push_back(bx,region);
            }
          }
        }// end if odd

        else{
          for(int j = 0; j < 7; j++) {
            for(int k = 0; k < 2; k++) {

              LogDebug("L1T") <<"region="<<j<<", card="<<k<<", rgnEt="<<rctInfo.rgnEt[j][k]<<std::endl;
              bool o = (((rctInfo.oBits >> (j * 4 + k)) && 0x1) == 0x1);
              bool t = (((rctInfo.tBits >> (j * 4 + k)) && 0x1) == 0x1);
              bool m = (((rctInfo.mBits >> (j * 4 + k)) && 0x1) == 0x1);
              bool q = (((rctInfo.qBits >> (j * 4 + k)) && 0x1) == 0x1);

              L1CaloRegion rgn = L1CaloRegion(rctInfo.rgnEt[j][k],o,t,m,q,rctInfo.crateID,j,k);     
              ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > *p4 =new ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >();	     
              CaloRegion region(*p4,0.,0.,(int) rgn.et(),(int) rgn.id().ieta(),(int) rgn.id().iphi(),0,0);
              resRCTRegions_->push_back(bx,region);
            }
          }
        }// end if even
      }// end of loop over BX
      return true;
    }
  }
}

DEFINE_L1T_UNPACKER(l1t::stage1::RCTUnpacker);

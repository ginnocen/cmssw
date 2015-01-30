#include "FWCore/Framework/interface/MakerMacros.h"
#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"
#include "PhysicsToBitConverter.h"
#include "rctDataBase.h"


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
      /*

         int nBX = int(ceil(block.header().getSize() / 6.)); 

      // Find the first and last BXs
      int firstBX = -(ceil((double)nBX/2.)-1);
      int lastBX;
      if (nBX % 2 == 0) {
      lastBX = ceil((double)nBX/2.)+1;
      } else {
      lastBX = ceil((double)nBX/2.);
      }
      */
      auto resRCTRegions_ = static_cast<CaloCollections*>(coll)->getCaloRegions();
      //resRCTRegions_->setBXRange(std::min(firstBX, resRCTRegions_->getFirstBX()), std::max(lastBX, resRCTRegions_->getLastBX()));
      auto resRCTEMCands_ = static_cast<CaloCollections*>(coll)->getCaloEmCands();
      //resRCTEMCands_->setBXRange(std::min(firstBX, resRCTEMCands_->getFirstBX()), std::max(lastBX, resRCTEMCands_->getLastBX()));


      // Initialise index
      int unsigned i = 0;
      std::vector <uint32_t> uint;
      uint.reserve(6);

      for (int bx=0; bx<1; bx++){

        unsigned int crate;
        bool even=0;

        PhysicsToBitConverter converter;
        rctDataBase database;
        int mp7link=(int)(block.header().getID()/2);
        database.GetLinkRCT(mp7link,crate,even);

        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);
        uint.push_back(block.payload()[i++]);

        LogDebug("L1T")<<"--------------- mp7 link ="<<mp7link<<"RCT crate id="<<crate<<", RCT crate even="<<even<<std::endl;

        if(!even) {

          for(int i=0;i<6;i++) converter.Set32bitWordLinkOdd(i,uint[i]);
          converter.Convert();

          for(int j = 0; j < 8; j++) {

            unsigned int hfet=(unsigned int)converter.GetHFEt(j);

            LogDebug("L1T")<<"UNPACKER, CRATE"<<crate<<"region="<<j<<", rgnEt="<<hfet<<std::endl;
            L1CaloRegion rgn = L1CaloRegion(hfet,0,crate,j);
            resRCTRegions_->push_back(rgn);
          }
          
          for(int j = 0; j < 4; j++) {

            unsigned int rank=(unsigned int)converter.GetNEEt(j);
            unsigned int reg=(unsigned int)converter.GetNEReg(j);
            unsigned int card=(unsigned int)converter.GetNECard(j);

            LogDebug("L1T")<<"UNPACKER, CRATE"<<crate<<"NON ISO em rank="<<rank<<", region="<<reg<<", card="<<card<<std::endl;

            L1CaloEmCand em = L1CaloEmCand(rank,reg,card,crate,false);
            resRCTEMCands_->push_back(em);
          }

          for(int j = 0; j < 4; j++) {

            unsigned int rank=converter.GetIEEt(j);
            unsigned int reg=converter.GetIEReg(j);
            unsigned int card=converter.GetIECard(j);

            LogDebug("L1T")<<"UNPACKER, CRATE"<<crate<<"ISO em rank="<<rank<<", region="<<reg<<", card="<<card<<std::endl;
            L1CaloEmCand em = L1CaloEmCand(rank,reg,card,crate,true);
            resRCTEMCands_->push_back(em);
          }

        }// end if odd


        else{

          for(int i=0;i<6;i++) converter.Set32bitWordLinkEven(i,uint[i]);
          converter.Convert();

          for(int j = 0; j < 7; j++) {
            for(int k = 0; k < 2; k++) {

              unsigned int RCet=(unsigned int)converter.GetRCEt(j,k);
              bool overflow=(bool)converter.GetRCOf(j,k);
              bool tauveto=(bool)converter.GetRCTau(j,k);
              bool hadveto=(bool)converter.GetRCHad(j,k);
              bool quiet=false;                                //to be fixed

              LogDebug("L1T")<<"UNPACKER, CRATE="<<crate<<",region="<<k<<", card="<<j<<", rgnEt="<<RCet<<", overflow="<<overflow<<", tauveto="<<tauveto<<", hadveto="<<hadveto<<std::endl;
              L1CaloRegion rgn = L1CaloRegion(RCet,overflow,tauveto,hadveto,quiet,crate,j,k);     
              resRCTRegions_->push_back(rgn);
            }
          }
        }// end if even
      }// end of loop over BX 
      return true;
    }
  }
}

DEFINE_L1T_UNPACKER(l1t::stage1::RCTUnpacker);

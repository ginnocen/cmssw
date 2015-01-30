#include "FWCore/Framework/interface/Event.h"

#include "EventFilter/L1TRawToDigi/interface/Packer.h"

#include "DataFormats/L1CaloTrigger/interface/L1CaloEmCand.h"
#include "DataFormats/L1TCalorimeter/interface/CaloEmCand.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegion.h"
#include "DataFormats/L1TCalorimeter/interface/CaloRegion.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloRegionDetId.h"
#include "DataFormats/L1CaloTrigger/interface/L1CaloCollections.h"

#include "PhysicsToBitConverter.h"
#include "rctDataBase.h"


#include "CaloTokens.h"

namespace l1t {
  namespace stage1 {
    class EmRegionPacker : public Packer {
      public:
        virtual Blocks pack(const edm::Event&, const PackerTokens*) override;
    };
  }
}

// Implementation

namespace l1t {
  namespace stage1 {
    Blocks
      EmRegionPacker::pack(const edm::Event& event, const PackerTokens* toks){ 
        edm::Handle<L1CaloRegionCollection> caloregion;
        event.getByToken(static_cast<const CaloTokens*>(toks)->getCaloRegionToken(), caloregion);

        edm::Handle<L1CaloEmCollection> caloemcand;
        event.getByToken(static_cast<const CaloTokens*>(toks)->getCaloEmCandToken(), caloemcand);

        std::vector<uint32_t> load[36];

        for (int i = 0; i <= 0; ++i) {
          int n = 0;
          PhysicsToBitConverter converter[18];
          for (auto j = caloregion->begin(); j != caloregion->end(); ++j, ++n) {

            int et=(int)j->et();
            int overFlow=(int)j->overFlow();
            int fineGrain=(int)j->fineGrain();
            int mip=(int)j->mip();
            //int quiet=(int)j->quiet();
            int rctCrate=(int)j->rctCrate();
            int rctCard=(int)j->rctCard();
            int rctRegion=(int)j->rctRegionIndex();  
            bool isHf=(bool)j->isHf();  

            if(!isHf){
              converter[rctCrate].SetRCEt(et,rctCard,rctRegion);
              converter[rctCrate].SetRCOf(overFlow,rctCard,rctRegion);
              converter[rctCrate].SetRCTau(fineGrain,rctCard,rctRegion);
              converter[rctCrate].SetRCHad(mip,rctCard,rctRegion);
              std::cout<<"CRATE"<<rctCrate<<"region="<<rctRegion<<", card="<<rctCard<<", rgnEt="<<et<<", overflow="<<overFlow<<", tauveto="<<fineGrain<<", hadveto="<<mip<<std::endl;

            }
            else{
              converter[rctCrate].SetHFEt(et,rctRegion);
            }
          }//end calo region

          int m = 0;
          for (auto j = caloemcand->begin(); j != caloemcand->end(); ++j, ++m) {

            int rank=(int)j->rank();
            int index=(int)j->index();
            int rctCrate=(int)j->rctCrate();
            bool isolated=(bool)j->isolated();  

            if(isolated){
              converter[rctCrate].SetIEEt(rank,index);
            }
            else{
              converter[rctCrate].SetNEEt(rank,index);
            }
          }//end of em cand

          for (int in=0;in<18;in++){
            converter[in].Extract32bitwords();
            for (int d=0;d<6;d++){
              load[2*in].push_back((uint32_t)converter[in].Get32bitWordLinkEven(d));
              load[2*in+1].push_back((uint32_t)converter[in].Get32bitWordLinkOdd(d));
            }
          }
        }
        
        rctDataBase database;

        Blocks res = {}; 
        
        for (int i = 0; i < 36; ++i) {
          unsigned int mycrateRCT=(int)(i/2);
          bool myRCTeven;
          if (i%2==0) myRCTeven=true;
          else myRCTeven=false;
          int linkMP7=-1;
          database.GetLinkMP7(mycrateRCT,myRCTeven,linkMP7);
          res.push_back(Block(2*linkMP7, load[i])); 
          std::cout<<"index RCT ="<<i<<",size="<<load[i].size()<<"index MP7="<<linkMP7<<"Block ID="<<2*linkMP7<<std::endl;
        }
        return res;
      }
  }
}
DEFINE_L1T_PACKER(l1t::stage1::EmRegionPacker);

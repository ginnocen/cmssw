#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "CaloCollections.h"

namespace l1t {
   class JetUnpacker : public Unpacker {
      public:
         virtual bool unpack(const unsigned block_id, const unsigned size, const unsigned char *data, UnpackerCollections *coll) override;
   };
}

// Implementation


namespace l1t {
   bool
   JetUnpacker::unpack(const unsigned block_id, const unsigned size, const unsigned char *data, UnpackerCollections *coll)
   {

     std::cout<< "Block ID  = " << block_id << " size = " << size;

     int nBX =1;
     int lastBX=1;
     int firstBX=0;
    
     auto res_ = static_cast<CaloCollections*>(coll)->getJets();
     res_->setBXRange(firstBX, lastBX);
     
     std::cout<< "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX<<std::endl;
     
     int unsigned i = 0;
     uint32_t raw_data0 = pop(data,i); 
     uint32_t raw_data1 = pop(data,i); 
     
     if ((raw_data0==0) || (raw_data1==0)) return false;
     
     std::cout<<"++++++++++++++ word0="<<std::hex<<raw_data0<<std::endl;
     std::cout<<"++++++++++++++ word1="<<std::hex<<raw_data1<<std::endl;
    
     uint16_t jet[4];
     jet[0] = raw_data0 & 0xFFFF;
     jet[1] = (raw_data0 >> 16) & 0xFFFF;
     jet[2] = raw_data1 & 0xFFFF;
     jet[3] = (raw_data1 >> 16) & 0xFFFF;
     
     std::cout<<"++++++++++++++ jet0="<<std::hex<<jet[0]<<std::endl;
     std::cout<<"++++++++++++++ jet1="<<std::hex<<jet[1]<<std::endl;
     std::cout<<"++++++++++++++ jet2="<<std::hex<<jet[2]<<std::endl;
     std::cout<<"++++++++++++++ jet3="<<std::hex<<jet[3]<<std::endl;
     
     int jetPt;
     int jetEta;
     int jetEtasign;
     int jetPhi;
     int jetqualflag;
     
     
     for (int ijet=0;ijet<4;ijet++){
     
     
       jetPt=jet[ijet] & 0x3F;
       jetEta=(jet[ijet]>>6 ) & 0x7;
       jetEtasign=(jet[ijet]>>9) & 0x1;
       jetPhi=(jet[ijet]>>10) & 0x1F;
       jetqualflag=(jet[ijet]>>15) & 0x1;
       
       std::cout<<"=================="<<std::endl;
       std::cout<<"jet number ="<<ijet<<std::endl;
       std::cout<<"jetPt ="<<std::hex<<jetPt<<std::endl;
       std::cout<<"jetEta ="<<std::hex<<jetEta<<std::endl;
       std::cout<<"jetEtasign ="<<std::hex<<jetEtasign<<std::endl;
       std::cout<<"jetPhi ="<<std::hex<<jetPhi<<std::endl;
       std::cout<<"jetqualflag ="<<std::hex<<jetqualflag<<std::endl;
       
       l1t::Jet jet = l1t::Jet();
       jet.setHwPt(jetPt);
       if(jetEtasign) jet.setHwEta(-1 * jetEta);
       else jet.setHwEta(jetEta);
       jet.setHwPhi(jetPhi);
       jet.setHwQual(jetqualflag);
       std::cout << "Jet: eta " << jet.hwEta() << " phi " << jet.hwPhi() << " pT " << jet.hwPt() << " qual " << jet.hwQual();
       res_->push_back(0,jet);
       
     }
     return true;
  } 
}

/*
namespace l1t {
   bool
   JetUnpacker::unpack(const unsigned block_id, const unsigned size, const unsigned char *data, UnpackerCollections *coll)
   {

     LogDebug("L1T") << "Block ID  = " << block_id << " size = " << size;

     int nBX = int(ceil(size / 12.)); // Since there are 12 jets reported per event (see CMS IN-2013/005)
     std::cout<<"this is the size of the object="<<size<<std::endl;
     std::cout<<"this is the number of BX="<<nBX<<std::endl;
     
     
     
     // Find the first and last BXs
     int firstBX = -(ceil((double)nBX/2.)-1);
     int lastBX;
     if (nBX % 2 == 0) {
       lastBX = ceil((double)nBX/2.)+1;
     } else {
       lastBX = ceil((double)nBX/2.);
     }
     
     std::cout<<"first BX index="<<firstBX<<std::endl;
     std::cout<<"last BX index="<<lastBX<<std::endl;

     auto res_ = static_cast<CaloCollections*>(coll)->getJets();
     res_->setBXRange(firstBX, lastBX);

     LogDebug("L1T") << "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX;

     // Initialise index
     int unsigned i = 0;

     // Loop over multiple BX and then number of jets filling jet collection
     for (int bx=firstBX; bx<lastBX; bx++){
       for (unsigned nJet=0; nJet < 12 && nJet < size; nJet++){
         uint32_t raw_data = pop(data,i); // pop advances the index i internally
         std::cout<<"++++++++++++++ my datas="<<std::hex<<raw_data<<std::endl;
         if (raw_data == 0)
            continue;

         l1t::Jet jet = l1t::Jet();

         jet.setHwPt(raw_data & 0x7FF);
         
         int abs_eta = (raw_data >> 11) & 0x7F;
         if ((raw_data >> 18) & 0x1) {
           jet.setHwEta(-1 * abs_eta);
         } else {
           jet.setHwEta(abs_eta);
         }

	 jet.setHwPhi((raw_data >> 19) & 0xFF);
         jet.setHwQual((raw_data >> 27) & 0x7); // Assume 3 bits for now? Leaves 2 bits spare

         LogDebug("L1T") << "Jet: eta " << jet.hwEta() << " phi " << jet.hwPhi() << " pT " << jet.hwPt() << " qual " << jet.hwQual();

         res_->push_back(bx,jet);
       }
     }

     return true;
   }
}
*/

DEFINE_L1T_UNPACKER(l1t::JetUnpacker);

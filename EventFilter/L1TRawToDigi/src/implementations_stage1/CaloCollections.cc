#include "FWCore/Framework/interface/Event.h"

#include "CaloCollections.h"

namespace l1t {
  namespace stage1 {
    CaloCollections::~CaloCollections()
    {
      event_.put(jets_,"stage1");
    }   
  }
}

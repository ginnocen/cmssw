///
/// \class l1t::Stage1Layer2CentralityAlgorithm
///
/// \authors: Gian Michele Innocenti
///           R. Alex Barbieri
///
/// Description: Centrality Algorithm HI

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "L1Trigger/L1TCalorimeter/interface/Stage1Layer2HFBitCountAlgorithmImp.h"
#include "L1Trigger/L1TCalorimeter/interface/PUSubtractionMethods.h"
#include "L1Trigger/L1TCalorimeter/interface/legacyGtHelper.h"

l1t::Stage1Layer2CentralityAlgorithm::Stage1Layer2CentralityAlgorithm(CaloParamsStage1* params) : params_(params)
{

}


l1t::Stage1Layer2CentralityAlgorithm::~Stage1Layer2CentralityAlgorithm() {


}


void l1t::Stage1Layer2CentralityAlgorithm::processEvent(const std::vector<l1t::CaloRegion> & regions,
							const std::vector<l1t::CaloEmCand> & EMCands,
							std::vector<l1t::CaloSpare> * spares) {
  
  double sumET=0.;
  double regionET=0.; 
  
  double sumETPlus=0.;
  double sumETMinus=0.;
  
  double regionETPlus=0.;    
  double regionETMinus=0.; 

  for(std::vector<CaloRegion>::const_iterator region = regions.begin(); region != regions.end(); region++) {

    if (region->hwEta() > 3 && region->hwEta() < 18) {
      continue;
    }
    regionETPlus=0.;
    regionETMinus=0.;
    regionET=0.;
    
    if(region->hwEta() >= 18) regionETPlus=region->hwPt();
    if(region->hwEta() <= 3) regionETMinus=region->hwPt();
    regionET=region->hwPt();
    
    sumETPlus +=regionETPlus;
    sumETMinus +=regionETMinus; 
    sumET +=regionET;
  } 
  
  ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > dummy(0,0,0,0);
  l1t::CaloSpare centrality (*&dummy,CaloSpare::CaloSpareType::Centrality,(int)sumETPlus,(int)sumETMinus,(int)sumET,0);
  spares->push_back(centrality);
}

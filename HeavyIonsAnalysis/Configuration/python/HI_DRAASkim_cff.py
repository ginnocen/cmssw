import FWCore.ParameterSet.Config as cms

import HLTrigger.HLTfilters.hltHighLevel_cfi
hltDmeson60 = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
hltDmeson60.HLTPaths = ["HLT_DmesonTrackingGlobalPt8_Dpt60_*"]
hltDmeson60.throw = False
hltDmeson60.andOr = True

HIDmeson60SkimSequence = cms.Sequence(
	hltDmeson60
)

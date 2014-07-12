import FWCore.ParameterSet.Config as cms

mp7BufferDumpToRaw = cms.EDProducer(
    "l1t::MP7BufferDumpToRaw",
    rxFile           = cms.untracked.string("rx_summary.txt"),
    txFile           = cms.untracked.string("tx_summary.txt"),

    # buffer dump file input parameters
    nTextHeaderLines = cms.untracked.int32(3),
    nFramesPerEvent  = cms.untracked.int32(45),
    txLatency        = cms.untracked.int32(61),
    nRxLinks         = cms.untracked.int32(72),
    nTxLinks         = cms.untracked.int32(72),

    # DAQ parameters
    fedId            = cms.untracked.int32(1),
    eventType        = cms.untracked.int32(238),
    fwVersion        = cms.untracked.int32(255),
    lenSlinkHeader   = cms.untracked.int32(16),  # length in 8 bit words !
    lenSlinkTrailer  = cms.untracked.int32(8),
    lenAMC13Header   = cms.untracked.int32(0),
    lenAMC13Trailer  = cms.untracked.int32(0),
    lenAMCHeader     = cms.untracked.int32(12),
    lenAMCTrailer    = cms.untracked.int32(8),
    rxBlockLength    = cms.untracked.vint32(41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41,
                                            41,41,41,41,41,41,41,41,41),
    txBlockLength    = cms.untracked.vint32(12,4,12,8,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0)

)

import FWCore.ParameterSet.Config as cms

source = cms.Source("EmptySource")

generator = cms.EDFilter("Pythia8GeneratorFilter",
    crossSection = cms.untracked.double(71.39e+09),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    filterEfficiency = cms.untracked.double(1.0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    ElectronPositronInitialState=cms.PSet(),
    comEnergy = cms.double(13.5),
    PythiaParameters = cms.PSet(
        processParameters = cms.vstring(
      'PDF:lepton = off',
      'WeakSingleBoson:ffbar2gmZ = on',
      '23:onMode = off',
      '23:onIfAny = 1 2 3 4 5'),
        parameterSets = cms.vstring('processParameters')
    )
)

ProductionFilterSequence = cms.Sequence(generator)

import FWCore.ParameterSet.Config as cms

process = cms.Process("JetTriggerAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 0

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '140X_dataRun3_Prompt_v4', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring('root://cmsxrootd.fnal.gov//store/data/Run2024G/Muon0/MINIAOD/PromptReco-v1/000/383/811/00000/bc8c0364-5756-4b72-a331-544fb21d1be0.root'),
      )

import FWCore.PythonUtilities.LumiList as LumiList

lumiB = LumiList.LumiList(filename = 'Cert_Collisions2024_eraB_Golden.json')
lumiC = LumiList.LumiList(filename = 'Cert_Collisions2024_eraC_Golden.json')
lumiD = LumiList.LumiList(filename = 'Cert_Collisions2024_eraD_Golden.json')
lumiE = LumiList.LumiList(filename = 'Cert_Collisions2024_eraE_Golden.json')
lumiF = LumiList.LumiList(filename = 'Cert_Collisions2024_eraF_Golden.json')

process.source.lumisToProcess = lumiB.getVLuminosityBlockRange() + lumiC.getVLuminosityBlockRange() + lumiD.getVLuminosityBlockRange() + lumiE.getVLuminosityBlockRange() + lumiF.getVLuminosityBlockRange()

process.TFileService = cms.Service("TFileService",
                                      fileName=cms.string("jettrigger.root"))

process.trg_analyzer = cms.EDAnalyzer('JetTriggerAnalyzer',
                 jets = cms.untracked.InputTag("slimmedJets","","RECO"),
                 ak8jets = cms.untracked.InputTag("slimmedJetsAK8","","RECO"),
                 hltresults = cms.untracked.InputTag("TriggerResults","","HLT"),
                 stageL1Trigger = cms.uint32(2),
                )

process.p = cms.Path(process.trg_analyzer)

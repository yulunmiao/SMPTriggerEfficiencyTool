#!/bin/bash

CMSSWVERSION="14_0_14"
GLOBALTAG="140X_dataRun3_Prompt_v4"

OUTPUTPATH="/store/group/lpcvvv/ymiao/Trigger/JetTrigger2024G/"
OUTPUTPATHPHOTON="/store/group/lpcvvv/ymiao/Trigger/PhotonTrigger2024G/"

INPUTFILE=$1
OUTPUTFILE="output_$2.root"

OUTPUTFULLPATH="$OUTPUTPATH$OUTPUTFILE"
OUTPUTFULLPATHPHOTON="$OUTPUTPATHPHOTON$OUTPUTFILE"

INPUTFILE=${INPUTFILE//\/store/root:\/\/cms-xrd-global.cern.ch\/\/store}
INPUTFILE=${INPUTFILE//,/\',\'}

tar -vxf CMSSW_$CMSSWVERSION.tar
source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=el9_amd64_gcc12
cd CMSSW_$CMSSWVERSION/src/
scramv1 b ProjectRename
cmsenv
export SCRAM_ARCH=el9_amd64_gcc12
cmsenv
scramv1 b
cd SMP/TriggerAnalyzer/test/
cat>run_jet_analyze.py<<EOF
import FWCore.ParameterSet.Config as cms

process = cms.Process("JetTriggerAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 0

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '$GLOBALTAG', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
      fileNames = cms.untracked.vstring('$INPUTFILE'),
      )

import FWCore.PythonUtilities.LumiList as LumiList

lumiG = LumiList.LumiList(filename = 'Cert_Collisions2024_378981_384380_Golden.json')

process.source.lumisToProcess = lumiG.getVLuminosityBlockRange()

process.TFileService = cms.Service("TFileService",
                                      fileName=cms.string("jettrigger.root"))

process.trg_analyzer = cms.EDAnalyzer('JetTriggerAnalyzer',
                 jets = cms.untracked.InputTag("slimmedJets","","RECO"),
                 ak8jets = cms.untracked.InputTag("slimmedJetsAK8","","RECO"),
                 hltresults = cms.untracked.InputTag("TriggerResults","","HLT"),
                 stageL1Trigger = cms.uint32(2),
                )

process.p = cms.Path(process.trg_analyzer)
EOF
cmsRun run_jet_analyze.py

xrdfs root://cmseos.fnal.gov mkdir -p ${OUTPUTPATH}
xrdcp jettrigger.root root://cmseos.fnal.gov//${OUTPUTFULLPATH}

cat>run_photon_analyze.py<<EOF
import FWCore.ParameterSet.Config as cms

process = cms.Process("PhotonTriggerAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 0

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '140X_dataRun3_Prompt_v4', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
      fileNames = cms.untracked.vstring('$INPUTFILE'),
      )

import FWCore.PythonUtilities.LumiList as LumiList
lumiG = LumiList.LumiList(filename = 'Cert_Collisions2024_378981_384380_Golden.json')

process.source.lumisToProcess = lumiG.getVLuminosityBlockRange()

process.TFileService = cms.Service("TFileService",
                                      fileName=cms.string("photontrigger.root"))

process.trg_analyzer = cms.EDAnalyzer('PhotonTriggerAnalyzer',
                 photons = cms.untracked.InputTag("slimmedPhotons","","RECO"),
                 hltresults = cms.untracked.InputTag("TriggerResults","","HLT"),
                 stageL1Trigger = cms.uint32(2),

                )

process.p = cms.Path(process.trg_analyzer)
EOF

cmsRun run_photon_analyze.py

xrdfs root://cmseos.fnal.gov mkdir -p ${OUTPUTPATHPHOTON}
xrdcp photontrigger.root root://cmseos.fnal.gov//${OUTPUTFULLPATHPHOTON}


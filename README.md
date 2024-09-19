# SMP Trigger Efficiency Tool

These codes study the trigger efficiency minitored by SMP, i.e.
HLT_Photon[ET]
HLT_AK8PFJet[PT]
HLT_AK8PFJetFwd[PT]
HLT_PFJetFwd[PT]       
using orthogonal data set(Muon dataset).
The ```condor``` folder contains codes and scripts to submit condor jobs to produce the histograms from MiniAOD, the ```code``` folder contains codes to produce trigger efficiency plots from histograms. 
Here we use study on 2024RunG as example to show how to use them
**N.B. The job submission assumes lpc environment, please modify accordingly if not on lpc cluster**
## Submit Jobs to Produce Histograms from MiniAOD
Go into ```condor``` by
```
cd condor/
```
### Check Prompt Reco MiniAOD 
Use the das website:https://cmsweb.cern.ch/das/, or ```dasgoclient``` on a cluster with proxy as following
```
dasgoclient -query="dataset=/Muon0/Run2024G-PromptReco*/MINIAOD"
dasgoclient -query="dataset=/Muon1/Run2024G-PromptReco*/MINIAOD"
```
This returns the datasets
```
/Muon0/Run2024G-PromptReco-v1/MINIAOD
/Muon1/Run2024G-PromptReco-v1/MINIAOD
```
### Check CMSSW version
Get the CMSSW version information by
```
dasgoclient -query="release dataset=/Muon0/Run2024G-PromptReco-v1/MINIAOD"
dasgoclient -query="release dataset=/Muon1/Run2024G-PromptReco-v1/MINIAOD"
```
This give the CMSSW release(s) for using this dataset
```
CMSSW_14_0_12
CMSSW_14_0_13
CMSSW_14_0_14
CMSSW_14_0_15
```
Using the highest version here should do the job, I used the CMSSW_14_0_14.
Check out CMSSW and put the histogram producer into it by
```
cmsrel CMSSW_14_0_14
cp -r SMP/ ./CMSSW_14_0_14/src
```

### Check https://cms-service-dqmdc.web.cern.ch/CAF/certification/ for json file
Use json file corresponding to the Run and copy it into, here we use /Collisions24/Cert_Collisions2024_378981_384380_Golden.json
```
wget https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24/Cert_Collisions2024_378981_384380_Golden.json
cp Cert_Collisions2024_378981_384380_Golden.json ./CMSSW_14_0_14/src/SMP/TriggerAnalyzer/test/
```
### Compress CMMSW into tarball
```
tar -vcf CMSSW_14_0_14.tar ./CMSSW_14_0_14/ 
```
### Check https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions for global tag
The global tag for 2024RunG is ```140X_dataRun3_Prompt_v4```
### Modify exe.sh
```
......
CMSSWVERSION="14_0_14"
GLOBALTAG="140X_dataRun3_Prompt_v4"
OUTPUTPATH="/store/group/lpcvvv/ymiao/Trigger/JetTrigger2024G/"
OUTPUTPATHPHOTON="/store/group/lpcvvv/ymiao/Trigger/PhotonTrigger2024G/"
......
lumiG = LumiList.LumiList(filename = 'Cert_Collisions2024_378981_384380_Golden.json')

process.source.lumisToProcess = lumiG.getVLuminosityBlockRange()
......
```
**N.B. if not on lpc, the whole output part need to be changed, i.e., the following block**
```
......
xrdfs root://cmseos.fnal.gov mkdir -p ${OUTPUTPATH}
xrdcp jettrigger.root root://cmseos.fnal.gov//${OUTPUTFULLPATH}
......
xrdfs root://cmseos.fnal.gov mkdir -p ${OUTPUTPATHPHOTON}
xrdcp photontrigger.root root://cmseos.fnal.gov//${OUTPUTFULLPATHPHOTON}
......
```
need to be changed corresponding to cluster
### Write jbl file and submit
The jbl file for RunG is ```submit2024G.jbl```. Submit the job by
```
condor_submit submit2024G.jbl
```
## Produce Efficiency Plot Locally
Once all condor job is done, merge them to two root files: one with jet triggers and one with photon trigger and copy them into ```hist```. Example output can already be found.
### Modify Codes and Run
For example in ```AK4PFJetFwdEfficiency.cc```, change the following lines
```
......
TFile *f = new TFile("../hist/JetTrigger2024G.root");
......
c->SaveAs("Run2024G_PFjetfwd_eta.png");
......
c1->SaveAs("Run2024G_PFjetfwd_eta.png");
......
``` 
and run by
```
root -l AK4PFJetFwdEfficiency.cc
```
Then efficiency plots will be ready in the ```code``` folder

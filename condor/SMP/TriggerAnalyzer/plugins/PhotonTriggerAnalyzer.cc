// -*- C++ -*-
//
// Package:    SMP/PhotonTriggerAnalyzer
// Class:      PhotonTriggerAnalyzer
//
/**\class PhotonTriggerAnalyzer PhotonTriggerAnalyzer.cc SMP/PhotonTriggerAnalyzer/plugins/PhotonTriggerAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Oliver Suranyi
//         Created:  Mon, 29 Aug 2022 14:58:43 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"

#include "TTree.h"
#include "TH1.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

using reco::TrackCollection;

const static int MAX = 200;

class PhotonTriggerAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns> {
public:
  explicit PhotonTriggerAnalyzer(const edm::ParameterSet&);
  ~PhotonTriggerAnalyzer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void beginRun(const edm::Run&, const edm::EventSetup&) override;
  virtual void endRun(const edm::Run&, const edm::EventSetup&) override {}
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  //TTree* Events;
  int evt, run, lumi;
  bool firstEvent;

  edm::InputTag trigResultsTag_;
//  HLTPrescaleProvider hltPrescaleProvider_;
  edm::Service<TFileService> fs;
  TH1D *acc_pt[1000],*acc_pt_ps[1000],*acc_eta[1000],*acc_eta_ps[1000],*inclusive_pt[1000],*inclusive_eta[1000];
  int pt_threshold_photon[4] = {120,150,175,200};
  // ----------member data ---------------------------
  edm::EDGetTokenT<std::vector<pat::Photon>> photonToken_;
  edm::EDGetTokenT<edm::TriggerResults> hltToken_;
//#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
//  edm::ESGetToken<SetupData, SetupRecord> setupToken_;
//#endif
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
PhotonTriggerAnalyzer::PhotonTriggerAnalyzer(const edm::ParameterSet& iConfig)
    :firstEvent(true),
    trigResultsTag_(iConfig.getUntrackedParameter<edm::InputTag>("hltresults")),
//    hltPrescaleProvider_(iConfig, consumesCollector(), *this),
    photonToken_(consumes<std::vector<pat::Photon>>(iConfig.getUntrackedParameter<edm::InputTag>("photons"))),
    hltToken_(consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("hltresults"))){
        usesResource("TFileService");

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
}

PhotonTriggerAnalyzer::~PhotonTriggerAnalyzer() {}

//
// member functions
//
void PhotonTriggerAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup){
  bool changed=false;
//  hltPrescaleProvider_.init(iRun,iSetup,trigResultsTag_.process(),changed);
}
// ------------ method called for each event  ------------
void PhotonTriggerAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  evt = iEvent.id().event();
  run = iEvent.id().run();
  lumi = iEvent.id().luminosityBlock();

  edm::Handle<edm::TriggerResults> hltresults;
  iEvent.getByToken(hltToken_,hltresults); 
  edm::TriggerNames const& triggerNames = iEvent.triggerNames(*hltresults);
  int n=0;
  
  bool tightphoton=false;
  float tightphotonet;
  float tightphotoneta;
  //Photon
  for (const auto& photon : iEvent.get(photonToken_)) {
    if(photon.photonID("cutBasedPhotonID-Fall17-94X-V2-tight")){
      tightphotonet=photon.et();
      tightphotoneta=photon.eta();
      tightphoton=true;
      break;
    }
  }

  for(unsigned int itrig = 0; itrig < hltresults->size(); itrig++){
    TString trigName = triggerNames.triggerName(itrig);
    //Photon trigger
    for(const auto& threshold : pt_threshold_photon){
      TString name = TString::Format("HLT_Photon%d_v",threshold);
      if(trigName.BeginsWith(name)){
        if(firstEvent){
          acc_pt[n]=fs->make<TH1D>(name+"_pt_acc" , name+"_pt_acc" ,40 , 50 , 250 );
          acc_pt_ps[n]=fs->make<TH1D>(name+"_pt_acc_ps" , name+"_pt_acc" ,40 , 50 , 250 );
          inclusive_pt[n]=fs->make<TH1D>(name+"_pt_inclusive" , name+"_pt_inclusive" , 40 , 50 , 250 );
          
          acc_eta[n]=fs->make<TH1D>(name+"_eta_acc" , name+"_eta_acc" , 25 , -5 , 5 );
          acc_eta_ps[n]=fs->make<TH1D>(name+"_eta_acc_ps" , name+"_eta_acc_ps" , 25 , -5 , 5 );
          inclusive_eta[n]=fs->make<TH1D>(name+"_eta_inclusive" , name+"_eta_inclusive" , 25 , -5 , 5);
        }
        if(tightphoton){
          inclusive_pt[n]->Fill(tightphotonet);
          if(hltresults->accept(itrig)){
            acc_pt[n]->Fill(tightphotonet);
//            acc_pt_ps[n]->Fill(tightphotonet,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);

          }
        }
        if(tightphoton && tightphotonet>threshold){
          inclusive_eta[n]->Fill(tightphotoneta);
          if(hltresults->accept(itrig)){
            acc_eta[n]->Fill(tightphotoneta);
//            acc_eta_ps[n]->Fill(tightphotoneta,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        n++;
      }
    }

  }
  firstEvent = false;
}

// ------------ method called once each job just before starting event loop  ------------
void PhotonTriggerAnalyzer::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void PhotonTriggerAnalyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void PhotonTriggerAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PhotonTriggerAnalyzer);

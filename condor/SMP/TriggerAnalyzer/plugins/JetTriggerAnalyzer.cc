// -*- C++ -*-
//
// Package:    SMP/JetTriggerAnalyzer
// Class:      JetTriggerAnalyzer
//
/**\class JetTriggerAnalyzer JetTriggerAnalyzer.cc SMP/JetTriggerAnalyzer/plugins/JetTriggerAnalyzer.cc

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

class JetTriggerAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns> {
public:
  explicit JetTriggerAnalyzer(const edm::ParameterSet&);
  ~JetTriggerAnalyzer() override;

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
  int pt_threshold_ak8_fwd[10] = {40,60,80,140,200,260,320,400,450,500};
  int pt_threshold_ak8[11] = {40,60,80,140,200,260,320,400,450,500,550};
  int pt_threshold_ak4_fwd[10] = {40,60,80,140,200,260,320,400,450,500};
  int pt_threshold_ak4[12] = {40,60,80,110,140,200,260,320,400,450,500,550};
  // ----------member data ---------------------------
  edm::EDGetTokenT<std::vector<pat::Jet>> jetToken_;  //used to select what tracks to read from configuration file
  edm::EDGetTokenT<std::vector<pat::Jet>> ak8JetToken_;
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
JetTriggerAnalyzer::JetTriggerAnalyzer(const edm::ParameterSet& iConfig)
    :firstEvent(true),
    trigResultsTag_(iConfig.getUntrackedParameter<edm::InputTag>("hltresults")),
//    hltPrescaleProvider_(iConfig, consumesCollector(), *this),
    jetToken_(consumes<std::vector<pat::Jet>>(iConfig.getUntrackedParameter<edm::InputTag>("jets"))),
    ak8JetToken_(consumes<std::vector<pat::Jet>>(iConfig.getUntrackedParameter<edm::InputTag>("ak8jets"))),
    hltToken_(consumes<edm::TriggerResults>(iConfig.getUntrackedParameter<edm::InputTag>("hltresults"))){
        usesResource("TFileService");

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
}

JetTriggerAnalyzer::~JetTriggerAnalyzer() {}

//
// member functions
//
void JetTriggerAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup){
  bool changed=false;
//  hltPrescaleProvider_.init(iRun,iSetup,trigResultsTag_.process(),changed);
}
// ------------ method called for each event  ------------
void JetTriggerAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  evt = iEvent.id().event();
  run = iEvent.id().run();
  lumi = iEvent.id().luminosityBlock();

  edm::Handle<edm::TriggerResults> hltresults;
  iEvent.getByToken(hltToken_,hltresults); 
  edm::TriggerNames const& triggerNames = iEvent.triggerNames(*hltresults);
  int n=0;
  float ak8fwdpt;
  bool bak8fwdpt=false;
  float leadingak8pt=-1.;
  float leadingak8eta=-100.;
  float ak4fwdpt;
  bool bak4fwdpt=false;
  float leadingak4pt=-1.;
  float leadingak4eta=-100.;
  
  //AK8jets
  if(iEvent.get(ak8JetToken_).size()!=0){
    leadingak8pt=iEvent.get(ak8JetToken_)[0].pt();
    leadingak8eta=iEvent.get(ak8JetToken_)[0].eta();
  }

  for (const auto& ak8jet : iEvent.get(ak8JetToken_)) {
    if (abs(ak8jet.eta())>3.2){
      ak8fwdpt=ak8jet.pt();
      bak8fwdpt=true;
      break;
    }
  }
  //AK4jets
  if(iEvent.get(jetToken_).size()!=0){
    leadingak4pt=iEvent.get(jetToken_)[0].pt();
    leadingak4eta=iEvent.get(jetToken_)[0].eta();
  }
  for (const auto& jet : iEvent.get(jetToken_)) {
    if (abs(jet.eta())>3.2){
      ak4fwdpt=jet.pt();
      bak4fwdpt=true;
      break;
    }
  }

  for(unsigned int itrig = 0; itrig < hltresults->size(); itrig++){
    TString trigName = triggerNames.triggerName(itrig);
    //Forward AK8jet trigger
    for(const auto& threshold : pt_threshold_ak8_fwd){
      TString name = TString::Format("HLT_AK8PFJetFwd%d_v",threshold);
      if(trigName.BeginsWith(name)){
        if(firstEvent){
          acc_pt[n]=fs->make<TH1D>(name+"_pt_acc" , name+"_pt_acc" , 65 , 50 , 700 );
          acc_pt_ps[n]=fs->make<TH1D>(name+"_pt_acc_ps" , name+"_pt_acc_ps" , 65 , 50 , 700 );
          inclusive_pt[n]=fs->make<TH1D>(name+"_pt_inclusive" , name+"_pt_inclusive" , 65 , 50 , 700 );

          acc_eta[n]=fs->make<TH1D>(name+"_eta_acc" , name+"_eta_acc" , 25 , -5 , 5 );
          acc_eta_ps[n]=fs->make<TH1D>(name+"_eta_acc_ps" , name+"_eta_acc_ps" , 25 , -5 , 5 );
          inclusive_eta[n]=fs->make<TH1D>(name+"_eta_inclusive" , name+"_eta_inclusive" , 25 , -5 , 5);
        }
        //if(hltPrescaleProvider_.rejectedByHLTPrescaler(*hltresults,itrig)){/
        //  n++;
        //  continue;
        //}
        if(bak8fwdpt){
          inclusive_pt[n]->Fill(ak8fwdpt);
          if(hltresults->accept(itrig)){
            acc_pt[n]->Fill(ak8fwdpt);
    //        acc_pt_ps[n]->Fill(ak8fwdpt,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        if (leadingak8pt>threshold){              
          inclusive_eta[n]->Fill(leadingak8eta);
          if(hltresults->accept(itrig)){
            acc_eta[n]->Fill(leadingak8eta);
     //       acc_eta_ps[n]->Fill(leadingak8eta,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        n++;
      }
    }
    //AK8jet trigger
    for(const auto& threshold : pt_threshold_ak8){
      TString name = TString::Format("HLT_AK8PFJet%d_v",threshold);
      if(trigName.BeginsWith(name)){
        if(firstEvent){
          acc_pt[n]=fs->make<TH1D>(name+"_pt_acc" , name+"_pt_acc" , 65 , 50 , 700 );
          acc_pt_ps[n]=fs->make<TH1D>(name+"_pt_acc_ps" , name+"_pt_acc_ps" , 65 , 50 , 700 );         
          inclusive_pt[n]=fs->make<TH1D>(name+"_pt_inclusive" , name+"_pt_inclusive" , 65 , 50 , 700 );

          acc_eta[n]=fs->make<TH1D>(name+"_eta_acc" , name+"_eta_acc" , 25 , -5 , 5 );
          acc_eta_ps[n]=fs->make<TH1D>(name+"_eta_acc_ps" , name+"_eta_acc_ps" , 25 , -5 , 5 );
          inclusive_eta[n]=fs->make<TH1D>(name+"_eta_inclusive" , name+"_eta_inclusive" , 25 , -5 , 5);
        }
        //if(hltPrescaleProvider_.rejectedByHLTPrescaler(*hltresults,itrig)){
        //  n++;
        //  continue;
        //}
        if(leadingak8pt>0){
          inclusive_pt[n]->Fill(leadingak8pt);
          if(hltresults->accept(itrig)){
            acc_pt[n]->Fill(leadingak8pt);
//            acc_pt_ps[n]->Fill(leadingak8pt,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        if(leadingak8pt>threshold){
          inclusive_eta[n]->Fill(leadingak8eta);
          if(hltresults->accept(itrig)){
            acc_eta[n]->Fill(leadingak8eta);
//            acc_eta_ps[n]->Fill(leadingak8eta,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        n++;
      }
    }
    //Forward AK4jet trigger
    for(const auto& threshold : pt_threshold_ak4_fwd){
      TString name = TString::Format("HLT_PFJetFwd%d_v",threshold);
      if(trigName.BeginsWith(name)){
        if(firstEvent){
          acc_pt[n]=fs->make<TH1D>(name+"_pt_acc" , name+"_pt_acc" , 65 , 50 , 700 );
          acc_pt_ps[n]=fs->make<TH1D>(name+"_pt_acc_ps" , name+"_pt_acc_ps" , 65 , 50 , 700 );
          inclusive_pt[n]=fs->make<TH1D>(name+"_pt_inclusive" , name+"_pt_inclusive" , 65 , 50 , 700 );

          acc_eta[n]=fs->make<TH1D>(name+"_eta_acc" , name+"_eta_acc" , 25 , -5 , 5 );
          acc_eta_ps[n]=fs->make<TH1D>(name+"_eta_acc_ps" , name+"_eta_acc_ps" , 25 , -5 , 5 );
          inclusive_eta[n]=fs->make<TH1D>(name+"_eta_inclusive" , name+"_eta_inclusive" , 25 , -5 , 5);
        }
        //if(hltPrescaleProvider_.rejectedByHLTPrescaler(*hltresults,itrig)){
        //  n++;
        //  continue;
        //}
        if(bak4fwdpt){
          inclusive_pt[n]->Fill(ak4fwdpt);
          if(hltresults->accept(itrig)){
            acc_pt[n]->Fill(ak4fwdpt);
//            acc_pt_ps[n]->Fill(ak4fwdpt,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        if (leadingak4pt>threshold){              
          inclusive_eta[n]->Fill(leadingak4eta);
          if(hltresults->accept(itrig)){
            acc_eta[n]->Fill(leadingak4eta);
//            acc_eta_ps[n]->Fill(leadingak4eta,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);      
          }
        }
        n++;
      }
    }
    //AK4jet trigger
    for(const auto& threshold : pt_threshold_ak4){
      TString name = TString::Format("HLT_PFJet%d_v",threshold);
      if(trigName.BeginsWith(name)){
        if(firstEvent){
          acc_pt[n]=fs->make<TH1D>(name+"_pt_acc" , name+"_pt_acc" , 65 , 50 , 700 );
          acc_pt_ps[n]=fs->make<TH1D>(name+"_pt_acc_ps" , name+"_pt_acc_ps" , 65 , 50 , 700 );
          inclusive_pt[n]=fs->make<TH1D>(name+"_pt_inclusive" , name+"_pt_inclusive" , 65 , 50 , 700 );

          acc_eta[n]=fs->make<TH1D>(name+"_eta_acc" , name+"_eta_acc" , 25 , -5 , 5 );
          acc_eta_ps[n]=fs->make<TH1D>(name+"_eta_acc_ps" , name+"_eta_acc" , 25 , -5 , 5 );
          inclusive_eta[n]=fs->make<TH1D>(name+"_eta_inclusive" , name+"_eta_inclusive" , 25 , -5 , 5);

        }
        //if(hltPrescaleProvider_.rejectedByHLTPrescaler(*hltresults,itrig)){
        //  n++;
        //  continue;
        //}
        if(leadingak4pt>0){
          inclusive_pt[n]->Fill(leadingak4pt);
          if(hltresults->accept(itrig)){
            acc_pt[n]->Fill(leadingak4pt);
//            acc_pt_ps[n]->Fill(leadingak4pt,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);
          }
        }
        if(leadingak4pt>threshold){
          inclusive_eta[n]->Fill(leadingak4eta);
          if(hltresults->accept(itrig)){
            acc_eta[n]->Fill(leadingak4eta);
//            acc_eta_ps[n]->Fill(leadingak4eta,hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).first*hltPrescaleProvider_.prescaleValues(iEvent,iSetup,trigName.Data()).second);

          }
        }
        n++;
      }
    }
  }
  firstEvent = false;
}

// ------------ method called once each job just before starting event loop  ------------
void JetTriggerAnalyzer::beginJob() {

}

// ------------ method called once each job just after ending the event loop  ------------
void JetTriggerAnalyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void JetTriggerAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
DEFINE_FWK_MODULE(JetTriggerAnalyzer);

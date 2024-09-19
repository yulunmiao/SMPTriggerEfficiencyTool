int PhotonEfficiency(){
    //TFile *f = new TFile("../hist/PhotonTrigger2024BCDEF.root");
    TFile *f = new TFile("../hist/PhotonTrigger2024G.root");
    TString triggername[1]={"HLT_Photon200"};
    
    Color_t color[1]={kBlack};
    TH1D* hpass_pt[1]; 
    TH1D* hinclusive_pt[1];
    TH1D* hpass_eta[1]; 
    TH1D* hinclusive_eta[1];
    TEfficiency* eff_eta[1];
    TEfficiency* eff_pt[1];
    TLegend *l=new TLegend(0.15,0.6,0.45,0.8);
    TLegend *l1=new TLegend(0.35,0.2,0.65,0.4);
    l->SetBorderSize(0);
    l1->SetBorderSize(0);

    for(unsigned int i=0;i<1;i++){
        hpass_pt[i] = (TH1D*) f->Get("trg_analyzer/"+triggername[i]+"_v_pt_acc");
        hinclusive_pt[i] = (TH1D*) f->Get("trg_analyzer/"+triggername[i]+"_v_pt_inclusive");
        eff_pt[i] = new TEfficiency(*hpass_pt[i],*hinclusive_pt[i]);
        eff_pt[i]->SetTitle(";leading photon E_{T}(GeV);efficiency");
        eff_pt[i]->SetMarkerStyle(8);
        eff_pt[i]->SetMarkerColor(color[i]);
        eff_pt[i]->SetLineColor(color[i]);

        hpass_eta[i] = (TH1D*) f->Get("trg_analyzer/"+triggername[i]+"_v_eta_acc");
        hinclusive_eta[i] = (TH1D*) f->Get("trg_analyzer/"+triggername[i]+"_v_eta_inclusive");
        eff_eta[i] = new TEfficiency(*hpass_eta[i],*hinclusive_eta[i]);
        eff_eta[i]->SetTitle(";leading photon #eta;efficiency");
        eff_eta[i]->SetMarkerStyle(8);
        eff_eta[i]->SetMarkerColor(color[i]);
        eff_eta[i]->SetLineColor(color[i]);

        l->AddEntry(eff_pt[i],triggername[i],"p");
        l1->AddEntry(eff_pt[i],triggername[i],"p");
    }
    TCanvas *c=new TCanvas;
    eff_pt[0]->Draw();
    gPad->Update(); 
    eff_pt[0]->GetPaintedGraph()->SetMinimum(0);
    eff_pt[0]->GetPaintedGraph()->SetMaximum(1);

    gPad->Update();
    l->Draw();
    c->SaveAs("Run2024G_Photon_pt.png");

    TCanvas *c1=new TCanvas;
    eff_eta[0]->Draw();
    gPad->Update(); 
    eff_eta[0]->GetPaintedGraph()->SetMinimum(0);
    eff_eta[0]->GetPaintedGraph()->SetMaximum(1);
    gPad->Update();
    l1->Draw();
    c1->SaveAs("Run2024G_Photon_eta.png");
    return 0;
}

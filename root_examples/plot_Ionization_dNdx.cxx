/** \file
 * This is an example to plot dN/dx for Ionization.\n
 * If you have ROOT installed this file will be compilied when running make\n
 * and you can find an executable in your build directory in root_examples:\n
 * -->   Plot_Ionization_dNdx   <--\n
 * A ROOT-file named Ionization_dNdx.root will be generated,
 * which contains TCanvas's with TGraphs dN/dx vs. energy.\n
 * These TGraphs are generated for muons, taus and electrons in \n
 * water, hydrogen and uranium \n
 * You will also find summary plots for every particle and medium.
 * @brief Example to plot dN/dx for Ionization
 * @author Jan-Hendrik Köhne
 */

#include "TGraph.h"
#include "TCanvas.h"
#include "PROPOSAL/Ionization.h"
#include "PROPOSAL/PROPOSALParticle.h"
#include "PROPOSAL/EnergyCutSettings.h"
#include "PROPOSAL/Medium.h"
#include "TFile.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TAxis.h"
#include <cmath>
#include <sstream>

using namespace std;
using namespace PROPOSAL;

int main()
{

    cout<<"\n----------------------------------------------------------------\n"
        <<"This is an example to plot dN/dx for Ionization.\n"
        <<"A ROOT-file named Ionization_dNdx.root will be generated,\n"
        <<"which contains TCanvas's with TGraphs dN/dx vs. energy.\n"
        <<"These TGraphs are generated for muons, taus and electrons in \n"
        <<"water, hydrogen and uranium \n"
        <<"You will also find summary plots for every particle and medium.\n"
        <<"----------------------------------------------------------------\n"
        <<endl;

    TFile *file     =   new TFile("Ionization_dNdx.root","RECREATE");

    PROPOSALParticle *mu    =   new PROPOSALParticle(ParticleType::MuMinus);
    PROPOSALParticle *tau   =   new PROPOSALParticle(ParticleType::TauMinus);
    PROPOSALParticle *e     =   new PROPOSALParticle(ParticleType::EMinus);

    Medium  *med1   =   new Medium("hydrogen",1.);
    Medium  *med2   =   new Medium("water",1.);
    Medium  *med3   =   new Medium("uranium",1.);

    EnergyCutSettings*  cuts    =   new EnergyCutSettings(e->GetMass(),-1);

    vector<Ionization*> ioniz;

    ioniz.push_back(new Ionization(mu,med1,cuts));
    ioniz.push_back(new Ionization(mu,med2,cuts));
    ioniz.push_back(new Ionization(mu,med3,cuts));
    ioniz.push_back(new Ionization(tau,med1,cuts));
    ioniz.push_back(new Ionization(tau,med2,cuts));
    ioniz.push_back(new Ionization(tau,med3,cuts));
    ioniz.push_back(new Ionization(e,med1,cuts));
    ioniz.push_back(new Ionization(e,med2,cuts));
    ioniz.push_back(new Ionization(e,med3,cuts));

    vector<TGraph*> graphs;
    graphs.resize(ioniz.size());


    vector<double> dNdx;
    dNdx.resize(ioniz.size());

    double energy;
    vector<int> counter(graphs.size(),0);

    stringstream graph_name;
    stringstream graph_title;

    vector<TCanvas*> cans;
    cans.resize(graphs.size());

    for(unsigned int i = 0 ; i < ioniz.size() ; i++)
    {
        ioniz.at(i)->EnableDNdxInterpolation();
        graphs.at(i)    =   new TGraph();

        graph_name<<ioniz.at(i)->GetParticle()->GetName()<<"_"<<ioniz.at(i)->GetMedium()->GetName();
        graph_title<<ioniz.at(i)->GetParticle()->GetName()<<" in "<<ioniz.at(i)->GetMedium()->GetName();

        graphs.at(i)->SetName(graph_name.str().c_str());
        graphs.at(i)->SetTitle(graph_title.str().c_str());

        cans.at(i)      =    new TCanvas(graph_name.str().c_str(),graph_name.str().c_str(),1024,768);

        graph_name.str("");
        graph_name.clear();
        graph_title.str("");
        graph_title.clear();

    }

    for(double log_energy = 0 ;  log_energy < 12; log_energy +=0.2)
    {
        for(unsigned int i = 0 ; i < ioniz.size() ; i++)
        {
            energy  =   ioniz.at(i)->GetParticle()->GetMass()   +   pow(10,log_energy);

            ioniz.at(i)->GetParticle()->SetEnergy(energy);

            dNdx.at(i)  =   ioniz.at(i)->CalculatedNdx();

            if(dNdx.at(i) !=0 )
            {
                graphs.at(i)->SetPoint(counter.at(i),energy,dNdx.at(i));
                counter.at(i)++;
            }
        }

    }

    for(unsigned int i = 0 ; i < graphs.size() ; i++)
    {
        cans.at(i)->cd();
        cans.at(i)->SetLogx();
        cans.at(i)->SetLogy();
        cans.at(i)->SetGridx();
        cans.at(i)->SetGridy();
        graphs.at(i)->SetMarkerStyle(3);
        graphs.at(i)->GetXaxis()->SetTitle("energy [MeV]");
        graphs.at(i)->GetXaxis()->SetTitleOffset(1.1);
        graphs.at(i)->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
        graphs.at(i)->GetYaxis()->SetTitleOffset(1.1);
        graphs.at(i)->Draw("AP");
        cans.at(i)->Write();

    }

    TCanvas* muons      =   new TCanvas("muons","muons",1024,768);
    TCanvas* taus       =   new TCanvas("taus","taus",1024,768);
    TCanvas* electrons  =   new TCanvas("electrons","electrons",1024,768);

    TCanvas* hydrogen   =   new TCanvas("hydrogen","hydrogen",1024,768);
    TCanvas* water      =   new TCanvas("water","water",1024,768);
    TCanvas* uranium    =   new TCanvas("uranium","uranium",1024,768);

    TMultiGraph* muons_gr      =   new TMultiGraph("muons","muons");
    TMultiGraph* taus_gr       =   new TMultiGraph("taus","taus");
    TMultiGraph* electrons_gr  =   new TMultiGraph("electrons","electrons");

    TMultiGraph* hydrogen_gr   =   new TMultiGraph("hydrogen","hydrogen");
    TMultiGraph* water_gr      =   new TMultiGraph("water","water");
    TMultiGraph* uranium_gr    =   new TMultiGraph("uranium","uranium");

    TLegend* muons_leg = new TLegend(0.7,0.1,0.9,0.3);
    muons_leg->SetFillColor(0);

    TLegend* taus_leg = new TLegend(0.7,0.1,0.9,0.3);
    taus_leg->SetFillColor(0);

    TLegend* electrons_leg = new TLegend(0.7,0.1,0.9,0.3);
    electrons_leg->SetFillColor(0);

    TLegend* hydrogen_leg = new TLegend(0.7,0.1,0.9,0.3);
    hydrogen_leg->SetFillColor(0);

    TLegend* water_leg = new TLegend(0.7,0.1,0.9,0.3);
    water_leg->SetFillColor(0);

    TLegend* uranium_leg = new TLegend(0.7,0.1,0.9,0.3);
    uranium_leg->SetFillColor(0);


    for(unsigned int i = 0 ; i < graphs.size() ; i++)
    {
        if(ioniz.at(i)->GetParticle()->GetType() == ParticleType::MuMinus)
        {
            muons_gr->Add(graphs.at(i),"P");
            muons_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetMedium()->GetName().c_str(),"p");

            switch (muons_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }

        }
        if(ioniz.at(i)->GetParticle()->GetType() == ParticleType::TauMinus)
        {
            taus_gr->Add(graphs.at(i),"P");
            taus_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetMedium()->GetName().c_str(),"p");

            switch (taus_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }
        }
        if(ioniz.at(i)->GetParticle()->GetType() == ParticleType::EMinus)
        {
            electrons_gr->Add(graphs.at(i),"P");
            electrons_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetMedium()->GetName().c_str(),"p");

            switch (electrons_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }
        }
    }

    muons->cd();
    muons->SetLogx();
    muons->SetLogy();
    muons->SetGridx();
    muons->SetGridy();
    muons_gr->Draw("A");
    muons_gr->GetXaxis()->SetTitle("energy [MeV]");
    muons_gr->GetXaxis()->SetTitleOffset(1.1);
    muons_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    muons_gr->GetYaxis()->SetTitleOffset(1.1);
    muons_leg->Draw("Same");
    muons->Write();

    taus->cd();
    taus->SetLogx();
    taus->SetLogy();
    taus->SetGridx();
    taus->SetGridy();
    taus_gr->Draw("A");
    taus_gr->GetXaxis()->SetTitle("energy [MeV]");
    taus_gr->GetXaxis()->SetTitleOffset(1.1);
    taus_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    taus_gr->GetYaxis()->SetTitleOffset(1.1);
    taus_leg->Draw("Same");
    taus->Write();

    electrons->cd();
    electrons->SetGridx();
    electrons->SetGridy();
    electrons->SetLogx();
    electrons->SetLogy();
    electrons_gr->Draw("A");
    electrons_gr->GetXaxis()->SetTitle("energy [MeV]");
    electrons_gr->GetXaxis()->SetTitleOffset(1.1);
    electrons_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    electrons_gr->GetYaxis()->SetTitleOffset(1.1);
    electrons_leg->Draw("Same");
    electrons->Write();

    for(unsigned int i = 0 ; i < graphs.size() ; i++)
    {
        if(ioniz.at(i)->GetMedium()->GetName().compare("hydrogen")==0)
        {
            hydrogen_gr->Add(graphs.at(i),"P");
            hydrogen_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetParticle()->GetName().c_str(),"p");

            switch (hydrogen_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }
        }
        if(ioniz.at(i)->GetMedium()->GetName().compare("water")==0)
        {
            water_gr->Add(graphs.at(i),"P");
            water_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetParticle()->GetName().c_str(),"p");

            switch (water_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }
        }
        if(ioniz.at(i)->GetMedium()->GetName().compare("uranium")==0)
        {
            uranium_gr->Add(graphs.at(i),"P");
            uranium_leg->AddEntry(graphs.at(i),ioniz.at(i)->GetParticle()->GetName().c_str(),"p");

            switch (uranium_gr->GetListOfGraphs()->Capacity())
            {
                case 1:
                    graphs.at(i)->SetMarkerColor(kRed);
                    break;
                case 2:
                    graphs.at(i)->SetMarkerColor(kBlue);
                    break;
                case 3:
                    graphs.at(i)->SetMarkerColor(kGreen);
                    break;
            }
        }
    }


    hydrogen->cd();
    hydrogen->SetLogx();
    hydrogen->SetLogy();
    hydrogen->SetGridx();
    hydrogen->SetGridy();
    hydrogen_gr->Draw("A");
    hydrogen_gr->GetXaxis()->SetTitle("energy [MeV]");
    hydrogen_gr->GetXaxis()->SetTitleOffset(1.1);
    hydrogen_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    hydrogen_gr->GetYaxis()->SetTitleOffset(1.1);
    hydrogen_leg->Draw("Same");
    hydrogen->Write();

    water->cd();
    water->SetLogx();
    water->SetLogy();
    water->SetGridx();
    water->SetGridy();
    water_gr->Draw("A");
    water_gr->GetXaxis()->SetTitle("energy [MeV]");
    water_gr->GetXaxis()->SetTitleOffset(1.1);
    water_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    water_gr->GetYaxis()->SetTitleOffset(1.1);
    water_leg->Draw("Same");
    water->Write();

    uranium->cd();
    uranium->SetLogx();
    uranium->SetLogy();
    uranium->SetGridx();
    uranium->SetGridy();
    uranium_gr->Draw("A");
    uranium_gr->GetXaxis()->SetTitle("energy [MeV]");
    uranium_gr->GetXaxis()->SetTitleOffset(1.1);
    uranium_gr->GetYaxis()->SetTitle("dN/dx [cm^{-1}]");
    uranium_gr->GetYaxis()->SetTitleOffset(1.1);
    uranium_leg->Draw("Same");
    uranium->Write();


    file->Close();

    return 0;
}

#include <iostream>
#include "PROPOSAL/Bremsstrahlung.h"
#include "PROPOSAL/Integral.h"
#include "PROPOSAL/Medium.h"
#include "PROPOSAL/Interpolant.h"
using namespace std;

double integrate(double min, double max, int N, double (*func)(double) ){
    double dx   =   max-min;
    dx          /=  N;
    double  integral    = func(min) + func(max);
            integral    /=2;

    for(int i=1;i<N;i++)integral += func(dx*i+min);

    return integral*dx;
}

double X2(double x){
    return x*x;
}

double X_YY(double x, double y){
    return x + y*y;
}

double _3X_2(double x){
    return 3*x + 2;
}


double dNdx_dNdxRnd(double rnd, double energy){

    double dNdxRnd;
    double dNdx;

    Medium *medium = new Medium("antares water",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    particle->SetEnergy(energy);
    EnergyCutSettings *cuts = new EnergyCutSettings(500.,0.1);

    CrossSections *brems = new Bremsstrahlung(particle, medium, cuts);

    brems->SetParametrization(1.);

    dNdx=brems->CalculatedNdx();
    dNdxRnd=brems->CalculatedNdx(rnd);
    cout<<"dNdx\t"<<dNdx<<"\t"<<"dNdxRnd\t "<<dNdxRnd<<"\t";

    return dNdx-dNdxRnd;

}


int main(){

    double energy1;
    double rnd;
    double diff;

    for(int i =1; i<11;i++)
    {
        for(int j =0; j<10;j++)
        {
            rnd =1./i;
            energy1 = 120 * pow(10.,j);
            diff=dNdx_dNdxRnd(rnd,energy1);
            cout<< "rnd:\t"<<rnd<<"\t energy:\t"<<energy1<<"\t dNdx-dNdx(rnd):\t"<<diff<<endl;
        }

    }

    ifstream in;
    in.open("bin/Brems_dEdx.txt");

    char firstLine[256];
    in.getline(firstLine,256);
    double dEdx_new;
    double energy;
    double dEdx;
    double ecut;
    double vcut;
    string med;
    string particleName;
    bool lpm;
    int para;

    cout.precision(16);

    Medium *medium = new Medium("antares water",1.);
    Particle *particle = new Particle("mu",1.,1.,1,.20,20,1e5,10);
    particle->SetEnergy(1e6);
    EnergyCutSettings *cuts = new EnergyCutSettings(500,-1);
    CrossSections *brems = new Bremsstrahlung(particle, medium, cuts);

    cout<<"dEdx:"<<endl;
    brems->EnableDEdxInterpolation();
    cout<<brems->CalculatedEdx()<<endl;

    brems->DisableDEdxInterpolation();
    cout<<brems->CalculatedEdx()<<endl;

    brems->EnableDEdxInterpolation();
    cout<<brems->CalculatedEdx()<<endl;

    double gna;
    double gna2;
    cout<<"dNdx:"<<endl;
    brems->EnableDNdxInterpolation();
    gna=brems->CalculatedNdx();

    brems->DisableDNdxInterpolation();
    gna2=brems->CalculatedNdx();

    cout<<gna/gna2<<endl;

    brems->EnableDNdxInterpolation();
    cout<<brems->CalculatedNdx()<<endl;

//    while(in.good())
//    {
//        in>>para>>ecut>>vcut>>lpm>>energy>>med>>particleName>>dEdx;
//        cout<<para<<"\t"<<ecut<<"\t"<<vcut<<"\t"<<lpm<<"\t"<<energy<<"\t"<<med<<"\t"<<particleName<<"\t"<<dEdx<<"\t";

//        Medium *medium = new Medium(med,1.);
//        Particle *particle = new Particle(particleName,1.,1.,1,.20,20,1e5,10);
//        particle->SetEnergy(energy);
//        EnergyCutSettings *cuts = new EnergyCutSettings(ecut,vcut);

//        CrossSections *brems = new Bremsstrahlung(particle, medium, cuts);


//        brems->SetParametrization(para);
//        brems->EnableLpmEffect(lpm);
//        brems->EnableDEdxInterpolation();
//        if(para==1 && ecut==500 && vcut == -1 && lpm ==true){
//            para = para*para;
//        }
//        dEdx_new=brems->CalculatedEdx();
//        cout<<dEdx_new<<"\t"<<particle->GetMass()<<"\t"<<brems->GetVMax()<<"\t"<<brems->GetVMin()<<"\t"<<brems->GetVUp()<<endl;
//        //ASSERT_NEAR(dEdx_new, dEdx, 5e-1*dEdx);

//        delete cuts;
//        delete medium;
//        delete particle;
//        delete brems;



//    }

    return 0;
}


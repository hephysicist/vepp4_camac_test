#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <stdio.h>
#include <unistd.h>
#include <boost/program_options.hpp>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include "camac.h"
using namespace std;
class Root_hist
{ 
	TCanvas *cName;
	TH1F *hName; 
	double mean;
	double rms;
	double max;
	public:
	Root_hist(){};	
	~Root_hist(){};	
	void makeHist()  //Creates window with histogram	
	{
		printf("new hist is going to be made\n");		
		cName = new TCanvas;
		hName = new TH1F("","",4097,0,4096);
		hName->Draw();
		printf("new hist was made\n");
	}
	
	void InitHist(const std::vector<int>  & data)//Adds data to histogramm (delay betwin iterations - 1 second)
	{
		for(unsigned int i=0;i<data.size();i++)
		//for(auto d : data)
		{
 			if(data[i]!=0)
			hName->Fill(data[i]);
		}
		cName->Modified();	
		cName->Update();
	        mean=hName->GetMean(1);
 		rms=hName->GetRMS(1);
		//MaxValue=hName->GetBinContent(h->GetMaximumBin())
		max=hName->GetMaximumBin();
	       
	}
	void ResetHist(void)
	{
		hName->Reset();
	}
	double GetMean(void)  { return mean; }
	double GetRMS(void) { return rms; }
	double GetMax(void) {return max; }
};
class Camac 
{
 	public:		
	ZCP *zcp;
	Counter *counter;
	HVPS *hvps;
	Diskriminator *diskr;
	Delay *del1; 
	Delay *del2;
	Delay *del3;
	Shaper *sh;
	void camacInit( int argc, char** argv)
	{
		Crate crate(0,5);
		char program_name[1024],output_file1[1024],output_file2[1024];
		int channel,position,status=0;//0-vvi off , 1-vvi on and set voltage , 2-reset vvi.
		int delay1=0,delay2=0;//nanoseconds	
		unsigned  DiskrValue1=0, DiskrValue2=0;
		unsigned  FEUVoltage0=0,FEUVoltage1=0;//volts
		double T,load=0;//seconds , Hz// 0 or 1 
		int charge[4],xq;
		int ZCPxq=0,x=0,q=0;
		int LAM=0;
		int N_count; 
		if(argc!=13)
		{
		cerr << "Usage: " << program_name << " <channel>  <position> <ticks> <Diskr.value1> <Diskr.value2> <FEU voltage0> <FEU voltage1> <delay1> <delay2> <N count> <status> <output file 1>" << endl;
		exit(1);
		}
		sscanf(argv[0],"%s",&program_name);
		sscanf(argv[1],"%d",&channel);
		sscanf(argv[2],"%d",&position);
		sscanf(argv[3],"%lf",&T); 
		sscanf(argv[4],"%d",&DiskrValue1);
		sscanf(argv[5],"%d",&DiskrValue2);
		sscanf(argv[6],"%d",&FEUVoltage0);
		sscanf(argv[7],"%d",&FEUVoltage1);
		sscanf(argv[8],"%d",&delay1);
		sscanf(argv[9],"%d",&delay2);
		sscanf(argv[10],"%i",&N_count);
		sscanf(argv[11],"%d",&status);
		sscanf(argv[12],"%s",&output_file1);
		cout << program_name << endl << "channel= " << channel <<endl<< "position= " << position <<endl<< "count time=" << T <<endl<< "Diskr.value 1 channel=" << DiskrValue1 <<endl<< "Diskr.value 2 channel=" <<DiskrValue2<<endl<<"V(feu0) = " << FEUVoltage0 <<endl<< "V(feu1) =  " << FEUVoltage1 <<endl<<"Delay1="<<delay1<<endl<<"Delay2="<<delay2<<endl<< "status"<< status  <<  endl;
		crate.C();
		crate.Z();
		counter = new Counter (&crate,7);
		hvps = new HVPS(&crate,2);
		diskr = new Diskriminator(&crate,4); 
		del1 = new Delay(&crate,6);
		del2 = new Delay(&crate,14);
		del3 = new Delay (&crate,16);
		sh = new Shaper(&crate,10);
		zcp = new ZCP(&crate,8);
		Coincidense *con = new Coincidense(&crate,position+4);
		ofstream f(output_file1);
		if(!f)
		cerr << "Can't open file 1 "<< endl;
		if((status==1) )
		{
			clog<< "file succesfully opened" << endl;
		
			//printf("FEU has turned on\n");
			del1->SetDelay(delay1);
			del2->SetDelay(delay2);
			del3->SetDelay(15);
			sh->SetForm(0,4);
			sh->SetForm(1,4);
			diskr->D_SetLevel(0,DiskrValue1);
			diskr->D_SetLevel(1,DiskrValue2);
			diskr->D_GetLevel(0);
			diskr->D_GetLevel(1);
			hvps->HVPS_On(0);
			hvps->HVPS_On(1);
			hvps->HVPS_SetV(0,FEUVoltage0); 
			hvps->HVPS_SetV(1,FEUVoltage1);
			hvps->HVPS_GetV(0);
			hvps->HVPS_GetV(1);
			con->SetMask(1,0);
			time_t t1=time(0);
			time_t last_time=t1;
			con->GetData(0);
		}
		if(status==2)
		{
			hvps->HVPS_ResetV(0);
			hvps->HVPS_ResetV(1);
			cout<<"FEU voltage has reseted\n"<<endl;		
		}
		if (status==0)
		{
			hvps->HVPS_Off(0);
			hvps->HVPS_Off(1);
			cout<<"FEU has turned off\n"<<endl;		
		}
	}
	int GetCharge(int N)// N is a number of channel of ZCP from 1 to 4
	{
		int ZCPxq,LAM=0,x,q, datacharge;
		while(LAM!=3)
		{
			usleep(100000);
			ZCPxq=zcp->GetZCPLam();
			LAM = ~ZCPxq&0x3;
			x = ((~ZCPxq)>>1)&0x1;
	  		q = ~ZCPxq&0x1;
			cout<<ZCPxq<<"\tLAM="<< LAM <<"\tx="<<x<<"\tq="<<q<< endl;	
		}
		if(LAM==3) 
		{
			datacharge=zcp->GetCharge(N);
		  	zcp->ResetLAM();
			printf("%d\n",datacharge);
			LAM=0;	
		}
	return datacharge;	
	}
	unsigned int GetCount(int N)// N is a number of channel of ZCP from 1 to 4
	{
		unsigned int datacount = counter->ReadCounter(0);
		return datacount;
	}
        void ResetCount(void) 
	{
		counter->ResetCounter();
	}
};


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
#include "camac1.3.h"
using namespace std;
class Root_hist
{ 
	TCanvas *cName;
	TH1F *hName;
	public: 
	Root_hist(){};	
	~Root_hist(){};	
	void makeHist()  //Creates window with histogram	
	{
		cName = new TCanvas;
		hName = new TH1F("","",4097,0,4096);
		hName->Draw();
	}
	
	int addToHist(int data[1000000])//Adds data to histogramm (delay betwin iterations - 1 second)
	{
		int Mean=0;
		for(int i=0;i<1e6;i++)
		{
 			if(data[i]!=0)
			hName->Fill(data[i]);
		}
		cName->Modified();	
		cName->Update();
	        Mean=hName->GetMean(1);
	        return Mean;
	}
	void ResetHist(void)
	{
		hName->Reset();
	}
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
	int count = 0;	
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
		int count = 0;
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
		counter = new Counter (&crate,position-1);
		hvps = new HVPS(&crate,position-6);
		diskr = new Diskriminator(&crate,position-4); 
		del1 = new Delay(&crate,position-2);
		del2 = new Delay(&crate,position+6);
		del3 = new Delay (&crate,position+8);
		sh = new Shaper(&crate,position+2);
		zcp = new ZCP(&crate,position);
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
			long count=0;
			con->GetData(0);
		}
		if(status==2)
		{
			hvps->HVPS_ResetV(0);
			hvps->HVPS_ResetV(1);
			cout<<"FEU voltage has reseted\n"<<endl;	
			exit(1);	
		}
		if (status==0)
		{
			hvps->HVPS_Off(0);
			hvps->HVPS_Off(1);
			cout<<"FEU has turned off\n"<<endl;
			exit(1);		
		}
	}
	int getCharge(int N)// N is a number of channel of ZCP from 1 to 4
	{
		int ZCPxq,LAM,x,q, datacharge;
		while(LAM!=3)
		{
			//usleep(10000);
			ZCPxq=zcp->GetZCPLam();
			LAM = ~ZCPxq&0x3;
			x = ((~ZCPxq)>>1)&0x1;
	  		q = ~ZCPxq&0x1;
			//cout<<ZCPxq<<"\tLAM="<< LAM <<"\tx="<<x<<"\tq="<<q<< endl;
			usleep(100000);	
		}
		if(LAM==3) 
		{
			datacharge=zcp->GetCharge(N);
		  	//time_t t=time(0);
		  	zcp->ResetLAM();
		  	count++;
			printf("%d\n",datacharge);	
		}
	return datacharge;	
	}
};


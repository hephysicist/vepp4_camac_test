#ifndef SAZ_CAMAC_H
#define SAZ_CAMAC_H
#include </usr/local/include/lsi6camac.h> 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
class Crate
{
int fd;
public:
   Crate(int card, int channel)
   {
	char device[1024];
	sprintf(device,"/dev/lsi6card%dchannel%d",card+1, channel+1);
	fd  = open(device,O_RDWR);
	if(fd<0) 
	{
		cerr << "ERROR: Unable to open device: " << device << endl;
		exit(1);

	}
   }
   ~Crate(void)
   {
      close(fd);
   }	
   int GetFd(void) { return fd; }
   void C(void) { CAM_C(fd);}
   void Z(void) { CAM_Z(fd);}
};

class Module
{
protected:
Crate * crate;
int position;
public:
    Module(Crate * c, int pos)
    {
	crate=c;
	position = pos;
    }
    int af16(int a,int f, unsigned * data)
    {
        int xq=CAM(crate->GetFd(), NAF(position,a,f), data);
	return xq;
	//clog << "af16 data=" << *data << " xq=" << (xq&0x3) << endl;
    }
    int af24(int a,int f, long unsigned * data)
    {
        int xq=CAM24(crate->GetFd(), NAF(position,a,f), data);
 	return xq;
	//clog << "af16 data=" << *data << " xq=" << (xq&0x3) << endl;
    }
    void WaitLam(int timeout)
    {
	
	CAM_LWAIT(crate->GetFd(),position,timeout);
	
    }
};

class Timer : public Module // Camac ???? block
{
	const double TICK_TIME=0.01; //seconds
public:
	Timer(Crate *c, int pos) : Module(c,pos) {}
	void SetTime(double time)
	{
		double dticks=time/TICK_TIME;
		unsigned ticks = (unsigned)dticks;
		//cout << "dticks=" << dticks << endl;
		//cout << "ticks = " << ticks <<  " time = " << time << endl;
		af16(0,17,&ticks);
	}
	double GetTime(void)
	{
		unsigned ticks=0;
		af16(0,1,&ticks);
		return ticks*TICK_TIME;
	}
	void Start(void)
	{
		unsigned data=0;
		af16(0,10,&data);
	}
	void Wait(void) { WaitLam(-1);}
        void Reset (void)
	{
        crate->C();
	}
};
class Counter : public Module // Camac C0301 block whitch have been remade to 32 bit counter
{ 
public:
	Counter (Crate *c, int pos): Module(c,pos) {}
	void ReadCounter(void)
	{
         unsigned int  data16[8], data32[4];
         int a=0;
         for(a=0;a<8;a+=2)
		{
		af16(a,0,&data16[a]);
		af16(a+1,0,&data16[a+1]);	
                data32[a/2]=data16[a]+65536*data16[a+1];
                printf("data IN[%d]=%d\n",a/2+1,data32[a/2]);
		}
	}
	unsigned ReadCounter(int a)
	{
         unsigned int  data16[2], data32;

		af16(2*a,0,&data16[0]);
		af16(2*a+1,0,&data16[1]);	
                data32=data16[0]+65536*data16[1];
              return data32;
	}

	void ResetCounter (void)
        {
	unsigned int data=0;
	int a=0;
	for(a=0;a<8;a++)
		{
		af16(a,2,&data);
		}
	}
};
class HVPS : public Module //High voltage power supply Б0300 block 
{
 public:
	HVPS (Crate * c, int pos) : Module(c,pos) {}

        void HVPS_On  (int HVout) //Turns the HVPS on
         {
	  af16(HVout,26,0);
         }

        void HVPS_Off  (int HVout) //Turns the HVPS off
         {
	  af16(HVout,24,0);
         }
        void HVPS_GetV (int HVout) //Returns the HVPS voltage
	 {
          unsigned V;
          af16(HVout,0,&V);
	  V=V&0xfff;
	  clog<< "channel" << HVout <<"voltage  is "<< V <<endl;
	 }
        void HVPS_SetV (int HVout, unsigned int voltage) //Set the HVPS voltage
	 {
	  af16(HVout,16,&voltage);
	 }
        void HVPS_ResetV (int HVout) //Reset the HVPS voltage and turns off overload on c
	 {
	  af16(HVout,10,0);
	 }
};

class Diskriminator : public Module // Camac D0302M block
{
 public:
	Diskriminator (Crate * c, int pos) : Module(c,pos) {}

        void D_SetLevel  (int channel, unsigned int level)
         {
	  level=(level-10)/5;
		if(level<0) 
		level=0;
	  af16(channel,17,&level);
         }

        void D_GetLevel (int channel)
         {
	  unsigned int level; 
	  af16(channel,1,&level);
         clog << "Diskr. level is "<< level*5+10 << endl;
	 }
};
class Delay : public Module // Camac L4001 block
{
 public:
	Delay (Crate * c, int pos) : Module(c,pos) {}

        void SetDelay  (unsigned int delay)
         {
	  af16(0,16,&delay);
         }
};
class ZCP  : public Module // Camac Ц0312 block
{
 public:
	ZCP (Crate * c, int pos) : Module(c,pos) {}

        int GetCharge (int channel)
         {
	  unsigned int data;
	  af16(channel,0,&data);
          return data;
         }
	int GetZCPLam (void)
         {
	  int xq=af16(0,8,0);
	 
          return xq;
	}
	int GetResetZCP ( int channel)
         {
	  unsigned int data;
	  af16(channel,2,&data);
          return data;         
	}
	void ResetLAM ()
         {
	  af16(0,10,0);        
	}
	void UseLAM (void)
         {
	  af16(0,26,0);
         }
 
};
class Shaper : public Module // Camac Л0311 block
{
 public:
	Shaper (Crate * c, int pos) : Module(c,pos) {}

        int GetForm ( int channel)
         {
	  unsigned int data;
	  af16(channel,1,&data);
          return data;
         }
	void SetForm ( int channel,unsigned int time)
         {
	  af16(channel,17,&time);
	 } 
};
class Coincidense : public Module // Camac ???? block 
{
 public:
	Coincidense (Crate * c, int pos) : Module(c,pos) {}

        void SetMask ( unsigned int mask,unsigned int out_channel)
         {
	  af16(out_channel,17,&mask);
         }
	int GetData ( int out_channel)
         {
	 unsigned int data;
	 af16(out_channel,1,&data);
	 return data;
	 } 
};
#endif

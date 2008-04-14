extern int scalex[];
extern int scaley[];
extern int screeny[];
extern int vid_pre_xlat2[];
extern int W,H;
#define RED 63488 
#define GREEN 2016
#define BLUE 31
void stretchblt(unsigned short *bufp,unsigned char *nes_imageb)
{
	unsigned int i,j;
	for (j=0; j<H; j++)
	{
		unsigned int sy=scaley[j]<<8;
		for (i=0; i<W; i++)
		{
			unsigned int spos=screeny[j]+i;
			unsigned int sx=scalex[i];
			bufp[spos]=vid_pre_xlat2[nes_imageb[sy+sx]];
			unsigned int k=(scalex[i+1]-scalex[i]);
			unsigned int ku=0;
	
			unsigned int nr,ng,nb;
			if (k>0) 
			{
			unsigned int r1,r2,g1,g2,b1,b2;
			unsigned int t1=bufp[spos];
			unsigned int t2=vid_pre_xlat2[nes_imageb[(sy)+(sx+1)]];
			r1=(t1 & RED)>>12;
			r2=(t2 & RED)>>12;
			g1=(t1 & GREEN)>>6;
			g2=(t2 & GREEN)>>6;
			b1=(t1 & BLUE)>>1;
			b2=(t2 & BLUE)>>1;
			nr=(r1+r2);
			ng=(g1+g2);
			nb=(b1+b2);
			bufp[spos]=(nr<<11) | (ng<<5)| nb;
			ku=1;
			}
			k=(scaley[j+1]-scaley[j]);
			if (k>0) 
			{
			unsigned int r1,r2,g1,g2,b1,b2;
			unsigned int t1=bufp[spos];
			unsigned int t2=vid_pre_xlat2[nes_imageb[(sy+256)+(sx)]];
			r1=(t1 & RED)>>12;
			g1=(t1 & GREEN)>>6;
			b1=(t1 & BLUE)>>1;
			r2=(t2 & RED)>>12;
			g2=(t2 & GREEN)>>6;
			b2=(t2 & BLUE)>>1;
			unsigned int nr2,ng2,nb2;
			nr2=(r1+r2);
			ng2=(g1+g2);
			nb2=(b1+b2);
			
			bufp[spos]=(nr2<<11) | (ng2<<5)| nb2;
			}
//	bufp++;
//	p++;
		}
//		p+=buffer_x-W;
	
	}
}
void stretchblt2(unsigned short *bufp,unsigned char *nes_imageb)
{
	unsigned int i,j;
	for (j=0; j<H; j++)
	{
		unsigned int sy=scaley[j]<<8;
		for (i=0; i<W; i++)
		{
			unsigned int spos=screeny[j]+i;
			unsigned int sx=scalex[i];
			bufp[spos]=vid_pre_xlat2[nes_imageb[sy+sx]];
		}
	}
}

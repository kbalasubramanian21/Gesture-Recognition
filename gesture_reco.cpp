#include "testApp.h"

//The gesreg function performs the recognition task and returns a double integer
double gesreg(unsigned char *pix,int w,int h)
{
    bool tog=0;
    long int sum=0,i=0,j=0;
    int avg=0,mat[240][3],m=0;
    double the=0;
    for(i=0;i<w*h;i++)
    sum+=pix[i];
    avg=sum/(i);
    //Make it a black and white image
     for(i=0;i<w*h;i++)
    {
        if(pix[i]>(avg/1.5))
        pix[i]=255;
        else
        pix[i]=0;
     }
    //Main algorithm starts here
    for(i=0;i<h;i++)
    {
        avg=0;sum=0;tog=0;m=1;mat[i][0]=0;
        for(j=0;j<w;j++)
        {
            if(pix[i*w+j]==tog*255 && ((j-sum)>5 || sum==0 ) && m<3 )
            {
                if(tog==0)
                    mat[i][0]++;
                else
                {
                    mat[i][m++]=(sum+j)/2;
                    if((j-sum)>150)
                        mat[i][0]=10;
                }
                sum=j;
                tog=!tog;
            }
        }
    }

    //Deriving the gesture from the matrix
    sum=0;avg=0;
    int stop=0,updwn=0,zero=0,real=0;

    for(i=0;i<h;i++)
    {
        if(mat[i][0]==1 && i<h/2)
             sum++;
       if(mat[i][0]==2 && i<h/2)
            avg++;
         if(i>h/2)
         {
             if(mat[i][0]==1)
                 updwn++;
             else if(mat[i][0]==10)
                stop++;
             else if(mat[i][0]==2)
                real++;
             else
                zero++;
         }
    }
    if((h/2-sum) < 80)
    {
         int temp=0;
         for(j=1;j<h/2 && mat[j][0]==1;j++)
        {
            the+=atan2((float)(mat[j][1]-temp),1);
            temp=mat[j][1];
        }
        the=the/sum;
        return the;
    }
    else if((h/2-avg) < 50)
        return 4.0000;

    else if((real>updwn) && (real>stop))
        return 16.00000;

    else if(updwn>stop)
    {
         int temp=0,ff=1;
         the=0;
        for(j=h/2;j<h && mat[j][0]==1;j++)
        {
            if(!ff)
            the+=atan2((float)(mat[j][1]-temp),1);
            temp=mat[j][1];
            ff=0;
        }
        the=the/updwn;
        return (the+12);
    }
    else if(updwn<stop)
        return 5;
    else
        return 10;
 }

//--------------------------------------------------------------
void testApp::setup()
{
    w=320;h=240;mode=0;ges=0,stop=1;times=7;timesb=0;tim=10;
    int baud = 9600;
    serial.setup("COM6", baud); //intialise serial communication
    vid.setVerbose(true);
    vid.listDevices();
    vid.setDesiredFrameRate(1);
    vid.initGrabber(w,h);
    color.allocate(w,h);
    gr.allocate(w,h);
}
//--------------------------------------------------------------
void testApp::update()
{
    unsigned char getserial[3];
    memset(getserial,0,3);
    vid.grabFrame();
    if(vid.isFrameNew() && !(tim--))
    {
        tim=20;
        color.setFromPixels(vid.getPixels(), w,h);
        gr = color;
        pix=gr.getPixels();
        ges=gesreg(pix,w,h);

        if(ges==5)
        {
            serial.writeByte('n');
            stop=0;
        }
        if(!stop && !(times--))
        {
            times=7;
            if(ges>-0.2 && ges <.2)
            {
                stop=1;
                mode=0;
                serial.writeByte(mode+48);
            }
            else if(ges==4)
            {
                mode=1;timesb=0;stop=1;
                serial.writeByte(mode+48);
            }
        }
        if(stop)
        {
            int alt=0;
             if(ges>11)
             {
                 ges=ges-12;
                 alt=1;
             }
             if(ges==4 && !(timesb--))
             {
                 timesb=0;
                 if(!alt)
                serial.writeByte('G');
                 else
                serial.writeByte('C');
             }
            else if(ges>-0.2 && ges <.2)
            {
                timesb=0;
                if(!alt)
                serial.writeByte('S');
                else
                serial.writeByte('B');
            }
            else if(ges>-.45 && ges<-.2)
            {
                timesb=0;
                if(!alt)
                serial.writeByte('L');
                else
                serial.writeByte('U');
            }
            else if(ges>.2 && ges<.45)
            {

            timesb=0;
            if(!alt)
            {
            serial.writeByte('R');
            }
            else
            {
            serial.writeByte('D');
            }
             }
            else if(ges==4)
                serial.writeByte('C');
            else if(ges==5)
                serial.writeByte('n');
            else
                serial.writeByte('o');
        }
        if(serial.available())
        {
            serial.readBytes(getserial,3);
            cout<<getserial;
        }
        gr.setFromPixels(pix,w,h);
    }
}
//--------------------------------------------------------------
void testApp::draw()
{
    gr.draw(w,h);
    ofFill();
}


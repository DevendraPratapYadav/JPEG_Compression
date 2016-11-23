/*
Devendra Pratap Yadav 
JPEG compression - encoding and decoding

https://github.com/DevendraPratapYadav/JPEG_Compression

Standard Huffman tables for AC and DC coefficients taken from:
https://engineering.purdue.edu/~bouman/grad-labs/JPEG-Image-Coding/pdf/lab.pdf
*/



#include <cmath>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
 #include <iomanip>
#include <vector>
#include <set>
#include <list>
#include <bitset>
#include <map>


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

 #define lli long long int
#define l9 <<endl
#define pb push_back
#define mp make_pair

#define REP(i,N) for (int i=0;i<N;i++)
#define REP1(i,N) for (int i=1;i<=N;i++)

#define im(x,y)  img.at<uchar>(x,y)
#define imo(x,y)  nimg.at<uchar>(x,y)
#define C(x,y)  C.at<uchar>(x,y)

#define scu(x) saturate_cast<uchar> (x)

#define PI 3.141592653589793
#define pii pair<double,double>
#define ff first
#define ss second



using namespace cv;
using namespace std;

int pri=0;


typedef unsigned char BYTE;

string dcHC[12] = {"00","010","011","100",
					"101","110","1110","11110",
					"111110","1111110","11111110","111111110"};



string acHC[16][11]={
"1010", "00", "01", "100", "1011", "11010", "1111000", "11111000", "1111110110", "1111111110000010", "1111111110000011",
"", "1100", "11011", "1111001", "111110110", "11111110110", "1111111110000100", "1111111110000101", "1111111110000110", "1111111110000111", "1111111110001000",
"", "11100", "11111001", "1111110111", "111111110100", "1111111110001001", "1111111110001010", "1111111110001011", "1111111110001100", "1111111110001101", "1111111110001110",
"", "111010", "111110111", "111111110101", "1111111110001111", "1111111110010000", "1111111110010001", "1111111110010010", "1111111110010011", "1111111110010100", "1111111110010101",
"", "111011", "1111111000", "1111111110010110", "1111111110010111", "1111111110011000", "1111111110011001", "1111111110011010", "1111111110011011", "1111111110011100", "1111111110011101",
"", "1111010", "11111110111", "1111111110011110", "1111111110011111", "1111111110100000", "1111111110100001", "1111111110100010", "1111111110100011", "1111111110100100", "1111111110100101",
"", "1111011", "111111110110", "1111111110100110", "1111111110100111", "1111111110101000", "1111111110101001", "1111111110101010", "1111111110101011", "1111111110101100", "1111111110101101",
"", "11111010", "111111110111", "1111111110101110", "1111111110101111", "1111111110110000", "1111111110110001", "1111111110110010", "1111111110110011", "1111111110110100", "1111111110110101",
"", "111111000", "111111111000000", "1111111110110110", "1111111110110111", "1111111110111000", "1111111110111001", "1111111110111010", "1111111110111011", "1111111110111100", "1111111110111101",
"", "111111001", "1111111110111110", "1111111110111111", "1111111111000000", "1111111111000001", "1111111111000010", "1111111111000011", "1111111111000100", "1111111111000101", "1111111111000110",
"", "111111010", "1111111111000111", "1111111111001000", "1111111111001001", "1111111111001010", "1111111111001011", "1111111111001100", "1111111111001101", "1111111111001110", "1111111111001111",
"", "1111111001", "1111111111010000", "1111111111010001", "1111111111010010", "1111111111010011", "1111111111010100", "1111111111010101", "1111111111010110", "1111111111010111", "1111111111011000",
"", "1111111010", "1111111111011001", "1111111111011010", "1111111111011011", "1111111111011100", "1111111111011101", "1111111111011110", "1111111111011111", "1111111111100000", "1111111111100001",
"", "11111111000", "1111111111100010", "1111111111100011", "1111111111100100", "1111111111100101", "1111111111100110", "1111111111100111", "1111111111101000", "1111111111101001", "1111111111101010",
"", "1111111111101011", "1111111111101100", "1111111111101101", "1111111111101110", "1111111111101111", "1111111111110000", "1111111111110001", "1111111111110010", "1111111111110011", "1111111111110100",
"11111111001", "1111111111110101", "1111111111110110", "1111111111110111", "1111111111111000", "1111111111111001", "1111111111111010", "1111111111111011", "1111111111111100", "1111111111111101", "1111111111111110"
};


map < string, pair<int,int> > acTable;

map < string, int > dcTable;

int catLimits[15];


int getCategory(int n)
{
	n=abs(n);
  REP(i,15)
  {
  	if(catLimits[i]>n)
  		return i;
  }
  return 11;

}


string getBinary(int a, int len)
{
	string b="";
	////cout<<"to bin "<<a<<endl;
	while(a>0)
	{
		if (a%2==0)
			b='0'+b;
		else
			b='1'+b;

		a=a/2;
		////cout<<b<<endl;
	}

	for (int i=b.length()+1;i<=len;i++)
		b='0'+b;

	////cout<<b l9;
	return b;


}// end getBinary


int bin2int(string b)	// skips the first bit
{

	int p=1;
	int val=0;
	for (int z=b.length()-1;z>0;z--)
		{
			val+= (b[z]-'0')*p;

			p*=2;
		}
	return val;

}


string encodeDC(int dc)
{
	int ctg=getCategory(dc);

	int neg=(dc<0);
	dc=abs(dc);

	string ev=dcHC[ctg];

	int r=0;

	if (ctg!=0)
		r=dc% (int)(pow(2.0,ctg-1));
	else
		return ev;						// not adding sign bit for dc=0. only huffman code is encoded

	if (neg)
	ev+='0';
	else
	ev+='1';


	/*
	string b="";

	while(r>0)
	{
		if (r%2==0)
			b='0'+b;
		else
			b='1'+b;

		r=r/2;
	}

	for (int i=b.length()+1;i<ctg;i++)
		b='0'+b;
		*/

	string b=getBinary(r,ctg-1);

	if (b.length()>0)
		ev+=b;

	return ev;

} // end encodeDC



vector <int> decodeDC(string s)
{
	vector <int> dcc;

	string b="";
	// parse string bit by bit
	for (int i=0;i<s.length();i++)
	{
		b+=s[i];
		if ( dcTable.find(b)==dcTable.end() )
			continue;

		int ctg=dcTable[b];

		if (ctg==0)
			{
				dcc.pb(0);

				//if (pri)
				//cout<<"Found "<<0<<" ending at "<<i<<endl;
				b="";
				continue;
			}

		string num="";

		int j;
		for (j=i+1;j<=i+ctg && j<s.length(); j++)
		{
			num+=s[j];
		}
		i=j-1;

		if (num.length()<1)
			{dcc.pb(0);
			 //if (pri)
				//cout<<"Error! Found "<<0<<" ending at "<<i<<endl;
			}
		else
			{	int neg=0;
				int val=0;

				if (num[0]=='0')
					{neg=1;}

				int p=1;
				for (int z=num.length()-1;z>0;z--)
					{
						val+= (num[z]-'0')*p;

						p*=2;
					}

				val+=pow(2.0,ctg-1);

				if (neg)
					val= (-1)*val;

				dcc.pb(val);

				//if (pri)
				//cout<<"Found "<<val<<" ending at "<<i<<endl;

			}

		b="";
	}	// end for


	return dcc;

} // end decodeDC



string encodeAC(int d,int x)
{

	//if (pri)
		//cout<<"Encoding pair: "<<d<<", "<<x<<endl;

	if (d==0 && x==0)
	{
		return acHC[0][0];
	}

	if (d==15 && x==0)
	{
		return acHC[15][0];
	}

	string pre="";
	while(d>15)
		{
			d=d-16;
			pre=pre+acHC[15][0];
		}

	int ctg=getCategory(x);

	int neg=(x<0);
	x=abs(x);

	string ev=acHC[d][ctg];

	if (neg)
	ev+='0';
	else
	ev+='1';

    ////cout<<"..\n"<<(pow(2.0,ctg-1)) l9;
	int r;

	if (ctg==0)
		r=0;
    else
        r=x% (int)(pow(2.0,ctg-1));

	string b=getBinary(r, ctg-1);

	if (b.length()>0)
		ev+=b;

	return pre+ev;

} // end encodeAC





string runLengthAC(vector <int> &ac)
{
	string code="";

	int cz=0; // consecutive zeros

	int lz=0;	// last non-zero element

	for (lz=ac.size()-1;lz>=0;lz--)
		{
			if (ac[lz]!=0)
			break;
		}

	if (lz==ac.size()-1)
		lz++;

	for (int i=1;i<lz;i++)  // start from 1 to skip dc element
	{
		if(ac[i]==0 && cz<15)
			{cz++; continue;}

		int x=ac[i];
		int run=cz;

        ////cout<<i<<"..\n";
		code+=encodeAC(run,x);
		cz=0;
	}

	code+=encodeAC(0,0);			// compulsary EOB for each block

	return code;

} // end RLAC



vector <vector <int> > decodeAC(string s)
{

    vector <vector <int> > acarr;
	vector <int> acc;
    acc.pb(1);  // dummy dc component
	string b="";
	// parse string bit by bit
	for (int i=0;i<s.length();i++)
	{
		b+=s[i];
		if ( acTable.find(b)==acTable.end() )
			continue;

		pair<int ,int> dx=acTable[b];

		int run=dx.first;
		int ctg=dx.second;

		if (ctg==0)
			{
				if (run==0)
					{
					// fill acc with 0 till end size=63

                    while(acc.size()<64)
                        acc.pb(0);

                    acarr.pb(acc);
                    ////cout<<"Got new AC" l9;
                    acc.clear();
                    acc.pb(1);  // dummy dc component
                    b="";
					//break;
                    continue;
					}

				if (run==15)
				{
					for (int i=1;i<=16;i++)
						acc.pb(0);
				}

				//if (pri)
				//cout<<"Found "<<0<<" ending at "<<i<<endl;
				b="";

				continue;
			}

		string num="";

		int j;
		for (j=i+1;j<=i+ctg && j<s.length(); j++)
		{
			num+=s[j];
		}
		i=j-1;

		if (num.length()<1)
			{acc.pb(0);
			// if (pri)
				//cout<<"Error! Found "<<0<<" ending at "<<i<<endl;
			}
		else
			{	int neg=0;
				int val=0;

				if (num[0]=='0')
					{neg=1;}

				int p=1;
				for (int z=num.length()-1;z>0;z--)
					{
						val+= (num[z]-'0')*p;

						p*=2;
					}

				val+=pow(2.0,ctg-1);

				if (neg)
					val= (-1)*val;

				while(run--)
					acc.pb(0);

				acc.pb(val);

				//if (pri)
				//cout<<"Found "<<val<<" ending at "<<i<<endl;

			}

		b="";
	}	// end for




	return acarr;

} // end decodeAC





vector<BYTE> getIntBYTE(int a)
{
 int n=a;
vector <BYTE> res;

res.pb( (n >> 24) & 0xFF);
res.pb( (n >> 16) & 0xFF);
res.pb( (n >> 8) & 0xFF);
res.pb(  n & 0xFF);

 return res;
}



/*
Metadata-
1.no of metadata elements (excluding/after this entry)
2. image cols
3. image rows
4. extra zeros added to last ac byte
5. extra zeros added to last dc byte
6. end of dc coeff -> value p means pth byte is end of dc coeff data
	dc coeff start at 1st byte and end at (p)th byte

*/
void writeFile(string s, vector<int> metadata,string name)
{
	//cout<<"Printing to file\n" <<s<<endl;

	FILE *img=fopen(name.c_str(),"w");
	int tbw=0;
	if (img==NULL)
		cout<<"Error opening file"<<endl;

	int c=0;

	while(s.length()%8!=0)		// add zeros to make the last char a full byte
		{s+='0'; metadata[3]++;}


	int mcount=metadata[0];

	for (int i=0;i<=mcount;i++)
	{
		//fprintf(img,"%d",metadata[i]);
		vector <BYTE>no=getIntBYTE(metadata[i]);

		for (int i=0;i<no.size();i++)
			 {fprintf(img,"%c",no[i]);

				//printf("%d. %X\n",(tbw),no[i]);
				tbw++;
			 }
	}

	//cout<<"DC and AC : \n" ;
	for (int i=7;i<s.length();i=i+8)
		{

			BYTE x=0;

			for (int j=i-7;j<=i;j++)
				x=(x<<1)|(s[j]=='1');

				//x+= (s[i]-'0')*p;

			fprintf(img,"%c",x);
			//printf("%d. %X\n",tbw,x);
			tbw++;

		}

	//cout l9;
		fclose(img);
}//writetofile


int getInt( BYTE *buff)
{
 int val=0;

 for (int i=0;i<4;i++)
	val=buff[i]|(val<<8);

 return val;
}


vector<string> readFile(string name, vector <int> &metadata)
{

	FILE *img2=fopen(name.c_str(),"r");

	vector<string> image;

	fseek(img2,0,SEEK_END);
	long lsize=ftell(img2);
	rewind(img2);

	//cout<<"file size :"<<lsize l9;
	BYTE *buffer= (BYTE*) malloc(sizeof(BYTE)*lsize );

	fread(buffer,1,lsize,img2);

	//lsize-=1;

	//cout<<"Buffer: \n";
	REP(i,lsize)
	{
		//printf("%X ",buffer[i]);

	}

	//cout l9;
	//freopen();
	int bp=0;

	if (img2==NULL)
		{cout<<"Error reading file"<<endl;
		 return image;
		}

	int c=0;

	c=getInt(buffer+bp);
	bp+=4;

	//fscanf(img2,"%d",&c);

	metadata.pb(c);
	//cout<<"Read metadata "<<c<<endl;

	for (int i=1;i<=c;i++)
	{
	 int k;
	 //fscanf(img2,"%d",&k);
	 k=getInt(buffer+bp);
	 bp+=4;
	 metadata.pb(k);

	 //cout<<"Read metadata "<<k<<endl;

	}

	//return image;

	int sep=metadata.back();

	int extraBytes=0;
	if (metadata.size()>2)
	extraBytes=metadata[ metadata.size()-2 ];



 	string s="";

	 int zz=1;

	int readBytes=0;

	while(bp<lsize)
	{

		if (readBytes==sep)
			{
				s=s.substr(0,s.length()-extraBytes);

				image.pb(s);
				s="";
			}


		BYTE byt=buffer[bp++];
		//zz=fscanf(img2,"%c",&byt);

		if (zz!=1)
			break;

		int ibyt=0|byt;

		s+=getBinary(ibyt,8);
		//cout<<bp<<". "<<getBinary(ibyt,8);
		//printf(" %X\n",byt);
		readBytes++;


	}

	if (metadata.size()>3)
	extraBytes=metadata[ metadata.size()-3 ];

	s=s.substr(0,s.length()-extraBytes);

	image.pb(s);
	s="";

	fclose(img2);
	return image;
} // end readFile






/*
Metadata-
1.no of metadata elements (excluding/after this entry)
2. image cols
3. image rows
4. extra zeros added to last ac byte
5. extra zeros added to last dc byte
6. end of dc coeff -> value p means pth byte is end of dc coeff data
	dc coeff start at 1st byte and end at (p)th byte
*/
void writeImage(int cols, int rows, string dc, string ac,string name)
{
 vector <int> metadata;

 metadata.pb(5);
 metadata.pb(cols);
 metadata.pb(rows);
 metadata.pb(0);
 metadata.pb(0);
 metadata.pb(0);

 int eb=0;

 while(dc.length()%8!=0)
 	{
	 	dc+='0'; eb++;
	 }

 metadata[4]=eb;

 metadata[5]=dc.length()/8;

  eb=0;

 while(ac.length()%8!=0)
 	{
	 	ac+='0'; eb++;
	 }
 metadata[3]=eb;

 string Alldata=dc+ac;
 writeFile(Alldata,metadata,name);


}// end write image














double QM[8][8] = {16,11,10,16,24,40,51,61,
                     12,12,14,19,26,58,60,55,
                     14,13,16,24,40,57,69,56,
                     14,17,22,29,51,87,80,62,
                     18,22,37,56,68,109,103,77,
                     24,35,55,64,81,104,113,92,
                     49,64,78,87,103,121,120,101,
                     72,92,95,98,112,100,103,99};



double **get2Darray(int r,int c)
{
    double **arr=new double*[r];

    for(int i = 0; i <r; i++)
    {
        arr[i]=new double[c];

    }

    return arr;

} // end get2darray


void show(String name, Mat &img, int wait)
{

 namedWindow( name, WINDOW_AUTOSIZE );// Create a window for display.
 imshow( name, img );
 if (wait>0)
 waitKey(wait);
 else
 waitKey(0);

}


double rmse(Mat &img,Mat &nimg)
{


 double sum=0;
 for(int i=0;i<img.rows;i++)
    {
      for (int j=0;j<img.cols;j++)
      {
            int diff=im(i,j)-imo(i,j);

            sum+=diff*diff;


      }
    }

    sum/=img.rows*img.cols;

    sum=sqrt (sum);

   cout<<"RMSE : "<<setprecision(3)<<sum<<endl;

    return sum;
} // end rmse

lli nr,nc;
lli rows,cols;


Mat makeImage(double **blue, double **green, double ** red)
{
    Mat img(nr,nc, CV_8UC3, Scalar(0,0,0));

    REP(r,nr)
    {
        REP(c,nc)
        {
            Vec3b p;
            p.val[0]=scu(blue[r][c]);
            p.val[1]=scu(green[r][c]);
            p.val[2]=scu(red[r][c]);

            img.at<Vec3b>(r,c)=p;

        }

    }

    return img;

} // end makeImage





double DCTcoeff[8][8][8][8]; // s(u,v,x,y)



void DCT(double **img,int row,int col,int iDCT)
{
    double arr[8][8];

    REP(r,8)
    {
        REP(c,8)
        {
            arr[r][c]=img[r+row][c+col];
            if (iDCT==0)
                arr[r][c]-=128;
        }
    }


    // now process this 8x8 block and copy value later

    double dct[8][8];

    double n=8,ar=0,ac=0;

    double v1=1.0/sqrt(8.0);
    double v2=sqrt(2.0/8.0);

     REP(r,8)
    {
        REP(c,8)
        {


            if (r==0)
                ar=v1;
            else
                ar=v2;

            if (c==0)
                ac=v1;
            else
                ac=v2;


            REP(rr,8)
            {
                REP(cc,8)
                {
                    // dct[r][c]+= arr[rr][cc]* cos( (2.0*double(rr)+1)*PI*double(r)/16.0 )*cos( (2.0*double(cc)+1)*PI*double(c)/16.0 );

                    if (iDCT)
                    {
                        if (rr==0)
                            ar=v1;
                        else
                            ar=v2;

                        if (cc==0)
                            ac=v1;
                        else
                            ac=v2;

                    }

                    if (iDCT==0)
                    dct[r][c]+= arr[rr][cc]* DCTcoeff[r][c][rr][cc];
                    else
                    dct[r][c]+= ar*ac*arr[rr][cc]* DCTcoeff[rr][cc][r][c];




                }
            }




            if (iDCT==0)
            dct[r][c]*=ar*ac;
            //cout<<r<<", "<<c<<": "<<dct[r][c]<<endl;

        }
    }

    // copy value back to image
    REP(r,8)
    {
        REP(c,8)
        {
            img[r+row][c+col]=dct[r][c];
            //if (iDCT)
            //img[r+row][c+col]+=128;


        }


    }


}// end DCT



vector <double> zigzag(double arr[8][8])
{

    vector <double> res;

    int r=0;
    int c=0;

    while(res.size()<64)
    {
        if (r>=0 && r<=7 && c>=0 && c<=7)
            res.pb( arr[r][c] );

        if (r==0 && c!=7)
            c++;
        else
            r++;


        while(r>=0 && r<7 && c>0 && c<=7)
        {
            res.pb( arr[r][c] );
            r++; c--;
        }

        if(r>=0 && r<=7 && c>=0 && c<=7)
            res.pb( arr[r][c] );

        if (c==0 && r!=7)
            r++;
        else
            c++;


        while(r>0 && r<=7 && c>=0 && c<7)
        {
            res.pb( arr[r][c] );
            r--; c++;
        }

    } // end while

    return res;

} // end zigzag



void Revzigzag(vector<double> res, double arr[8][8])
{
    reverse(res.begin(), res.end());

    int r=0;
    int c=0;

    while(res.size()>0)
    {
        if (r>=0 && r<=7 && c>=0 && c<=7)
            { arr[r][c] =res.back(); res.pop_back(); }

        if (r==0 && c!=7)
            c++;
        else
            r++;


        while(r>=0 && r<7 && c>0 && c<=7)
        {
             arr[r][c]=res.back(); res.pop_back();
            r++; c--;
        }

        if(r>=0 && r<=7 && c>=0 && c<=7)
            {arr[r][c] =res.back(); res.pop_back();}

        if (c==0 && r!=7)
            r++;
        else
            c++;


        while(r>0 && r<=7 && c>=0 && c<7)
        {
            arr[r][c] =res.back(); res.pop_back();
            r--; c++;
        }

    } // end while

} // end Revzigzag


void quantize(double **img,int iDCT)
{
   for (int r=0;r<=nr-8;r+=8)
    {
        for(int c=0;c<=nc-8;c+=8)
        {

            // process a 8x8 block here

            for (int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {

                    if (iDCT)
                        img[r+i][c+j]*=QM[i][j];
                    else
                        img[r+i][c+j]/=QM[i][j];

                    img[r+i][c+j]=round(img[r+i][c+j]);
                    //cout<<img[r+i][c+j]<<" ";
                }
                //cout l9;
            }


        }

    } // end for


} // end quantize




void DCTstatic(double img[8][8],int rr,int cc,double sub,int iDCT)
{
    double arr[8][8];


    /*
    REP(r,8)
    {
        REP(c,8)
        {

            REP(rrr,8)
            {
                REP(ccc,8)
                {
                    DCTcoeff[r][c][rrr][ccc]= cos( (2.0*double(rrr)+1)*PI*double(r)/16.0 )*cos( (2.0*double(ccc)+1)*PI*double(c)/16.0 );

                }
            }

        }
    }
    */



    double dct[8][8];

    REP(r,8)
    {
        REP(c,8)
        {
            arr[r][c]=img[r][c]-sub;
            dct[r][c]=0;
        }
    }


    // now process this 8x8 block and copy value later


    double n=8,ar=0,ac=0;

    double v1=1.0/sqrt(8.0);
    double v2=sqrt(2.0/8.0);

    REP(r,8)
    {
        REP(c,8)
        {


            if (r==0)
                ar=v1;
            else
                ar=v2;

            if (c==0)
                ac=v1;
            else
                ac=v2;


            REP(rr,8)
            {
                REP(cc,8)
                {
                    // dct[r][c]+= arr[rr][cc]* cos( (2.0*double(rr)+1)*PI*double(r)/16.0 )*cos( (2.0*double(cc)+1)*PI*double(c)/16.0 );

                    if (iDCT)
                    {
                        if (rr==0)
                            ar=v1;
                        else
                            ar=v2;

                        if (cc==0)
                            ac=v1;
                        else
                            ac=v2;

                    }

                    if (iDCT==0)
                    dct[r][c]+= arr[rr][cc]* DCTcoeff[r][c][rr][cc];
                    else
                    dct[r][c]+= ar*ac*arr[rr][cc]* DCTcoeff[rr][cc][r][c];


                    //cout<<dct[r][c]<<"  ->  ";

                }
            }


           // cout<<r<<", "<<c<<": "<<dct[r][c]<<endl;

            if (iDCT==0)
            dct[r][c]*=ar*ac;

            //cout<<r<<", "<<c<<": "<<dct[r][c]<<endl;
        }
    }


    //vector <double> res;

    // copy value back to image
    REP(r,8)
    {
        REP(c,8)
        {
            img[r+rr][c+cc]=dct[r][c];
            //cout<<setw(3)<<dct[r][c]<<" ";
            if (iDCT)
            {
                 img[r+rr][c+cc]+=128;
            }
            //res.pb(img[r+rr][c+cc]);

        }
        //cout l9;
    }

    //return res;

}// end DCTstatic



void findDCT(double **img,int iDCT)
{


    REP(r,8)
    {
        REP(c,8)
        {

            REP(rr,8)
            {
                REP(cc,8)
                {
                    DCTcoeff[r][c][rr][cc]= cos( (2.0*double(rr)+1)*PI*double(r)/16.0 )*cos( (2.0*double(cc)+1)*PI*double(c)/16.0 );

                }
            }

        }
    }



    for (int r=0;r<=nr-8;r+=8)
    {
        for(int c=0;c<=nc-8;c+=8)
        {
            //DCT(img,r,c,iDCT);

            double bl[8][8];
            for (int i=r;i<r+8;i++)
            {
                for(int j=c;j<c+8;j++)
                        bl[i-r][j-c]=img[i][j];
            }

            int sub=0;
            if(iDCT==0)
                sub=128;

            DCTstatic(bl,0,0,sub,iDCT);

            for (int i=r;i<r+8;i++)
            {
                for(int j=c;j<c+8;j++)
                        img[i][j]=bl[i-r][j-c];
            }

        }

    } // end for



}   // end findDCT



//ac has the dc element at beginning as well . remove it while encoding
// dc has diff of next and prev. add dc[i] to prev to get current element
void getACDC(double ** img,vector <int> &dc, vector < vector<int> >&ac)
{


    for (int r=0;r<=nr-8;r+=8)
    {
        for(int c=0;c<=nc-8;c+=8)
        {
            double arr[8][8];

            // process a 8x8 block here
            dc.pb(round(img[r][c]));

            int p=0;

             for (int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                arr[i][j]=img[r+i][c+j];
                }
            }

            vector <double> zz=zigzag(arr);
            vector <int> zz1;

            for(int k=0;k<zz.size();k++)
                zz1.pb( round(zz[k]) );

            ac.pb(zz1);

        }

    } // end for

    for (int i=dc.size()-1;i>0;i--)
        dc[i]=dc[i]-dc[i-1];            // difference of next -prev, add diff to prev to get next


} // end getACDC


// dc has diff of next and prev. add dc[i] to prev to get current element
void putACDC(double ** img,vector <int> &dc, vector < vector<int> >&ac)
{

     for (int i=1;i<dc.size();i++)
        dc[i]=dc[i]+dc[i-1];            // difference of next -prev, add diff to prev to get next



    int pp=0;

    for (int r=0;r<=nr-8;r+=8)
    {
        for(int c=0;c<=nc-8;c+=8)
        {
            double arr[8][8];

            // process a 8x8 block here

            int p=0;

            vector <double> zz;

            for(int k=0;k<ac[pp].size();k++)
                zz.pb( ac[pp][k] ) ;

            Revzigzag(zz,arr);

             for (int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                    img[r+i][c+j]=arr[i][j];
                }
            }


            img[r][c] = dc[pp++];

        }

    } // end for



} // end putACDC




string DCencoded( vector <int> &dc )
{
    string ans="";

     REP(i,dc.size())
     {
            ans+=encodeDC(dc[i]);
     }

     return ans;

} // DCencoded

string ACencoded( vector < vector<int> >&ac )
{
    string ans="";

     REP(i,ac.size())
    {
        //cout<<i<<". runlAC" l9;
        ans+=runLengthAC( ac[i] );

    }
     return ans;

} // ACencoded



void writeChannel(double **blue,string name)
{
     findDCT(blue,0);


    quantize(blue,0);

    vector < int>dc;
    vector < vector<int> > ac;

    getACDC(blue,dc,ac);

    //cout<<"DC "<<dc.size() l9;
    //cout<<"AC "<<ac.size() l9;

    string dcs="",acs="";

    dcs=DCencoded(dc);

    acs=ACencoded(ac);


    writeImage(nc,nr,dcs,acs,name);


}// end process channel


void readChannel(double **blue,string name)
{
    vector < int > metadata;
    vector <string> image;

    image=readFile(name,metadata);

    nc=metadata[1];
    nr=metadata[2];

    string mydc=image[0];
    string myac=image[1];

    vector <int> dcarr=decodeDC(mydc);

    vector < vector<int> > acarr=decodeAC(myac);

    putACDC(blue,dcarr,acarr);

    quantize(blue,1);

    findDCT(blue,1);

}// end process channel




int main( int argc, char** argv)
{



    // INITIALIZATION of maps

    catLimits[0]=1;

    REP1(i,14)
    {
        catLimits[i]=catLimits[i-1]*2;
    }

    for (int j=0;j<12;j++)
    {
        dcTable[ dcHC[j] ]=j;

    }


    for (int i=0;i<16;i++)
    {

        for (int j=0;j<11;j++)
        {
            if (acHC[i][j].length()>0)
                acTable[ acHC[i][j] ]= mp(i,j);

        }

    }
    // INIT END

    string fnm="";

    cout<<"Enter choice: \n1. Encode .tiff file to .dat\n2. Decode .dat file to image"<<endl;
    int choice=0;
    cin>>choice;

    Mat img;

    if (choice==1)
        {
             cout<<"Enter name of .tiff file to read. Place it in same folder as executable.\n";
            cin>>fnm;

            img = imread(fnm,CV_LOAD_IMAGE_COLOR);

            show("Input Image",img,1000);

            nr=img.rows; nc=img.cols;
            rows=nr; cols=nc;

            while(nr%8!=0)
                nr++;
            while(nc%8!=0)
                nc++;
        }
    else
    {
        cout<<"The program will read the encoded file from { img1.dat, img2.dat,img3.dat } containing the three channels of image.\n";


        vector < int > metadata;
        vector <string> image;

        image=readFile("img1.dat",metadata);

        nc=metadata[1];
        nr=metadata[2];

    }



    // read images
    //string image1("1.tiff");



    //if (pri)
        //cout<<"Original image: Rows="<<nr<<",  Cols="<<nc l9;

    //nr=nr+(nr%8); nc=nc+(nc%8);



    //if (pri)
        //cout<<"New image: Rows="<<nr<<",  Cols="<<nc l9;

    double **blue=new double*[nr];
    double **green=new double*[nr];
    double **red=new double*[nr];

    for(int i = 0; i <nr; i++)
    {
        blue[i]=new double[nc];
        green[i]=new double[nc];
        red[i]=new double[nc];
    }
   // double green[nr][nc];

    //double red[nr][nc];




    REP(r,nr)
    {
        REP(c,nc)
        {
            blue[r][c]=0;
            green[r][c]=0;
            red[r][c]=0;
        }
    }


     if (choice==1)
     {
            REP(r,rows)
            {
                REP(c,cols)
                {
                    Vec3b p=img.at<Vec3b>(r,c);
                    blue[r][c]=p.val[0];
                    green[r][c]=p.val[1];
                    red[r][c]=p.val[2];
                }

            }

    }
    /*
    findDCT(blue,0);
    findDCT(green,0);
    findDCT(red,0);

    quantize(blue,0);


    cout<<"intial dct: \n";
     REP(r,nr)
    {
        REP(c,nc)
        {
        cout<<setw(3)<<round(blue[r][c])<<" ";
        }
        cout<<endl;
    }
    vector < int>dc;
    vector < vector<int> > ac;

    getACDC(blue,dc,ac);

    cout<<"DC coeff" l9;
    REP(i,dc.size())
        cout<<dc[i]<<" ";
   cout l9;


    cout<<"AC coeff:" l9;
   REP(i,ac.size())
    {

       REP(j,ac[i].size())
            cout<<ac[i][j]<<" ";
   cout l9;
   }


    string dcs="",acs="";

    dcs=DCencoded(dc);
    acs=ACencoded(ac);

    cout<<"DC: \n"<<dcs l9;
    cout<<"AC: \n"<<acs l9;

     //file write begin
    //writeImage(nc,nr,dcs,acs,"output.dat");

    vector < int > metadata;
    vector <string> image;

    image=readFile("output.dat",metadata);



    cout<<"dc : \n"<<image[0]<<endl<<dcs<<endl;
    cout<<"ac : \n"<<image[1]<<endl<<acs<<endl;
    // file read ends


    string mydc=image[0];
    string myac=image[1];

    vector <int> dcarr=decodeDC(mydc);

    vector < vector<int> > acarr=decodeAC(myac);






    cout<<"read DC coeff" l9;
    REP(i,dcarr.size())
        cout<<dcarr[i]<<" ";
    cout l9;


    cout<<"read AC coeff:" l9;
   REP(i,acarr.size())
    {

        cout<<i<<". -> "<<acarr.size() l9;
       REP(j,acarr[i].size())
            cout<<acarr[i][j]<<" ";
   cout l9;
   }


    putACDC(blue,dcarr,acarr);

    quantize(blue,1);

    findDCT(blue,1);
    findDCT(green,1);
    findDCT(red,1);

    */


     if (choice==1)
    {writeChannel(blue,"img1.dat");
    writeChannel(green,"img2.dat");
    writeChannel(red,"img3.dat");
    }
    else
    {readChannel(blue,"img1.dat");
    readChannel(green,"img2.dat");
    readChannel(red,"img3.dat");

    // reconstruct image using three channels
    Mat nimg=makeImage( blue, green, red );

    show("Decoded Image",nimg,0);

    imwrite("Decoded.jpg",nimg);




    }



    Mat img1 = imread(fnm,CV_LOAD_IMAGE_COLOR);

    Mat img2=imread("Decoded.jpg",CV_LOAD_IMAGE_COLOR);

    rmse(img1,img2);




    /*


    int off=16;
    REP(r,8)
    {
        REP(c,8)
        {
            cout<<round(blue[r+off][c+off])<<" ";
        }
        cout l9;
    }
    cout l9 l9;
    findDCT(blue,0);

    REP(r,8)
    {
        REP(c,8)
        {
            cout<<round(blue[r+off][c+off])<<" ";
        }
        cout l9;
    }

cout l9 l9;
    findDCT(blue,1);

    REP(r,8)
    {
        REP(c,8)
        {
            cout<<round(blue[r+off][c+off])<<" ";
        }
        cout l9;
    }
    */





  /*
    double zz[8][8];
    int cc=1;
    REP(i,8)
    {
        REP(j,8)
        {
            zz[i][j]=cc++;
            cout<<setw(2)<<zz[i][j]<<" ";
        }
        cout l9;
    }

    vector <double> ans=zigzag(zz);
    REP(i,ans.size())
    {
        cout<<ans[i]<<" ";
    }
    cout l9;


    Revzigzag(ans,zz);

    REP(i,8)
    {
        REP(j,8)
        {
            cout<<setw(2)<<zz[i][j]<<" ";
        }
        cout l9;
    }
*/




/*

    double ex[8][8]={143	,	147	,	149	,	152	,	156	,	147	,	146	,	149	,
151	,	146	,	143	,	154	,	148	,	144	,	153	,	132	,
147	,	143	,	145	,	149	,	144	,	145	,	128	,	133	,
152	,	145	,	145	,	144	,	146	,	134	,	130	,	137	,
146	,	143	,	142	,	147	,	124	,	127	,	139	,	138	,
139	,	145	,	139	,	127	,	126	,	135	,	139	,	141	,
145	,	137	,	124	,	130	,	138	,	136	,	140	,	144	,
144	,	124	,	136	,	134	,	137	,	139	,	142	,	145

    };



          double ex[8][8]={  35	,	25	,	25	,	32	,	30	,	31	,	35	,	35	,
            23	,	13	,	13	,	20	,	18	,	19	,	23	,	23	,
            23	,	13	,	13	,	20	,	18	,	19	,	23	,	23	,
            27	,	17	,	17	,	24	,	22	,	23	,	27	,	27	,
            23	,	13	,	13	,	20	,	18	,	19	,	23	,	23	,
            22	,	12	,	12	,	19	,	17	,	18	,	22	,	22	,
            26	,	16	,	16	,	23	,	21	,	22	,	26	,	26	,
            24	,	14	,	14	,	21	,	19	,	20	,	24	,	24	};


vector <double>  res;
res=DCTstatic(ex,0,0,128,0);

cout l9 l9;
int pp=0;
REP(r,8)
    {
        REP(c,8)
        {
            ex[r][c]=res[pp++];
            cout<<round(ex[r][c])<<" ";
        }
        cout l9;
    }


cout l9 l9;
res=DCTstatic(ex,0,0,0,1);

pp=0;
REP(r,8)
    {
        REP(c,8)
        {
            ex[r][c]=res[pp++];
            cout<<ex[r][c]<<" ";
        }
        cout l9;
    }



*/




} // end main

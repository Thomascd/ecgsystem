#pragma once


#define BUFFER_SIZE	512
#define LP	1
#define HP	2
#define NOTCH 4

#define FILTER_FIR_40_LP   0
#define FILTER_FIR_60Notch 1
#define FILTER_FIR_0_05_HP 2

#define PI 3.14159265f

class CFilter
{
public:
	
	CFilter(int SamplingRate);
	int sampling_rate;				//�����W�v
	float FIR_40_LP(float samp);	//40Hz fir�C�q�o�i��
	float FIR_60Notch(float samp);
	float FIR_0_05_HP(float samp);
	float FixFilter(int samp);		//��X�o�i��
	float *Coeff_40_LP;				//�o�i���Y��
	float *Coeff_60Notch;
	float *Coeff_0_05_HP;
	void LoadFilterCoeff(int sampling_rate);//�p��Y��
public:
	float RTConvolution(float *xn,float *hn,int hn_len);  //�Y���o�i
	float Sinc(float FC,int n,int FilterLen);			//�p��Y�ƮɥΨ쪺sinc
	void CreatFIRCoeff(float *Coeff,float CutFreq,int SamplingRate,int FilterLen,int FilterType);
	void Convolution(float *xn,float *yn,float *hn,int hn_len,int xn_len);
	float *Filter_Buffer_40,*Filter_Buffer_60,*Filter_Buffer_0_05;
	~CFilter(void);
};


#include "Filter.h"

#define SMOOTH_FILTER_LEN	8
#define RANG_BUFFER_SIZE	512

#define ANA_TASK_BEAT_DETECT	1
#define ANA_TASK_TEMPLETE		2
#define ANA_TASK_DEBUG_VIEW		4
#define ANA_TASK_BEAT_DETECT_NODIS		8
#define ANA_TASK_FFT			16
#define ANA_TASK_HRV			32
/*
class CMeanDeviation
{
public:

	CMeanDeviation(float *buffer,int MaxBuffer);
	void InsertValue(float value);  //return true of false for present value is over mean or not
	bool IsOverMean(float value);
	int Max,Min;
	int Index;
	float *MAbuffer;  //Moving average buffer
	float mean;
	void MeanCalculation();
	~CMeanDeviation();

private:
	int max_buffer;
};*/


class CPeakDetection
{
public:

//Peak Detection
	~CPeakDetection(void);
	CPeakDetection(int s,int dis_buffer_size,int BeatAverageTimes,int Resolution);
								//S: �����W�v
								//dis_buffer_size: ��ܤ߹q�ϵe����rect.width()�j�p
								//BeatAverageTimes: �߹q�Ϫ���������,�]1�Y�i

	int		BeatNumber;			//�֭p�߸�
	float	BeatPerMin;			//�C�����߸�
	float	BeatInterval;		//�߸������j�ɶ�(��) 
	int		IsBeatOccur();		//�I�s InsertValue����Y���߸��o�ͫh�|�Ǧ^�߸���m 
	void	InsertValue(int sample);			//�N�C�����˼ƭȥ�J�èϥ�IsBeatOccur();���˼ƭȥ�t
												//�d�ݬO�_���߸�
	UINT	HighFreqComp;				//���o���W����
	void 	ChangeThreshold(float thres);	//�]�w����,
	UINT	GetThreshold();				//���o�ثe������
	int  	Is_Templet_Finish();		//�Ǧ^�O�_�w�NQRS�i��JPeak_Shape
	bool	IsTimeStable();

	UINT HFComponentMax,HFComponentMin; //���W�������̤j�ȻP�̤p��
	int HFMaxMinDiff;
	int  Templet_max_count;				//�C�Ӥ߹q�Ϫ����˭Ӽ�,�������W�v*0.8
	UINT BeatAverageTimes;				//�x�s�߹q�Ϫ��Ӽ�
	UINT TempleteIndex;					//�ثe����쪺�߹q�Ϫ��x�s��m
										//Peak_Shape[TempleteIndex][i]
	float Decision;
	int 	**Peak_Shape;					//�x�s�߹q�Ϫ��x�}
	float		*BeatAverage();					//�����ثe���߸��˪O
	int HFmean;
	float BeatInterval_ms;
	float DecisionThreshold;
	int PeakPosIndex,PeakAmount;
	int FindingPosition(int RPos);
	int GetPeakPosition();
	bool	IsTempleteStable();
	int GetPeakDeviation();				//�Y���߸��h�|�Ǧ^�P�ثe������R�i�̰��I�P�ثe����sample��m���t
	int PeakDeviation;
	int *TraceWindowSlot;
	int HFMaxDiffBack,HFMaxDiffFront,DiffFrontAdd,DiffFrontAdd_Lag1;
	float SlidingWindowThreshold;
	float MaxMinDiffThreshold;
	int TimeFactor,max_count;
private:

//Peak Detection
	int sample_counter;
	int Resolution;
	int BeatPosition;
	float MaxMinThreshold;
	int *Rangbuffer;
	int Piezo_sb_Count,Piezo_Window_Skip,Crossing_Up,Crossing_Down,*Shift_buffer,*Window_buffer;
	UINT Window_Size,Shift_Window_Size,Group_Delay;
	UINT Threshold;
	float BeatInterval_average;
	int sample_count,SystemTick;
	bool BeatPresent;
	int DetectionRoutine();
	int Sampling_Rate;
	int Normalize,UpDownCount;
	int FindPeakPosition(int stable);
	bool MayBePeak(int mean);
//	CMeanDeviation *HFComponentMean;
//	CMeanDeviation *TempleteMean;
//Templete
	int Shape_Count,UpdatePeakShapFinish;
	UINT BeatQueue[3],BeatQueue_f,BeatQueue_r,BeatNum;
	float *Beat_Average;
	void AverageTemplete();
	bool IsTempleteMatch();
	bool TempleteStable;
	void FindTemplete(int BeatPos);
//Time mean
	bool TimeStable;
	int TimeMeanIndex,TimeStableCount;
	float TimeMeanBuf[5],TimeMean;
	int Max0_2,MaxLeast0_2,MaxFuture0_2;
	
	int RRInterval_Index,RRInterval[512];

//Adjestable threshold

	float	*TempleteBuffer;
	float   *Decision_Window_buffer,DecisionMax,DecisionMin;
	float	DecisionMean(int diff);
	bool	IsQRS(int mean);
	float	*DecisionMeanBuffer;
	int		Decision_Crossing_Up,Decision_Crossing_Down,DecisionMeanIndex;
//Mean of MaxMin diff
	float	DiffMean(int diff);
	int		*DiffMeanBuffer,DiffMeanIndex;
//Filter


};


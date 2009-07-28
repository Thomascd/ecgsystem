
class CECGRecorder : public CObject
{
public:
	CECGRecorder();

	CString		Name;
	CString		Sex;
	CString		Age;
	int		SamplimgFreq;
	int		Resolution;
	UINT	BeatNum;

	CWordArray	AverageECG;			//�ϥ�short int
	CWordArray	AverageECG_Index;	//�ϥ�short int
	CWordArray  HR_Freq;			//�ϥίB�I���
	CWordArray	RRInterval;			//�ϥίB�I���
	CWordArray	ecg_data;			//�ϥ�short int
	CWordArray  peak_pos_ary;		//�ϥ�short int
	
	void SerializeAll(CArchive *ecg_archive);
	void Remove();
public:
	~CECGRecorder(void);
};

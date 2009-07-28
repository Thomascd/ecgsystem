#include "globol.h"

//������Buffer�����R�A?
//�]���ݭn�̷Ӹ˸m���t�רӨM�wbuffer�j�p,Buffer�j�p���@����,�N����b�P�@�ӵ��c���ŧi���@�˪��j�p���}�C

//Why dont define buffer as static?
//I need determinate the size of buffer with speed of device.
//If the size of buffer is not fix, the array with different size doesn't be defined in the same struct.

struct BUFFER{
    BYTE    *Buffer;
    UINT    r_front,r_rear;
    UINT    Remaining_Byte;
    UINT    Length;
    BYTE    Flag;
};


#define AVAILABLE 1
#define FULL      2

#define NO_PIPE  0xff

#define PIPE_FREE 0xff    //no one use this pipe

#define BUFFER_AMOUNT 10

#define PIPE struct BUFFER

extern PIPE rb[BUFFER_AMOUNT];
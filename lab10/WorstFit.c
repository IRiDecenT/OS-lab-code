#include "WorstFit.h"
#include "BestFit.h"

int WorstFit_alloc(char job, int size)
{
    return BestFit_alloc(job, size);
}

int WorstFit_free(char job)
{
    return _basic_free(job, nodeCmpByLen_Worst,
                    allocated_header, free_header);
}

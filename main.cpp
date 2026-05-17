#include <iostream>

#include "SigMFDataType.h"
#include "SigMFDataset.h"

int main()
{
    SigMFDataset sigmfData = SigMFDataset("/var/home/edwsanch/Downloads/trimmedSamples.sigmf-data", SigMFDataType("cf32_le"), 0, 1, 0);

    std::cout << sigmfData.getDataType().getRawDataType() << '\n';
    return 0;
}

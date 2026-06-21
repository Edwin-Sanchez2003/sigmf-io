# SigMFDataset
The SigMF Dataset class is meant to interface with the file system to load signal data into memory.

## Data Loading
A SigMFDataset refers to the file on disk that contains signal data. This, along with a SigMFMetadata file, is used to load data from disk into memory.

To load the signal data, you need to know the following parameters from the metadata:

### Global
From the global field, you need:
* **core:datatype** - this field gives you information on how individual samples are stored on disk.
    - primitive data type on disk
    - endianness of the data (byte order)
    - sample type (whether the data is complex or real-valued signal data).
* **core:
* **core:offset** - this field (used in SigMFCollections) is used to record the relative position of the file within a stream of recorded SigMFDataset files. **core:sample_start** values within a SigMFDataset must be greater than or equal to this value.


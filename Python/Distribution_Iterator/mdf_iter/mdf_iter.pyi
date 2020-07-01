import pandas

from typing import Iterator, List

from mdf_iter.IFileInterface import IFileInterface


class CANRecord(object):
    """Simple object with all data as public attributes.
    
    """
    TimeStamp: float
    BusChannel: int
    ID: int
    IDE: bool
    DLC: int
    DataLength: int
    Dir: bool
    EDL: bool
    BRS: bool
    DataBytes: List[int]
    ...


class CANIterator(object):
    """Iterator over CAN records.
    
    """
    def __iter__(self) -> Iterator[CANRecord]: ...
    ...


class MdfFile(object):
    """Mdf file loaded by the C++ library.
    
    """
    def __init__(self, data_source: IFileInterface, cache_size: int = 4096):
        """Constructor. Requires a callback to read data through.
        
        :param data_source:     Object implementing IFileInterface, to support data access through read and seek calls.
        :param cache_size:      Specify how large the internal buffer is. Larger values reduces the number of read
                                calls.
        :raises ValueError:     Invalid input arguments, more detail in the error message.
        :raises RuntimeError:   Error from the C++ library, more detail in the error message.
        """
        ...
    
    def get_can_iterator(self) -> CANIterator:
        """Get a corresponding CAN record iterator from the C++ library.
        
        :return: CAN iterator.
        """
        ...
    
    def get_data_frame(self) -> pandas.DataFrame:
        """If all required dependencies are detected, allows the export of the entire file as a pandas DataFrame.
        
        :return: pandas DataFrame with all CAN data.
        """
        ...
    
    def get_first_measurement(self) -> int:
        """Get the epoch of the first CAN measurement in the file.
        
        :return: Timestamp in nanoseconds since epoch.
        """
        ...
    
    ...

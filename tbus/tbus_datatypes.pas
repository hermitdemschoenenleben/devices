unit TBus_DataTypes;

{$mode objfpc}{$H+}

interface

type
  {These aliases refer to the LabView names for basic data types.
   They are more clear than the standard Pascal names.}
  I8  = ShortInt;
  I16 = SmallInt;
  I32 = LongInt;
  U8  = byte;
  U16 = word;
  U32 = DWord;
  integer = I32;
  {int64 is already predefined}
  float = double;

type
  tBusRecord = packed record
      Data : U32;
      Address : U32;
      Duration : single; {32 bit floating point}
      Flags : U32;
    end;
  pBusData = ^tBusData;
  tBusData = packed array[0..0] of tBusRecord;

Const {Flags}
  FlRead =  $00000000;{from hardware to computer}
  FlWrite = $00000001;{from computer to hardware}

implementation

end.


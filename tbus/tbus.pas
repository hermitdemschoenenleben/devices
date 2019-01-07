
{
Main library TBus dll

Its purpose is to translate between user-friendly parameters and
hardware internal binary registers.

It is optimized for user friendlyness and simple hardware implementation,
it is not optimized for maximum speed, although current GHz processors
should still allow an effective speed of several MHz on the hardware, it
will most likely limited by the hardware interface anyhow.

Thijs Wendrich
Institute of Quantum Optics
University of Hannover
Welfengarten 1
30167 Hannover
Germany

June 2009
}


{compile with Lazarus 0.9.26.2 beta with FPC 2.2.2}
{compile using "menu->Run->Build file" not with F9 (run)}
{$MODE Delphi}
{$SMARTLINK on}
{$VERSION 0.8.3}

{$DEFINE TBus_Debug}

{
To do:
  - Error checking, especially during file loading
  - Add indexed interface fuctions ('SetIntParamI')

  - Demo program that can manipulate any stack/card/parameter,
      also to test new cards. With extensive error checking.
  - Stack Editor

Note:
  - FloatParam, BoolParam and IntParam are operational
  - GetBusWriteData (convert parameters to busdata) is operational
  ==> Simple output-only devices can already make use of this code directly
  - Card files are searched first by name, then by '.card.xml' finally '.xml'
      in first the given path, then the current directory and finally
      in the path of the stack file.
      This searching may cause some delay during loading.
}

Library TBus;
{DLL interface}

uses
   SysUtils, xmlread, dom, strutils, math, classes,
   TBus_DataTypes;

{ $R tbus.rc}


procedure GetTBusVersion(var s : shortstring); stdcall;
begin
  s:='TBus.dll version 0.8.3 for NI-RT, 27 oct 2010';
{  s:='TBus.dll version 0.8.3 for WIN32, 27 oct 2010';}
{$ifdef TBus_Debug}
  s:=s+', with debug options';
{$endif}
end;


const
  eOK = 0;
  eUnknownParameter = -1;
  eUnknownCard  = -2;
  eUnknownStack = -3;
  eUnknownRegister = -4;
  eInvalidType = -5;
  eInvalidStackFile = -6;
  eInvalidCardFile  = -7;
  eDuplicateStackName = -8;
  eInvalidArrayLength = -9;
  eReadPatternMismatch = -10;
  eMissingInterface = -11;
  eUnknownError = -100;

type
  eCardFileNotFound = class(Exception);

type
  tMyStringList = Class(tStringList)
    public
      procedure AddPureCommaText(s : string);
  end;
  tParameter = class
    constructor Create(node : tDOMNode);
    destructor Destroy; override;
    function GetBit(i : I32) : boolean; virtual; abstract;
    procedure SetBit(pos : I32; value : boolean); virtual; abstract;
    private
{    fname : shortstring; {*}{replace with tStringList}
    fName : tMyStringList;
  end;
  tBoolParam = class(tParameter)
    constructor Create(node : tDOMNode);
    procedure setvalue(v : shortstring); overload;
    procedure setvalue(v : boolean); overload;
    function getValue : boolean;
    function GetBit(i : I32) : boolean; override;
    procedure SetBit(pos : I32; value : boolean); override;
    private
    fvalue : boolean;
  end;
  tIntParam = class(tParameter)
    constructor Create(node : tDOMNode);
    procedure setvalue(v : shortstring); overload;
    procedure setValue(v : U32); overload;
    function getValue : U32;
    function GetBit(i : I32) : boolean; override;
    procedure SetBit(pos : I32; value : boolean); override;
    private
    fvalue : U32;
    fMin,fMax : U32;
  end;
  tFloatParam = class(tParameter)
    constructor Create(node : tDOMNode);
    procedure setvalue(v : shortstring{'242.54397'}); overload;
    procedure setvalue(v : double); overload;
    function GetValue : double;
    function GetBit(i : I32) : boolean; override;
    procedure SetBit(pos : I32; value : boolean); override;
    private
    fvalue : extended;
    fmin, fmax : extended;
    fOnUnderflow, fOnOverflow : (oClip,oModulo,oIgnore,oAllow);
    fAbs : boolean;
  end;
  tBusState = class;
  tCard = class;
  tBusDirection = (directionRead,directionWrite);
  tBusState = class
    fDirection : tBusDirection;
    fDuration : double;
    fAddressOffset : U32;
    fState :  array[0..31] of {0=lsb, 31=msb}
      record
        bit : I32;
        parameter : tParameter; {reference to card.parameters}
        value :  boolean;
      end;
    constructor Create(card : tCard; node : tDOMNODE);
    function GetBusstate : U32;
    procedure SetBusState(state : U32);
  end;
  tRegister = class
    fname : shortstring;
    fPreferedMethod : shortstring;{}
    fDefaultDuration : float;     {}
    fSequence : array of tBusState;{}
    constructor Create(card : tCard; node : tDOMNode);
  end;
  pStack = ^tStack;
  tCard = class
    fName : shortstring;
    fType : shortstring;
    fAddress : U32;
    parameters : array of tParameter;
    registers : array of tRegister;
    owner : pStack;
    interfaceType : shortstring;
    interfaceID : shortstring;
    constructor Create(aowner : pstack; name : ShortString; cardtype : ShortString; address : U32);
    procedure LoadFromFile(filename : string);
    function FindParameter(name : shortstring) : tParameter;
    function FindRegister(name : shortstring) : tRegister;
  end;
  tStack = class
    fname : shortstring;
    ffilename : string;
    cards : array of tCard;
    constructor Create(name : shortstring);
    function FindCard(name : shortstring) : tCard;
{    procedure AddCard(name : ShortString; cardtype : ShortString; address : byte);}
  end;

var
  Stacks : array of tStack;

{-------------------------------------------------------------}

procedure WriteLog(s : string);
{$ifdef TBus_Debug}
var
  f : TextFile;
  fname : String;
{$endif}
begin
{$ifdef TBus_Debug}
//        writeln(s);
{$endif}
end;

function MyStrToFloat(s : string) : double;
var
  i : integer;
  u : string;
begin
  result:=0;
  s:=uppercase(s);
  for i:=1 to length(s) do
    if s[i]=',' then s[i]:='.';
  i:=length(s);
  u:='';
  while (i>0) and (s[i] in ['A'..'Z']) do
    begin
      u:=s[i]+u;
      i:=i-1;
    end;
  s:=copy(s,1,length(s)-length(u));
  try
    result:=strtofloat(s);
    if (u='MS') then result:=result*1e-3;
    if (u='US') then result:=result*1e-6;
    if (u='NS') then result:=result*1e-9;
    if (u='PS') then result:=result*1e-12;
    if (u='FS') then result:=result*1e-15;
    if (u='KS') then result:=result*1e+3;
  except
    {*}
  end;
{  writelog('MyStrToFloat: value:'+s+' unit:'+u+'  result:'+floattostr(result));}
end;

function MyStrToBool(s : string) : boolean;
begin
  s:=uppercase(s);
  result:=false;
  if (s='T') or (s='TRUE') or
     (s='H') or (s='HI') or (s='HIGH') or
     (s='1') or
     (s='ON') then result:=true;
end;

function bin2int(s : string) : integer;
var
  i : integer;
begin
  result:=0;
  for i:=1 to length(s) do
    case s[i] of
      '0','l','L','f','F' : result:=result shl 1;
      '1','h','H','t','T' : result:=(result shl 1)+1;
    else
      result:=result shl 1;
      {raise eConversionError}
    end;
end;

function FindAttribute(element : tDOMNode; name : string) : tDOMNode;
var
  j : integer;
begin
  result:=nil;
  name:=uppercase(name);
  if element.HasAttributes then
    for j:=0 to  element.Attributes.Length-1 do
      if name=uppercase(element.Attributes.Item[j].NodeName) then
        result:=element.Attributes.Item[j];
end;

function FindStack(name : shortstring) : tStack;
var
  i : integer;
begin
  result:=nil;
  name:=uppercase(name);
  for i:=0 to length(stacks)-1 do
    if name=uppercase(stacks[i].fname) then result:=stacks[i];
end;

{-------------tMyStringList--------------------------------}

procedure tMyStringList.AddPureCommaText(s : string);
var
  t : string;
  i : integer;
begin
  t:='';
  for i:=1 to length(s) do
    if s[i]=Delimiter then
      begin
        Add(trim(t));
        t:='';
      end
    else
      t:=t+s[i];
  Add(trim(t));
end;

{-------------tBusState--------------------------------}

constructor tBusState.Create(card : tCard; node : tDOMNODE);
  function GetSubStringR(s : string; index : I32) : string;
  var
    i,n : integer;
  begin
    result:='';
    i:=length(s);
    n:=0;
    while (i>0) and (n<index) do
      begin
        if s[i]=',' then n:=n+1;
        i:=i-1;
      end;
    while (i>0) and (s[i]<>',') do
      begin
        result:=s[i]+result;
        i:=i-1;
      end;
  end;
var
  i,j,k : integer;
  s,t : string;
  AttrName : string;
begin {<BusPattern direction="w" pattern="1,0,0,0, 0,0,1,1" duration="25ns" />}
  for i:=0 to length(fstate)-1 do       {"msb,   ..  , lsb"}
    begin
      fstate[i].parameter:=nil;
      fstate[i].bit:=0;
      fstate[i].value:=false;
    end;
  if node=nil then exit;
  fAddressOffset:=0;
  fDuration:=25e-9;
  if node.HasAttributes then
    for i:=0 to node.Attributes.Length-1 do
      begin
        writelog(node.Attributes[i].NodeName);
        writelog(node.Attributes[i].NodeValue);
{        writelog('length fstate = '+inttostr(length(fstate)));}
        AttrName:=uppercase(node.Attributes[i].NodeName);
        if AttrName='DIRECTION' then
          begin
            s:=uppercase(node.Attributes[i].NodeValue);
            if (s='W') or (s='WRITE') then fDirection:=DirectionWrite;
            if (s='R') or (s='READ') then fDirection:=DirectionRead;
          end;
        try
        if AttrName='DURATION' then
          fDuration:=mystrtofloat(node.Attributes[i].NodeValue);
        except
        end;
        if (AttrName='ADDRESS') or (AttrName='ADDRESSOFFSET') then
          begin;
            fAddressOffset:=StrToInt(node.Attributes[i].NodeValue);
          end;
        if AttrName='PATTERN' then
          begin
            for j:=0 to length(fstate)-1 do
              begin
                s:=GetSubStringR(node.Attributes[i].NodeValue,j);
                s:=trimset(s,[#0..#32]);{  StripWhiteSpace(s);}
                s:=UpperCase(s);
{                writelog('element :'+inttostr(j)+'  s='+s);}
                if s='1' then
                  begin
                    fstate[j].parameter:=nil;
                    fstate[j].bit:=0;
                    fstate[j].value:=true;
{                    writelog('1');}
                  end
                else
                if (s='0') or (s='') then
                  begin
                    fstate[j].parameter:=nil;
                    fstate[j].bit:=0;
                    fstate[j].value:=false;
{                    writelog('0');}
                  end
                else
                if card is tCard then
                begin
{                  writelog('valid card, reference to param');}
                  for k:=1 to length(s) do
                    begin
                      if s[k] in ['(','{'] then s[k]:='[';
                      if s[k] in [')','}'] then s[k]:=']';
                    end;
                  if s[length(s)]=']' then {format is 'param[bit]'}
                      begin;
                        s:=copy(s,1,length(s)-1);
                        t:='';
                        k:=length(s);
                        while (k>0) and (s[k]<>'[') do
                          begin
                            t:=s[k]+t;
                            k:=k-1;
                          end;
                        fstate[j].bit:=strtoint(t);
                        s:=copy(s,1,k-1);
                      end
                    else {format is 'param', no bit index (only useful for boolparam)}
                      begin
                        t:='0';
                        fstate[j].bit:=0;
                      end;
{                  writelog('param:'+s+'---- bit:'+t+'--');}
                  fstate[j].parameter:=Card.FindParameter(s);
                  if fstate[j].parameter=nil then
                    writelog('parameter "'+s+'" not found');
                  writelog('param:'+s+'---- bit:'+t+'--');
                end
                else
                begin
                  {s is not in [0,1] or this tBusstate is not part of a Card}
                end;
              end;
          end;
      end;
end;

function tBusState.GetBusstate : U32;
var
  i : I32;
  r : I32;
begin
  r:=0;
  for i:=0 to length(fstate)-1 do
    begin
      if fstate[i].parameter<>nil then
          begin
            if fstate[i].parameter.GetBit(fstate[i].bit) then
              r:=r+(1 shl i);
          end
        else
          begin
            if fstate[i].value then
              r:=r+(1 shl i);
          end;
    end;
  result:=r;
{  writelog('GetBusState:'+inttostr(result))}
end;

procedure tBusState.SetBusState(state : U32);
var
  i : I32;
begin
  for i:=0 to 31 do
    begin
      if fstate[i].parameter<>nil then
          begin
            fstate[i].parameter.SetBit(fstate[i].bit, ((state shr i)and 1)=1);
          end;
      {else constant bit, read only}
    end;
end;

{-------------tRegister--------------------------------}

constructor tRegister.Create(card : tCard; node : tDOMNode);
var {<Register name="asdfasdfsd">...</Register>}
  i : I32;
begin
  setlength(fSequence,0);
  fPreferedMethod:='';
  fDefaultDuration:=25e-9;
  fname:='';
  if node=nil then exit;
  if node.HasAttributes then
    for i:=0 to node.Attributes.Length-1 do
      begin
        if uppercase(node.Attributes[i].NodeName)='NAME' then
          fname:=node.Attributes[i].NodeValue;
        if uppercase(node.Attributes[i].NodeName)='PREFEREDMETHOD' then
          fPreferedMethod:=node.Attributes[i].NodeValue;
        try
        if uppercase(node.Attributes[i].NodeName)='DEFAULTDURATION' then
          fDefaultDuration:=mystrtofloat(node.Attributes[i].NodeValue);
        except
          fDefaultDuration:=25e-9;{minimal default value}
        end;
      end;
  if node.HasChildNodes then;
    for i:=0 to node.ChildNodes.Count-1 do
      begin
        if uppercase(node.ChildNodes[i].NodeName)='BUSPATTERN' then
          begin
            setlength(fSequence,length(fSequence)+1);
            fSequence[length(fSequence)-1]:=tBusState.Create(card,node.ChildNodes[i]);
          end;
      end;
end;

{-------------tParameter--------------------------------}

constructor tParameter.Create(node : tDOMNode);
var
  i : I32;
begin
  if node=nil then exit;
  fname:=tMyStringList.Create;
  fname.CaseSensitive:=false;
  fname.Sorted:=false;
  fname.Delimiter:=',';
  for i:=0 to node.Attributes.Length-1 do
    if uppercase(node.Attributes[i].NodeName)='NAME' then
      fname.AddPureCommaText(node.Attributes[i].NodeValue);
{      fname.CommaText:=node.Attributes[i].NodeValue;}

{      fname:=node.Attributes[i].NodeValue;}
      {*}{name should be a comma separated list of different names for this parameter}
end;

destructor tParameter.Destroy;
begin
  fName.free;
  inherited Destroy;
end;

{-------------tBoolParam--------------------------------}

constructor tBoolParam.Create(node : tDOMNode);
 var
  i : I32;
begin
  inherited Create(node);
  if node=nil then exit;
  for i:=0 to node.Attributes.Length-1 do
    if uppercase(node.Attributes[i].NodeName)='DEFAULTVALUE' then
      setvalue(node.Attributes[i].NodeValue);
end;

function tBoolParam.getValue : boolean;
begin
  result:=fValue;
end;

procedure tBoolParam.setvalue(v : shortstring);
begin
  v:=uppercase(v);
  if (v='T') or (v='TRUE') or (v='1') or (v='H') or (v='HI') or (v='HIGH') then fvalue:=true;
  if (v='F') or (v='FALSE') or (v='0') or (v='L') or (v='LO') or (v='LOW') then fvalue:=false;
end;

procedure tBoolParam.setvalue(v : boolean); overload;
begin;
  fvalue:=v;
end;

function tBoolParam.GetBit(i : I32) : boolean;
begin            {boolparam has no index}
  result:=fValue;
end;

procedure tBoolParam.SetBit(pos : I32; value : boolean);
begin         {boolparam has no index}
  fValue:=value;
end;

{-------------tIntParam--------------------------------}

constructor tIntParam.Create(node : tDOMNode);
 var
  i : I32;
begin
  inherited Create(node);
  fMin:=0;
  fMax:=$FFFFFFFF;
  if node=nil then exit;
  for i:=0 to node.Attributes.Length-1 do
    if uppercase(node.Attributes[i].NodeName)='DEFAULTVALUE' then
      setvalue(node.Attributes[i].NodeValue);
end;

procedure tIntParam.setvalue(v : shortstring);
{var
  i : I32;}
begin  {hex: $1234, 0x1234; bin: #1010101, %10101; }
{  v:=uppercase(v);
  for i:=0 to length(v)-1 do
    if not(v[i] in ['A'..'Z','0'..'9']) then v[i]=' ';}
  v:=trimset(v,[#0..#32]);
  if length(v)>=2 then
      begin
        if (v[1]='$') then fvalue:=hex2dec(copy(v,2,length(v)))
        else
        if ((v[1]='0')and(v[2]='x')) then fvalue:=hex2dec(copy(v,3,length(v)))
        else
        if (v[1]='#') or (v[1]='%') then fvalue:=bin2int(copy(v,2,length(v)))
        else
        fvalue:=strtoint(v);
      end
    else
      fvalue:=strtoint(v);
end;

procedure tIntParam.setValue(v : U32);
begin
  fValue:=v;
end;

function tIntParam.getValue : U32;
begin
  result:=fValue;
end;

function tIntParam.GetBit(i : I32) : boolean;
begin  {to do: msb/lsb first}
  result:=(((1 SHL i) AND fValue)<>0);
end;

procedure tIntParam.SetBit(pos : I32; value : boolean);
var
  m : I32;
begin  {to do: msb/lsb first}
  m:=1 SHL pos;
  if value=true then
      fValue:=fValue OR m
    else
      fValue:=fValue AND (not m);
end;

{-------------tFloatParam--------------------------------}

constructor tFloatParam.Create(node : tDOMNode);
 var
  i : I32;
  n : string;
begin
  inherited Create(node);
  fmin:=-infinity;
  fmax:=+infinity;
  fAbs:=false;
  fOnOverflow:=oAllow;
  fOnUnderflow:=oAllow;
  if node=nil then exit;
  for i:=0 to node.Attributes.Length-1 do
    begin
      if uppercase(node.Attributes[i].NodeName)='DEFAULTVALUE' then
        setvalue(node.Attributes[i].NodeValue);
      if uppercase(node.Attributes[i].NodeName)='MIN' then
        fMin:=MyStrToFloat(node.Attributes[i].NodeValue);
      if uppercase(node.Attributes[i].NodeName)='MAX' then
        fMax:=MyStrToFloat(node.Attributes[i].NodeValue);
      if uppercase(node.Attributes[i].NodeName)='ABS' then
        fAbs:=MyStrToBool(node.Attributes[i].NodeValue);
      if uppercase(node.Attributes[i].NodeName)='ONOVERFLOW' then
        begin
          n:=uppercase(node.Attributes[i].NodeValue);
          if n='IGNORE' then fOnOverflow:=oIgnore;
          if n='CLIP' then fOnOverflow:=oClip;
          if n='MODULO' then fOnOverflow:=oModulo;
          if n='ALLOW' then fOnOverflow:=oAllow;
        end;
      if uppercase(node.Attributes[i].NodeName)='ONUNDERFLOW' then
        begin
          n:=uppercase(node.Attributes[i].NodeValue);
          if n='IGNORE' then fOnUnderflow:=oIgnore;
          if n='CLIP' then fOnUnderflow:=oClip;
          if n='MODULO' then fOnUnderflow:=oModulo;
          if n='ALLOW' then fOnUnderflow:=oAllow;
        end;
    end;
end;

procedure tFloatParam.setvalue(v : shortstring);
var
  i : I32;
begin
{  v:=uppercase(v);}
  for i:=1 to length(v) do
    if v[i]=',' then v[i]:='.';
  try
    SetValue(strtofloat(v));
  except
    {*}
  end;
end;

procedure tFloatParam.setvalue(v : double);
begin
  if fAbs=true then v:=Abs(v);
  if v<fMin then
    begin
      case fOnUnderflow of
        oIgnore : exit;
        oModulo : v:=v - fMax*floor(v/fMax);
        oClip   : v:=fMin;
        oAllow  : ;
      end;
    end;
  if v>fMax then
    begin
      case fOnOverflow of
        oIgnore : exit;
        oModulo : v:=v - fMax*floor(v/fMax);
        oClip   : v:=fMax;
        oAllow  : ;
      end;
    end;
  fvalue:=v;
end;

function tFloatParam.GetValue : double;
begin
  GetValue:=fValue;
end;

function tFloatParam.GetBit(i : I32) : boolean;
type
  tExt = packed record
           arr : array[0..7] of byte;
           exp : word;
         end;
var
  R : ^tExt;
  exponent : integer;
  sign : boolean;
  b : integer;
begin
{  result:=false;
  if (i>=0) and (i<length(fValue)) then result:=fValue[i];}
  R:=@fvalue;
  sign:=(R.exp AND $8000)<>0;
  exponent:=(R.exp AND $7FFF)-16383;
  b:=(i+63-exponent) {shr 3};
  if (0<=b) and (b<64) then
      begin
        result:=(R.arr[b shr 3] and ($01 shl (b and $07)))<>0;
      end
    else
      result:=false;
{  result:=b;}
end;

procedure tFloatParam.SetBit(pos : I32; value : boolean);
begin
  if getbit(pos)<>value then
    begin
      if value=true then
          fvalue:=fvalue+intpower(2,pos){may fail in extreme cases (1e20+1e-10)}
        else
          fvalue:=fvalue-intpower(2,pos);
    end;
end;

{---------------tStack----------------------------------}

constructor tStack.Create(name : shortstring);
begin
  fName:=name;
  setlength(cards,0);
end;

function tStack.FindCard(name : shortstring) : tCard;
var
  i : I32;
begin
  result:=nil;
  name:=uppercase(name);
  for i:=0 to length(cards)-1 do
    if name=uppercase(cards[i].fname) then result:=cards[i];
end;

{-------------------tCard-------------------------------------}

constructor tCard.Create(aowner : pstack; name : ShortString; cardtype : ShortString; address : U32);
begin;
  owner:=aowner;
  fName:=name;
  fAddress:=address;
  fType:=cardtype;
  interfaceType:='';
  interfaceID:='';
  setlength(parameters,0);
  setlength(registers,0);
  LoadFromFile(cardtype);
end;

procedure tCard.LoadFromFile(filename : string);
var
  doc : tXMLDocument;
  i : I32;
  s : string;
begin
  writelog('****time marker 0:'+FormatDateTime('YYYY MM DD hh:nn:ss.zz',now));
  if not fileexists(filename) then
    begin
      if fileexists(filename+'.card.xml') then
        filename:=filename+'.card.xml'
      else
      if fileexists(filename+'.xml') then
        filename:=filename+'.xml'
      else
      begin
        filename:=ExtractFileName(filename);
        if not fileexists(filename) then
        begin
          if fileexists(filename+'.card.xml') then
            filename:=filename+'.card.xml'
          else
          if fileexists(filename+'.xml') then
            filename:=filename+'.xml'
          else
          if assigned(owner) then
            begin;
              if fileexists(ExtractFilePath(owner.ffilename)+filename) then
                filename:=ExtractFilePath(owner.ffilename)+filename
              else
              if fileexists(ExtractFilePath(owner.ffilename)+filename+'.card.xml') then
                filename:=ExtractFilePath(owner.ffilename)+filename+'.card.xml'
              else
              if fileexists(ExtractFilePath(owner.ffilename)+filename+'.xml') then
                filename:=ExtractFilePath(owner.ffilename)+filename+'.xml'
              else
              begin;
                WriteLog('card file '+filename+' not found');
                exit; {raising an exception during construction is trouble}
  {             raise eCardFileNotFound.Create('card describtion file not found at '+filename);}
              end
            end
          else
          begin
            WriteLog('card file '+filename+' not found');
            exit; {raising an exception during construction is trouble}
  {         raise eCardFileNotFound.Create('card describtion file not found at '+filename);}
          end;
        end;
      end;
    end;
  WriteLog('Opening card file '+ExpandFileName(filename));
  writelog('****time marker 1:'+FormatDateTime('YYYY MM DD hh:nn:ss.zz',now));
  ReadXMLFile(doc,filename);
  writelog('****time marker 2:'+FormatDateTime('YYYY MM DD hh:nn:ss.zz',now));

  if doc.DocumentElement.HasAttributes then
    for i:=0 to doc.DocumentElement.Attributes.Length-1 do
      begin
        if uppercase(doc.DocumentElement.Attributes[i].NodeName)='INTERFACETYPE' then
          interfaceType:=doc.DocumentElement.Attributes[i].NodeValue;
{        if uppercase(doc.DocumentElement.Attributes[i].NodeName)='INTERFACEID' then
          interfaceID:=doc.DocumentElement.Attributes[i].NodeValue;}
          {interfaceID should not be here in the card file, but should be in the stack file}
      end;

  for i:=0 to  Doc.DocumentElement.ChildNodes.count-1 do
    begin
      s:=uppercase(Doc.DocumentElement.ChildNodes.Item[i].NodeName);
      if (s='FLOATPARAMETER') or (s='FLOAT') then
        begin
          setlength(parameters,length(parameters)+1);
          parameters[length(parameters)-1]:=tFloatParam.create(Doc.DocumentElement.ChildNodes.Item[i]);
        end;
      if (s='INTPARAMETER') or (s='INTEGERPARAMETER') or (s='INTPARAM') or (s='INT') or (s='INTEGER') then
        begin
          setlength(parameters,length(parameters)+1);
          parameters[length(parameters)-1]:=tIntParam.create(Doc.DocumentElement.ChildNodes.Item[i]);
        end;
      if (s='BOOLPARAMETER') or (s='BOOLEANPARAMETER') or (s='BOOLPARAM') or (s='BOOL') then
        begin
          setlength(parameters,length(parameters)+1);
          parameters[length(parameters)-1]:=tBoolParam.create(Doc.DocumentElement.ChildNodes.Item[i]);
        end;
      if s='REGISTER' then
        begin
          setlength(registers,length(registers)+1);
          registers[length(registers)-1]:=tRegister.create(self,Doc.DocumentElement.ChildNodes.Item[i]);
        end;
    end;
  doc.free;
end;

function tCard.FindParameter(name : shortstring) : tParameter;
var
  i : I32;
begin
  result:=nil;
  name:=uppercase(name);
  for i:=0 to length(parameters)-1 do
    if parameters[i].fname.IndexOf(name)>=0 then result:=parameters[i];
{    if name=uppercase(parameters[i].fname) then result:=parameters[i];}
end;

function tCard.FindRegister(name : shortstring) : tRegister;
var
  i : I32;
begin
  result:=nil;
  name:=uppercase(name);
  for i:=0 to length(registers)-1 do
    if name=uppercase(registers[i].fname) then result:=registers[i];
end;

{--------------------DLL interface-----------------------------------------}

function OpenStack(StackName : shortstring; StackFile : shortstring) : I32; stdcall;
{load stack description from file location path}
var
  doc : tXMLDocument;
  i : I32;
  A,B,C,D : tDOMNode;
  stack : tStack;
begin
  result:=eOk;
  if not fileexists(Stackfile) then
    begin;
      if fileexists(Stackfile+'.stack.xml') then
        Stackfile:=Stackfile+'.stack.xml'
      else
      if fileexists(Stackfile+'.xml') then
        Stackfile:=Stackfile+'.xml'
      else
      begin
        WriteLog('stack file '+stackfile+' not found');
        result:=eInvalidStackFile;
        exit;
      end;
    end;
  WriteLog('Opening stack file '+StackFile);
  ReadXMLFile(doc,StackFile);
  setlength(stacks,length(stacks)+1);
  stacks[length(stacks)-1]:=tStack.Create(StackName);
  stack:=stacks[length(stacks)-1];
  stack.ffilename:=ExpandFileName(StackFile);
  for i:=0 to Doc.DocumentElement.ChildNodes.count-1 do
    begin
{      writelog(Doc.DocumentElement.NodeName);}
      if (uppercase(Doc.DocumentElement.NodeName)='STACK') and
         (uppercase(doc.DocumentElement.ChildNodes.Item[i].NodeName)='CARD') then
        begin
          A:=FindAttribute(Doc.DocumentElement.ChildNodes.Item[i],'Name');
          B:=FindAttribute(Doc.DocumentElement.ChildNodes.Item[i],'Type');
          C:=FindAttribute(Doc.DocumentElement.ChildNodes.Item[i],'Address');
          D:=FindAttribute(Doc.DocumentElement.ChildNodes.Item[i],'InterfaceID');
          if (A=nil) or (B=nil) or (C=nil) then
            begin
              result:=eInvalidStackFile;
            end
          else
            begin
              setlength(stack.cards,length(stack.cards)+1);
              try
              stack.cards[length(stack.cards)-1]:=tCard.Create(@stack,A.NodeValue,B.NodeValue,strtoint(C.NodeValue));
              if assigned(D) then
                begin
                  stack.cards[length(stack.cards)-1].interfaceID:=D.NodeValue;
                  writelog('InterfaceID='+D.NodeValue);
                end;
              except
                on eCardFileNotFound do result:=eInvalidCardFile;
                else
                result:=eUnknownError;
              end
            end;
        end;
    end;
  doc.free;
end;

function CloseStack(Stack : shortstring) : I32;  stdcall;
{unload stack description from memory,
(automatically called on DLL unload)}
var
{  s : tStack;}
  i,j : integer;
begin
  result:=eOk;

  for i:=length(stacks)-1 downto 0 do
    if (assigned(stacks[i])) and (uppercase(Stack)=uppercase(stacks[i].fname)) then
      begin
        for j:=i to length(stacks)-2 do
          stacks[j]:=stacks[j+1];
        setlength(stacks,length(stacks)-1);
      end;
    {if never found then result:=eUnkownStack; }

(*  s:=findstack(stack);
  if s<>nil then
      begin
        s.free;
        {*}{remove s from array}
      end
    else
      result:=eUnkownStack;*)
end;

function CloseAllStacks : I32; stdcall;
var
  i : I32;
begin
  result:=eOk;
  try
    for i:=0 to length(stacks)-1 do
      if assigned(stacks[i]) then stacks[i].Free;
    setlength(stacks,0);
  except
    result:=eUnknownError;
  end;
end;

function SetBoolParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; value : LongBool) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tBoolParam then
                tBoolParam(p).SetValue(value)
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function SetIntParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; value : I32) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tIntParam then
                tIntParam(p).SetValue(value)
              else
              if p is tFloatParam then
                tFloatParam(p).SetValue(value)
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function SetFloatParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; value : double) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tFloatParam then
                tFloatParam(p).setvalue(value)
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetBusDataSize(Stack : shortstring; Card : shortstring; reg : shortstring;
            var Size : I32) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  r : tRegister;
begin
 result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              r:=c.FindRegister(reg);
              if r<>nil then
                  size:=length(r.fSequence)
                else
                  result:=eUnknownRegister;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function RegisterToBus(Stack : shortstring; Card : shortstring; reg : shortstring;
  size : I32; Bus : pBusData) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  r : tRegister;
  i : I32;
begin  {convert internal parameters to busdata}
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              r:=c.FindRegister(reg);
              if (r<>nil)  then
                  begin
                    if length(r.fSequence)<=size then
                        begin
                          for i:=0 to length(r.fSequence)-1 do
                            begin
                              Bus^[i].Address:=c.fAddress+r.fSequence[i].fAddressOffset;
                              Bus^[i].Data:=r.fSequence[i].GetBusstate;
                              Bus^[i].Duration:=r.fSequence[i].fDuration;
                              if r.fSequence[i].fDirection=directionRead
                                then
                                  Bus^[i].Flags:=FlRead
                                else
                                  Bus^[i].Flags:=FlWrite;
                            end;
                        end
                      else
                      result:=eInvalidArrayLength;
                  end
                else
                  result:=eUnknownRegister;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function BusToParam(Stack : shortstring;  Card : shortstring; reg : shortstring;
            size : I32; data : pBusData) : I32; stdcall;
{convert busdata to internal parameters}
var
  s : tStack;
  c : tCard;
  r : tRegister;
  i : I32;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              r:=c.FindRegister(reg);
              if (r<>nil)  then
                  begin
                    if length(r.fSequence)=size then
                        begin
                          for i:=0 to length(r.fSequence)-1 do
                            begin
{                              WriteLog(inttostr(Data^[i].Address)+' '+inttostr(c.fAddress));
                              WriteLog(inttostr(Data^[i].Flags)+' '+inttostr(FlRead));}
                              if (Data^[i].Address=c.fAddress+r.fSequence[i].fAddressOffset) and
                                 ((Data^[i].Flags and FlWrite)=FlRead) and
                                 (r.fSequence[i].fDirection = DirectionRead) then
                                begin;
                                  r.fSequence[i].SetBusstate(Data^[i].Data);
                                end
                              else
                              if (((Data^[i].Flags and FlWrite)=FlRead) and
                                  (r.fSequence[i].fDirection = DirectionWrite)) or
                                 (((Data^[i].Flags and FlWrite)=FlWrite) and
                                  (r.fSequence[i].fDirection = DirectionRead)) then
                                begin
                                  result:=eReadPatternMismatch;
{                                  WriteLog('Read Pattern Mismatch');}
                                end;
                              {else direction=write}
                              {To do: check that write only bits match}
                            end;
                        end
                      else
                      result:=eInvalidArrayLength;
                  end
                else
                  result:=eUnknownRegister;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetBoolParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; var value : LongBool) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tBoolParam then
                value:=tBoolParam(p).fvalue
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetIntParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; var value : I32) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tIntParam then
                value:=tIntParam(p).fvalue
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetFloatParam(Stack : shortstring; Card : shortstring;
  Parameter : shortstring; var value : double) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  s:=findstack(stack);
  if s<>nil then
      begin
        c:=s.findcard(card);
        if c<>nil then
            begin
              p:=c.findparameter(parameter);
              if p is tFloatParam then
                value:=tFloatParam(p).GetValue
              else
                result:=eUnknownParameter;
            end
          else
            result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetNumberOfStacks : I32; stdcall;
begin
  result:=length(Stacks);
end;

function GetStackName(index : I32; var StackName : shortstring) : I32; stdcall;
begin
  if @StackName=nil then WriteLog('recieved StackName=0 as input to function GetStackName');
  if (index>=0) and (index<length(stacks)) then
      begin
        StackName:=stacks[index].fname;
        result:=eOk;
      end
    else
      begin
        StackName:='';
        result:=eUnknownStack;
      end;
end;

function GetNumberOfCards(stack : shortstring; var NumberOfCards : I32) : I32; stdcall;
var
  s : tStack;
begin
  result:=eOk;
  NumberOfCards:=0;
  s:=findStack(stack);
  if s<>nil then
      NumberOfCards:=length(s.cards)
    else
      result:=eUnknownStack;
end;

function GetCardName(stack : shortstring; index : I32; var CardName : shortstring) : I32; stdcall;
var
  s : tStack;
begin
  if @CardName=nil then WriteLog('recieved CardName=0 as input to function GetCardName');
  CardName:='';
  s:=findStack(stack);
  if s<>nil then
      begin
      if (index>=0) and (index<length(s.cards)) then
          begin
            CardName:=s.cards[index].fname;
            result:=eOk;
          end
        else
          result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetCardType(stack : shortstring; index : I32; var CardType : shortstring) : I32; stdcall;
var
  s : tStack;
begin
  if @CardType=nil then WriteLog('recieved CardName=0 as input to function GetCardName');
  CardType:='';
  s:=findStack(stack);
  if s<>nil then
      begin
      if (index>=0) and (index<length(s.cards)) then
          begin
            CardType:=s.cards[index].fType;
            result:=eOk;
          end
        else
          result:=eUnknownCard;
      end
    else
      result:=eUnknownStack;
end;

function GetNumberOfParameters(stack : shortstring; card : shortstring; var NumberOfParameters : I32) : I32; stdcall;
var
  s : tStack;
  c : tCard;
begin
  result:=eOK;
  NumberOfParameters:=0;
  s:=findStack(stack);
  if s<>nil then
    begin;
      c:=s.FindCard(card);
      if c<>nil then
          NumberOfParameters:=length(c.parameters)
        else
          result:=eUnknownCard;
    end
    else
    result:=eUnknownStack;
end;

function GetParameterName(stack : shortstring; card : shortstring; index : I32; var ParameterName : shortstring) : I32; stdcall;
var  {returns the first name of the parameter, the alternative names are not shown}
  s : tStack;
  c : tCard;
begin
  result:=eOK;
  ParameterName:='';
  s:=findStack(stack);
  if s<>nil then
    begin;
      c:=s.FindCard(card);
      if c<>nil then
        begin
        if (index>=0) and (index<length(c.parameters)) then
          ParameterName:=c.parameters[index].fName[0];{*}{Return the first name of this register}
          {else result:=eUnkownParameter}
        end
        else
          result:=eUnknownCard;
    end
    else
      result:=eUnknownStack;
end;

function GetParameterType(stack : shortstring; card : shortstring; param : shortstring; var ParamType : shortstring) : I32; stdcall;
var
  s : tStack;
  c : tCard;
  p : tParameter;
begin
  result:=eOk;
  ParamType:='';
  s:=findStack(stack);
  if s<>nil then
    begin;
      c:=s.FindCard(card);
      if c<>nil then
        begin
          p:=c.FindParameter(param);
          if p<>nil then
            begin
              if p is tFloatParam then ParamType:='float'
              else
              if p is tIntParam then ParamType:='int'
              else
              if p is tBoolParam then ParamType:='bool'
              else
                ParamType:='unknown';
            end
            else
              result:=eUnknownParameter;
        end
        else
          result:=eUnknownCard;
    end
    else
      result:=eUnknownStack;
end;

function GetNumberOfRegisters(stack : shortstring; card : shortstring; var NumberOfRegisters : I32) : I32; stdcall;
var
  s : tStack;
  c : tCard;
begin
  result:=eOK;
  s:=findStack(stack);
  if s<>nil then
    begin;
      c:=s.FindCard(card);
      if c<>nil then
          NumberOfRegisters:=length(c.registers)
        else
          result:=eUnknownCard;
    end
    else
      result:=eUnknownStack;
end;

function GetRegisterName(stack : shortstring; card : shortstring; index : I32; var RegisterName : shortstring) : I32; stdcall;
var
  s : tStack;
  c : tCard;
begin
  RegisterName:='';
  result:=eOK;
  s:=findStack(stack);
  if s<>nil then
    begin;
      c:=s.FindCard(card);
      if c<>nil then
        begin
        if (index>=0) and (index<length(c.registers)) then
            RegisterName:=c.registers[index].fname
          else
            result:=eUnknownRegister;
        end
        else
          result:=eUnknownCard;
    end
    else
    result:=eUnknownStack;
end;

function GetStackInterface(stack : shortstring; var iType, iID : shortstring) : I32; stdcall;
var
  s : tStack;
  i : integer;
begin
  iType:='';
  iID:='';
  result:=eOK;
  s:=findStack(stack);
  if s<>nil then
    begin
      result:=eMissingInterface;
      for i:=0 to length(s.cards)-1 do
        if s.cards[i].interfaceType<>'' then
          begin
            iType:=s.cards[i].interfaceType;
            iID:=s.cards[i].interfaceID;
            result:=eOK;
          end;
    end
    else
      result:=eUnknownStack;
end;

procedure GetErrorString(error : I32; var ErrorString : shortstring); stdcall;
begin
  case error of
    eOK : ErrorString:='Ok';
    eUnknownParameter : ErrorString:='Unknown parameter';
    eUnknownCard : ErrorString:='Unknown card';
    eUnknownStack : ErrorString:='Unknown stack';
    eUnknownRegister : ErrorString:='Unkown register';
    eInvalidType : ErrorString:='Invalid data type';
    eInvalidStackFile : ErrorString:='Invalid stack file';
    eInvalidCardFile  : ErrorString:='Invalid card file';
    eDuplicateStackName : ErrorString:='Duplicate stack name';
    eReadPatternMismatch : ErrorString:='Read pattern mismatch';
    eUnknownError : ErrorString:='Unknown error';
    eMissingInterface : ErrorString:='No interface found in stack';
    eInvalidArrayLength : ErrorString:='Invalid array length';
  else
    ErrorString:='Unknown error message';
  end;
end;

{-----------------------------------------------------------------------}

procedure WriteLogStart;
var
  s : shortstring;
begin
  GetTBusVersion(s);
  writelog('---------------'+FormatDateTime('YYYY MM DD hh:nn',now)+'----------------');
  writelog('Version: '+s);
end;

exports
  GetTBusVersion,
  OpenStack,
  CloseStack,
  CloseAllStacks,
  GetErrorString,
  GetNumberOfStacks,
  GetStackName,
  GetNumberOfCards,
  GetCardName,
  GetCardType,
  GetNumberOfParameters,
  GetParameterName,
  GetParameterType,
  GetNumberOfRegisters,
  GetRegisterName,
  SetBoolParam,
  GetBoolParam,
  SetIntParam,
  GetIntParam,
  SetFloatParam,
  GetFloatParam,
  GetBusDataSize,
  BusToParam,
  RegisterToBus,
  GetStackInterface;

begin;
  WriteLogStart;
  setlength(Stacks,0);
end.

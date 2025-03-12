# GPS Tracker Platform Communication Protocol

 
 
 
GPS TRACKER COMMUNICATION PROTOCOL 
V1.0.5
 
CONTENTS 
CONTENTS ............................................................................................................................. 0 
一. Communication protocol ................................................................................................. 1 
二、Protocol No. ..................................................................................................................... 1 
三、Uplink command ............................................................................................................. 6 
1. Real-time location .............................................................................................. 6 
2. HEARTBEAT PACKET(XT) .............................................................................. 6 
3. Location request(VI1) ........................................................................................ 7 
4.Blind Spots Uploading(BC) ................................................................................ 7 
5. device alarm(ALRM) .......................................................................................... 8 
四、Downlink command ........................................................................................................ 9 
1.  cut off oil-engine/recovery oil&engine(S20) ................................................ 9 
2.  Response to location request(CR) ............................................................... 9 
3.  Fortification(SF) ............................................................................................ 10 
4.  Fortification(SF2) .......................................................................................... 10 
5.  Disarming(CF) ............................................................................................... 10 
6.  Disarming(CF2) ............................................................................................. 10 
7.  Main Number Bind(UR) ............................................................................... 11 
response format .................................................................................................... 11 
*HQ, YYYYYYYYYY,V4,UR# .............................................................................. 11 
8.  Sever setting(IP) ........................................................................................... 11 
9.  Terminal password setting(MP) .................................................................. 12 
10.  Interval settings(XT/NXT) .......................................................................... 12 
11.  Alarm setting(KC) ........................................................................................ 13 
12.  Device reboot(CQ) ...................................................................................... 13 
13.  reset to defaults(RESET) ........................................................................... 13 
14.  Network access point(APN) ...................................................................... 14 
15.  Answer mode(ACPC) ................................................................................. 14 
16.  IMEI setting .................................................................................................. 14 
17.  language setting(SLAN) ............................................................................. 15 
18.  audiomonitor(CALB) ................................................................................... 15 
19.  Query device information(INFO) ............................................................... 15 
20.  working mode setting(WMOD) .................................................................. 16 

一. Communication protocol 
Introduction 
This document defines the Car GPS positioning service platform for application-layer 
interface protocol. Relevant interface protocol is only used for the interaction between the 
platform and positioning terminal. 
二、Protocol No. 
1．Protocol List 
protocol No.  Data packet Description  Respond  Uplink or 
Downlink 
V1  Location data packet  NO  uplink 
XT  Heartbeat packet  YES  uplink 
VI1  Location Request packet  YES  uplink 
VI  Location Response packet  NO  Downlink 
S20  Start cut-off oil&engine Data  YES  Downlink 
packet 
V4  Instruction acknowledgment  NO  uplink 
packet 
SF  Fortification  YES  Downlink 
SF2  Fortification,version II   YES  Downlink 
CF  Disarm packet  YES  Downlink 
CF2  Disarm packet, version II  YES  Downlink 
TG  Platform distributes sms  YES  Downlink 
UR  Main number bind  YES  Downlink 
IP  Modify IP  YES  Downlink 
ST  Setting sms interception number YES  Downlink 
MP  Terminal password setting  YES  Downlink 
XT/NXT  uploading interval setting  YES  Downlink 
KC  Alarm Setting  YES  Downlink 
CQ  Device Reboot  YES  Downlink 
RESET  reset to defaults  YES  Downlink 
APN  APN network access point  YES  Downlink 
setting 
SQQ  Family Number setting  YES  Downlink 
ACPC  Answer mode setting  YES  Downlink 
SIMEI  IMEI setting  YES  Downlink 
SLAN  language setting  YES  Downlink 
CALB  monitor  YES  Downlink 
PWM  the power saving mode setting  YES  Downlink 
1 
 

OVSP  Overspeed Setting  YES  Downlink 
INFO  Query the device status  YES  Downlink 
ALRM  Alarm  YES  uplink 
  
2. Uplink&Downlink description 
Description  Remark 
Server-----à Terminal  Downlink 
Terminal-------à Server  Uplink 
  
3. Packet definition 
*XX,YYYYYYYYYY,cmd,HHmmss,S,latitude,D,longitude,G,speed,direction,D
DMMYY,vehicle_status, pw ,3,mcc,mnc,lac,cid, lac,cid, lac,cid # 
 
Information Field Description： 
Format  FieldName  Remark 
*XX  IHDR  IHDR 
YYYYYYYYYY  Terminal  Terminal No. 
No. 
cmd  Operation  Command names, refer to the "Protocol List" 
name 
HHmmss  Time  Automotive machine time, standard time, 8 hour 
time difference with GMT 
S  Data valid  Data valid bit（A/V/B）,A representative of GPS 
bit  positioning data is valid data，V indicates that the 
GPS data is invalid positioning data B represents 
Compass 
latitude  latitude  latitude，format DDFF.FFFF, DD：Latitude Degree
（00 ~ 90）,FF.FFFF：Latitude points（00.0000 ~ 
59.9999），Reserved four decimals 
D  Latitude  Latitude symbo（l N： northern latitude，S：southern 
symbol  latitude） 
longitude  longitude  longitude，formatDDDFF.FFFF，DDD： Longitude 
Degree（000 ~ 180），FF.FFFF： Longitude points
（00.0000 ~ 59.9999），Reserved four decimals 
G  longitude  longitude symbol（E：east longitude，W：west 
symbol  longitude） 
speed  speed  speed，range000.00 ~ 999.99 knots Reserved two 
decimals.The information fields maybe null,that means 
the speed is 0. 1Kn=1.852 km/h  
direction  Azimuth  Azimuth，Due north is 0 degree，resolution is 1 
degree，Clockwise direction.The information fields 
2 
 

maybe null,that means the degree is 0 
DDMMYY  day/month/ day/month/year 
year 
vehicle_status  Terminal  Terminal Status,total 4 bytes,represent vehicle 
Status  machine component state， vehicle component 
status and alarm status ... 
pw  Power  device  power  percentage  00-99      99  means 
Capacity  100% 
count  Base  3 
station 
quantity 
mcc  Country  Country Code 
Code 
mnc  Operator  Operator code 
code 
lac  Base  District code 
station code 
cidf  Base  District ID 
station code 
lac  Base  District code 
station code 
cidf  Base  District ID 
station code 
lac  Base  District code 
station code 
cidf  Base  District ID 
station code 
 
4. Terminal Status (alarm) analysis 
vehicle_status use ASCII character represent 16 hexadecimal value，below is 
every byte each  specific meaning of the variable，bit represent use negative 
logic，that is bit = 0 valid. show as below table： 
   
FFF9FFFF  FFF9FFEF=Cut-off engine  FFFFFFFBFF=sos 
Bit 
The first byte  The second byte  The third byte  The forth byte 
order 
GPS  Receiver  fault 
0  0  Temperature alarm  0  0  door open  0  Theft alarm 
alarm 
three  times  password  Analog  quantity   Vehicle  fortified 
1  0  1  0  0  robbery alarm 
error alarm  transfinite alarm  status 
2  0  GPRS Occlusion alarm  0  remainsos alarmstate  0  ACC关 ACC off  0  overspeed alarm 
3  0  vehicle  in  the  cut-off  0  host  powered  by  the  1  reserve  0  illegal ignition alarm 
3 
 

oil&engine state  backup battery 
Storage battery removal  Storage  battery  has  No  entry  cross-border 
4  0  0  1  reserve  0 
state  been removed  alarm 
The high level sensor 1  open  circuit  for  GPS  gps  antenna  open 
5  0  0  0  engine  0 
is high  antenna  circuit alarm 
The high level sensor,2  short  circuit  for  Gps  gps  antenna  short 
6  0  0  0  Custom alarm  0 
is high  antenna  circuit alarm 
The low level sensor 1  The low level sensor 2  vehicle  No  entry  cross-border 
7  0  0  0  0 
bond strap  bond strap  overspeed  alarm 
 
5.Instruction acknowledgment packet definition 
*XX,YYYYYYYYYY,ack,rHHmmss,HHmmss,S,latitude,D,longitude,G,speed,dir
ection,DDMMYY,vehicle_status,mcc,mnc,lac,cid# 
Information Field Description 
Format  FieldName  Remark 
*XX  IHDR  IHDR 
YYYYYYYYYY  Terminal  Terminal No./IMEI NO. 
No. 
ack  Instruction  Instruction acknowledgment packet 
acknowledg
ment 
packet 
cmd  Confirmatio Confirmed operation command, please refer to 
n command  "Protocol List" 
ret  Return  Return parameters confirmation 
parameters 
rHHmmss  Instruction  Value  of  time  field  for  the  downlink  instruction 
time  packet  
HHmmss  time  time for acknowledgement Packet 
confirmatio
n 
S  Data  valid  Data valid bit（A/V/B），A representative of GPS 
bit  positioning data is valid data，V indicates that the 
GPS data is invalid positioning data B represents 
Compass 
latitude  latitude  latitude，format DDFF.FFFF, DD：Latitude Degree
（00 ~ 90）,FF.FFFF：Latitude Points（00.0000 ~ 
59.9999），Reserved four decimals 
D  latitude  latitude symbol（N：northern latitude，S:southern 
symbol  latitude） 
longitude  longitude  longitude，format DDDFF.FFFF，DDD：（000 ~ 
180），FF.FFFF：（00.0000 ~ 59.9999），Reserved 
4 
 

four decimals 
G  longitude  longitude symbol（E：east longitude，W：west 
symbol  longitude） 
speed  speed  speed，range000.00 ~ 999.99 knots Reserved two 
decimals.The information fields maybe null,that means 
the speed is 0。1kn=1.852km/h 
direction  Azimuth  Azimuth，due north is 0 degree，resolution is 1 
degree，Clockwise direction.The information fields 
maybe empty,that means the degree is 0 
DDMMYY  day/  day/ month/year 
month/year 
vehicle_status  Terminal  Terminal  Status,total  4  bytes.  represent  vehicle 
Status  machine  component  state，  vehicle  component 
status and alarm status ... 
mcc  Country  Country code 
code 
mnc  Operator  Operator code 
code 
lac  Location  Location area code 
area code 
cid  Base  District ID 
station code 
 
6.Returned parameters (ret)Acknowledgement Packet defined 
ret use ASCII Characters represent 16 hex value, total two bytes 
0x80-0xFF Indicates success 
 0x00-0x7F Indicates fail 
16 hex value  Remark 
0x00  Device support, but the operation failed 
0x01  device does not support this operation 
0x02  Beyond the index range 
0x03-0x7F  The operation failed, the error message is 
undefined 
0x80  Successful operation 
0x81-0xFF  Successful  operation,  But  the  return  result  is 
undefined 
   
   
 
 
5 
 

三、Uplink command 
0. Real-time location (Simple Version-New) 
GPS(US satellite) locating： 
*XX,YYYYYYYYYY,VP1,A,latitude,D,longitude,G,speed,direction,DDMMYY,ve
hicle_status# 
 
GPS (BeiDou Satellite) locating： 
*XX,YYYYYYYYYY,VP1,B,latitude,D,longitude,G,speed,direction,DDMMYY# 
 
LBS locating：GSM BaseStation data separated by Y: 
*XX,YYYYYYYYYY,VP1,V,mcc,mnc,lac,cid,relveYlac,cid,relveYlac,cid,relv
eY# 
Example: 
GPS|BeiDou： 
*HQ,353505910449999,VP1,A,2239.4210,N,11400.8825,E,0.00,348,180814,FF
FFFFFF,90# 
LBS: 
*HQ,353505910449999,VP1,V,460,0,9376,8532,99Y9876,4357,99Y0,0,99Y# 
 
Response：No 
 
1. Real-time location 
LAC CID  Less than 3 fill 0 
*XX,YYYYYYYYYY,V1,HHmmss,S,latitude,D,longitude,G,speed,direction,DD
MMYY,vehicle_status ,pw,3, mcc,mnc,lac,cid,lac,cid,lac,cid # 
eg: 
*HQ,353505910449999,V1,052825,A,2239.4210,N,11400.8825,E,0.00,348,18
0814,FFFFFFFF,90,3,460,0,9376,8532,9876,4357,0,0 # 
RESPOND： NO 
  
2. HEARTBEAT PACKET(XT) 
*XX,YYYYYYYYYY,XT# 
6 
 

EG: 
*HQ,353505910449999,XT# 
RESPOND FORMAT:no 
Temporarily not to do this 
3. Location request(VI1) 
*XX,YYYYYYYYYY,VI1,HHmmss,Code,latitude,D,longitude,G,speed,direction,
DDMMYY,vehicle_status,mcc,mnc,lac,cid,pw,lac-cid-signal# 
eg: 
*HQ,353505910449999,VI1,052825,0,2239.4210,N,11400.8825,E,0.00,348,1
80814,FFFFFFFF,1CC,0,25FC,F48,90,  25FC-F48-10|  25FC-F48-6| 
25FC-F48-7# 
Respond： yes 
Response format as follows: 
*HQ,YYYYYYYYYY,VI,HHmmss,Display_Time,Code,Info_lenth,Inf
ormation# 
format  FieldName  Remark 
Display_Time  time  Display time, unit: second, range: 
5-65535, Display_Time = 0 means 
65536 seconds. (Uplink On line time) 
Code  Coding scheme  0：GB2312，1：unicode，Other:undefined 
Info_length  Message length  Message length，0-255，0  equivalent to 
256，over 256 modulo by 256，handle or 
LCD screen in 
Information  MESSAGE  Display information,length is less than 
256bytes.(128 words) 
eg:  
*HQ,0000000000,VI,130305,60,0,26,深圳市南山区中山立交桥附近# 
response：No 
4.Blind Spots Uploading(BC) 
*XX,YYYYYYYYYY,BC,HHmmss,Length,Segment# 
Segment:S,latitude,D,longitude,G,speed,direction,DDHHmmss,vehicle_status
7 
 

,mcc,mnc,lac,cid; 
response： no 
eg: 
*HQ,353505910449999,BC,052825,138,A,2239.4210,N,11400.8825,E,0.00,3
48,03182512,FFFFFFFF,1CC,0,25FC,F48;A,2239.4210,N,11400.8825,E,0.00
,348, 03182612,FFFFFFFF,1CC,0,25FC,F48# 
 
format  FieldName  Remark 
XX  time  Display time, unit: second, range: 
5-65535, Time= 0 means 65536 
seconds. (Uplink On line time) 
Length  Segment length  indication for the length of segment 
Segment  Complement  complement uploading data, no more than 100 
uploading  points, A plurality of points with a semicolon (;) 
information  separated; the contents of a single point is: 
S,latitude,D,longitude,G,speed,direction
,DDHHmmss,vehicle_status,mcc,mnc,la
c,cid; (Data valid bit, latitude, longitude 
identification, longitude, longitude 
identification, speed, direction, every 
second day, device status, country 
code, network type (operator code), 
location area code, base station code) 
5. device alarm(ALRM) 
*XX,YYYYYYYYYY,ALRM,type,HHmmss # 
response：no 
field  remark 
type  Alarm type numeral 1-n: 
1: SOS alarm 
2: Low battery alarm 
3: Geo-fence alarm 
 
eg: 
*HQ,353505910449999,ALRM,1,052825,0,2239.4210,N,11400.8825,E,0.00,3
8 
 

48,180814,FFFFFFFF,1CC,0,25FC,F48,90,  5FC-F48-10|  25FC-F48-6| 
25FC-F48-7# 
 
四、Downlink command 
1.  cut off oil-engine/recovery oil&engine(S20) 
 *XX,YYYYYYYYYY,S20,HHmmss,C,T# 
field  remark 
C  Ultimate  power  mode.  1  or  other 
digits:  Static  cut  off  oil&engine,no 
engine detection,power relay always 
pull in ， turn off the circuit 
T  cut  off  or  recovery  0  represents 
recovery,1 represents cut off 
  
eg:*HQ,0000000000,S20,130305,1,1# 
If vehicle does not support the power cutoff function,after received the 
command,return the information directly. 
*HQ,2020916012,V4,S20,00,130305,050316,A,2212.8745,N,11346.6574,
E,14.28,028, 220902,FFFFFBFF, 460,000,27A6,0F70#  
Finally is completely power off, and according to the C provision way to 
keep power off, after complete power off then return information 
*HQ,2020916012,V4,S20,80,130305,050316,A,2212.8745,N,11346.6574,
E,14.28,028,220902,F7FFFBFF, 460,000,27A6,0F70# 
:Method of recovery oil&engine,downlink 
*HQ,0000000000,S20,130305,1,0# 
Return information 
*HQ,2020916012,V4,S20,80,130305,050316,A,2212.8745,N,11346.6574,
E,14.28,028,220902,FFFFFBFF, 460,000,27A6,0F70# 
2.  Response to location request(CR) 
*HQ,YYYYYYYYYY,CR# 
response： yes 
*HQ,YYYYYYYYYY,V4,CR# 
after received the command,device will upload one location data immediately. 
9 
 

3.  Fortification(SF) 
*HQ,YYYYYYYYYY,SF# 
eg: 
*HQ,135790246811221,SF# 
response：yes 
Response format is as follows 
*HQ,135790246811221,V4,SF,050316,A,2212.8745,N,11346.6574,E,14.28,02
8,220902,FFFFFFFF,mcc,mnc,lac,cid# 
4.  Fortification(SF2) 
*HQ,YYYYYYYYYY,SF2,HHmmss# 
eg: 
*HQ,135790246811221,SF2,HHmmss# 
response： yes 
Response format is as follows 
*HQ,135790246811221,V4,SF2,80,130305,050316,A,2212.8745,N,11346.657
4,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid# 
  
5.  Disarming(CF) 
*HQ, YYYYYYYYYY,CF# 
eg: 
*HQ,135790246811221,CF # 
response： yes 
Response format is as follows 
*HQ,135790246811221,V4,CF,050316,A,2212.8745,N,11346.6574,E,14.28,02
8,220902,FFFFFBFF,mcc,mnc,lac,cid# 
6.  Disarming(CF2) 
*HQ, YYYYYYYYYY,CF2,HHmmss# 
eg: 
*HQ,135790246811221,CF2,130305# 
response：yes 
10 
 

Response format is as follows 
*HQ,135790246811221,V4,CF2,130305,050316,A,2212.8745,N,11346.6574,
E,14.28,028,220902,FFFFFBFF,mcc,mnc,lac,cid# 
 
7.  Main Number Bind(UR) 
*HQ, YYYYYYYYYY,UR,NUM_LIST# 
eg: 
*HQ,135790246811221,UR,15014333333，1343333333，0，0，0# 
response： yes 
field  remark 
NUM_LIST  Binding number list,multiple numbers 
separated by |, supports a maximum 
of  six  numbers,  the  first  one  is  the 
main  number,  the  rest  is  frequently 
used numbers (family number) 
response format 
*HQ, YYYYYYYYYY,V4,UR# 
8.  Sever setting(IP) 
*HQ,135790246811221,IP,INDEX,IP,PORT,YM,HHmmss# 
format  fieldname  remark 
IP  IP  IP address（32bit） 
INDEX  digits  1  or  2  is  priority 
number.1  is  for  IP 
priority.2  for  domain 
priority 
IP  IP  Use 0 instead (abandoned) 
PORT  port No.   Port  number  is  16 
hexadecimal 
YM  domain  domain 
eg： 
11 
 

*HQ,135790246811221,IP,1,00000000,1a7c,www.gps588.com，130305# 
response： yes 
 Response format is as follows: 
*HQ,135790246811221,V4,IP,80,130305,050316,A,2212.8745,N,11346.6574,
E,14.28,028,220902,FFFFFBFF,mcc,mnc,lac,cid# 
9.  Terminal password setting(MP) 
*XX,YYYYYYYYYY,MP,msg,HHmmss# 
msg is the content of sending includes fields as follows: 
field  remark 
Old_password  Terminal old password (6 digits) 
New_password  Terminal new password (6 digits) 
eg: 
*HQ,353505910449999,MP,000000,123456# 
response： yes 
Response format is as follows: 
Modify the password is correct then return information 
*HQ,135790246811221,V4,MP,80,130305,050316,A,2212.8745,N,11346.
6574,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid # 
  Modify the password is error: 
*HQ,135790246811221,V4,MP,03,130305,050316,A,2212.8745,N,113
46.6574,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid # 
Ret error field definition 
Hexadecimal value  remark 
03  操作失败，原密码不正确 Operation fails, the 
original password is not correct 
04  操作失败，新密码超出范围Operation fails, the 
original password is beyond range 
10.   Interval settings(XT/NXT) 
*XX,YYYYYYYYYY,[XT/NXT],seconds# 
field  remark 
XT  Terminal driving packets upload interval 
NXT  Terminal resting packets upload interval 
seconds  Corresponding to the time interval of upload 
data  packets  while  driving  [5,3600],static 
12 
 

range[10,7200]units:SEC(s) 
 eg: 
*HQ,353505910449999,NXT,10# 
respond： yes 
Response format is as follows: 
*HQ,135790246811221,V4,NXT# 
11. Alarm setting(KC) 
*XX,YYYYYYYYYY,KC,key,Type # 
field  remark 
Type  Alarm  Type:  1  SMS,  2  Phone  Key 
Key,  values, 0, SOS button, key 1,1key, 2,2 
key 
  
eg: 
*HQ,353505910449999,KC,0,1 # 
response： yes 
response format is as follows: 
*HQ,135790246811221,V4,KC# 
12. Device reboot(CQ) 
*XX,YYYYYYYYYY,CQ# 
eg: 
*HQ,353505910449999,CQ# 
response： yes 
*HQ,135790246811221,V4,CQ # 
13. reset to defaults(RESET) 
*XX,YYYYYYYYYY,RESET,HHmmss# 
eg: 
*HQ,353505910449999,RESET，130305# 
response： yes 
response format is as follows: 
*HQ,135790246811221,V4,RESET,80,130305,050316,A,2212.8745,N,11346.
13 
 

6574,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid # 
14. Network access point(APN) 
*XX,YYYYYYYYYY,APN,Name,User,PWD# 
field  remark 
Name  Local operators APN name 
User  Access  network  operators 
corresponding account 
PWD  The operator password 
 Eg: SPAIN APN: 
*HQ,353505910449999,APN,zap.vivo.com.br,vivo,vivo# 
response： yes 
Response format is as follows: 
*HQ,135790246811221,V4,APN,# 
15. Answer mode(ACPC) 
After closing the answer mode, the device can not receive calls 
*HQ,YYYYYYYYYY,ACPC,OPERATION# 
eg: 
*HQ,135790246811221,ACPC,1# 
response： yes 
field  remark 
OPERATION  1, open the answer mode 0, close the 
answer mode (off by default) 
response format is as follows: 
*HQ,135790246811221,V4,ACPC# 
16. IMEI setting 
*HQ,YYYYYYYYYY,SIMEI,NUM,130305# 
eg: 
*HQ,135790246811221,SIMEI,135790246811221130305# 
response： yes 
field  remark 
NUM  IMEI NUMBER 15 digits 
response format is as follows: 
14 
 

*HQ,135790246811221,V4,SIMEI,80,130305,050316,A,2212.8745,N,11346.6
574,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid# 
17. language setting(SLAN) 
*HQ,YYYYYYYYYY,SLAN,lan,HHmmss# 
eg: 
*HQ,135790246811221,SLAN,en,130305# 
response： yes 
field  remark 
lan  International language simple code cn 
Simplified Chinese, en English 
response format is as follows: 
*HQ,135790246811221,V4,SLAN,80,130305,050316,A,2212.8745,N,11346.6
574,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid# 
 
 
18. audiomonitor(CALB) 
Starts the Listener, the device will take the initiative to call the phone number 
bound 
*HQ,YYYYYYYYYY,CALB,HHmmss# 
eg: 
*HQ,135790246811221,CALB,HHmmss# 
response： yes 
response format is as follows: 
*HQ,135790246811221,V4,CALB,130305,050316,A,2212.8745,N,11346.6574
,E,14.28,028,220902,FFFFFFFF,mcc,mnc,lac,cid# 
19. Query device information(INFO) 
*HQ,YYYYYYYYYY,INFO# 
response： yes 
field  remark 
operationList   use , to show the value of equipment information 
to query list 
 
15 
 

Display the information list 
VOL  electric quantity percentage 00-99 
IP  domain,ip  and  port  for  the  binding 
server eg： 219.148.126，8169 
UPF  Uploading  data  frequency, 
format:travel  Upload  time  |  resting 
upload  time  if  there  is  no  static 
uploading  time,should  be  null 
units:second 
PWM  running mode refer to "power saving mode",mode 
No. 
SOS  family  number ，   multiple  values 
among  use,  separated  the 
corresponding array according to the 
Settings order，no value is null eg：
13510060482，0，0，0，0 
ALM  Alarm mode, respectively SOS keyboard, 1 key, 2 
key Alarm mode 
APN  Apn name currently used,APN name 
in NVRAM 
response format is as follows: 
*HQ,135790246811221,V4,INFO,80 ，
132.44.55.33,8090,60,1,13578882828,0,0,0,0,1,cmnet,0，50# 
operationList： operating value for Server downlink 
result：According to the results of operation List combined, multiple values 
separated by （,） 
 
eg: 
*HQ,135790246811221,INFO# 
20. working mode setting(WMOD) 
*HQ,YYYYYYYYYY,WMOD,TYPE,TIME1,TIME2# 
eg: 
TYPE 0,1,2 
*HQ,135790246811221,1,0,0# 
*HQ,135790246811221,2,30,30# 
16 
 

interpretation：the  device  off  after  finished  setting,  after  30  minutes,  automatic  on, 
restart,working for 30 minutes, and then enter a dormant state. 
Every 24 hours a loop 
Respond： yes 
field  remark 
TIME1  Device start to work after the current time how many minutes 
TIME2  device enters the shutdown state after how long working time 
 
Winnie 
Shenzhen HuaSunTeK Co., Ltd. 
Website: www.etrendauction.com.cn  
E-mail: sales@etrendauction.com.cn  
TEL: +86-755-83624286  
Mobile:+86-15013835989  
Ali TM: etrendkey  
SKYPE: digitalease1023 
Wechat: +86-15013835989  
What's APP: +86-15013835989 
 
17 
 

## Extracted Tables

| protocol No. | Data packet Description |  |  |  |  | Respond | Uplink or |
| --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  | Downlink |
| V1 | Location data packet |  |  |  |  | NO | uplink |
| XT | Heartbeat packet |  |  |  |  | YES | uplink |
| VI1 | Location Request packet |  |  |  |  | YES | uplink |
| VI | Location Response packet |  |  |  |  | NO | Downlink |
| S20 | Start cut-off oil&engine Data |  |  |  |  | YES | Downlink |
|  | packet |  |  |  |  |  |  |
| V4 | Instruction acknowledgment |  |  |  |  | NO | uplink |
|  | packet |  |  |  |  |  |  |
| SF | Fortification |  |  |  |  | YES | Downlink |
| SF2 | Fortification,version | II |  |  |  | YES | Downlink |
| CF | Disarm packet |  |  |  |  | YES | Downlink |
| CF2 | Disarm packet, version |  |  | II |  | YES | Downlink |
| TG | Platform distributes sms |  |  |  |  | YES | Downlink |
| UR | Main number bind |  |  |  |  | YES | Downlink |
| IP | Modify IP |  |  |  |  | YES | Downlink |
| ST | Setting sms interception number |  |  |  |  | YES | Downlink |
| MP | Terminal password setting |  |  |  |  | YES | Downlink |
| XT/NXT | uploading interval setting |  |  |  |  | YES | Downlink |
| KC | Alarm Setting |  |  |  |  | YES | Downlink |
| CQ | Device Reboot |  |  |  |  | YES | Downlink |
| RESET | reset to defaults |  |  |  |  | YES | Downlink |
| APN | APN network access point |  |  |  |  | YES | Downlink |
|  | setting |  |  |  |  |  |  |
| SQQ | Family Number setting |  |  |  |  | YES | Downlink |
| ACPC | Answer mode setting |  |  |  |  | YES | Downlink |
| SIMEI | IMEI setting |  |  |  |  | YES | Downlink |
| SLAN | language setting |  |  |  |  | YES | Downlink |
| CALB | monitor |  |  |  |  | YES | Downlink |
| PWM | the power saving mode setting |  |  |  |  | YES | Downlink |

| Overspeed Setting | YES | Downlink |
| --- | --- | --- |
| Query the device status | YES | Downlink |
| Alarm | YES | uplink |

| Description |  |  | Remark |  |
| --- | --- | --- | --- | --- |
| Server-----à Terminal |  | Downlink |  |  |
| Terminal-------à Server |  | Uplink |  |  |

| Format |  |  | FieldName |  |  | Remark |  |
| --- | --- | --- | --- | --- | --- | --- | --- |
| *XX |  | IHDR |  |  | IHDR |  |  |
| YYYYYYYYYY |  | Terminal 
No. |  |  | Terminal No. |  |  |
| cmd |  | Operation 
name |  |  | Command names, refer to the "Protocol List" |  |  |
| HHmmss |  | Time |  |  | Automotive machine time, standard time, 8 hour 
time difference with GMT |  |  |
| S |  | Data valid 
bit |  |  | Data valid bit（A/V/B）,A representative of GPS 
positioning data is valid data，V indicates that the 
GPS data is invalid positioning data B represents 
Compass |  |  |
| latitude |  | latitude |  |  | latitude，format DDFF.FFFF, DD：Latitude Degree
（00 ~ 90）,FF.FFFF：Latitude points（00.0000 ~ 
59.9999），Reserved four decimals |  |  |
| D |  | Latitude 
symbol |  |  | Latitude symbo（l N： northern latitude，S：southern 
latitude） |  |  |
| longitude |  | longitude |  |  | longitude，formatDDDFF.FFFF，DDD： Longitude 
Degree（000 ~ 180），FF.FFFF： Longitude points
（00.0000 ~ 59.9999），Reserved four decimals |  |  |
| G |  | longitude 
symbol |  |  | longitude symbol（E：east longitude，W：west 
longitude） |  |  |
| speed |  | speed |  |  | speed，range000.00 ~ 999.99 knots Reserved two 
decimals.The information fields maybe null,that means 
the speed is 0. 1Kn=1.852 km/h |  |  |
| direction |  | Azimuth |  |  | Azimuth，Due north is 0 degree，resolution is 1 
degree，Clockwise direction.The information fields |  |  |

|  |  | maybe null,that means the degree is 0 |
| --- | --- | --- |
| DDMMYY | day/month/
year | day/month/year |
| vehicle_status | Terminal 
Status | Terminal Status,total 4 bytes,represent vehicle 
machine component state， vehicle component 
status and alarm status ... |
| pw | Power 
Capacity | device  power  percentage  00-99      99  means 
100% |
| count | Base 
station 
quantity | 3 |
| mcc | Country 
Code | Country Code |
| mnc | Operator 
code | Operator code |
| lac | Base 
station code | District code |
| cidf | Base 
station code | District ID |
| lac | Base 
station code | District code |
| cidf | Base 
station code | District ID |
| lac | Base 
station code | District code |
| cidf | Base 
station code | District ID |

| Bit 
order | The first byte |  | The second byte |  | The third byte |  | The forth byte |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | 0 | Temperature alarm | 0 | GPS  Receiver  fault 
alarm | 0 | door open | 0 | Theft alarm |
| 1 | 0 | three  times  password 
error alarm | 1 | Analog  quantity 
transfinite alarm | 0 | Vehicle  fortified 
status | 0 | robbery alarm |
| 2 | 0 | GPRS Occlusion alarm | 0 | remainsos alarmstate | 0 | ACC关 ACC off | 0 | overspeed alarm |
| 3 | 0 | vehicle  in  the  cut-off | 0 | host  powered  by  the | 1 | reserve | 0 | illegal ignition alarm |

|  |  | oil&engine state |  | backup battery |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 4 | 0 | Storage battery removal 
state | 0 | Storage  battery  has 
been removed | 1 | reserve | 0 | No  entry  cross-border 
alarm |
| 5 | 0 | The high level sensor 1 
is high | 0 | open  circuit  for  GPS 
antenna | 0 | engine | 0 | gps  antenna  open 
circuit alarm |
| 6 | 0 | The high level sensor,2 
is high | 0 | short  circuit  for  Gps 
antenna | 0 | Custom alarm | 0 | gps  antenna  short 
circuit alarm |
| 7 | 0 | The low level sensor 1 
bond strap | 0 | The low level sensor 2 
bond strap | 0 | vehicle 
overspeed | 0 | No  entry  cross-border 
alarm |

|  | Format |  |  | FieldName |  |  | Remark |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| *XX |  |  | IHDR |  |  | IHDR |  |  |
| YYYYYYYYYY |  |  | Terminal 
No. |  |  | Terminal No./IMEI NO. |  |  |
| ack |  |  | Instruction 
acknowledg
ment 
packet |  |  | Instruction acknowledgment packet |  |  |
| cmd |  |  | Confirmatio
n command |  |  | Confirmed operation command, please refer to 
"Protocol List" |  |  |
| ret |  |  | Return 
parameters |  |  | Return parameters confirmation |  |  |
| rHHmmss |  |  | Instruction 
time |  |  | Value  of  time  field  for  the  downlink  instruction 
packet |  |  |
| HHmmss |  |  | time 
confirmatio
n |  |  | time for acknowledgement Packet |  |  |
| S |  |  | Data  valid 
bit |  |  | Data valid bit（A/V/B），A representative of GPS 
positioning data is valid data，V indicates that the 
GPS data is invalid positioning data B represents 
Compass |  |  |
| latitude |  |  | latitude |  |  | latitude，format DDFF.FFFF, DD：Latitude Degree
（00 ~ 90）,FF.FFFF：Latitude Points（00.0000 ~ 
59.9999），Reserved four decimals |  |  |
| D |  |  | latitude 
symbol |  |  | latitude symbol（N：northern latitude，S:southern 
latitude） |  |  |
| longitude |  |  | longitude |  |  | longitude，format DDDFF.FFFF，DDD：（000 ~ 
180），FF.FFFF：（00.0000 ~ 59.9999），Reserved |  |  |

|  |  | four decimals |
| --- | --- | --- |
| G | longitude 
symbol | longitude symbol（E：east longitude，W：west 
longitude） |
| speed | speed | speed，range000.00 ~ 999.99 knots Reserved two 
decimals.The information fields maybe null,that means 
the speed is 0。1kn=1.852km/h |
| direction | Azimuth | Azimuth，due north is 0 degree，resolution is 1 
degree，Clockwise direction.The information fields 
maybe empty,that means the degree is 0 |
| DDMMYY | day/ 
month/year | day/ month/year |
| vehicle_status | Terminal 
Status | Terminal  Status,total  4  bytes.  represent  vehicle 
machine  component  state，  vehicle  component 
status and alarm status ... |
| mcc | Country 
code | Country code |
| mnc | Operator 
code | Operator code |
| lac | Location 
area code | Location area code |
| cid | Base 
station code | District ID |

| 16 hex value | Remark |
| --- | --- |
| 0x00 | Device support, but the operation failed |
| 0x01 | device does not support this operation |
| 0x02 | Beyond the index range |
| 0x03-0x7F | The operation failed, the error message is 
undefined |
| 0x80 | Successful operation |
| 0x81-0xFF | Successful  operation,  But  the  return  result  is 
undefined |
|  |  |
|  |  |

|  | format |  |  | FieldName |  |  | Remark |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Display_Time |  |  | time |  |  | Display time, unit: second, range: 
5-65535, Display_Time = 0 means 
65536 seconds. (Uplink On line time) |  |  |
| Code |  |  | Coding scheme |  |  | 0：GB2312，1：unicode，Other:undefined |  |  |
| Info_length |  |  | Message length |  |  | Message length，0-255，0  equivalent to 
256，over 256 modulo by 256，handle or 
LCD screen in |  |  |
| Information |  |  | MESSAGE |  |  | Display information,length is less than 
256bytes.(128 words) |  |  |

|  | format |  |  | FieldName |  |  | Remark |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| XX |  |  | time |  |  | Display time, unit: second, range: 
5-65535, Time= 0 means 65536 
seconds. (Uplink On line time) |  |  |
| Length |  |  | Segment length |  |  | indication for the length of segment |  |  |
| Segment |  |  | Complement 
uploading 
information |  |  | complement uploading data, no more than 100 
points, A plurality of points with a semicolon (;) 
separated; the contents of a single point is: 
S,latitude,D,longitude,G,speed,direction
,DDHHmmss,vehicle_status,mcc,mnc,la
c,cid; (Data valid bit, latitude, longitude 
identification, longitude, longitude 
identification, speed, direction, every 
second day, device status, country 
code, network type (operator code), 
location area code, base station code) |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| type |  |  | Alarm type numeral 1-n: 
1: SOS alarm 
2: Low battery alarm 
3: Geo-fence alarm |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| C |  |  | Ultimate  power  mode.  1  or  other 
digits:  Static  cut  off  oil&engine,no 
engine detection,power relay always 
pull in ， turn off the circuit |  |  |
| T |  |  | cut  off  or  recovery  0  represents 
recovery,1 represents cut off |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| NUM_LIST |  |  | Binding number list,multiple numbers 
separated by |, supports a maximum 
of  six  numbers,  the  first  one  is  the 
main  number,  the  rest  is  frequently 
used numbers (family number) |  |  |

|  | format |  |  | fieldname |  |  | remark |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| IP |  |  | IP |  |  | IP address（32bit） |  |  |
| INDEX |  |  | digits |  |  | 1  or  2  is  priority 
number.1  is  for  IP 
priority.2  for  domain 
priority |  |  |
| IP |  |  | IP |  |  | Use 0 instead (abandoned) |  |  |
| PORT |  |  | port No. |  |  | Port  number  is  16 
hexadecimal |  |  |
| YM |  |  | domain |  |  | domain |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| Old_password |  |  | Terminal old password (6 digits) |  |  |
| New_password |  |  | Terminal new password (6 digits) |  |  |

| Hexadecimal value | remark |
| --- | --- |
| 03 | 操作失败，原密码不正确 Operation fails, the 
original password is not correct |
| 04 | 操作失败，新密码超出范围Operation fails, the 
original password is beyond range |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| XT |  |  | Terminal driving packets upload interval |  |  |
| NXT |  |  | Terminal resting packets upload interval |  |  |
| seconds |  |  | Corresponding to the time interval of upload 
data  packets  while  driving  [5,3600],static |  |  |

|  | range[10,7200]units:SEC(s) |
| --- | --- |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| Type 
Key, |  |  | Alarm  Type:  1  SMS,  2  Phone  Key 
values, 0, SOS button, key 1,1key, 2,2 
key |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| Name |  |  | Local operators APN name |  |  |
| User |  |  | Access  network  operators 
corresponding account |  |  |
| PWD |  |  | The operator password |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| OPERATION |  |  | 1, open the answer mode 0, close the 
answer mode (off by default) |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| NUM |  |  | IMEI NUMBER 15 digits |  |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| lan |  |  | International language simple code cn 
Simplified Chinese, en English |  |  |

|  | field |  |  | remark |  |  |
| --- | --- | --- | --- | --- | --- | --- |
| operationList |  |  | use , to show the value of equipment information 
to query list |  | of equipment information |  |

| VOL | electric quantity percentage 00-99 |
| --- | --- |
| IP | domain,ip  and  port  for  the  binding 
server eg： 219.148.126，8169 |
| UPF | Uploading  data  frequency, 
format:travel  Upload  time  |  resting 
upload  time  if  there  is  no  static 
uploading  time,should  be  null 
units:second |
| PWM | running mode refer to "power saving mode",mode 
No. |
| SOS | family  number ，   multiple  values 
among  use,  separated  the 
corresponding array according to the 
Settings order，no value is null eg：
13510060482，0，0，0，0 |
| ALM | Alarm mode, respectively SOS keyboard, 1 key, 2 
key Alarm mode |
| APN | Apn name currently used,APN name 
in NVRAM |

|  | multiple  values |
| --- | --- |
| among  use,  separated  the |  |
| corresponding array |  |

|  | field |  |  | remark |  |
| --- | --- | --- | --- | --- | --- |
| TIME1 |  |  | Device start to work after the current time how many minutes |  |  |
| TIME2 |  |  | device enters the shutdown state after how long working time |  |  |
//Note: Third party website changed. Updated link from:
//      http://www.whatismyip.com/automation/n09230945.asp
//      to...
//      http://automation.whatismyip.com/n09230945.asp
/*
$CHECKING:OFF
deflng a-z
'print whatismyip$
FUNCTION whatismyip$
c=_OPENCLIENT("TCP/IP:80:automation.whatismyip.com")
if c<>0 then
e$=chr$(13)+chr$(10)
x$="GET /n09230945.asp HTTP/1.1"+e$
x$=x$+"Host: automation.whatismyip.com"+e$
x$=x$+""+e$
put #c,,x$
t!=timer
do
_delay 0.05 '50ms delay (20 checks per second)
get #c,,a2$
a$=a$+a2$
i=instr(a$,"Content-Length:")
if i then
i2=instr(i,a$,e$)
if i2 then
l=val(mid$(a$,i+15,i2-i-14))
i3=instr(i2,a$,e$+e$)
if i3 then
i3=i3+4 'move i3 to start of data
if (len(a$)-i3+1)=l then
close c
ip$=mid$(a$,i3,l)
'verify that it is an ip
x=0
cx=0
nx=0
for i5=1 to len(ip$)
av=asc(mid$(ip$,i5,1))
if av>=48 and av<=57 then
    cx=cx+1
    if cx=4 then goto error
    x=x*10
    x=x+(av-48)
else
    if cx=0 or x>255 or av<>46 then goto error
    nx=nx+1
    if nx=4 then goto error
    cx=0
    x=0
end if
next i5
if cx=0 or x>255 then goto error
if nx<>3 then goto error
whatismyip$=ip$
exit function
end if 'availabledata=l
end if 'i3
end if 'i2
end if 'i
loop until timer>t!+5 'wait up to 5 seconds for reply
close c
end if
error:
END FUNCTION
*/
qbs* WHATISMYIP(){
qbs *tqbs;
long tmp_long;
long tmp_fileno;
unsigned long qbs_tmp_base=qbs_tmp_list_nexti;
unsigned char *tmp_mem_static_pointer=mem_static_pointer;
unsigned long tmp_cmem_sp=cmem_sp;

//#include "data1.txt"
qbs *_FUNC_WHATISMYIP_STRING_WHATISMYIP=NULL;
if (!_FUNC_WHATISMYIP_STRING_WHATISMYIP)_FUNC_WHATISMYIP_STRING_WHATISMYIP=qbs_new(0,0);
int32 *_FUNC_WHATISMYIP_LONG_C=NULL;
if(_FUNC_WHATISMYIP_LONG_C==NULL){
_FUNC_WHATISMYIP_LONG_C=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_C=0;
}
qbs *_FUNC_WHATISMYIP_STRING_E=NULL;
if (!_FUNC_WHATISMYIP_STRING_E)_FUNC_WHATISMYIP_STRING_E=qbs_new(0,0);
qbs *_FUNC_WHATISMYIP_STRING_X=NULL;
if (!_FUNC_WHATISMYIP_STRING_X)_FUNC_WHATISMYIP_STRING_X=qbs_new(0,0);
float *_FUNC_WHATISMYIP_SINGLE_T=NULL;
if(_FUNC_WHATISMYIP_SINGLE_T==NULL){
_FUNC_WHATISMYIP_SINGLE_T=(float*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_SINGLE_T=0;
}
qbs *_FUNC_WHATISMYIP_STRING_A2=NULL;
if (!_FUNC_WHATISMYIP_STRING_A2)_FUNC_WHATISMYIP_STRING_A2=qbs_new(0,0);
qbs *_FUNC_WHATISMYIP_STRING_A=NULL;
if (!_FUNC_WHATISMYIP_STRING_A)_FUNC_WHATISMYIP_STRING_A=qbs_new(0,0);
int32 *_FUNC_WHATISMYIP_LONG_I=NULL;
if(_FUNC_WHATISMYIP_LONG_I==NULL){
_FUNC_WHATISMYIP_LONG_I=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_I=0;
}
int32 *_FUNC_WHATISMYIP_LONG_I2=NULL;
if(_FUNC_WHATISMYIP_LONG_I2==NULL){
_FUNC_WHATISMYIP_LONG_I2=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_I2=0;
}
int32 *_FUNC_WHATISMYIP_LONG_L=NULL;
if(_FUNC_WHATISMYIP_LONG_L==NULL){
_FUNC_WHATISMYIP_LONG_L=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_L=0;
}
int32 *_FUNC_WHATISMYIP_LONG_I3=NULL;
if(_FUNC_WHATISMYIP_LONG_I3==NULL){
_FUNC_WHATISMYIP_LONG_I3=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_I3=0;
}
qbs *_FUNC_WHATISMYIP_STRING_IP=NULL;
if (!_FUNC_WHATISMYIP_STRING_IP)_FUNC_WHATISMYIP_STRING_IP=qbs_new(0,0);
int32 *_FUNC_WHATISMYIP_LONG_X=NULL;
if(_FUNC_WHATISMYIP_LONG_X==NULL){
_FUNC_WHATISMYIP_LONG_X=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_X=0;
}
int32 *_FUNC_WHATISMYIP_LONG_CX=NULL;
if(_FUNC_WHATISMYIP_LONG_CX==NULL){
_FUNC_WHATISMYIP_LONG_CX=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_CX=0;
}
int32 *_FUNC_WHATISMYIP_LONG_NX=NULL;
if(_FUNC_WHATISMYIP_LONG_NX==NULL){
_FUNC_WHATISMYIP_LONG_NX=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_NX=0;
}
int32 *_FUNC_WHATISMYIP_LONG_I5=NULL;
if(_FUNC_WHATISMYIP_LONG_I5==NULL){
_FUNC_WHATISMYIP_LONG_I5=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_I5=0;
}
int64 fornext_value116;
int64 fornext_finalvalue116;
int64 fornext_step116;
unsigned char fornext_step_negative116;
int32 *_FUNC_WHATISMYIP_LONG_AV=NULL;
if(_FUNC_WHATISMYIP_LONG_AV==NULL){
_FUNC_WHATISMYIP_LONG_AV=(int32*)mem_static_malloc(4);
*_FUNC_WHATISMYIP_LONG_AV=0;
}

if (new_error) goto exit_subfunc;

*_FUNC_WHATISMYIP_LONG_C=func__openclient(qbs_new_txt_len("TCP/IP:80:automation.whatismyip.com",35));
if ((-(*_FUNC_WHATISMYIP_LONG_C!= 0 ))||new_error){
qbs_set(_FUNC_WHATISMYIP_STRING_E,qbs_add(func_chr( 13 ),func_chr( 10 )));
qbs_cleanup(qbs_tmp_base,0);
qbs_set(_FUNC_WHATISMYIP_STRING_X,qbs_add(qbs_new_txt_len("GET /n09230945.asp HTTP/1.1",27),_FUNC_WHATISMYIP_STRING_E));
qbs_cleanup(qbs_tmp_base,0);
qbs_set(_FUNC_WHATISMYIP_STRING_X,qbs_add(qbs_add(_FUNC_WHATISMYIP_STRING_X,qbs_new_txt_len("Host: automation.whatismyip.com",31)),_FUNC_WHATISMYIP_STRING_E));
qbs_cleanup(qbs_tmp_base,0);
qbs_set(_FUNC_WHATISMYIP_STRING_X,qbs_add(qbs_add(_FUNC_WHATISMYIP_STRING_X,qbs_new_txt_len("",0)),_FUNC_WHATISMYIP_STRING_E));
qbs_cleanup(qbs_tmp_base,0);
sub_put2(*_FUNC_WHATISMYIP_LONG_C,NULL,byte_element((uint64)_FUNC_WHATISMYIP_STRING_X->chr,_FUNC_WHATISMYIP_STRING_X->len),0);
qbs_cleanup(qbs_tmp_base,0);
*_FUNC_WHATISMYIP_SINGLE_T=func_timer(NULL,0);
do{
sub__delay( 0.05E+0 );
sub_get2(*_FUNC_WHATISMYIP_LONG_C,NULL,_FUNC_WHATISMYIP_STRING_A2,0);
qbs_cleanup(qbs_tmp_base,0);
qbs_set(_FUNC_WHATISMYIP_STRING_A,qbs_add(_FUNC_WHATISMYIP_STRING_A,_FUNC_WHATISMYIP_STRING_A2));
qbs_cleanup(qbs_tmp_base,0);
*_FUNC_WHATISMYIP_LONG_I=func_instr(NULL,_FUNC_WHATISMYIP_STRING_A,qbs_new_txt_len("Content-Length:",15),0);
qbs_cleanup(qbs_tmp_base,0);
if ((*_FUNC_WHATISMYIP_LONG_I)||new_error){
*_FUNC_WHATISMYIP_LONG_I2=func_instr(*_FUNC_WHATISMYIP_LONG_I,_FUNC_WHATISMYIP_STRING_A,_FUNC_WHATISMYIP_STRING_E,1);
qbs_cleanup(qbs_tmp_base,0);
if ((*_FUNC_WHATISMYIP_LONG_I2)||new_error){
*_FUNC_WHATISMYIP_LONG_L=qbr(func_val(func_mid(_FUNC_WHATISMYIP_STRING_A,*_FUNC_WHATISMYIP_LONG_I+ 15 ,*_FUNC_WHATISMYIP_LONG_I2-*_FUNC_WHATISMYIP_LONG_I- 14 ,1)));
qbs_cleanup(qbs_tmp_base,0);
*_FUNC_WHATISMYIP_LONG_I3=func_instr(*_FUNC_WHATISMYIP_LONG_I2,_FUNC_WHATISMYIP_STRING_A,qbs_add(_FUNC_WHATISMYIP_STRING_E,_FUNC_WHATISMYIP_STRING_E),1);
qbs_cleanup(qbs_tmp_base,0);
if ((*_FUNC_WHATISMYIP_LONG_I3)||new_error){
*_FUNC_WHATISMYIP_LONG_I3=*_FUNC_WHATISMYIP_LONG_I3+ 4 ;
if ((qbs_cleanup(qbs_tmp_base,-((_FUNC_WHATISMYIP_STRING_A->len-*_FUNC_WHATISMYIP_LONG_I3+ 1 )==*_FUNC_WHATISMYIP_LONG_L)))||new_error){
sub_close(*_FUNC_WHATISMYIP_LONG_C,1);
qbs_set(_FUNC_WHATISMYIP_STRING_IP,func_mid(_FUNC_WHATISMYIP_STRING_A,*_FUNC_WHATISMYIP_LONG_I3,*_FUNC_WHATISMYIP_LONG_L,1));
qbs_cleanup(qbs_tmp_base,0);
*_FUNC_WHATISMYIP_LONG_X= 0 ;
*_FUNC_WHATISMYIP_LONG_CX= 0 ;
*_FUNC_WHATISMYIP_LONG_NX= 0 ;
*_FUNC_WHATISMYIP_LONG_I5= 1 ;
fornext_value116=*_FUNC_WHATISMYIP_LONG_I5;
fornext_finalvalue116=_FUNC_WHATISMYIP_STRING_IP->len;
fornext_step116= 1 ;
if (fornext_step116<0) fornext_step_negative116=1; else fornext_step_negative116=0;
if (new_error) goto fornext_error116;
goto fornext_entrylabel116;
while(1){
fornext_value116=fornext_step116+(*_FUNC_WHATISMYIP_LONG_I5);
*_FUNC_WHATISMYIP_LONG_I5=fornext_value116;
qbs_cleanup(qbs_tmp_base,0);
fornext_entrylabel116:
if (fornext_step_negative116){
if (fornext_value116<fornext_finalvalue116) break;
}else{
if (fornext_value116>fornext_finalvalue116) break;
}
fornext_error116:
*_FUNC_WHATISMYIP_LONG_AV=qbs_asc(func_mid(_FUNC_WHATISMYIP_STRING_IP,*_FUNC_WHATISMYIP_LONG_I5, 1 ,1));
qbs_cleanup(qbs_tmp_base,0);
if (((-(*_FUNC_WHATISMYIP_LONG_AV>= 48 ))&(-(*_FUNC_WHATISMYIP_LONG_AV<= 57 )))||new_error){
*_FUNC_WHATISMYIP_LONG_CX=*_FUNC_WHATISMYIP_LONG_CX+ 1 ;
if ((-(*_FUNC_WHATISMYIP_LONG_CX== 4 ))||new_error){
goto LABEL_ERROR;
}
*_FUNC_WHATISMYIP_LONG_X=*_FUNC_WHATISMYIP_LONG_X* 10 ;
*_FUNC_WHATISMYIP_LONG_X=*_FUNC_WHATISMYIP_LONG_X+(*_FUNC_WHATISMYIP_LONG_AV- 48 );
}else{
if (((-(*_FUNC_WHATISMYIP_LONG_CX== 0 ))|(-(*_FUNC_WHATISMYIP_LONG_X> 255 ))|(-(*_FUNC_WHATISMYIP_LONG_AV!= 46 )))||new_error){
goto LABEL_ERROR;
}
*_FUNC_WHATISMYIP_LONG_NX=*_FUNC_WHATISMYIP_LONG_NX+ 1 ;
if ((-(*_FUNC_WHATISMYIP_LONG_NX== 4 ))||new_error){
goto LABEL_ERROR;
}
*_FUNC_WHATISMYIP_LONG_CX= 0 ;
*_FUNC_WHATISMYIP_LONG_X= 0 ;
}
}
fornext_exit_115:;
if (((-(*_FUNC_WHATISMYIP_LONG_CX== 0 ))|(-(*_FUNC_WHATISMYIP_LONG_X> 255 )))||new_error){
goto LABEL_ERROR;
}
if ((-(*_FUNC_WHATISMYIP_LONG_NX!= 3 ))||new_error){
goto LABEL_ERROR;
}
qbs_set(_FUNC_WHATISMYIP_STRING_WHATISMYIP,_FUNC_WHATISMYIP_STRING_IP);
qbs_cleanup(qbs_tmp_base,0);
goto exit_subfunc;
}
}
}
}
}while((!(-((func_timer(NULL,0))>(*_FUNC_WHATISMYIP_SINGLE_T+ 5 ))))&&(!new_error));
dl_exit_114:;
sub_close(*_FUNC_WHATISMYIP_LONG_C,1);
}
LABEL_ERROR:;
exit_subfunc:;

//#include "free1.txt"
qbs_free(_FUNC_WHATISMYIP_STRING_E);
qbs_free(_FUNC_WHATISMYIP_STRING_X);
qbs_free(_FUNC_WHATISMYIP_STRING_A2);
qbs_free(_FUNC_WHATISMYIP_STRING_A);
qbs_free(_FUNC_WHATISMYIP_STRING_IP);

if ((tmp_mem_static_pointer>=mem_static)&&(tmp_mem_static_pointer<=mem_static_limit)) mem_static_pointer=tmp_mem_static_pointer; else mem_static_pointer=mem_static;
cmem_sp=tmp_cmem_sp;
qbs_maketmp(_FUNC_WHATISMYIP_STRING_WHATISMYIP);return _FUNC_WHATISMYIP_STRING_WHATISMYIP;
}

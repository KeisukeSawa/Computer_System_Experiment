#include "libs.h"       /* ����my3664h�����Ƥϡ�libs/libs.h�����礷�� */
#include <stdio.h>
#include <stdlib.h> // rand
volatile int tma_flag=FALSE;
volatile int sec_flag=FALSE;
volatile int tmv_flag=FALSE;
volatile long sec=86120;
volatile int tempo_flag=FALSE;
int tempo_compare=0;

#define DEBUG          /* �ǥХå���ϡ�������Ƥ��� */

// �����ư���˲���ɽ������ʤ��褦�ˤ��롣
//static unsigned int matrix_led_pattern[8]=
//  //{0x007e,0x0011,0x0011,0x0011,0x007e,0x7f00,0x4900,0x4900};
//{0x7e7e,0x1111,0x1111,0x0011,0x007e,0x7f00,0x4900,0x4900};
/*��0��7�Υǡ���(�ܺ٤ϡ����Υꥹ�Ȥ��ɤ�)*/

static unsigned int matrix_led_pattern[8]=
{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

/* int_timera() �� int_imterv() �γ���롼����(�ܥȥ�ϡ��դν���) */

#pragma interrupt                   /*��������롼����Ǥ��뤳�Ȥλ���*/
void int_timera(void){              
        volatile static int count=0;
	BIT_CLR(IRR1,6);      /* ����Υ�����A�����ߥե饰���ꥢ */
	tma_flag = TRUE;
	//	EI();         /* ɬ�פ˱����� EI()��¹�  */
	/*32��ƤӽФ��줿�顤ifʸ���椬�¹Ԥ���ơ�sec_flag��ͭ���ˤʤ�*/
	if(++count >= 1){
	      count=0;
	      sec_flag = TRUE;
	      sec++;               /* sec�ϡ�1�ä��Ȥ˥��󥯥���Ȥ����*/
	}
	// 24���ˤʤä���ꥻ�å�
	if(sec >= 86400){
	  sec = 0;
	}
	
#ifdef DEBUG
       { /*�ܥȥ�ϡ��ս�����;�ϥ����å��ѥ�����*/
	 volatile long int loop;
	 for(loop=0;loop<1;loop++); 
       }

	if((IRR1 & 0x40)!=0){ /* �����ߥե饰�ξ��֥����å� */
	 ENABLE_LED_GREEN();  /* �⤷�����ߥե饰��Ω�äƤ����п�LED���� */
       }
#endif
}

#pragma interrupt
void int_timerv(void){         /* ��1[msec] ���Ȥ˸ƤӽФ����褦�ˤ���*/
       static unsigned int column=0;
       unsigned int i,p;
       /*���ԡ����������*/

       //������tempo��tempo_compare��const�Ǥʤ����ѿ��ˤ��롣
       //â�������η׻���ܥȥ�ϡ��դ����¹Ԥ����,�������Ť��Τ�
       //�ȥåץϡ��դǷ׻���Ԥ���
       //       const int tempo=120;
       //       const int tempo_compare = 1000/(tempo * 16 / 60);
       //       const int tempo_compare = 3735 / tempo ;
       static int tempo_count=0;

       TCSRV &= ~0xe0;         /* ������V�γ����ߥե饰���ꥢ*/
       tmv_flag=TRUE;
       /*int_timerv�ϡ�����ػߤΤޤ�,����ư��*/

       /* 1ms ���Ȥ˥ϡ��ɥꥢ�륿�����ư�����������(matrix_led����)*/

       column=(column+1) & 0x0007; /* column=(column++)&0x0007�ϡ�NG�Ǥ���  */ 
       ENABLE_MATRIX_LATCH();      /*��å���ͭ���ˤ�������΢��D-FF�򥻥å� */

       /*16bit (1��ʬ) �򥷥ꥢ��ž��*/
       for(p=matrix_led_pattern[column],i=0;i<16;i++,p<<=1){        
	 if((p&0x8000)==0){
	   DISABLE_MATRIX_SIN();
	 }else{
	   ENABLE_MATRIX_SIN();
	 }
	 SET_H_MATRIX_SCLK();SET_L_MATRIX_SCLK();   /* pos���å����*/
       }

       ENABLE_MATRIX_BLANK();   /*�ʲ�����ȴ�������ѹ���ɽ�������ʤ�*/
       DISABLE_MATRIX_LATCH();  ENABLE_MATRIX_LATCH();  /*�����ȴ��*/
       SELECT_MATRIX_COLUMN(column);               /*j���ܤ�����ѹ�*/
       DISABLE_MATRIX_BLANK();                          /*����������*/


       /*���ԡ�����Ϣ����*/
       if(snd_check()==TRUE){
	 if(tempo_compare < ++tempo_count){
	   tempo_count = 0;
	   tempo_flag = TRUE;
	 }
       }


#ifdef DEBUG
       { /*�ܥȥ�ϡ��ս�����;�ϥ����å��ѥ�����*/
	 volatile int loop;
	 for(loop=0;loop<10;loop++);
	 /*�Ȥ����ﲼ�Ǥε�ư(â�������θ�˥������ϤΥ����ɤ��ɲ�)*/
	 /*loop<461 :10ʬ���٤Ǥ�,��þ���ǧ�Ǥ���(â�������ڲ����1�����)*/
       }

       if((TCSRV & 0xe0)!=0){ /* �����ߥե饰�ξ��֥����å� */
	 ENABLE_LED_RED();    /* �⤷�����ߥե饰��Ω�äƤ����ֿ�LED����*/
       }
#endif
}

/*�����޴ط���������*/
void timer_init(void){
	/*������A������(1/32[sec]���ȤΥ����޳����ߤ�����) */
	TMA = 0x9f; /* ������A TCA�ȥץꥹ������W�Υꥻ�å�(2011/12/15�ɲ�) */
	TMA = 0x9b; /* ������A 1/32[sec]��γ��������                      */
	/* 0x9b => 1001 1011 ������ (����ϡ����Υꥹ�Ȥ򸫤�)            */

	IENR1 |= 0x40; 	     /* ������A�γ�����������ͭ���ˤ���*/

	/*������V������(�ޥ˥奢��򸫤�,�ʲ���3�Ԥα��դ�񤭴�����)*/
	TCORA = 124;    /*�����ߴֳ֤���1ms�ˤʤ�褦�ˤ���(125ʬ��)*/
	TCRV0 = 0x4b;  /*����ڥ��ޥå�A�ǳ����ߡ����κݤ˥����󥿥��ꥢ*/
	TCRV1 = 0x01;  /*����å��ϡ������Φ���128ʬ��(16MHz/128)���Ѥ���*/

	/*timerv��ɬ��0���饹�����Ȥ����� */
	TCNTV = 0x00;          /* ������V�����������󥿤�0���鳫��*/
	BIT_CLR(IRR1,6);       /* ������A�γ����ߥե饰���ꥢ   */
	TCSRV &= ~0xe0;        /* ������V�γ����ߥե饰���ꥢ   */

	/* ������W������(���ԡ�����)*/
	TCRW = 0xbc;
}

/* main() �ʤɤγ����ߥ롼����ʳ��ν����ϡ��ȥåץϡ��դν���*/

/* User Interface �Υ��ơ��ȥޥ��� (�����������)*/

/* Ŭ�ڤʥ⡼��̾������뤳�Ȥ�˾�ޤ�������MODE_0���Ȥ��롣 */
/* �����������򤹤�ʤ�С�Ŭ�ڤʥơ��֥�����ɬ�פ����� */
enum MENU_MODE{
  MODE_OUT_OF_MIN=-1,
  MODE_0,
  MODE_1,
  MODE_2,
  MODE_3,
  MODE_4,
  MODE_5,
  MODE_6,
  MODE_7,
  MODE_OUT_OF_MAX
};

enum SW_CODE{
  KEY_NONE=0,         //��������̵�� 
  KEY_SHORT_U=(1<<4), //��û����
  KEY_SHORT_D=(1<<3), //��û����
  KEY_SHORT_L=(1<<2), //��û����
  KEY_SHORT_R=(1<<1), //��û����
  KEY_SHORT_C=(1<<0), //���û����
  KEY_LONG_U=(0x80 | (1<<4)), //��Ĺ����
  KEY_LONG_D=(0x80 | (1<<3)), //��Ĺ����
  KEY_LONG_L=(0x80 | (1<<2)), //��Ĺ����
  KEY_LONG_R=(0x80 | (1<<1)), //��Ĺ����
  KEY_LONG_C=(0x80 | (1<<0))  //���Ĺ����
};

typedef struct _UI_DATA{
  int mode;
  int prev_mode;
  unsigned char sw;
}UI_DATA;

extern void do_mode0(UI_DATA* ui_data);
extern void do_mode1(UI_DATA* ui_data);
extern void do_mode2(UI_DATA* ui_data);
extern void do_mode4(UI_DATA* ui_data);
extern void do_mode5(UI_DATA* ui_data); // add mode
extern void do_mode7(UI_DATA* ui_data);

UI_DATA* ui(char sw){ /* �ߡ��귿���ࡼ�������ɤä��Ǽ������ɤ��ͤ��� */
  static UI_DATA ui_data={MODE_0,MODE_0,};
  int prev_mode;

  ui_data.sw=(sw & 0x9f); /*ǰ�Τ���ˡ�b6,b5��0�ˤ��Ƥ���*/
  prev_mode=ui_data.mode;

  switch(ui_data.mode){
  case MODE_0:
    do_mode0(&ui_data);
    break;
  case MODE_1:
    do_mode1(&ui_data);
    break;
  case MODE_2:
    do_mode2(&ui_data);
    break;
  case MODE_4:
    do_mode4(&ui_data);
    break;
  case MODE_5: // add mode
    do_mode5(&ui_data);
  case MODE_7: // add mode
    do_mode7(&ui_data);
  default:
    break;
 }

  ui_data.prev_mode=prev_mode;

  return &ui_data;
}

void do_mode0(UI_DATA* ud){
  static int matrix_scroll=FALSE;
  static int next_mode_data=MODE_0;
  int prev_next_mode_data;
  static char str[2];

  /*�⡼��0��ɬ���¹Ԥ��륳���ɤ򵭽�*/
  prev_next_mode_data=next_mode_data;

  // �ޥȥ�å���LED�򸵤��᤹
  static unsigned int matrix_led_pattern[8]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

  if(ud->prev_mode!=ud->mode){  /* ¾�Υ⡼�ɤ���⡼��0�����ܤ������˼¹� */
    /*ɬ�פʤ顤���餫�Υ⡼�ɤν��������*/
    lcd_putstr(0,0,"MODE0->MODE0"); /*�⡼��0�ν��ɽ��*/
    next_mode_data=MODE_0;
    prev_next_mode_data=MODE_0;
    matrix_scroll=FALSE;
  }

  switch(ud->sw){  /*�⡼����ǤΥ������������*/

  case KEY_SHORT_U: /* ��û���� */
    if(next_mode_data  < (MODE_OUT_OF_MAX-1)){
      next_mode_data++;
    }
    break;

  case KEY_SHORT_D: /* ��û���� */
    if(next_mode_data  > (MODE_OUT_OF_MIN+1)){
      next_mode_data--;
    }
    break;

  case KEY_SHORT_L: /* ��û���� */
    FLIP_LED_RED(); //�ֿ�LED�Υե�åפ�¹�
    break;

  case KEY_SHORT_R: /* ��û���� */
    FLIP_LED_GREEN();
    break;

  case KEY_LONG_L: /* ��Ĺ���� */
    FLIP_LED_BLUE();
    break;

  case KEY_LONG_R: /* ��Ĺ���� */
    if(matrix_scroll==FALSE){//�ޥȥꥯ��LED�Υ�������ե饰�Υե�å�
      matrix_scroll=TRUE;
    }else{
      matrix_scroll=FALSE;
    }

    break;

  case KEY_LONG_C: /* ���������Ĺ���� */
    ud->mode=next_mode_data;/*���ϡ��⡼���ѹ�*/
    break;

  default: /*�嵭�ʳ�*/
    break;
  }

  /* �⡼�ɤν�λ���˽������륳���� */
  if((prev_next_mode_data!=next_mode_data) || sec_flag==TRUE){ 
    /* ���δ�˾����⡼�ɤ��ͤ��Ѥ�ä����ν��� */
    str[0]='0'+next_mode_data;
    str[1]='\0';
    lcd_putstr(11,0,str);
  }

  if(sec_flag==TRUE){ /* 1�ä��Ȥν���*/
    lcd_clear();
    lcd_putdec(0,1,5,sec);      /* LCD�β��ι�(1����)�ˡ��в��ÿ���ɽ�� */
    lcd_putstr(0,0,"MODE0->MODE");
    lcd_putstr(11,0,str);

    /*�����ȡ������Ǥϡ�LCD�κ����������1�ä��Ȥ˹ԤäƤ��롣        */
    /*����ϡ������졤ͽ�����̥Υ����ǡ�LCD��ɽ���˸�ư�ȯ�����Ƥ⡤ */
    /*1�ø�ˤϡ�����������Ȥ������̤���Ԥ��Ƥ��롣�ϡ��ɤ������Ǥϡ� */
    /*�����鹩�פ��Ƥ⡤�ɤ��褦��̵���Υ��������ä��ꤹ��ΤǤ��ġ�    */
    /*�Զ���ȯ����Ψ�ϡ��֥����Ȥ򤢤����٤�����Сײ����뤳�Ȥ���ǽ��*/

    /*�Ĥ��Ǥˡ�matrix_led�Υ��������ԤäƤߤ�*/
    if(matrix_scroll!=FALSE){ 
      /*FALSE�ǥ����å����Ƥ���Τϡ�0�ǤΥ����å����������̤˹�®������*/
      /*�ʤ��������δؿ��ϡ�ñ�ʤ�ǥХå������ȤʤΤǡ��¹Ԥ�³������*/
      /*�ե���ȥơ��֥��ȴ���Ф��Ƥ��ޤ���ɽ�����Ѥˤʤ�ޤ���*/
      matrix_font_debug_out_sample(matrix_led_pattern);
    }

    sec_flag=FALSE;
  }

}

  /*���å��󥿥��ޡ��Υ��르�ꥺ��ΰ���(���� mode1)*/
void mode1_time(int set){
  static int m,s,l=0,n=0;
  if(set!=0){
    l=set;
    n=1;
  }
  if(set>=0){
  lcd_putstr(0,1,"U30D10R5L3");
  m=l/60;
  s=l%60;
  char data[6];
  //  long m,s;
    //    s=set % 60;
    //    m=(set/ 60); 
    data[0]='0'+m/10;
    data[1]='0'+m%10;
    data[2]=':';
    data[3]='0'+s/10;
    data[4]='0'+s%10;
    data[5]='\0';
    lcd_putstr(16-5,1,data);
    if(l>0)
    l--;
    if(l==0&&n==1)
      snd_play("C");
  }
}

void do_mode1(UI_DATA* ud){
  if(ud->prev_mode!=ud->mode){  /* ¾�Υ⡼�����ܤ������˼¹� */
    /*ɬ�פʤ顤���餫�Υ⡼�ɤν��������*/
    lcd_clear();
    lcd_putstr(0,0,"MODE1"); /*�⡼��1�ν��ɽ��*/
    lcd_putstr(0,1,""); /*�⡼��1�ν��ɽ��*/
  }
  switch(ud->sw){  /*�⡼����ǤΥ������������*/

  case KEY_SHORT_U: /* ��û���� */
    mode1_time(1800);
    break;
    
  case KEY_SHORT_D: /* ��û���� */
    mode1_time(600);
    break;

  case KEY_SHORT_L: /* ��û���� */
    mode1_time(180);
    break;
  case KEY_SHORT_R: /* ��û���� */
    mode1_time(300);
    break;
  default:
    mode1_time(0);
  }/* /switch */
   /*�⡼��1�ϡ�����ܥ��󤬲����줿�顤MODE0�����*/
  switch(ud->sw){    /*�⡼����ǤΥ������������*/
  case KEY_LONG_C:   /* ���������Ĺ���� */
    ud->mode=MODE_0; /* ���ϡ��⡼��0����� */
    break;
  }

}

/*����ɽ���Υ��르�ꥺ��ΰ���*/
void show_sec(void){
  char data[9];
  long h,m,s;
  long sec_hold=sec; /* �ͤ��������Ƥ������ˡ�sec���Ѥ��ȷ��ʤΤǡ�   */
                    /* �����ǡ�sec���ͤ���ޤ��롣sec���ͤϡ��ܥȥ�ϡ���*/
                    /* ���Ѳ������Ƥ���Τǡ����������Ƚ�������Ѳ����롣*/
  
  s=sec_hold % 60;
  m=(sec_hold / 60); /* �����ǡ�h���ͤη������ϡ����ڤ��Ƥ��ʤ�����͡�*/
  h=(sec_hold / 3600);
  /* �ҥ�Ȥϡ���sec�ϡ�int����*/
  data[0]='0'+h/10;
  data[1]='0'+h%10;
  if(sec_hold>3600){
    while(1){
      if(sec_hold-3600<0){
	break;
      }
      sec_hold=sec_hold-3600;
    }
  }
  data[2]=':';
  while(m>=60)
    m-=60;
  data[3]='0'+m/10;
  data[4]='0'+m%10;
  data[5]=':';
  data[6]='0'+s/10;
  data[7]='0'+s%10;
  data[8]='\0';
  if(sec==86400) sec=0;

  lcd_putstr(16-10,1,data);
}


//ƣ�椬do_mode2�ޤǻȤäƤޤ�
char astrisk[9];
char settime[9];
int tim[9];
volatile int tsetflag=FALSE;
int data=1,ast=7;
void do_mode2(UI_DATA* ud){
  settime[2]=':';
  settime[5]=':';
  astrisk[8]='\0';
  int i=0;
long sec_henkou=0;
  if(ud->prev_mode!=ud->mode ){ 
    /* ¾�Υ⡼�����ܤ������˼¹� �⤷���ϡ�1�ä��Ȥ�ɽ��*/
    /*ɬ�פʤ顤���餫�Υ⡼�ɤν��������*/
    lcd_clear();  //0123456789ABCDEF
    lcd_putstr(0,0,"MODE2:sec�� �ˎ�������"); /*�⡼��2�ν��ɽ��*/
    show_sec();
    sec_flag=FALSE;

     for(i=0;i<8;i++){
       astrisk[i]=' ';
       tim[i]=0;
  }
  }
  
  if((sec_flag)==TRUE){
    if(tsetflag==TRUE){
      lcd_clear();
      astrisk[ast]='*';
      
      switch(ud->sw){
      case KEY_SHORT_R:
	astrisk[ast]=' ';
	if((ast==1) || (ast==4)){
	  ast=ast+2;
	}
	else if(ast==7){
	  ast=0;
	}
	else{
	  ast++;
	}
	
	break;
    case KEY_SHORT_L:
      astrisk[ast]=' ';
      	if((ast==3) || (ast==6)){
	  ast=ast-2;
	}
	else if(ast==0){
	  ast=7;
	}
	else{
	  ast--;
	}
	
	break;
	  case KEY_SHORT_U:
	    if(ast==0){
	      if(tim[ast]==2){
		tim[ast]=0;
	      }
	      else if(tim[ast]==1){	
		tim[ast]+=1;
		tim[ast+1]=0;
	      }
	      else
		tim[ast]+=1;
	    }
	    if(ast==1){
	      if(tim[ast-1]==2)
		if(tim[ast]==3){
		  tim[ast]=0;
		}
		else{
		  tim[ast]+=1;
		}
	      else if(tim[ast]==9){
		tim[ast]=0;
	      }
	      else
		tim[ast]+=1;
	    }
	    
	    if((ast==4) || (ast==7)){
	      if(tim[ast]==9){
		tim[ast]=0;
	      }
	      else
		tim[ast]+=1;
	    }
	    if(ast==3 || ast==6){
	      if(tim[ast]==5)
		tim[ast]=0;
	      else
		tim[ast]+=1;
	    }
	    break;
      case KEY_SHORT_D:
	if(ast==0){
	      if(tim[ast]==0){
		tim[ast]=2;
		tim[ast+1]=0;
	      }
	      else 	
		tim[ast]-=1;
	    }
	    if(ast==1){
	      if(tim[ast-1]==2)
	      if(tim[ast]==0){
		  tim[ast]=3;
		}
		else
		  tim[ast]-=1;
	      else
		if(tim[ast]==0)
		  tim[ast]=9;
		else
		  tim[ast]-=1;
	      
	    }
	    if((ast==4) || (ast==7)){
	      if(tim[ast]==0){
		tim[ast]=9;
	      }
	      else
		tim[ast]-=1;
	    }
	    if(ast==3 || ast==6){
	      if(tim[ast]==0)
		tim[ast]=5;
	      else
		tim[ast]-=1;
	    }
	    break;
      }
      
      lcd_putstr(8,0,astrisk);
      for(i=0;i<9;i++)
	if((i!=2) && (i!=5))
	 settime[i]='0'+tim[i];
      lcd_putstr(8,1,settime);
      sec_flag=FALSE;
    }
    else{
       lcd_clear();  //0123456789ABCDEF
    lcd_putstr(0,0,"MODE2:sec�� �ˎ�������"); /*�⡼��2�ν��ɽ��*/
    show_sec();
    sec_flag=FALSE;
    }
  }
  /*�⡼��2�ϡ�����ܥ��󤬲����줿�顤MODE0�����*/
  switch(ud->sw){    /*�⡼����ǤΥ������������*/
  case KEY_LONG_C:   /* ���������Ĺ���� */
    ud->mode=MODE_0; /* ���ϡ��⡼��0����� */
    break;
    
  case KEY_LONG_L:
    tsetflag=TRUE;
    break;
  case KEY_LONG_R:
    sec_henkou=tim[0]*36000+tim[1]*3600+tim[3]*600+tim[4]*60+tim[6]*10+tim[7];
    sec=sec_henkou;
    tsetflag=FALSE;
    break;
  }
}


//mode_4 ���塼�ƥ��󥰡����ܡ�
void do_mode4(UI_DATA* ud){

  static int row;
  static int all[8]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
  int i;
  
  if(ud->prev_mode!=ud->mode){
    lcd_clear();
    lcd_putstr(0,0,"MODE4:SHOOTING");
    lcd_putstr(0,1,"SCORE=0");
    row=3;
    for(i=0;i<=7;i++){
      all[i]=0x0000;
    }
    all[row]=0x8000;
  }
  switch(ud->sw){  /*�⡼����ǤΥ������������*/

  case KEY_LONG_C:  /* ���������Ĺ���� */
    ud->mode=MODE_0; /* ���ϡ��⡼��0����� */
    break;
    
  case KEY_SHORT_R: /* ��û����  ���������*/
    if(row < 7){
      row++;
      all[row]=all[row] | 0x8000;
      all[row-1]=all[row-1] & ~(0x8000);
    }
    break;

  case KEY_SHORT_C:/* ���������û����  ���������*/
    if(row > 0){
      row--;
      all[row]=all[row] | 0x8000;
      all[row+1]=all[row+1] & ~(0x8000);
    }
    break;
    
  }
  for(i=0; i<=7; i++){
    matrix_led_pattern[i]=all[i];
  }
}

// MODE_5�ʶ��߾�����
void do_mode5(UI_DATA* ud){

   lcd_clear();  //0123456789ABCDEF
   lcd_putstr(0,0,"MODE5:syougi"); /*�⡼��2�ν��ɽ��*/

   matrix_led_pattern[0] = 0x8001;
   matrix_led_pattern[1] = 0x8001;
   matrix_led_pattern[2] = 0x8001;
   matrix_led_pattern[3] = 0x8001;
   matrix_led_pattern[4] = 0x8001;
   matrix_led_pattern[5] = 0x8001;
   matrix_led_pattern[6] = 0x8001;
   matrix_led_pattern[7] = 0x8001;

   
  
  /*�⡼��5�ϡ�����ܥ��󤬲����줿�顤MODE0�����*/
  switch(ud->sw){    /*�⡼����ǤΥ������������*/
  case KEY_LONG_C:   /* ���������Ĺ���� */
    ud->mode=MODE_0; /* ���ϡ��⡼��0����� */
    break;
  }

  
}
// char da[]="������";
char da[5];

char dat[6];

void do_mode7(UI_DATA* ud){
    char x='0', y='0', z='0';
    int a=0, b=0, c=0;
    int n=0, m=0;
    da[1]='e';
    da[2]='a';
da[3]='t';
da[4]='\0';
dat[1]='b';
dat[2]='i';
dat[3]='t';
dat[4]='e';
dat[5]='\0';
    /* 3�������ͤ����� */
    // srand((unsigned int)time(NULL));
    x = (char)(rand()%10);
    do {
      y = (char)(rand()%10);
    } while(y == x);
    do {
      z = (char)(rand()%10);
    } while(z == x||z == y);

    for (;;) {

        /* �桼�����Ϥ������1�ΰ̡�10�ΰ̡�100�ΰ̤�ʬ���� */
      
        /* �����������ͤ�Ϥ��� */
        if (n < 12||n > 987||a == b||a == c||b == c)
            continue;

        /* �֥����ȡפο� */
        n = 0;
        if (a == x) n++;
        if (b == y) n++;
        if (c == z) n++;
	da[0]='0'+n;
        /* �֥Х��ȡפο� */
        m = 0;
        if (a == y || a == z) m++;
        if (b == x || b == y) m++;
        if (c == x || c == y) m++;
	dat[0]='0'+m;
        if (n != 3){
	  lcd_putstr(1,1,da);
	  lcd_putstr(7,1,dat);
	}
        else {
	  // putstr("Hit!\n");
            break;
        }
    }
    //return 0;
}



int main(void){
         UI_DATA* ui_data=NULL;
	unsigned char sw=KEY_NONE;

	//	char test[]="hoge";

	DI();           /* ǰ�Τ����®�䤫��DI() */ 
	io_init();      /* I/O�ݡ��Ȥν�����롼����(matrix,lcd�����ޤ�) */
	lcd_init();     /* LCD�ν�����롼�����¹� */

	/*LCD�ν�����֤ϡ���������OFF��,�֥�󥯤�OFF�Ȳ��ˤ��Ƥ���*/
	lcd_cursor(OFF);
	lcd_blink(OFF); 

	/*���ԡ����ѤΥƥ�ݷ׻�(�Ȥꤢ�����ν���ͤ�����)*/
	{
	  int tempo=120;
	  tempo_compare = 3735 / tempo; /* 1000/(tempo * 16 / 60) ��Ÿ�� */
	}

	timer_init();   /* �����ޤν�������¹�(EI��ľ���˼¹�) */
        EI();	        /* �����ߤ����  */

	//Ⱦ�ѥ������ʰ���:���ϤǤ��ʤ��ʤ顤�������饳�ԥڤ��衣
	//���������� ���������� ���������� �����Î� �ŎƎǎȎ� �ʎˎ͎̎� �ώЎюҎ� �׎؎َڎ� �ԎՎ� �܎��� �ގ� �� ����������
	
	//�ǽ�ϡ��⡼��0����¹Ԥ����ꡣ
	lcd_putstr(0,0,"MODE0->MODE0");

	for(;;){	  /* �Ȥ߹��ߥ����ƥ�ϡ�����Ū�ˤ�̵�¥롼�פǼ¹� */


	  /* �ʲ���ifʸ����ϡ�¿ʬ��1/32�ä��Ȥ˽�����Ԥ�*/
	  if(tma_flag == TRUE){ 
	    sw = sw_mng();       /* �����å������ϥ����å� libs/key.c */
	    ui_data=ui(sw);           /* �桼�����󥿥ե������μ¹� */

	    if(ui_data->mode==MODE_1){
	      /* MODE1���ä��顤�������ϤΥǥХå������ȤΥǥХå����� */
	      key_debug_out_sample(); 
	    }

	    tma_flag=FALSE;
	  }

	  /* �ʲ��ν����ϡ�1ms���Ȥ˹Ԥ� */
	  if(tmv_flag==TRUE){

#if 0
	    /*���ԡ����Υ����쥯�ȥ���ȥ���ϡ�beta2����*/
	    /*�Ǥ��ʤ��褦�ˤʤ�ޤ�����*/
	    /*���ԡ����ѤΥݡ��Ȥϡ�������W�Ȥ��������ޤ���*/
	    /*�����޽��Ϥ�ľ�ܽ��Ϥ��뤳�Ȥˤʤ�ޤ�*/
	    /*���ԡ����Ѥδؿ� snd_mng(),snd_play(),snd_stop()�ʤɤ�*/
	    /*���Ѥ��뤳��*/

	    /*�Ȥꤢ�������⡼��1�λ��ˡ���Υ�����û�������줿�顤   */
	    /*(���Τˤϡ�û������Υ���줿����)��500Hz�β���û���Ф�   */
	    /*�褦�ʥ��ԡ����Υ���ץ륳���ɡ�                        */
	    /*���ޡ��Ȥ˥��ԡ������Ĥ餹�Τ��ä��顤�ɤ����ǡ� �ե饰 */
	    /*��Ω�ä��顤 FLIP_SPEAKER_DIRECT_CONTROL();��¹Ԥ���   */
	    if(ui_data!=NULL && ui_data->mode==MODE_1 && sw==KEY_SHORT_U){
		FLIP_SPEAKER_DIRECT_CONTROL();
	    }
#endif
	    tma_flag=FALSE;
	  }

	  if(tempo_flag){
	    snd_mng();
	    tempo_flag = FALSE;
	  }


	  SLEEP(); /*���������ޤǡ����꡼�פ�����(���������㸺!)*/
	}
	return 0;  /*���ιԤϼ¹Ԥ���ʤ������ٹ��Ф��ʤ����ޤ��ʤ�*/
}


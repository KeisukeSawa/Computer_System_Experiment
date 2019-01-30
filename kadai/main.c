#include "libs.h"       /* 前のmy3664hの内容は，libs/libs.hへ統合した */
#include <stdio.h>
#include <stdlib.h> // rand
volatile int tma_flag=FALSE;
volatile int sec_flag=FALSE;
volatile int tmv_flag=FALSE;
volatile long sec=86120;
volatile int tempo_flag=FALSE;
int tempo_compare=0;

#define DEBUG          /* デバッグ中は，定義しておく */

// 初期起動時に何も表示されないようにする。
//static unsigned int matrix_led_pattern[8]=
//  //{0x007e,0x0011,0x0011,0x0011,0x007e,0x7f00,0x4900,0x4900};
//{0x7e7e,0x1111,0x1111,0x0011,0x007e,0x7f00,0x4900,0x4900};
/*列0〜7のデータ(詳細は，過去のリストを読め)*/

static unsigned int matrix_led_pattern[8]=
{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

/* int_timera() や int_imterv() の割込ルーチン(ボトムハーフの処理) */

#pragma interrupt                   /*割込処理ルーチンであることの指定*/
void int_timera(void){              
        volatile static int count=0;
	BIT_CLR(IRR1,6);      /* 本来のタイマA割り込みフラグクリア */
	tma_flag = TRUE;
	//	EI();         /* 必要に応じて EI()を実行  */
	/*32回呼び出されたら，if文の中が実行されて，sec_flagが有効になる*/
	if(++count >= 32){
	      count=0;
	      sec_flag = TRUE;
	      sec++;               /* secは，1秒ごとにインクリメントされる*/
	}
	// 24時になったらリセット
	if(sec >= 86400){
	  sec = 0;
	}
	
#ifdef DEBUG
       { /*ボトムハーフ処理の余力チェック用コード*/
	 volatile long int loop;
	 for(loop=0;loop<1;loop++); 
       }

	if((IRR1 & 0x40)!=0){ /* 割り込みフラグの状態チェック */
	 ENABLE_LED_GREEN();  /* もし割り込みフラグが立ってたら緑色LED点灯 */
       }
#endif
}

#pragma interrupt
void int_timerv(void){         /* 約1[msec] ごとに呼び出されるようにする*/
       static unsigned int column=0;
       unsigned int i,p;
       /*スピーカ周りの値*/

       //下記のtempoやtempo_compareをconstでなく，変数にする。
       //但し，この計算をボトムハーフで毎回実行すると,処理が重いので
       //トップハーフで計算を行う。
       //       const int tempo=120;
       //       const int tempo_compare = 1000/(tempo * 16 / 60);
       //       const int tempo_compare = 3735 / tempo ;
       static int tempo_count=0;

       TCSRV &= ~0xe0;         /* タイマVの割り込みフラグクリア*/
       tmv_flag=TRUE;
       /*int_timervは，割込禁止のまま,全力動作*/

       /* 1ms ごとにハードリアルタイムで動作させたい処理(matrix_led周り)*/

       column=(column+1) & 0x0007; /* column=(column++)&0x0007は，NGです…  */ 
       ENABLE_MATRIX_LATCH();      /*ラッチを有効にし，舞台裏でD-FFをセット */

       /*16bit (1列分) をシリアル転送*/
       for(p=matrix_led_pattern[column],i=0;i<16;i++,p<<=1){        
	 if((p&0x8000)==0){
	   DISABLE_MATRIX_SIN();
	 }else{
	   ENABLE_MATRIX_SIN();
	 }
	 SET_H_MATRIX_SCLK();SET_L_MATRIX_SCLK();   /* posエッジ一つ*/
       }

       ENABLE_MATRIX_BLANK();   /*以下の筒抜けと列変更は表示させない*/
       DISABLE_MATRIX_LATCH();  ENABLE_MATRIX_LATCH();  /*一瞬筒抜け*/
       SELECT_MATRIX_COLUMN(column);               /*j番目の列に変更*/
       DISABLE_MATRIX_BLANK();                          /*点灯させる*/


       /*スピーカ関連処理*/
       if(snd_check()==TRUE){
	 if(tempo_compare < ++tempo_count){
	   tempo_count = 0;
	   tempo_flag = TRUE;
	 }
       }


#ifdef DEBUG
       { /*ボトムハーフ処理の余力チェック用コード*/
	 volatile int loop;
	 for(loop=0;loop<10;loop++);
	 /*とある条件下での挙動(但し，その後にキー入力のコードを追加)*/
	 /*loop<461 :10分程度では,破綻を確認できず(但し，検証回数は1回だけ)*/
       }

       if((TCSRV & 0xe0)!=0){ /* 割り込みフラグの状態チェック */
	 ENABLE_LED_RED();    /* もし割り込みフラグが立ってたら赤色LED点灯*/
       }
#endif
}

/*タイマ関係の値設定*/
void timer_init(void){
	/*タイマAの設定(1/32[sec]ごとのタイマ割り込みを生成) */
	TMA = 0x9f; /* タイマA TCAとプリスケーラWのリセット(2011/12/15追加) */
	TMA = 0x9b; /* タイマA 1/32[sec]毎の割込の設定                      */
	/* 0x9b => 1001 1011 の内訳 (解説は，過去のリストを見よ)            */

	IENR1 |= 0x40; 	     /* タイマAの割り込み生成を有効にする*/

	/*タイマVの設定(マニュアルを見て,以下の3行の右辺を書き換えよ)*/
	TCORA = 124;    /*割り込み間隔が約1msになるようにする(125分周)*/
	TCRV0 = 0x4b;  /*コンペアマッチAで割り込み。その際にカウンタクリア*/
	TCRV1 = 0x01;  /*クロックは，内部のΦの128分周(16MHz/128)を用いる*/

	/*timervも必ず0からスタートさせる */
	TCNTV = 0x00;          /* タイマVの内部カウンタを0から開始*/
	BIT_CLR(IRR1,6);       /* タイマAの割り込みフラグクリア   */
	TCSRV &= ~0xe0;        /* タイマVの割り込みフラグクリア   */

	/* タイマWの設定(スピーカ用)*/
	TCRW = 0xbc;
}

/* main() などの割り込みルーチン以外の処理は，トップハーフの処理*/

/* User Interface のステートマシン (ここを作り込む)*/

/* 適切なモード名を入れることが望ましいが，MODE_0〜とする。 */
/* 美しい書き方をするならば，適切なテーブルを持つ必要がある */
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
  KEY_NONE=0,         //キー入力無し 
  KEY_SHORT_U=(1<<4), //上短押し
  KEY_SHORT_D=(1<<3), //下短押し
  KEY_SHORT_L=(1<<2), //左短押し
  KEY_SHORT_R=(1<<1), //右短押し
  KEY_SHORT_C=(1<<0), //中央短押し
  KEY_LONG_U=(0x80 | (1<<4)), //上長押し
  KEY_LONG_D=(0x80 | (1<<3)), //下長押し
  KEY_LONG_L=(0x80 | (1<<2)), //左長押し
  KEY_LONG_R=(0x80 | (1<<1)), //右長押し
  KEY_LONG_C=(0x80 | (1<<0))  //中央長押し
};

typedef struct _UI_DATA{
  int mode;
  int prev_mode;
  unsigned char sw;
}UI_DATA;

extern void do_mode0(UI_DATA* ui_data);
extern void do_mode1(UI_DATA* ui_data);
extern void do_mode2(UI_DATA* ui_data);
extern void do_mode3(UI_DATA* ui_data);
extern void do_mode4(UI_DATA* ui_data);
extern void do_mode5(UI_DATA* ui_data); // add mode
extern void do_mode6(UI_DATA* ui_data);
extern void do_mode7(UI_DATA* ui_data);

UI_DATA* ui(char sw){ /* ミーリ型？ムーア型？どっちで実装？良く考えて */
  static UI_DATA ui_data={MODE_0,MODE_0,};
  int prev_mode;

  ui_data.sw=(sw & 0x9f); /*念のために，b6,b5を0にしておく*/
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
  case MODE_3:
    do_mode3(&ui_data);
    break;
  case MODE_4:
    do_mode4(&ui_data);
    break;
  case MODE_5: // add mode
    do_mode5(&ui_data);
    break;
  case MODE_6:
    do_mode6(&ui_data);
    break;
  case MODE_7: // add mode
    do_mode7(&ui_data);
    break;
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

  /*モード0で必ず実行するコードを記述*/
  prev_next_mode_data=next_mode_data;

  // マトリックスLEDを元に戻す
  static unsigned int matrix_led_pattern[8]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

  if(ud->prev_mode!=ud->mode){  /* 他のモードからモード0に遷移した時に実行 */
    /*必要なら，何らかのモードの初期化処理*/
    lcd_putstr(0,0,"MODE0->MODE0"); /*モード0の初期表示*/
    next_mode_data=MODE_0;
    prev_next_mode_data=MODE_0;
    matrix_scroll=FALSE;
  }

  switch(ud->sw){  /*モード内でのキー入力別操作*/

  case KEY_SHORT_U: /* 上短押し */
    if(next_mode_data  < (MODE_OUT_OF_MAX-1)){
      next_mode_data++;
    }
    break;

  case KEY_SHORT_D: /* 下短押し */
    if(next_mode_data  > (MODE_OUT_OF_MIN+1)){
      next_mode_data--;
    }
    break;

  case KEY_SHORT_L: /* 左短押し */
    FLIP_LED_RED(); //赤色LEDのフリップを実行
    break;

  case KEY_SHORT_R: /* 右短押し */
    FLIP_LED_GREEN();
    break;

  case KEY_LONG_L: /* 左長押し */
    FLIP_LED_BLUE();
    break;

  case KEY_LONG_R: /* 右長押し */
    if(matrix_scroll==FALSE){//マトリクスLEDのスクロールフラグのフリップ
      matrix_scroll=TRUE;
    }else{
      matrix_scroll=FALSE;
    }

    break;

  case KEY_LONG_C: /* 中央キーの長押し */
    ud->mode=next_mode_data;/*次は，モード変更*/
    break;

  default: /*上記以外*/
    break;
  }

  /* モードの終了時に処理するコード */
  if((prev_next_mode_data!=next_mode_data) || sec_flag==TRUE){ 
    /* 次の希望するモードの値が変わった時の処理 */
    str[0]='0'+next_mode_data;
    str[1]='\0';
    lcd_putstr(11,0,str);
  }

  if(sec_flag==TRUE){ /* 1秒ごとの処理*/
    lcd_clear();
    lcd_putdec(0,1,5,sec);      /* LCDの下の行(1行目)に，経過秒数を表示 */
    lcd_putstr(0,0,"MODE0->MODE");
    lcd_putstr(11,0,str);

    /*コメント：ここでは，LCDの再描画処理を1秒ごとに行っている。        */
    /*これは，万が一，予期せぬノイズで，LCDの表示に誤動作が発生しても， */
    /*1秒後には，回復させるという効果を期待している。ハードの世界では， */
    /*いくら工夫しても，防ぎようが無いノイズがあったりするのです…。    */
    /*不具合の発生確率は，「コストをある程度かければ」下げることが可能。*/

    /*ついでに，matrix_ledのスクロールも行ってみる*/
    if(matrix_scroll!=FALSE){ 
      /*FALSEでチェックしているのは，0でのチェックの方が一般に高速だから*/
      /*なお，下記の関数は，単なるデバッグアウトなので，実行をし続けたら*/
      /*フォントテーブルを抜け出してしまい，表示が変になります。*/
      matrix_font_debug_out_sample(matrix_led_pattern);
    }

    sec_flag=FALSE;
  }

}

  /*キッチンタイマーのアルゴリズムの一部(山西 mode1)*/
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
  if(ud->prev_mode!=ud->mode){  /* 他のモード遷移した時に実行 */
    /*必要なら，何らかのモードの初期化処理*/
    lcd_clear();
    lcd_putstr(0,0,"MODE1"); /*モード1の初期表示*/
    lcd_putstr(0,1,""); /*モード1の初期表示*/
  }
  switch(ud->sw){  /*モード内でのキー入力別操作*/

  case KEY_SHORT_U: /* 上短押し */
    mode1_time(1800);
    break;
    
  case KEY_SHORT_D: /* 下短押し */
    mode1_time(600);
    break;

  case KEY_SHORT_L: /* 左短押し */
    mode1_time(180);
    break;
  case KEY_SHORT_R: /* 右短押し */
    mode1_time(300);
    break;
  default:
    mode1_time(0);
  }/* /switch */
   /*モード1は，真中ボタンが押されたら，MODE0に戻る*/
  switch(ud->sw){    /*モード内でのキー入力別操作*/
  case KEY_LONG_C:   /* 中央キーの長押し */
    ud->mode=MODE_0; /* 次は，モード0に戻る */
    break;
  }

}

/*時計表示のアルゴリズムの一部*/
void show_sec(void){
  char data[9];
  long h,m,s;
  long sec_hold=sec; /* 値を生成している最中に，secが変わると嫌なので，   */
                    /* ここで，secの値を捕まえる。secの値は，ボトムハーフ*/
                    /* で変化させているので，運が悪いと処理中に変化する。*/
  
  s=sec_hold % 60;
  m=(sec_hold / 60); /* ここで，hの値の健全性は，検証していないからね。*/
  h=(sec_hold / 3600);
  /* ヒントは，「secは，int型」*/
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


//藤井がdo_mode2まで使ってます
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
    /* 他のモード遷移した時に実行 もしくは，1秒ごとに表示*/
    /*必要なら，何らかのモードの初期化処理*/
    lcd_clear();  //0123456789ABCDEF
    lcd_putstr(0,0,"MODE2:secｦ ﾋｮｳｼﾞ"); /*モード2の初期表示*/
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
    lcd_putstr(0,0,"MODE2:secｦ ﾋｮｳｼﾞ"); /*モード2の初期表示*/
    show_sec();
    sec_flag=FALSE;
    }
  }
  /*モード2は，真中ボタンが押されたら，MODE0に戻る*/
  switch(ud->sw){    /*モード内でのキー入力別操作*/
  case KEY_LONG_C:   /* 中央キーの長押し */
    ud->mode=MODE_0; /* 次は，モード0に戻る */
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


void do_mode3(UI_DATA* ud){
  static int flag;  /* キー入力の有効 */
  static int count;
  static int ready;
  static int i;
  static int score;
  static int num[10];
  static char mem[]="**********";
  static char ans[]="**********";

  if(ud->prev_mode!=ud->mode){  /* 他のモード遷移した時に実行 */
    /*必要なら，何らかのモードの初期化処理*/
    lcd_clear();
    lcd_putstr(0,0,"MODE3"); /*モード3の初期表示*/
    lcd_putstr(0,1,"ｼﾀ_ﾅｶﾞｵｼ_ｽﾀｰﾄ"); /*モード3の初期表示*/
    srand(sec);
    ready=3; /* 開始秒数の変数 */
    count=0;
    flag=0;
    score=0;
    for(i=0; i<10; i++){
      num[i]=rand()%5;
      //      if(num[i]!=num[i-1] && i!=0) num[i]%=(num[i]+1);
      switch(num[i]){
      case 0:
	mem[i]='U';
	break;
      case 1:
	mem[i]='D';
	break;
      case 2:
	mem[i]='L';
	break;
      case 3:
	mem[i]='R';
	break;
      case 4:
	mem[i]='C';
	break;
      }
      ans[i]='*';  
    }
    ans[i]='\0';     
    for(i=0; i<8; i++)
      matrix_led_pattern[i]=0x0000;
  }

  /*モード1は，真中ボタンが押されたら，MODE0に戻るだけの単純な処理*/
  /*それに，beta2のバージョンでは，音楽再生機能の起動部分を追加*/
  /*但し，main関数内で，キーのデバッグ表示を行っている*/
  if(sec_flag==TRUE){ /* 1秒ごとの処理*/
    switch(flag){
    case 0:
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_LONG_C:  /* 中央キーの長押し */
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
      case KEY_LONG_D:  /* 下キー長押しで開始 */
	flag=1;
	break;
      }
      break;
    case 1:
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_LONG_C:  /* 中央キーの長押し */
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
      }
      lcd_clear();
      lcd_putstr(0,0,"MODE3"); /*モード3の初期表示*/
      lcd_putdec(0,1,5,ready); /* LCDの下の行(1行目)に，経過秒数を表示 */
      if(ready==0) flag=2;
      ready--;
      sec_flag=FALSE;     
      break;
    case 2:
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_LONG_C:  /* 中央キーの長押し */
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
      }
      if(count<10){
	lcd_putstr(0,0,"ﾋｶﾘｦ_10ｺ_ｵﾎﾞｴﾙ"); /*モード3の初期表示*/
	for(i=0; i<8; i++)
	  matrix_led_pattern[i]=0x0000;
	switch(mem[count]){
	case 'U':                 /* 上 */
	  matrix_led_pattern[3]=0x0003;
	  matrix_led_pattern[4]=0x0003;
	  break;
	case 'D':                 /* 下 */
	  matrix_led_pattern[3]=0x00c0;
	  matrix_led_pattern[4]=0x00c0;
	  break;
	case 'L':                 /* 左 */
	  matrix_led_pattern[0]=0x0018;
	  matrix_led_pattern[1]=0x0018;
	  break;
	case 'R':                 /* 右 */
	  matrix_led_pattern[6]=0x0018;
	  matrix_led_pattern[7]=0x0018;
	  break;
	case 'C':                 /* 中央 */
	  matrix_led_pattern[3]=0x0018;
	  matrix_led_pattern[4]=0x0018;
	  break;
	}
	lcd_putstr(count,1,"*");
	count++;
	sec_flag=FALSE;     
      }
      if(count == 10){
	flag=3;
	count=0;
      }
      break;
    case 3:
      if(count==0) lcd_clear();
      matrix_led_pattern[0]=0x0000;
      matrix_led_pattern[1]=0x3c00;
      matrix_led_pattern[2]=0x4200;
      matrix_led_pattern[3]=0x5200;
      matrix_led_pattern[4]=0x6200;
      matrix_led_pattern[5]=0x4200;
      matrix_led_pattern[6]=0xbc00;
      matrix_led_pattern[7]=0x8000;
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_LONG_C:  /* 中央キーの長押し */
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
      case KEY_SHORT_U: /* 上のキー */
	lcd_putstr(count,0,"U");
	ans[count]='U';
	if(mem[count]!='U'){
	  flag=4;
	  ready=3;
	  score=count;
	}
	count++;
	break;
      case KEY_SHORT_D: /* 下のキー */
	lcd_putstr(count,0,"D");
	ans[count]='D';
	if(mem[count]!='D'){
	  flag=4;
	  ready=3;
	  score=count;
	}
	count++;
	break;
      case KEY_SHORT_L: /* 左のキー */
	lcd_putstr(count,0,"L");
	ans[count]='L';
	if(mem[count]!='L'){
	  flag=4;
	  ready=3;
	  score=count;
	}
	count++;
	break;
      case KEY_SHORT_R: /* 右のキー */
	lcd_putstr(count,0,"R");
	ans[count]='R';
	if(mem[count]!='R'){
	  flag=4;
	  ready=3;
	  score=count;
	}
	count++;
	break;
      case KEY_SHORT_C:  /* 中央キー */
	lcd_putstr(count,0,"C");
	ans[count]='C';
	if(mem[count]!='C'){
	  flag=4;
	  ready=3;
	  score=count;
	}
	count++;
	break;
      }
      break;
    case 4:
      if(count==0) lcd_clear(); /**?**/
      matrix_led_pattern[0]=0x0000;
      matrix_led_pattern[1]=0x0000;
      matrix_led_pattern[2]=0x007c;
      matrix_led_pattern[3]=0x0012;
      matrix_led_pattern[4]=0x0012;
      matrix_led_pattern[5]=0x007c;
      matrix_led_pattern[6]=0x0000;
      matrix_led_pattern[7]=0x0000;
      //
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_SHORT_D:  /* 下キー */
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
      }
      //
      lcd_clear();
      lcd_putstr(0,0,ans);
      lcd_putstr(0,1,"Score=");
      lcd_putdec(6,1,3,score); /* LCDの下の行(1行目)に，経過秒数を表示 */
      if(ready<0){
	flag=5;
	ready=3;
      }
      ready--;
      sec_flag=FALSE;     
      break;
      
    case 5:
      if(count==0) lcd_clear(); /**?**/
      lcd_clear();
      lcd_putstr(0,0,ans);
      lcd_putstr(0,1,mem);
      if(ready<0){
	flag=4;
	ready=3;
      }
      ready--;
      sec_flag=FALSE;     
      break;
    }
  }
  else{
    sec_flag=FALSE;
    //
    switch(ud->sw){  /*モード内でのキー入力別操作*/
    case KEY_LONG_C:  /* 中央キー長押し */
      ud->mode=MODE_0; /* 次は，モード0に戻る */
      break;
    }
    //
  }
}


//mode_4 シューティング（倉本）
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
  switch(ud->sw){  /*モード内でのキー入力別操作*/

  case KEY_LONG_C:  /* 中央キーの長押し */
    ud->mode=MODE_0; /* 次は，モード0に戻る */
    break;
    
  case KEY_SHORT_R: /* 右短押し  自機の操作*/
    if(row < 7){
      row++;
      all[row]=all[row] | 0x8000;
      all[row-1]=all[row-1] & ~(0x8000);
    }
    break;

  case KEY_SHORT_C:/* 中央キーの短押し  自機の操作*/
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

// MODE_5（挟み将棋）
void do_mode5(UI_DATA* ud){

  static int row;
  static int line;
  static int play[8]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
  static int koma[8]={0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001,0x8001};
  static int all[8]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
  int playermode = 11; 
  // int playerrow = 0;
  // int playerline = 0;
  int i;

  if(ud->prev_mode!=ud->mode){
    lcd_clear();
    lcd_putstr(0,0,"MODE5:syougi");
    row=0;
    line=7;
    for(i=0;i<=7;i++){
      play[i]=0x0000;
    }
    play[row]=0x8000;
  }

  switch(ud->sw){  /*モード内でのキー入力別操作*/

    case KEY_LONG_C:  /* 中央キーの長押し */
      ud->mode=MODE_0; /* 次は，モード0に戻る */
      break;
    
    case KEY_SHORT_R: /* 右短押し*/
      if(row < 7){
      int red = 0x0000 | (1 << (line));
      int green =  0x0000 | (1 << (line+8));
      row++;
      play[row] = red | green;
      play[row-1] = 0x0000;
      }
      break;

  case KEY_SHORT_L:/* 左キーの短押し */
    if(row > 0){
      int red = 0x0000 | (1 << (line));
      int green =  0x0000 | (1 << (line+8));
      row--;
      play[row] = red | green;
      play[row+1] = 0x0000;
    }
    break;

  case KEY_SHORT_U: /* 上キーの短押し */
    if(line > 0){
      int red = 0x0000 | (1 << (line-1));
      int green =  0x0000 | (1 << (line-1+8));
      play[row] = red | green;
      line--;
    }
    break;

  case KEY_SHORT_D: /* 下キーの短押し */
    if(line < 7){
      int red = 0x0000 | (1 << (line+1));
      int green =  0x0000 | (1 << (line+1+8));
      play[row] = red | green;
      line++;
    }
    break;

  case KEY_SHORT_C: /* 中央キーの短押し */
    if(playermode == 11){
      //playerrow = row;
      //playerline = line;
      playermode = 12;
    }
    else if(playermode == 12){
      // 次はrow,lineを所得→緑に書き換え
      // その後、plyerrowとplayerlineを元に、元の緑をnullに戻す
      
      
      
    }
    else if(playermode == 21){

    }
    else if(playermode == 22){
      
    }
    break;
    
  }

  for(i=0; i<=7; i++){
    all[i] = play[i] | koma[i];
  }
  

  for(i=0; i<=7; i++){
    matrix_led_pattern[i]=all[i];
  }

}


// 藤井　ピンポンゲーム
int ball_x,ball_y=2;//ボールのx座標とy座標
int ita=3;  //下の部分の左側の座標
int hajime_flag=FALSE;
int ue_flag=TRUE; //ボールの上下の移動方向
int migi_flag=TRUE; //ボールの左右の移動方向
int reset_flag=FALSE;
int restart_flag=FALSE;
int s6,count3=0,hanekai=0;
int level=1,level_flag=FALSE;
int pinpon_pattern[]={
  0x0000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100};

void do_mode6(UI_DATA* ud){
  int i=0;
  if(ud->prev_mode!=ud->mode ){ 
    lcd_clear();
    lcd_putstr(0,0,"MODE6:ﾋﾟﾝﾎﾟﾝｹﾞｰﾑ"); /*モード2の初期表示*/
    lcd_putstr(0,1,"center push");
    s6=sec%8;
    ball_x=s6;
    for(i=0;i<8;i++)
      if(i==3 || i==4)
	matrix_led_pattern[i] = 0x8080;
      else
	matrix_led_pattern[i] = 0x0000;
    matrix_led_pattern[ball_x]=matrix_led_pattern[ball_x]^ pinpon_pattern[ball_y];
  }
  if((tmv_flag)==TRUE){
    if(level_flag==TRUE){
      lcd_clear();
      lcd_putstr(0,0,"level=");
      lcd_putdec(8,0,1,level);
      lcd_putstr(0,1,"left back");
      switch(ud->sw){
      case KEY_SHORT_U:
	if(level<5)
	level++;
	break;
      case KEY_SHORT_D:
	if(level>1)
	  level--;
	break;
      case KEY_SHORT_L:
	  level_flag=FALSE;
	   lcd_clear();
	   lcd_putstr(0,0,"MODE6:ﾋﾟﾝﾎﾟﾝｹﾞｰﾑ"); /*モード2の初期表示*/
	   lcd_putstr(0,1,"center push");
	  break;
      }
    }
    if(hajime_flag == TRUE){
      if(level_flag==FALSE){
      switch(ud->sw){  /*モード内でのキー入力別操作*/
      case KEY_SHORT_L:
	if(ita > 0){
	  matrix_led_pattern[ita] =matrix_led_pattern[ita]^ 0x8080;
	  matrix_led_pattern[ita+1] =matrix_led_pattern[ita+1]^ 0x8080;
	  ita--;
	}
	break;
      case KEY_SHORT_R:
	if(ita < 6){
	  matrix_led_pattern[ita] =matrix_led_pattern[ita]^ 0x8080;
	  matrix_led_pattern[ita+1] =matrix_led_pattern[ita+1]^ 0x8080;
	  ita++;
	}
	break;
      }
      matrix_led_pattern[ita] =	matrix_led_pattern[ita] | 0x8080;
      matrix_led_pattern[ita+1] =matrix_led_pattern[ita+1]| 0x8080;
      count3++;
      if(count3==(6-level)){
	count3=0;
	matrix_led_pattern[ball_x]=matrix_led_pattern[ball_x]^ pinpon_pattern[ball_y];
       	if(ue_flag==TRUE)
	  if(ball_y <7)
	    ball_y++;
	  else{
	    ball_y--;
	    ue_flag=FALSE;
	  }
	else{
	  if(ball_y>1)
	    ball_y--;
	  else{
	    if(ball_x==ita || ball_x==ita+1){
	      ball_y++;
	      hanekai++;
	      ue_flag=TRUE;
	    }
	    else{
	      hajime_flag=FALSE;
	      reset_flag=TRUE;
	      ball_y--;
	    }
	  }
	}
	if(migi_flag==TRUE)
	  if(ball_x < 7 )
	    ball_x++;
	  else{
	    migi_flag=FALSE;
	    ball_x--;
	  }
	else{
	  if(ball_x >0)
	    ball_x--;
	  else{
	    migi_flag=TRUE;
	    ball_x++;
	  }
	}
	lcd_clear();
	lcd_putstr(4,1,"point");
	lcd_putdec(0,1,4,hanekai);
       	matrix_led_pattern[ball_x] = matrix_led_pattern[ball_x] ^ pinpon_pattern[ball_y];
      }
      }
    }
    if(reset_flag==TRUE){
      	lcd_clear();
	lcd_putstr(0,0,"up push");
	lcd_putstr(4,1,"point");
	lcd_putdec(0,1,4,hanekai);
      if((ud->sw)==KEY_SHORT_U){
	restart_flag=TRUE;
	reset_flag=FALSE;
	ball_y=2;//ボールのx座標とy座標
	ita=3;  //下の部分の左側の座標
	ue_flag=TRUE; //ボールの上下の移動方向
	migi_flag=TRUE; //ボールの左右の移動方向
	count3=0;
	hanekai=0;
	 s6=sec%8;
	 ball_x=s6;
	 for(i=0;i<8;i++)
	   if(i==3 || i==4)
	     matrix_led_pattern[i] = 0x8080;
	   else
	     matrix_led_pattern[i] = 0x0000;
	 matrix_led_pattern[ball_x]=matrix_led_pattern[ball_x]^ pinpon_pattern[ball_y];
       }
      }
    }
    if(restart_flag==TRUE){
       lcd_clear();
       lcd_putstr(0,0,"MODE6:ﾋﾟﾝﾎﾟﾝｹﾞｰﾑ"); /*モード2の初期表示*/
       lcd_putstr(0,1,"center push");
       if((ud->sw)==KEY_SHORT_C){
	 restart_flag=FALSE;

    }
  }
      switch(ud->sw){
      case KEY_LONG_C:   /* 中央キーの長押し */
	ball_y=2;//ボールのx座標とy座標
	ita=3;  //下の部分の左側の座標
	ue_flag=TRUE; //ボールの上下の移動方向
	migi_flag=TRUE; //ボールの左右の移動方向
	count3=0;
	hanekai=0;
	for(i=0;i<8;i++)
	matrix_led_pattern[i]=0x0000;
	reset_flag=FALSE;
	restart_flag=FALSE;
	hajime_flag=FALSE;

	
	ud->mode=MODE_0; /* 次は，モード0に戻る */
	break;
	
      case KEY_SHORT_C:
	if(reset_flag==FALSE)
	  if(restart_flag==FALSE)
	    if(level_flag==FALSE)
	  hajime_flag=TRUE;
	break;

      case KEY_LONG_D:
	if(hajime_flag==FALSE)
	  if(reset_flag==FALSE){
	    restart_flag=FALSE;
	    level_flag=TRUE;
	  }
	break;
      }
}


// char da[]="ｲｰﾄ";

char da[5];
char dat[6];
char ans[10];
//
char x='0', y='0', z='0';
int a=1, b=2, c=3;
int n=0, m=0;
int i=3;
int as=1;
int number;
int count = 0;
//
char astr[4];

void do_mode7(UI_DATA* ud){   //吉武　kazuatege-mu
    int s=sec;
    da[1]='e';
    da[2]='a';
    da[3]='t';
    da[4]='\0';
    dat[1]='b';
    dat[2]='i';
    dat[3]='t';
    dat[4]='e';
    dat[5]='\0';
    ans[0]='C';
    ans[1]='O';
    ans[2]='R';
    ans[3]='R';
    ans[4]='E';
    ans[5]='C';
    ans[6]='T';
    ans[7]='!';
    ans[8]='!';
    ans[9]='\0';
      if(ud->prev_mode!=ud->mode ){ 
    lcd_clear();  //0123456789ABCDEF
    astr[0]=' ';
    astr[1]=' ';
    astr[2]='*';
    astr[3]='\0';
    lcd_putstr(13,0,astr); /*モード2の初期表示*/

    //
    x='0';
    y='0';
    z='0';
    a=1;
    b=2;
    c=3;
    n=0;
    m=0;
    i=3;
    as=1;
    count=0;
    //
    
    /* 3桁の乱数値を生成 */
    srand(s);
    x = (char)(rand()%9)+1;
    do {
      y = (char)(rand()%9)+1;
    } while(y == x);
    do {
      z = (char)(rand()%9)+1;
      } while(z == x||z == y);
    
  }
    
    
    /*for(i=0;i<8;i++){
      astrisk[i]=' ';
    }
    astrisk[ast]='*';*/
    //for(;;){
    if(tmv_flag==TRUE){

    lcd_putdec(13,1,1,a);
    lcd_putdec(14,1,1,b);
    lcd_putdec(15,1,1,c);
    // number=((int)x)*100 + ((int)y)*10 + ((int)z);
    // lcd_putdec(1,0,3,number);

      switch(i){
      case 1:
	switch(ud->sw){  /*モード内でのキー入力別操作*/

	case KEY_SHORT_U: /* 上短押し */
	  /*if(a==9)
	    a=0;
	    else*/
	    a++;
  
	  break;
    
	case KEY_SHORT_D: /* 下短押し */

	  	  if(a==0)
	    a=9;
	  else
	    a--;

	  break;

	case KEY_SHORT_L: /* 左短押し */
	  break;
	case KEY_SHORT_R: /* 右短押し */
	  i=2;
	  astr[0]=' ';
	  astr[1]='*';
	  lcd_putstr(13,0,astr); /*モード2の初期表示*/
	  break;
	case KEY_LONG_C:   /* 中央キーの長押し */
	  ud->mode=MODE_0; /* 次は，モード0に戻る */
	  break;
	case KEY_SHORT_C:
	  count++;
	  /* 「イート」の数 */
	  n = 0;
	  if (a == x) n++;
	  if (b == y) n++;
	  if (c == z) n++;
	  da[0]='0'+n;
	  /* 「バイト」の数 */
	  m = 0;
	  if (a == y || a == z) m++;
	  if (b == x || b == z) m++;
	  if (c == x || c == y) m++;
	  dat[0]='0'+m;
	  if (n != 3){
	    lcd_putstr(0,1,da);
	    lcd_putstr(7,1,dat);
	  }
	  else{
	    lcd_clear();
	    lcd_putstr(0,1,ans);
	    lcd_putdec(1,0,3,count);
	    lcd_putstr(4,0,"ｶｲ");
	  }
	  break;
	}/* /switch */
	break;

      case 2:
	switch(ud->sw){  /*モード内でのキー入力別操作*/

	case KEY_SHORT_U: /* 上短押し */
	  /*	  if(b==9)
	    b=0;
	  else*/
	    b++;
	  break;
    
	case KEY_SHORT_D: /* 下短押し */
	 
	  if(b==0)
	    b=9;
	  else
	    b--;
	  break;

	case KEY_SHORT_L: /* 左短押し */
	  i=1;
	  astr[1]=' ';
	  astr[0]='*';
	  lcd_putstr(13,0,astr); /*モード2の初期表示*/
	  break;
	case KEY_SHORT_R: /* 右短押し */
	  i=3;
	  astr[1]=' ';
	  astr[2]='*';
	  lcd_putstr(13,0,astr); /*モード2の初期表示*/
	  break;
	case KEY_LONG_C:   /* 中央キーの長押し */
	  ud->mode=MODE_0; /* 次は，モード0に戻る */
	  break;
	case KEY_SHORT_C:
	  count++;
	  /* 「イート」の数 */
	  n = 0;
	  if (a == x) n++;
	  if (b == y) n++;
	  if (c == z) n++;
	  da[0]='0'+n;
	  /* 「バイト」の数 */
	  m = 0;
	  if (a == y || a == z) m++;
	  if (b == x || b == z) m++;
	  if (c == x || c == y) m++;
	  dat[0]='0'+m;
	  if (n != 3){
	    lcd_putstr(0,1,da);
	    lcd_putstr(7,1,dat);
	  }
	  else{
	    lcd_clear();
	    lcd_putstr(0,1,ans);
	    lcd_putdec(1,0,3,count);
	    lcd_putstr(4,0,"ｶｲ");
	    //	    lcd_putstr(1,0,count+"ｶｲ");
	  }
	  break;
	}/* /switch */
	break;

      case 3:
	switch(ud->sw){  /*モード内でのキー入力別操作*/

	case KEY_SHORT_U: /* 上短押し */
	  /* if(c==9)
	    c=0;
	    else*/
	    c++;
	  break;
    
	case KEY_SHORT_D: /* 下短押し */
	  if(c==0)
	    c=9;
	  else
	    c--;
	  break;

	case KEY_SHORT_L: /* 左短押し */
	  i=2;
	  astr[2]=' ';
	  astr[1]='*';
	  lcd_putstr(13,0,astr); /*モード2の初期表示*/
	  break;
	case KEY_SHORT_R: /* 右短押し */
	  //	  astr[2]=' ';
	  //	  astr[0]='*';
	  //lcd_putstr(13,0,astr); /*モード2の初期表示*/
	  break;
	case KEY_LONG_C:   /* 中央キーの長押し */
	  ud->mode=MODE_0; /* 次は，モード0に戻る */
	  break;
	case KEY_SHORT_C:
	  count++;
	  /* 「イート」の数 */
	  n = 0;
	  if (a == x) n++;
	  if (b == y) n++;
	  if (c == z) n++;
	  da[0]='0'+n;
	  /* 「バイト」の数 */
	  m = 0;
	  if (a == y || a == z) m++;
	  if (b == x || b == z) m++;
	  if (c == x || c == y) m++;
	  dat[0]='0'+m;
	  if (n != 3){
	    lcd_putstr(0,1,da);
	    lcd_putstr(7,1,dat);
	  }
	  else{
	    lcd_clear();
	    lcd_putstr(0,1,ans);
	    lcd_putdec(1,0,3,count);
	    lcd_putstr(4,0,"ｶｲ");
	    //	    lcd_putstr(1,0,count+"ｶｲ");
	  }
	  break;
	}/* /switch */
	break;
      }
       
    
    }
    /*  if(KEY_SHORT_U) 
      if (n < 12||n > 987||a == b||a == c||b == c)
	continue;*/

	 
        
    //return 0;
}


int main(void){
         UI_DATA* ui_data=NULL;
	unsigned char sw=KEY_NONE;

	//	char test[]="hoge";

	DI();           /* 念のために速やかにDI() */ 
	io_init();      /* I/Oポートの初期化ルーチン(matrix,lcd周りも含む) */
	lcd_init();     /* LCDの初期化ルーチンを実行 */

	/*LCDの初期状態は，カーソルOFFで,ブリンクもOFFと仮にしておく*/
	lcd_cursor(OFF);
	lcd_blink(OFF); 

	/*スピーカ用のテンポ計算(とりあえずの初期値を設定)*/
	{
	  int tempo=120;
	  tempo_compare = 3735 / tempo; /* 1000/(tempo * 16 / 60) を展開 */
	}

	timer_init();   /* タイマの初期設定を実行(EIの直前に実行) */
        EI();	        /* 割り込みを許可  */

	//半角カタカナ一覧:入力できないなら，ここからコピペせよ。
	//ｱｲｳｴｵ ｶｷｸｹｺ ｻｼｽｾｿ ﾀﾁﾂﾃﾄ ﾅﾆﾇﾈﾉ ﾊﾋﾌﾍﾎ ﾏﾐﾑﾒﾓ ﾗﾘﾙﾚﾛ ﾔﾕﾖ ﾜｦﾝ ﾞﾟ ｯ ｧｨｩｪｫ
	
	//最初は，モード0から実行を想定。
	lcd_putstr(0,0,"MODE0->MODE0");

	for(;;){	  /* 組み込みシステムは，基本的には無限ループで実行 */


	  /* 以下のif文の中は，多分，1/32秒ごとに処理を行う*/
	  if(tma_flag == TRUE){ 
	    sw = sw_mng();       /* スイッチの入力チェック libs/key.c */
	    ui_data=ui(sw);           /* ユーザインタフェースの実行 */

	    if(ui_data->mode==MODE_1){
	      /* MODE1だったら，キー入力のデバッグアウトのデバッグ出力 */
	      key_debug_out_sample(); 
	    }

	    tma_flag=FALSE;
	  }

	  /* 以下の処理は，1msごとに行う */
	  if(tmv_flag==TRUE){

#if 0
	    /*スピーカのダイレクトコントロールは，beta2から*/
	    /*できないようになりました。*/
	    /*スピーカ用のポートは，タイマWというタイマから*/
	    /*タイマ出力を直接出力することになります*/
	    /*スピーカ用の関数 snd_mng(),snd_play(),snd_stop()などを*/
	    /*活用すること*/

	    /*とりあえず，モード1の時に，上のキーが短押しされたら，   */
	    /*(正確には，短押しの離された時に)，500Hzの音を短く出す   */
	    /*ようなスピーカのサンプルコード。                        */
	    /*スマートにスピーカを鳴らすのだったら，どこかで， フラグ */
	    /*が立ったら， FLIP_SPEAKER_DIRECT_CONTROL();を実行する   */
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


	  SLEEP(); /*割込が入るまで，スリープさせる(消費電力低減!)*/
	}
	return 0;  /*この行は実行されないが、警告を出さないおまじない*/
}


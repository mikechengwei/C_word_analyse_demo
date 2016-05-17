#include <string.h>  
#include <stdio.h>  
#include <stdlib.h> 

int poolsize;// default size of text/data/stack
char *content;
enum{
  VR=1,//变量
  Eq=2,
  SQ=3,//十六进制
  DS=4,//十进制
  EG=5,//八进制
  Assign=6//赋值

};
enum{
  JIA=7,
  JIANG=8,
  CHU=9,
  CHENG=10,
  JJ=11,//++
  JIJI=12,//--
};
enum{
  DD=13,//大于等于 
  ZY=14,//左移
  DY=15,//大于
  XY=16,//小于
  XD=17,//小于等于
  YY=18,//右移
  BDY=19,//不等于
  F=20,//非
  Y=21,//与
  And=22,
  Or=23,
  Lor=24,

};
enum{
  Xor=25,//^
  Mod=26,//%
  Mul=27,//*
  Brak=28,//[
  Cond=29//?

};
enum{
   KEY=30,//关键字
};
struct Token
{
  char text[20];
  int state;

};
// 关键词
char *keyword[] = {"auto","double","int","struct","break","else","long","switch","case","enum","register",
        "typedef","char","extern","return","union","const","float","short","unsigned","continue","for",
         "signed","void","default","goto","sizeof","volatile","do","while","static","if"};

bool search(char word[]){ //做关键字的判断
   // printf("%s\n", word);
   for (int i = 0; i < sizeof(keyword)/sizeof(char *); ++i)
   {
     if(strcmp(keyword[i], word) == 0){

          return 1;
     }
   }
   return 0;

} 
//主程序
int  
main(int argc, char **argv){ 
   
    poolsize = 256 ; // arbitrary size
    //struct Token tokenList[256];
    struct Token tokenList[256];
    memset(tokenList,0,poolsize*sizeof(struct Token));
    FILE *fp=fopen("/Users/chengwei/Desktop/c/code.c","r");
    if(fp==NULL)
    {
      printf("can't open file\n");
      return -1;
    }
    char c,old_c;
    int i=0;
    int j=0;
    while ((c=fgetc(fp))!=EOF) //EOF代表文件结尾的意思，底部实现返回-1
    {  
      if(c==' '||c=='\n'){
              if(j!=0){
                i++;
                j=0;
              }

      }else{
        long old_pos;//记住上一个文件读取位置

        if(j==0){ //第一个单词 这里的判断可以省略，不过为了以防万一。
          
           if(c=='#'){ //我们直接忽略头文件
                while((c=fgetc(fp))!=EOF){
                  if(c!=0&&c!='\n'){

                  }else{
                    break;
                  }
                }
                continue;
           }
           //进入标识符的条件
          if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')){
               tokenList[i].text[j]=c;
               //printf("%c\n",c );
               j++;
               
               while((c=fgetc(fp))!=EOF){

                 if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_')){
                    //printf("%c\n",c );
                    tokenList[i].text[j]=c;
                    j++;
                 }else{                    
                    break;
                 }
               }
               if(search(tokenList[i].text)==1){
                  tokenList[i].state=KEY;
               }else{
                  tokenList[i].state=VR;
               }
               old_pos=ftell(fp);
               fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
            
               i++;
               j=0;
               
               continue;
          }
          //进入数字的判断
          int c_val;//根据值判断是8进制还是十六进制，还是10进制
         
          if(c >= '0' && c <= '9'){
             old_pos=ftell(fp);
             c_val=c-'0';
             if(c_val>0){//进入十进制
                tokenList[i].text[j]=c;
                j++;
                tokenList[i].state=DS;//十进制的状态
                while((c=fgetc(fp))!=EOF){
                   if(c>='0'&&c<='9'){
                      tokenList[i].text[j]=c;
                      j++;
                   }else{
                      break;
                   }
                }
             }else{
                tokenList[i].text[j]=c;
                j++;
                c=fgetc(fp);//读取下一位,因为十六进制是0x..而八进制是05,所以我们以第二位区别
                if(c=='x'||c=='X'){//进入十六进制
                   tokenList[i].text[j]=c;
                   j++;
                   tokenList[i].state=SQ;
                   while((c=fgetc(fp))!=EOF){
                     if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')){
                       tokenList[i].text[j]=c;
                       j++;
                     }else{
                      break;
                     }
                   }
                }else{//进入八进制
                   if(c >= '0' && c <= '7'){
                     tokenList[i].text[j]=c;
                     j++;
                     tokenList[i].state=EG;
                     while((c=fgetc(fp))!=EOF){
                       if(c >= '0' && c <= '7'){
                         tokenList[i].text[j]=c;
                         j++;
                       }
                       else{
                         break;
                        }
                      }
                   }else{
                       fseek(fp, old_pos, SEEK_SET);
                   }
                   
                }
             }
             i++;
             j=0;
             continue;
          }

          //进入特殊字符的判断
          if(c=='/'){ 
            c=fgetc(fp);
            if(c=='/'){ //进入注释
              while((c=fgetc(fp))!=EOF){
                 if(c!=0&&c!='\n'){
                   continue;
                 }else{
                   break;
                 }
              }

             }else{
                tokenList[i].text[j]='/';
                j++;
                tokenList[i].state=CHU;
                 old_pos=ftell(fp);
                fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             i++;
             j=0;
             continue;
          }
          //逻辑运算符
          if(c=='!'){
             tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
            if(c=='='){
              tokenList[i].state=BDY;
               tokenList[i].text[j]=c;
               j++;
             }else{
              tokenList[i].state=F;
               old_pos=ftell(fp);
               fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             i++;
             continue;
          }
          if(c=='<'){
             tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='='){
              tokenList[i].state=XD;
               tokenList[i].text[j]=c;
               j++;
             }else{
               if(c=='<'){
                tokenList[i].state=ZY;
                 tokenList[i].text[j]=c;
                 j++;
               }else{
                 tokenList[i].state=XY;
                 old_pos=ftell(fp);
                 fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
               }
             }
             i++;
             continue;
          }
          if(c=='>'){
             tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='='){
              tokenList[i].state=DD;
               tokenList[i].text[j]=c;
               j++;
             }else{
               if(c=='>'){
                tokenList[i].state=YY;
                 tokenList[i].text[j]=c;
                 j++;
               }else{
                 tokenList[i].state=DY;
                 old_pos=ftell(fp);
                 fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
               }
             }
             i++;
             continue;
          }
          if(c=='&'){
            tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='&'){
                 tokenList[i].state=Y;
                 tokenList[i].text[j]=c;
                 j++;
             }else{
                  tokenList[i].state=And;
                  old_pos=ftell(fp);
                  fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             i++;
             continue;

          }
          if(c=='|'){
            tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='|'){
                 tokenList[i].state=Lor;
                 tokenList[i].text[j]=c;
                 j++;
             }else{
                  tokenList[i].state=Or;
                  old_pos=ftell(fp);
                  fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             i++;
            continue;
          }
         
          //赋值还是等于
          if(c=='='){
             tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='='){
                tokenList[i].text[j]=c;
                tokenList[i].state=Eq;
                j++;
             }else{
                tokenList[i].state=Assign;
                old_pos=ftell(fp);
                fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             continue;
          }
          //++和加法
          if(c=='+'){
            tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='+'){
               tokenList[i].text[j]=c;
               j++;
               tokenList[i].state=JJ;
             }else{
               tokenList[i].state=Assign;
                old_pos=ftell(fp);
                fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             continue;
          }
          //减法和--
          if(c=='-'){
            tokenList[i].text[j]=c;
             j++;
             c=fgetc(fp);
             if(c=='-'){
               tokenList[i].text[j]=c;
               j++;
               tokenList[i].state=JIANG;
             }else{
               tokenList[i].state=JIJI;
                old_pos=ftell(fp);
                fseek(fp, old_pos-1, SEEK_SET);//回到上一个读取的位置
             }
             continue;
          }
           if(c=='^'){
            tokenList[i].text[j]='^';
            tokenList[i].state=Xor;
            i++;
            continue;
          }
          if(c=='%'){
            tokenList[i].text[j]='%';
            tokenList[i].state=Mod;
            i++;
            continue;
          }
          if(c=='*'){
            tokenList[i].text[j]='*';
            tokenList[i].state=Mul;
            i++;
            continue;
          }
          if(c=='['){
            tokenList[i].text[j]='[';
            tokenList[i].state=Brak;
            i++;
            continue;
          }
          if(c=='?'){
            tokenList[i].text[j]='?';
            tokenList[i].state=Cond;
            i++;
            continue;
          }
          if(c == '~' || c == ';' || c == '{' || c == '}' || c == '(' || c == ')' || c == ']' || c == ',' || c == ':'){
            tokenList[i].text[j]=c;
            tokenList[i].state=0;
            j=0;
            i++;
            continue;
          }         
        }
       
      }    
    } 
    // printf("%d\n",i);
     fclose(fp);
     for (int n = 0; n < i; ++n)
     {
      printf("%s->%d\n", tokenList[n].text,tokenList[n].state);
     }
 }

 
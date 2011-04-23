#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <iconv.h>

#include "wryip.h"

#define QQWRY "QQWry.dat"  
#define REDIRECT_MODE_1 0x01  
#define REDIRECT_MODE_2 0x02  
#define MAXBUF 255  
  
/*unsigned long getValue( 获取文件中指定的16进制串的值，并返回 
  FILE *fp, 指定文件指针 
  unsigned long start, 指定文件偏移量 
  int length) 获取的16进制字符个数/长度 
*/  
unsigned long getValue(FILE *fp, unsigned long start, int length)  
{  
    unsigned long variable=0;  
    long val[length],i;  
  
    fseek(fp,start,SEEK_SET);  
    for(i=0;i<length;i++)  
        {  
            /*过滤高位，一次读取一个字符*/  
            val[i]=fgetc(fp)&0x000000FF;  
        }   
    for(i=length-1;i>=0;i--)  
        {  
            /*因为读取多个16进制字符，叠加*/  
            variable=variable*0x100+val[i];  
        }   
    return variable;  
};  
  
  
/*int getString( 获取文件中指定的字符串，返回字符串长度 
  FILE *fp, 指定文件指针 
  unsigned long start, 指定文件偏移量 
  char **string) 用来存放将读取字符串的字符串空间的首地址 
*/  
static int getString(FILE *fp, unsigned long start, char **string)  
{  
    unsigned long i=0;  
    char val;  
    fseek(fp,start,SEEK_SET);  
    /*读取字符串，直到遇到0x00为止*/  
    do  
        {  
            val=fgetc(fp);  
            /*依次放入用来存储的字符串空间中*/  
            *(*string+i)=val;  
            i++;  
        }while(val!=0x00);  
    /*返回字符串长度*/  
    return i;  
};  
  
  
/*void getAddress( 读取指定IP的国家位置和地域位置 
  FILE *fp, 指定文件指针 
  unsigned long start, 指定IP在索引中的文件偏移量 
  char **country, 用来存放国家位置的字符串空间的首地址 
  char **location) 用来存放地域位置的字符串空间的首地址 
*/  
static void getAddress(FILE *fp, unsigned long start, char **country, char **location)  
{  
    unsigned long redirect_address,counrty_address,location_address;  
    char val;  
          
    start+=4;  
    fseek(fp,start,SEEK_SET);  
    /*读取首地址的值*/  
    val=(fgetc(fp)&0x000000FF);  
  
    if(val==REDIRECT_MODE_1)  
        {  
            /*重定向1类型的*/  
            redirect_address=getValue(fp,start+1,3);  
            fseek(fp,redirect_address,SEEK_SET);  
            /*混合类型，重定向1类型进入后遇到重定向2类型 
              读取重定向后的内容，并设置地域位置的文件偏移量*/  
            if((fgetc(fp)&0x000000FF)==REDIRECT_MODE_2)  
                {  
                    counrty_address=getValue(fp,redirect_address+1,3);  
                    location_address=redirect_address+4;  
                    getString(fp,counrty_address,country);  
                }  
            /*读取重定向1后的内容，并设置地域位置的文件偏移量*/  
            else  
                {  
                    counrty_address=redirect_address;  
                    location_address=redirect_address+getString(fp,counrty_address,country);  
                }  
        }  
    /*重定向2类型的*/  
    else if(val==REDIRECT_MODE_2)  
        {  
            counrty_address=getValue(fp,start+1,3);  
            location_address=start+4;  
            getString(fp,counrty_address,country);  
        }  
    else  
        {  
            counrty_address=start;  
            location_address=counrty_address+getString(fp,counrty_address,country);  
        }  
          
    /*读取地域位置*/  
    fseek(fp,location_address,SEEK_SET);  
    if((fgetc(fp)&0x000000FF)==REDIRECT_MODE_2||(fgetc(fp)&0x000000FF)==REDIRECT_MODE_1)  
        {  
            location_address=getValue(fp,location_address+1,3);  
        }  
    getString(fp,location_address,location);  
  
    return;  
};  
  
  
/*void getHead( 读取索引部分的范围（在文件头中，最先的2个8位16进制） 
  FILE *fp, 指定文件指针 
  unsigned long *start, 文件偏移量，索引的起止位置 
  unsigned long *end) 文件偏移量，索引的结束位置 
*/  
void getHead(FILE *fp,unsigned long *start,unsigned long *end)  
{  
    /*索引的起止位置的文件偏移量，存储在文件头中的前8个16进制中 
      设置偏移量为0，读取4个字符*/  
    *start=getValue(fp,0L,4);  
    /*索引的结束位置的文件偏移量，存储在文件头中的第8个到第15个的16进制中 
      设置偏移量为4个字符，再读取4个字符*/  
    *end=getValue(fp,4L,4);  
};  
  
  
/*unsigned long searchIP( 搜索指定IP在索引区的位置，采用二分查找法； 
  返回IP在索引区域的文件偏移量 
  一条索引记录的结果是，前4个16进制表示起始IP地址 
  后面3个16进制，表示该起始IP在IP信息段中的位置，文件偏移量 
  FILE *fp, 
  unsigned long index_start, 索引起始位置的文件偏移量 
  unsigned long index_end, 索引结束位置的文件偏移量 
  unsigned long ip) 关键字，要索引的IP 
*/  
unsigned long searchIP(FILE *fp, unsigned long index_start,
                       unsigned long index_end, unsigned long ip)  
{  
    unsigned long index_current,index_top,index_bottom;  
    unsigned long record;  
    index_bottom=index_start;  
    index_top=index_end;  
    /*此处的7，是因为一条索引记录的长度是7*/  
    index_current=((index_top-index_bottom)/7/2)*7+index_bottom;  
    /*二分查找法*/  
    do{  
        record=getValue(fp,index_current,4);  
        if(record>ip)  
            {  
                index_top=index_current;  
                index_current=((index_top-index_bottom)/14)*7+index_bottom;  
            }  
        else  
            {  
                index_bottom=index_current;  
                index_current=((index_top-index_bottom)/14)*7+index_bottom;  
            }  
    }while(index_bottom<index_current);  
    /*返回关键字IP在索引区域的文件偏移量*/  
    return index_current;  
};  
  
  
/*unsigned long putAll( 导出所有IP信息到文件文件中，函数返回导出总条数 
  FILE *fp, 
  FILE *out, 导出的文件指针，必须拥有写权限 
  unsigned long index_start, 索引区域的起始文件偏移量 
  unsigned long index_end) 索引区域的结束文件偏移量 
*/  
unsigned long putAll(FILE *fp, FILE *out, unsigned long index_start, unsigned long index_end)  
{  
    unsigned long i,count=0;  
    unsigned long start_ip,end_ip;  
    char *country;  
    char *location;  
          
    country=(char*)malloc(255);  
    location=(char*)malloc(255);  
          
    /*此处的7，是因为一条索引记录的长度是7*/  
    for(i=index_start;i<index_end;i+=7)  
        {  
            /*获取IP段的起始IP和结束IP， 
              起始IP为索引部分的前4位16进制 
              结束IP在IP信息部分的前4位16进制中，靠索引部分指定的偏移量找寻*/  
            start_ip=getValue(fp,i,4);  
            end_ip=getValue(fp,getValue(fp,i+4,3),4);  
            /*导出IP信息，格式是 
              起始IP\t结束IP\t国家位置\t地域位置\n*/  
            fprintf(out,"%d.%d.%d.%d",(start_ip&0xFF000000)>>0x18,\  
  
                    (start_ip&0x00FF0000)>>0x10,(start_ip&0x0000FF00)>>0x8,start_ip&0x000000FF);  
            fprintf(out,"\t");  
            fprintf(out,"%d.%d.%d.%d",(end_ip&0xFF000000)>>0x18, \  
  
                    (end_ip&0x00FF0000)>>0x10,(end_ip&0x0000FF00)>>0x8,end_ip&0x000000FF);  
            getAddress(fp,getValue(fp,i+4,3),&country,&location);  
            fprintf(out,"\t%s\t%s\n",country,location);  
            count++;  
        }  
    /*返回导出总条数*/  
    return count;  
};  
  
  
/*判断一个字符是否为数字字符， 
  如果是，返回0 
  如果不是，返回1*/  
int beNumber(char c)  
{  
    if(c>='0'&&c<='9')  
        return 0;  
    else  
        return 1;  
};  
  
  
/*函数的参数是一个存储着IP地址的字符串首地址 
  返回该IP的16进制代码 
  如果输入的IP地址有错误，函数将返回0*/  
unsigned long getIP(char *ip_addr)  
{  
    unsigned long ip=0;  
    int i,j=0;  
    /*依次读取字符串中的各个字符*/  
    for(i=0;i<strlen(ip_addr);i++)  
        {  
            /*如果是IP地址间隔的‘.’符号 
              把当前读取到的IP字段的值，存入ip变量中 
              （注意，ip为叠加时，乘以16进制的0x100） 
              并清除临时变量的值*/  
            if(*(ip_addr+i)=='.')  
                {  
                    ip=ip*0x100+j;  
                    j=0;  
                }  
            /*往临时变量中写入当前读取到的IP字段中的字符值 
              叠加乘以10，因为输入的IP地址是10进制*/  
            else  
                {  
                    /*判断，如果输入的IP地址不规范，不是10进制字符 
                      函数将返回0*/  
                    if(beNumber(*(ip_addr+i))==0)  
                        j=j*10+*(ip_addr+i)-'0';  
                    else  
                        return 0;  
                }  
        }  
    /*IP字段有4个，但是‘.’只有3个，叠加第四个字段值*/  
    ip=ip*0x100+j;  
    return ip;  
};  
  

//////lib code
int searchIPCountry(const char *ipaddr, char *country)
{
    FILE *fp; /*打开QQWry.dat的文件指针*/  
    unsigned long index_start,index_end,current; /*索引部分的起始位置的文件偏移量 
                                                   索引部分的结束位置的文件偏移量 
                                                   待搜索IP地址的索引条目的文件偏移量*/  
    // char *country; /*国家位置*/  
    char *location; /*地域位置*/  
    // country=(char*)malloc(MAXBUF);  
    location=(char*)malloc(MAXBUF);  

    /*打开QQWry.dat文件*/  
    if((fp=fopen(QQWRY,"rb"))==NULL) {
        printf("[-] Error : Can not open the file %s.\n", QQWRY);  
        showend();  
        return 2;  
    } else {
        printf("[+] Open the file [ %s ] successfully.\n", QQWRY);
    }
  
    /*显示QQWry.dat文件信息*/  
    getHead(fp,&index_start,&index_end);  
    getAddress(fp,getValue(fp,index_end+4,3),&country,&location);  
    printf("[+] Version of QQWry.dat : [ %s %s ]\n",country,location);  
    printf("[+] Index Location [ 0x%X - 0x%X ].\n",index_start,index_end);  


    /*-i参数，搜索IP*/  
    unsigned long ipn;
  
    ipn=getIP(ipaddr);  
    if(ipn==0) {
        printf("[-] Error : the IP Address inputed.\n");  
        showend();  
        return 3;  
    }  
  
    /*搜索IP在索引区域的条目的偏移量*/  
    current = searchIP(fp, index_start, index_end, ipn);
    printf("[+] Address of index for [ %X ] is %X\n", ipn, current);
  
    /*获取该IP对因的国家地址和地域地址*/  
    getAddress(fp, getValue(fp, current+4,3), &country, &location);
    printf("[+] Get the location for the IP address.\n");  
    printf("[+] [ IP Address ] %d.%d.%d.%d\n",(ipn&0xFF000000)>>0x18,
           (ipn&0x00FF0000)>>0x10, (ipn&0x0000FF00)>>0x8, ipn&0x000000FF);  
    printf("[+] [ Location ] %s %s\n", country, location);
    
    free(location);

    return 0;
}

int searchIPProvince(const char *ipaddr, char *province)
{
    return 0;
}

int searchIPCity(const char *ipaddr, char *province, char *city)
{
    return 0;
}


//////////// standalone code  
/*显示logo信息*/  
void logo(void)  
{  
    printf("=============================================================================\n");  
    printf("--- Get the IP info.s from QQWry.dat v0.1 by dorainm dorainm@gmail.com ---\n");  
    printf("=============================================================================\n");  
};  
  
  
/*显示程序语法*/  
void usage(char *app_name)  
{  
    printf("\nUsage : %s [options]\n",app_name);  
    printf("options:\n");  
    printf(" -a <address> Search and display the Informations by Location Address.(*)\n");  
    printf(" -i <IP> Search and display the Informations by IP Address.\n");  
    printf(" -o <FILE> Output all the informations to a text file.\n");  
    printf(" -local Display the localhost IP's informations.(*)\n");  
    printf(" -updata Update the QQWry.dat from the Internet.(*)\n\n");  
    printf("ps: the optionss marked (*) are incompleted.\n");  
};  
  
  
/*显示结束信息*/  
void showend(void)  
{  
    printf("\n\nThe command completed successfully.\n\n");  
};  


#if defined(STANDALONE_WRYIP)
/*主函数*/  
int main(int argc, char *argv[])  
{  
    FILE *fp; /*打开QQWry.dat的文件指针*/  
    unsigned long index_start,index_end,current; /*索引部分的起始位置的文件偏移量 
                                                   索引部分的结束位置的文件偏移量 
                                                   待搜索IP地址的索引条目的文件偏移量*/  
    char *country; /*国家位置*/  
    char *location; /*地域位置*/  
    country=(char*)malloc(MAXBUF);  
    location=(char*)malloc(MAXBUF);  
          
    logo();  
    if(argc<3)  
        {  
            usage(argv[0]);  
            showend();  
            return 1;  
        }   
          
    /*打开QQWry.dat文件*/  
    if((fp=fopen(QQWRY,"rb"))==NULL)  
        {  
            printf("[-] Error : Can not open the file %s.\n",QQWRY);  
            showend();  
            return 2;  
        }  
    else  
        printf("[+] Open the file [ %s ] successfully.\n",QQWRY);  
  
    /*显示QQWry.dat文件信息*/  
    getHead(fp,&index_start,&index_end);  
    getAddress(fp,getValue(fp,index_end+4,3),&country,&location);  
    printf("[+] Version of QQWry.dat : [ %s %s ]\n",country,location);  
    printf("[+] Index Location [ 0x%X - 0x%X ].\n",index_start,index_end);  
          
    /*判断第一个参数的值*/  
    if((strncmp(argv[1],"-i",2)==0)||(strncmp(argv[1],"-I",2)==0))  
        {  
            /*-i参数，搜索IP*/  
            unsigned long ip;  
  
            ip=getIP(argv[2]);  
            if(ip==0)  
                {  
                    printf("[-] Error : the IP Address inputed.\n");  
                    showend();  
                    return 3;  
                }  
  
            /*搜索IP在索引区域的条目的偏移量*/  
            current=searchIP(fp,index_start,index_end,ip);  
            printf("[+] Address of index for [ %X ] is %X\n",ip,current);  
  
            /*获取该IP对因的国家地址和地域地址*/  
            getAddress(fp,getValue(fp,current+4,3),&country,&location);  
            printf("[+] Get the location for the IP address.\n");  
            printf("[+] [ IP Address ] %d.%d.%d.%d\n",(ip&0xFF000000)>>0x18,(ip&0x00FF0000)>>0x10,(ip&0x0000FF00)>>0x8,ip&0x000000FF);  
            printf("[+] [ Location ] %s %s\n",country,location);  
        }  
    else if((strncmp(argv[1],"-o",2)==0)||(strncmp(argv[1],"-O",2)==0))  
        {  
            /*-o参数，解压缩数据库，导出IP信息到文本文件*/  
            FILE *out;  
            unsigned long num;  
            if((out=fopen(argv[2],"w"))==NULL)  
                {  
                    printf("[-] Error create the output text file [ %s ].\n","out.txt");  
                    showend();  
                }  
            else  
                {  
                    printf("[+] Create the output text file [ %s ] successfully.\n","out.txt");  
                }  
  
            /*导出IP条目信息*/  
            printf("[+] Outputing the informations ...");  
            num=putAll(fp,out,index_start,index_end);  
            printf("Finished.\n");  
            fclose(out);  
            /*显示导出条目的数量*/  
            printf("[+] The Total items number is [ %d ].",num);  
        }  
    /*关闭文件指针，释放变量空间，结束程序*/  
    fclose(fp);  
    free(country);  
    free(location);  
    showend();  
    return 0;  
}  
#endif

/*

  =============================================================================
  --- Get the IP info.s from QQWry.dat v0.1   by dorainm  dorainm@gmail.com ---
  =============================================================================

  Usage : showip [options]
  options:
  -a <address>    Search and display the Informations by Location Address.(*)
  -i <IP>         Search and display the Informations by IP Address.
  -o <FILE>       Output all the informations to a text file.
  -local          Display the localhost IP's informations.(*)
  -updata         Update the QQWry.dat from the Internet.(*)

  ps:  the optionss marked (*) are incompleted.


  The command completed successfully.


  showip -i 222.19.211.254

  =============================================================================
  --- Get the IP info.s from QQWry.dat v0.1   by dorainm  dorainm@gmail.com ---
  =============================================================================
  [+]  Open the file [ QQWry.dat ] successfully.
  [+]  Version of QQWry.dat : [ 纯真网络 2006年3月5日IP数据 ]
  [+]  Index Location [ 0x37A265 - 0x535EB1 ].
  [+]  Address of index for [ DE13D3FE ] is 51BB44
  [+]  Get the location for the IP address.
  [+]  [ IP Address ] 222.19.211.254
  [+]  [  Location  ] 云南大学 国家示范性软件学院


  The command completed successfully.



  导出所有IP信息，语法是 showip -o out.txt
*/

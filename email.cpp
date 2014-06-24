#include <stdio.h>
#include <WinSock.h>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
#pragma  comment(lib, "ws2_32.lib")


int g_msg_queue_id = 0;

typedef struct _trap_message
{
    long msg_type;
    unsigned char msg_text[128];
}trap_message;

struct Base64Date6
{
  unsigned int d4:6;
  unsigned int d3:6;
  unsigned int d2:6;
  unsigned int d1:6;
};
// base64
char ConvertToBase64  (char   c6);
void EncodeBase64     (char   *dbuf, char *buf128, int len);
void SendMail         (char   *email,char *body);
int  OpenSocket       (struct sockaddr *addr);

int main()
{
  char EmailTo[]         = "cindypata@126.com";
  char EmailContents[1024];//   = "From: \"cindy\"<cindypata@126.com>\r\n"
                           //"To: \"pata\"<cindypata@126.com>\r\n"
			//			   "Subject: 【提醒】传感器读数超出警报阈值\r\n\r\n"
                        //   "请注意调试^^";
    int fd = -1;
    key_t key;

    fd = open("/home/yuliang/cunix/cindyunix/hw4_6a/msg_queue",O_CREAT,0660);
    if (fd == -1)
    {
        printf("Open msg_queue file error!\n");
        return -1;
    }
    close(fd);

    key = ftok("/home/yuliang/cunix/cindyunix/hw4_6a/msg_queue",'t');
    if (key == -1)
    {
        printf("init_module ftok error\n");
        return -1;
    }

    if((g_msg_queue_id = msgget(key,IPC_CREAT|0660)) == -1)
    {
        printf("msgget failed!\n");
        return -1;
    }

    printf("creat msg queue suceed,id = %d\n",g_msg_queue_id);

    trap_message msg = {0};
    int msg_len = 0;
    while(1)
    {
        memset(&msg,0,sizeof(msg));
        msg_len = msgrcv(g_msg_queue_id, &msg.msg_text, sizeof(msg.msg_text),0,IPC_NOWAIT);
        if(msg_len<0)
        {
            sleep(3);
            continue;
        }
        printf("%s\n",msg.msg_text);
        sscanf(msg.msg_text,"%d[%d]<%d>",nodeid,data,timet);
        memset(&EmailContents,0,sizeof(EmailContents));
        sprintf(EmailContents,"From: \"cindy\"<cindypata@126.com>\r\nTo: \"pata\"<cindypata@126.com>\r\nSubject: 【提醒】传感器%d读数超出警报阈值\r\n\r\n数据为%d请注意调试^^",nodeid,data);
        SendMail(EmailTo, EmailContents);
        sleep(0);
    }
  SendMail(EmailTo, EmailContents);
  getchar();
  return 0;
}

char ConvertToBase64(char uc)
{
  if(uc < 26)
  {
    return 'A' + uc;
  }
  if(uc < 52)
  {
    return 'a' + (uc - 26);
  }
  if(uc < 62)
  {
    return '0' + (uc - 52);
  }
  if(uc == 62)
  {
    return '+';
  }
  return '/';
}

// base64
void EncodeBase64(char *dbuf, char *buf128, int len)
{
  struct Base64Date6 *ddd      = NULL;
  int           i        = 0;
  char          buf[256] = {0};
  char          *tmp     = NULL;
  char          cc       = '\0';

  memset(buf, 0, 256);
  strcpy_s(buf, 256, buf128);
  for(i = 1; i <= len/3; i++)
  {
    tmp             = buf + (i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
    dbuf[(i-1)*4+3] = ConvertToBase64((unsigned int)ddd->d4);
  }
  if(len % 3 == 1)
  {
    tmp             = buf + (i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = '=';
    dbuf[(i-1)*4+3] = '=';
  }
  if(len%3 == 2)
  {
    tmp             = buf+(i-1)*3;
    cc              = tmp[2];
    tmp[2]          = tmp[0];
    tmp[0]          = cc;
    ddd             = (struct Base64Date6 *)tmp;
    dbuf[(i-1)*4+0] = ConvertToBase64((unsigned int)ddd->d1);
    dbuf[(i-1)*4+1] = ConvertToBase64((unsigned int)ddd->d2);
    dbuf[(i-1)*4+2] = ConvertToBase64((unsigned int)ddd->d3);
    dbuf[(i-1)*4+3] = '=';
  }
  return;
}
// Send
void SendMail(char *email, char *body)
{
  int     sockfd     = {0};
  char    buf[1500]  = {0};
  char    rbuf[1500] = {0};
  char    login[128] = {0};
  char    pass[128]  = {0};
  WSADATA WSAData;
  struct sockaddr_in their_addr = {0};
  WSAStartup(MAKEWORD(2, 2), &WSAData);
  memset(&their_addr, 0, sizeof(their_addr));
  
  their_addr.sin_family = AF_INET;
  their_addr.sin_port   = htons(25);
  hostent* hptr         = gethostbyname("smtp.126.com"); 
  memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length); 
  printf("IP of smpt.126.com is : %d:%d:%d:%d\n", 
          their_addr.sin_addr.S_un.S_un_b.s_b1, 
          their_addr.sin_addr.S_un.S_un_b.s_b2, 
          their_addr.sin_addr.S_un.S_un_b.s_b3, 
          their_addr.sin_addr.S_un.S_un_b.s_b4); 

  // connect
  sockfd = OpenSocket((struct sockaddr *)&their_addr);
  memset(rbuf, 0, 1500);
  while(recv(sockfd, rbuf, 1500, 0) == 0)
  {
    cout<<"reconnect..."<<endl;
    Sleep(2);// 2s reconnect
    sockfd = OpenSocket((struct sockaddr *)&their_addr);
    memset(rbuf, 0, 1500);
  }

  cout<<rbuf<<endl;

  // EHLO
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"EHLO REceive: "<<rbuf<<endl;

  // AUTH LOGIN
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Auth Login Receive: "<<rbuf<<endl;

  // USER
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "cindypata@126.com");
  memset(login, 0, 128);
  EncodeBase64(login, buf, strlen(buf));
  sprintf_s(buf, 1500, "%s\r\n", login);
  send(sockfd, buf, strlen(buf), 0);
  cout<<"Base64 UserName: "<<buf<<endl;
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"User Login Receive: "<<rbuf<<endl;

  // PASSWORD
  sprintf_s(buf, 1500, "tongtongzhuang");
  memset(pass, 0, 128);
  EncodeBase64(pass, buf, strlen(buf));
  sprintf_s(buf, 1500, "%s\r\n", pass);
  send(sockfd, buf, strlen(buf), 0);
  cout<<"Base64 Password: "<<buf<<endl;

  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Password Receive: "<<rbuf<<endl;

  // MAIL FROM
  memset(buf, 0, 1500);
  sprintf_s(buf, 1500, "MAIL FROM: <cindypata@126.com>\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"set Mail From Receive: "<<rbuf<<endl;

  // RCPT TO 
  sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Tell Sendto Receive: "<<rbuf<<endl;

  // DATA 准备
  sprintf_s(buf, 1500, "DATA\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Mail Prepare Receive: "<<rbuf<<endl;

  // Send
  sprintf_s(buf, 1500, "%s\r\n.\r\n", body); 
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Send Mail Receive: "<<rbuf<<endl;

  // QUIT
  sprintf_s(buf, 1500, "QUIT\r\n");
  send(sockfd, buf, strlen(buf), 0);
  memset(rbuf, 0, 1500);
  recv(sockfd, rbuf, 1500, 0);
  cout<<"Quit Receive: "<<rbuf<<endl;

  //close
  closesocket(sockfd);
  WSACleanup();
  return;
}
// open TCP Socket
int OpenSocket(struct sockaddr *addr)
{
  int sockfd = 0;
  sockfd=socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    cout<<"Open sockfd(TCP) error!"<<endl;
    exit(-1);
  }
  if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
  {
    cout<<"Connect sockfd(TCP) error!"<<endl;
    exit(-1);
  }
  return sockfd;
} 

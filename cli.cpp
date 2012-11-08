/* This is the client side code to connect to a tic-tac-toe game server.
 * Source Code written by Indradhanush Gupta.
 * Website  :  
 * Github
 * Facebook :
 * Twitter  :
 * Quora    : 
 * E-mail   : indradhanush.gupta@gmail.com
 * 
 * To report any bugs please send me an e-mail.
 * Tips are welcome. 
 *
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "tictac.h"
#define  PORT "9987"

using namespace std;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, port_no, n, connectfd, bytes_sent, bytes_recvd;
	char cbuffer[512], sname[64], cname[64];
	char *ptr = &cbuffer[0];
	char *ptr_port = (char *)&PORT;
	struct sockaddr_in serv_addr;
	struct hostent *he;
	
	int count = 0, inp, x, y, ni, inp_true = 0, toss;
	char serv_choice, cli_choice, nc;
	char choice_buffer[2], co_ordinates_buffer[2], toss_buffer;

	system("clear");

	if (argc != 2)
	{
		perror("Incomplete arguments!");
		return 1;
	}
	port_no = atoi(ptr_port);
	he = gethostbyname(argv[1]);
	if (he == NULL)
	{
		perror("No Such Host!");
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{	
		perror("Sorry. Socket could not be created!");
		return 1;
	}

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr = *((struct in_addr *)he->h_addr);  

	connectfd = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (connectfd == -1)
	{	
		perror("Sorry. Could not connect to server.");
		return 1;
	}
		
	cout<<"Enter your name : ";
	cin>>cname;
	do
	{
		static int flag = 0;
		bytes_sent = send(sockfd, &cname, sizeof(cname), 0);
		if (bytes_sent == -1 && flag == 0)
		{
			cout<<"PLAYER DATA NOT SENT!"<<endl<<"Trying Again...";  
			continue;
		}
		else
		{		cli_choice = 'X';
	    	
			flag = 1;
			memset(&sname, 0, sizeof(sname));
			bytes_recvd = recv(sockfd, &sname, sizeof(sname), 0);
			if (bytes_recvd == -1)
				cout<<"COULD NOT ACQUIRE PLAYER INFORMATION!"<<endl<<"Trying Again..."<<endl; 
			else
				cout<<"You have joined "<<sname<<" for a game of Tic-Tac-Toe."<<endl;
		}
	}while(bytes_sent == -1 || bytes_recvd == -1);
	
	cout<<"Creating game. Please wait..."<<endl;
	sleep(2);
	cout<<endl<<"Game created!"<<endl<<endl<<"Doing a toss...";
	
	bytes_recvd = recv(sockfd, &toss_buffer, sizeof(toss_buffer), 0);
	if (bytes_recvd == -1)
	{
		perror("TOSS BUFFER not received");
		return 1;
	}

	toss = toss_buffer - '0';
	if (toss == 0)		
	{
		cout<<endl<<sname<<" WON the toss."<<endl;
		cout<<sname<<" is choosing. Please wait..."<<endl<<endl;
		memset(&choice_buffer, 0, sizeof(choice_buffer));
		bytes_recvd = recv(sockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_recvd == -1)
		{
			perror("CHOICE BUFFER not received!");
			return 1;
		}
		else
		{
			serv_choice = choice_buffer[0];
			cli_choice = choice_buffer[1];
			cout<<sname<<" has chosen "<<serv_choice<<endl<<endl<<"You will play with "<<cli_choice<<endl;
			cout<<endl<<"Lets Play!"<<endl<<endl;
		}
	}
	else
	{
		cout<<endl<<"You won the TOSS!"<<endl;
		do
    	{
    		cout<<cname<<" Enter Your Choice (X or O): ";
			cin>>cli_choice;
			if (cli_choice == 'X' || cli_choice == 'x')
			{	
	    		serv_choice = 'O';
	    		cli_choice = 'X';
	    		inp_true = 1;
				cout<<endl<<sname<<" gets O."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}
			else if (cli_choice == 'O' || cli_choice == 'o' || cli_choice == '0')
			{
			    serv_choice = 'X';
        		cli_choice = 'O';
	    		inp_true = 1;
	    		cout<<endl<<sname<<" gets X."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}	
			else
			{
	    		cout<<"\nInvalid Choice! Please Choose Again..."<<endl;
	    		inp_true == 0;
			}
    	}while(inp_true == 0);

		memset(&choice_buffer, 0, sizeof(choice_buffer));
		choice_buffer[0] = serv_choice;
		choice_buffer[1] = cli_choice;

		bytes_sent = send(sockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_sent == -1)
		{
			perror("CHOICE BUFFER could not be sent.");
			return 1;
		}

	}

	if (serv_choice == 'X')
	{
		inp = 1;
		cout<<sname<<" will play first."<<endl<<endl;
	
	}
	else
	{
		inp = 2;
		cout<<"You will play first."<<endl<<endl;
	}

	
	init();
	cout<<endl<<"Starting Game..."<<endl;
	sleep(3);
	display();
	
	while (count < 9)
	{
		memset(&co_ordinates_buffer, 0, sizeof(co_ordinates_buffer));
		
		if (inp % 2 != 0 )
		{
			cout<<endl<<sname<<"'s turn. Please wait..."<<endl;
			bytes_recvd = recv(sockfd, &co_ordinates_buffer, sizeof(co_ordinates_buffer), 0);
			if (bytes_recvd == -1)
			{
				perror("CO-ORDINATES BUFFER not recieved!");
				return 1;
			}
			x = co_ordinates_buffer[0] - '0';
			y = co_ordinates_buffer[1] - '0';
			ni = input(serv_choice, x, y);
			if (ni == 0)
			{	
				inp ++;
				cout<<endl<<sname<<" has played. Updating Matrix..."<<endl;
			}
		}
		else 
		{
			cout<<endl<<"Your turn. Enter co-ordinates separated by a space : ";
			cin>>x>>y;
			ni = input(cli_choice, x, y);
			if (ni == 0)
			{
				inp ++;
				sprintf(&co_ordinates_buffer[0], "%d", x);
				sprintf(&co_ordinates_buffer[1], "%d", y);
				cout<<endl<<"Updating Matrix..."<<endl;
				
				bytes_sent = send(sockfd, &co_ordinates_buffer, sizeof(co_ordinates_buffer), 0);
				if (bytes_sent == -1)
				{
					perror("CO-ORDINATES BUFFER could not be sent!");
					return 1;
				}
			}
		}
		
		count ++;
		sleep(2);
		system("clear");
		display();

		if (count >=5)
		{	
			nc = check();
			if (nc == 'f')
				continue;
			else if (serv_choice == nc)
			{	
				cout<<endl<<"You loose."<<endl<<sname<<" has won."<<endl;	
				break;
			}
			else if (cli_choice == nc)
			{
				cout<<endl<<"Congrats! You have won!!!"<<endl<<sname<<" lost."<<endl;
				break;
			}
		}

	}
	
	if (nc == 'f')
		cout<<endl<<"Game ends in a draw."<<endl;

	cout<<endl<<"Thank You for playing Tic-tac-Toe"<<endl;
	close(sockfd);
	return 0;
}
	


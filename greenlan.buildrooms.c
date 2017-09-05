/* Nathan Greenlaw
 * ONID: greenlan
 * CS344
 * Program 2 Build Rooms*/

/* Using the Program Outline, the program will be used to generate rooms for the game */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Create a room struct
struct Room
{
	int id;
	char name[1][20]; 
	char roomType[1][20];
	int numOutboundConnections;
	int choosen;
	struct Room* outboundConnections[6];
};

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
int IsGraphFull(struct Room **r)
{
	int i=0;
	for(i=0; i<7;i++)
	{
		if(r[i]->numOutboundConnections < 3)
		{
			return false;
		} 
	}
	return true;
}

// Returns a random Room, does not validate if a connection can be added
struct Room* GetRandomRoom(struct Room **r)
{
	int rNum = rand()%7;
	return r[rNum];	
}

// Returns true if Rooms x and y are the same Room, false otherwise
int IsSameRoom(struct Room *x, struct Room *y)
{
	if(strcmp(x->name[0],y->name[0]))
	{
		return false;
	}
	else
	{
		return true;
	}
}


// Returns true if a connection can be added from Room x, false otherwise
int CanAddConnectionFrom(struct Room *x, struct Room *y)
{
	if(x->numOutboundConnections < 6)// and connection isnt already there
	{
		int e=0;
		for(e=0; e < x->numOutboundConnections; e++)
		{
			if(IsSameRoom(x->outboundConnections[e],y))
			{
				return false;
			}
		} 

		return true;
	}

	else
	{
		return false;
	}
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room *x, struct Room *y)
{
	// Add connection to room array for both rooms
	x->outboundConnections[x->numOutboundConnections] = y; 
	
	// increments the number of connections a room has
	(x->numOutboundConnections)+=1;
}

// Adds a random, valid outbound connection from a room to another rooom
void AddRandomConnection(struct Room **r)
{
	struct Room* A; 
	struct Room* B;

	while(true)
	{
		A = GetRandomRoom(r);
		
		if(A->numOutboundConnections <6)
		{
			break;
		}
	}
	
	do
	{
		B = GetRandomRoom(r);	
	}while(CanAddConnectionFrom(A,B) == false || IsSameRoom(A,B) == true);

	ConnectRoom(A,B);
	ConnectRoom(B,A);
}

int main()
{
	srand(time(NULL));
	// Create all the rooms and add them to array
	struct Room **rooms = malloc(7*sizeof(struct Room *));
	
	// array containing the names of the rooms for easier assigning
	char r[10][20];
	strcpy(r[0],"Mario");
	strcpy(r[1],"Luigi");
	strcpy(r[2],"Peach");
	strcpy(r[3],"DK");
	strcpy(r[4],"Toad");
	strcpy(r[5],"Yoshi");
	strcpy(r[6],"Bowser");
	strcpy(r[7],"Boo");
	strcpy(r[8],"Wario");
	strcpy(r[9],"Waluigi");
	
	struct Room **totalRooms = malloc(10*sizeof(struct Room *));
	
	int f=0;
	for(f=0; f <10; f++)
	{
		totalRooms[f] = malloc(sizeof(struct Room));
	}

	int i=0;
	for(i=0; i<10; i++)
	{
		strcpy(totalRooms[i]->name[0],r[i]);
		totalRooms[i]->id = i;
		totalRooms[i]->numOutboundConnections = 0; 
		totalRooms[i]->choosen = 0;
		strcpy(totalRooms[i]->roomType[0], "MID_ROOM");
	}

	// Randomly choose 7 from the 10 rooms
	int j=0;
	for(j=0; j<7; j++)
	{
		int num = rand()%10;
		
		while(totalRooms[num]->choosen == 1)
		{
			num = rand()%10;
		}
		totalRooms[num]->choosen = 1;	
		rooms[j] = totalRooms[num];			
	}

	// Pick START_ROOM and END_ROOM
	int n = rand()%7;
	strcpy(rooms[n]->roomType[0], "START_ROOM");

	int m = rand()%7;
	while(!(strcmp(rooms[m]->roomType[0], "START_ROOM")))
	{
		m= rand()%7;
	}
	strcpy(rooms[m]->roomType[0], "END_ROOM");	

	// Create all the connections in graph
	while (IsGraphFull(rooms) == false)
	{
		AddRandomConnection(rooms);
	}

	// Create the directory for the rooms
	char dirName [50];
	int pid = getpid();
	sprintf(dirName,"%s%d","greenlan.rooms.",pid);	

	int directory = mkdir(dirName, 0755);

	// Create the files
	int k=0;
	for(k=0; k<7; k++)
	{
		char fname [50];
		sprintf(fname,"%s/%s",dirName,rooms[k]->name[0]);
		FILE *f = fopen(fname, "w");

		const char *name = rooms[k]->name[0];
		fprintf(f,"ROOM NAME: %s\n",name);
		
		int p=0;
		for (p=0; p<rooms[k]->numOutboundConnections; p++)
		{
			fprintf(f,"CONNECTION %d: %s\n",p, rooms[k]->outboundConnections[p]->name);
		}
		
		char *rt = rooms[k]->roomType;
		fprintf(f,"ROOM TYPE: %s\n",rt); 
		
		fclose(f);		
	}

	//Free memory
	int v=0;
	for(v=0; v<10; v++)
	{
		free(totalRooms[v]);
	}
	free(totalRooms);
	free(rooms);

	return 0;
}




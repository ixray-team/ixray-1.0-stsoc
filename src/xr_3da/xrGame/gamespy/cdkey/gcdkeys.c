/******
gcdkeys.c
GameSpy CDKey SDK Server Code
  
Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com

******

 Please see the GameSpy CDKey SDK documentation for more 
 information

******/

/********
INCLUDES
********/

#include "gcdkeys.h"
#include "../nonport.h"
#include "../available.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#ifdef GUSE_ASSERTS
	#define gassert(a) assert(a)
#else
	#define gassert(a)
#endif

#define BUFSIZE 512

#ifdef __cplusplus
extern "C" {
#endif

/********
DEFINES
********/
#define VAL_PORT 29910
/* #define VAL_ADDR "key.gamespy.com" */
/*#define VAL_ADDR "204.182.161.103"*/
#define VAL_TIMEOUT 2000
#define VAL_RETRIES 2
#define INBUF_LEN 1024
#define MAX_PRODUCTS 4

/********
TYPEDEFS
********/
typedef enum {cs_sentreq, cs_gotok, cs_gotnok, cs_done} gsclientstate_t;

typedef struct gsclient_s
{
	int localid;
	char hkey[33];
	int sesskey;
	int ip;
	unsigned long sttime;
	int ntries;
	gsclientstate_t state;
	void *instance;
	AuthCallBackFn authfn;
	char *errmsg;
	char *reqstr;
	int reqlen;
} gsclient_t;


typedef struct gsnode_s
{
	gsclient_t *client;
	struct gsnode_s *next, *prev;
} gsnode_t;

typedef struct gsproduct_s
{
	int pid;
	gsnode_t clientq;
} gsproduct_t;


/********
GLOBALS
********/
char gcd_hostname[64] = "";

/********
PROTOTYPES
********/
static void send_auth_req(gsproduct_t *prod, gsclient_t *client, char *challenge, char *response);
static void resend_auth_req(gsclient_t *client);
static void send_disconnect_req(gsproduct_t *prod, gsclient_t *client);
static void cdkey_process_buf(char *buf, int len, struct sockaddr *fromaddr);
static void process_oks(char *buf, int isok);
static void process_ison(char *buf, struct sockaddr_in *fromaddr);
static void process_ucount(char *buf, struct sockaddr_in *fromaddr);


static int get_sockaddrin(char *host, int port, struct sockaddr_in *saddr, struct hostent **savehent);
static void xcode_buf(char *buf, int len);
static char *value_for_key(const char *s, const char *key);


static void add_to_queue(gsnode_t *t, gsnode_t *que);
static gsnode_t *remove_from_queue(gsnode_t *t, gsnode_t *que);
static int gcd_init_common(int gameid);
static int init_incoming_socket();
static gsproduct_t *find_product(int gameid);

/********
VARS
********/
static SOCKET sock = INVALID_SOCKET;
static unsigned short localport = 0;
static char enc[9]; /* used for xor encoding */
static struct sockaddr_in valaddr;

static int numproducts = 0;
gsproduct_t products[MAX_PRODUCTS];


/****************************************************************************/
/* PUBLIC FUNCTIONS */
/****************************************************************************/


int gcd_init(int gameid)
{
	int ret;
	const char defaulthost[] =  {'k','e','y','.','g','a','m','e','s','p','y','.','c','o','m','\0'}; //key.gamespy.com

	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return -1;

	if (sock == INVALID_SOCKET) //hasn't been initialized yet
	{
		/* set up the UDP socket */
		SocketStartUp();
		ret = init_incoming_socket();
		if (ret < 0)
			return ret;

		if (gcd_hostname[0] == 0)
			strcpy(gcd_hostname, defaulthost);
		get_sockaddrin(gcd_hostname,VAL_PORT,&valaddr,NULL);
	}


	return gcd_init_common(gameid);

}


#ifdef QR2CDKEY_INTEGRATION
extern struct qr2_implementation_s static_qr2_rec;
int gcd_init_qr2(qr2_t qrec, int gameid)
{
	// check if the backend is available
	if(__GSIACResult != GSIACAvailable)
		return -1;

	if (qrec == NULL)
		qrec = &static_qr2_rec;

	localport = (unsigned short)-1; /* we don't process any incoming data ourselves - it gets passed from the QR SDK */

	sock = qrec->hbsock; 
	qrec->cdkeyprocess = cdkey_process_buf;
	/* grab the outgoing address from the QR SDK */
	memset(&valaddr,0,sizeof(struct sockaddr_in));
	valaddr.sin_family = AF_INET;
	valaddr.sin_port = htons((unsigned short)VAL_PORT);
	valaddr.sin_addr.s_addr = qrec->hbaddr.sin_addr.s_addr;
	return gcd_init_common(gameid);
	
}

#endif


void gcd_shutdown(void)
{
	int i;
	/* Make sure everyone is disconnected */
	for (i = 0 ; i < numproducts ; i++)
		gcd_disconnect_all(products[i].pid);
	if(localport != (unsigned short)-1)
	{
		closesocket(sock);
		SocketShutDown();
	}
	sock = INVALID_SOCKET;
	numproducts = 0;
}

void gcd_authenticate_user(int gameid, int localid, unsigned int userip, char *challenge, 
						   char *response, AuthCallBackFn authfn, void *instance)
{
	gsnode_t *node;
	gsclient_t *client;
	char hkey[33];
	char *errmsg = NULL;
	char badcdkey_t[] = {'B','a','d',' ','C','D',' ','K','e','y','\0'}; //Bad CD Key
	char keyinuse_t[] = {'C','D',' ','K','e','y',' ','i','n',' ','u','s','e','\0'}; //CD Key in use
	gsproduct_t *prod = find_product(gameid);

	gassert(prod);
	if (prod == NULL)
		return;

	 /* get the hashed key */
	strncpy(hkey, response, 32);
	hkey[32] = 0;

	/* if response is bogus, lets kill them */
	if (strlen(response) < 72) 
		errmsg = goastrdup(badcdkey_t);

	/* First, scan the current list for the same, or similar client */
	node = &prod->clientq;
	while ((node = node->next) != NULL)
	{
		/* make sure the localid isn't being reused 
	Change this code if you want to allow multiple users with the same CD Key on the
	same server */
		gassert(node->client->localid != localid); 
		if (strcmp(hkey, node->client->hkey) == 0) 
		{ /* they appear to be on already!! */
			errmsg = goastrdup(keyinuse_t);
			break;
		}
	} 

	/* Create a new client */
	client = (gsclient_t *)gsimalloc(sizeof(gsclient_t));
	gassert(client);
	client->localid = localid;
	client->ip = (int)userip;
	client->instance = instance;
	client->errmsg = NULL;
	client->reqstr = NULL;
	client->authfn = authfn;
	strcpy(client->hkey, hkey);
	node = (gsnode_t *)gsimalloc(sizeof(gsnode_t));
	gassert(node);
	node->client = client;
	add_to_queue(node, &prod->clientq);

	if (errmsg != NULL) 
	{ /* there was already and error, mark them to die */
		client->state = cs_gotnok;
		client->errmsg = errmsg;
	} else 	/* They aren't on this server, lets check the validation server */
		send_auth_req(prod, client,challenge, response);	
}

void gcd_disconnect_user(int gameid, int localid)
{
	gsnode_t *node;
	gsproduct_t *prod = find_product(gameid);

	gassert(prod);
	if (prod == NULL)
		return;

	/* First, scan the list for the client*/
	node = &prod->clientq;
	while ((node = node->next) != NULL)
	{
		if (node->client->localid == localid)
		{
			send_disconnect_req(prod, node->client);
			remove_from_queue(node, &prod->clientq);
			if (node->client->reqstr != NULL)
				gsifree(node->client->reqstr);
			gsifree(node->client);
			gsifree(node);
			return;
		}
	}
	/* No client found -- we should never get here! 
	But we may if you call disconnect_user during an negative authentication 
	(they are already removed) */

}

void gcd_disconnect_all(int gameid)
{
	gsnode_t *node;

	gsproduct_t *prod = find_product(gameid);

	gassert(prod);
	if (prod == NULL)
		return;

	/* Clear the entire list */
	node = &prod->clientq;
	while ((node = node->next) != NULL)
	{
		send_disconnect_req(prod, node->client);
		remove_from_queue(node, &prod->clientq);
		if (node->client->reqstr != NULL)
			gsifree(node->client->reqstr);
		gsifree(node->client);
		gsifree(node);
		node = &prod->clientq;
	}
}

char *gcd_getkeyhash(int gameid, int localid)
{

	gsproduct_t *prod = find_product(gameid);
	gsnode_t *node;

	gassert(prod);
	if (prod == NULL)
		return "";

	node = &prod->clientq;

	/* Scan the list for the client*/
	while ((node = node->next) != NULL)
		if (node->client->localid == localid)
			return node->client->hkey;
	return "";	
}

void gcd_think(void)
{
	static char indata[INBUF_LEN]; 
	struct sockaddr_in saddr;
	int saddrlen = sizeof(saddr);
	fd_set set; 
	struct timeval timeout = {0,0};
	int error;
	int i;
	gsnode_t *node, *oldnode;
	char validated_t[] = {'V','a','l','i','d','a','t','e','d','\0'}; //Validated
	char timeout_t[] = {'V','a','l','i','d','a','t','i','o','n',' ','T','i','m','e','o','u','t','\0'}; //Validation Timeout

	gassert (sock != INVALID_SOCKET);
	/* First, check for data on the socket and process commands */

	if (localport != (unsigned short)-1) /* don't check if we are getting data from the QR SDK instead */
	{
		FD_ZERO ( &set );
		FD_SET ( sock, &set );
		while (1)
		{
			error = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
			if (SOCKET_ERROR == error || 0 == error)
				break;
			/* else we have data */
			error = recvfrom(sock, indata, INBUF_LEN - 1, 0, (struct sockaddr *)&saddr, &saddrlen);
			if (error != SOCKET_ERROR)
			{
				indata[error] = '\0';
				cdkey_process_buf(indata, error, (struct sockaddr *)&saddr);
			}
		}	
	}

	for (i = 0 ; i < numproducts ; i++)
	{		
		/* Next, update the status of any clients and make callbacks */
		node = &products[i].clientq;
		while ((node = node->next) != NULL)
		{
			switch (node->client->state)
			{
			case cs_sentreq:
				if (current_time() < node->client->sttime + VAL_TIMEOUT)
					break; /* keep waiting */
				if (node->client->ntries <= VAL_RETRIES)
				{ /* resend */
					resend_auth_req(node->client);
					break;
				} /* else, go ahead an auth them, the val server timed out */			
			case cs_gotok:
				 /* if authorized or they timed out with no response, just auth them */
					node->client->authfn(products[i].pid, node->client->localid, 1,
						node->client->state == cs_gotok ? validated_t : timeout_t,
						node->client->instance);
					node->client->state = cs_done;
					gsifree(node->client->reqstr);
					node->client->reqstr = NULL;
				break;
			case cs_gotnok:
				/* remove them first, in case the user calls disconnect */
				oldnode = node;
				node = node->prev;
				remove_from_queue(oldnode, &products[i].clientq);
				
				oldnode->client->authfn(products[i].pid, oldnode->client->localid, 0, 
					oldnode->client->errmsg == NULL ? "" : oldnode->client->errmsg,
					oldnode->client->instance);
				gsifree(oldnode->client->reqstr);
				if (oldnode->client->errmsg != NULL)
					gsifree(oldnode->client->errmsg);
				gsifree(oldnode->client);
				gsifree(oldnode);
				break;
			default:
				break;
			}
		} 
	}
}


/****************************************************************************/
/* UTIL FUNCTIONS */
/****************************************************************************/
static void cdkey_process_buf(char *buf, int len, struct sockaddr *fromaddr)
{
	char tok[32];
	char *pos;
	char uok_t[] = {'u','o','k','\0'}; //uok
	char unok_t[] = {'u','n','o','k','\0'}; //unok
	char ison_t[] = {'i','s','o','n','\0'}; //ison
	char ucount_t[] = {'u','c','o','u','n','t','\0'}; //ucount
	xcode_buf(buf, len);
	
	tok[0] = 0;
	if (buf[0] == '\\')
	{
		pos = strchr(buf+1,'\\');
		if (pos && (pos - buf <= 32)) /* right size token */
		{
			strncpy(tok, buf+1,pos-buf-1);
			tok[pos-buf-1] = 0;
		}
	}
	if (!tok[0])
		return; /* bad command */

	if (!strcmp(tok, uok_t))
	{
		process_oks(buf, 1);
	} else if (!strcmp(tok, unok_t))
	{
		process_oks(buf, 0);
	} else if (!strcmp(tok, ison_t))
	{
		process_ison(buf, (struct sockaddr_in *)fromaddr);
	} else if (!strcmp(tok, ucount_t))
	{
		process_ucount(buf, (struct sockaddr_in *)fromaddr);
	} else
		return; /* bad command */
}

static int init_incoming_socket()
{
	int ret;
	struct sockaddr_in saddr;
	int saddrlen;
	
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		return -1;
	get_sockaddrin(NULL,0,&saddr,NULL);
	ret = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
	if (ret == SOCKET_ERROR)
		return -1;
	
	saddrlen = sizeof(saddr);
	ret = getsockname(sock,(struct sockaddr *)&saddr, &saddrlen);
	if (ret == SOCKET_ERROR)
		return -1;
	localport = saddr.sin_port;
	
	return 0;
}

static int gcd_init_common(int gameid)
{
	gsproduct_t *prod;
	gassert(numproducts < MAX_PRODUCTS);
	if (numproducts >= MAX_PRODUCTS)
		return -1; //too many products
	prod = &products[numproducts++];
	prod->pid = gameid;
	prod->clientq.next = NULL;
	prod->clientq.prev = NULL;
	prod->clientq.client = NULL;
	srand((unsigned int)current_time());
	enc[0]='g';enc[1]='a';enc[2]='m';enc[3]='e';
	enc[4]='s';enc[5]='p';enc[6]='y';enc[7]=0;	
	return 0;
}
static gsclient_t *find_client(char *keyhash, int sesskey)
{
	gsnode_t *node;
	int i;

	for (i = 0 ; i < numproducts ; i++)
	{
		node = &products[i].clientq;
		while ((node = node->next) != NULL)
		{
			if (strcmp(keyhash, node->client->hkey) == 0 && (sesskey == -1 || node->client->sesskey == sesskey)) 
				return node->client;
		} 
	}
	return NULL;
}

static void process_oks(char *buf, int isok)
{
	int sesskey;
	char keyhash[33];
	gsclient_t *client;
	const char skey_t[] = {'s','k','e','y','\0'}; //skey
	const char cd_t[] = {'c','d','\0'}; //cd
	const char errmsg_t[] = {'e','r','r','m','s','g','\0'}; //errmsg
	
/* Samples
\uok\\cd\fe6667736f0c8ed7ff5cd9c0e74f\skey\2342
\unok\\cd\fe6667736f0c8ed7ff5cd9c0e74f\skey\23423\errmsg\Already playing on xyz server */
	sesskey = atoi(value_for_key(buf,skey_t));
	strncpy(keyhash,value_for_key(buf,cd_t),32);
	keyhash[32] = 0;
	
	client = find_client(keyhash, sesskey);
	if (!client)
		return;
	if (client->sesskey != sesskey) /* bad session key */
		return;
	if (client->state == cs_done) /* too late to do anything! */
		return;
	if (isok)
		client->state = cs_gotok;
	else
	{
		client->state = cs_gotnok;
		client->errmsg = goastrdup(value_for_key(buf,errmsg_t));
	}
	
}
static void process_ucount(char *buf, struct sockaddr_in *fromaddr)
{
	char outbuf[64];
	char ucountformat[] = {'\\','u','c','o','u','n','t','\\','%','d','\0'}; //\\ucount\\%d
	char pid_t[] = {'p','i','d','\0'}; //pid
	gsnode_t *node;
	int count = 0;
	int len;
	gsproduct_t *prod;
	char *pos = value_for_key(buf, pid_t);
	if (pos[0] == 0 && numproducts > 0) //not present.. use the first product
		prod = &products[0];
	else
		prod = find_product(atoi(pos));
	if (prod != NULL)
	{	
		node = &prod->clientq;
		while ((node = node->next) != NULL)
		{
			count++;
		} 	
	}
	len = sprintf(outbuf, ucountformat, count);
	xcode_buf(outbuf, len);
	sendto(sock, outbuf, len, 0, (struct sockaddr *)fromaddr, sizeof(struct sockaddr_in));
}

static void process_ison(char *buf, struct sockaddr_in *fromaddr)
{
	char outbuf[64];
	int sesskey;
	int len;
	gsclient_t *client;
	const char uonformat[] = {'\\','u','o','n','\\','\\','s','k','e','y','\\','%','d','\0'}; //\\uon\\\\skey\\%d
	const char uoffformat[] = {'\\','u','o','f','f','\\','\\','s','k','e','y','\\','%','d','\0'}; //\\uoff\\\\skey\\%d
	const char skey_t[] = {'s','k','e','y','\0'}; //skey
	const char cd_t[] = {'c','d','\0'}; //cd
/* \ison\\cd\fe6667736f0c8ed7ff5cd9c0e74f\skey\32423 */
/* \uon\\skey\32423 OR \uoff\\skey\32423 */

	sesskey = atoi(value_for_key(buf,skey_t));
	if ( (client = find_client(value_for_key(buf,cd_t), -1)) != NULL 
		&& client->state == cs_done) /* If they are connected, return on */
		len = sprintf(outbuf, uonformat,sesskey);
	else
		len = sprintf(outbuf, uoffformat,sesskey);
	xcode_buf(outbuf, len);
	sendto(sock, outbuf, len, 0, (struct sockaddr *)fromaddr, sizeof(struct sockaddr_in));
}

static void send_disconnect_req(gsproduct_t *prod, gsclient_t *client)
{
	char buf[BUFSIZE];
	int len;
	const char discformat[] = {'\\','d','i','s','c','\\','\\','p','i','d','\\','%','d','\\','c','d','\\','%','s','\\','i','p','\\','%','d','\0'}; //\\disc\\\\pid\\%d\\cd\\%s\\ip\\%d
	
/* \disc\\pid\12\cd\fe6667736f0c8ed7ff5cd9c0e74f\ip\2342342 */
	len = sprintf(buf,discformat,
					prod->pid, client->hkey,client->ip);
	xcode_buf(buf, len);
	sendto(sock, buf, len, 0, (struct sockaddr *)&valaddr, sizeof(valaddr));
}

static void send_auth_req(gsproduct_t *prod, gsclient_t *client, char *challenge, char *response)
{
	char buf[BUFSIZE];
	int len;
	const char authformat[] = {'\\','a','u','t','h','\\','\\','p','i','d','\\','%','d','\\','c','h','\\','%','s','\\','r','e','s','p','\\','%','s','\\','i','p','\\','%','d','\\','s','k','e','y','\\','%','d','\0'}; //\\auth\\\\pid\\%d\\ch\\%s\\resp\\%s\\ip\\%d\\skey\\%d

	client->state = cs_sentreq;
	client->sesskey = (unsigned int)(rand() ^ current_time()) % 16384;
	client->sttime = current_time();
	client->ntries = 1;
/* \auth\\pid\12\ch\efx3232\resp\fe6667736f0c8ed7ff5cd9c0e74f98fd69e4da39560b82f40a628522ed10f0165c1d44a0\ip\2342342\skey\132432 */
	len = snprintf(buf, BUFSIZE, authformat,
			prod->pid, challenge, response, client->ip, client->sesskey);
	buf[BUFSIZE-1] = '\0'; // sometimes snprintf doesn't null terminate
	xcode_buf(buf, len);
	sendto(sock, buf, len, 0, (struct sockaddr *)&valaddr, sizeof(valaddr));
	/* save a copy for resends */
	client->reqstr = (char *)gsimalloc(len);
	memmove(client->reqstr, buf, len);
	client->reqlen = len;
}

static void resend_auth_req(gsclient_t *client)
{
	client->sttime = current_time();
	client->ntries++;		
	sendto(sock, client->reqstr, client->reqlen, 0, (struct sockaddr *)&valaddr, sizeof(valaddr));
}

/* value_for_key: this returns a value for a certain key in s, where s is a string
containing key\value pairs. If the key does not exist, it returns  ""
Note: the value is stored in a common buffer. If you want to keep it, make a copy! */
static char *value_for_key(const char *s, const char *key)
{
	static int valueindex;
	char *pos,*pos2;
	char slash_t[] = {'\\','\0'}; 
	char keyspec[256];
	static char value[2][256];

	valueindex ^= 1;
	strcpy(keyspec, slash_t);
	strcat(keyspec,key);
	strcat(keyspec,slash_t);
	pos = strstr(s,keyspec);
	if (!pos)
		return "";
	pos += strlen(keyspec);
	pos2 = value[valueindex];
	while (*pos && *pos != '\\' && (pos2 - value[valueindex] < 200))
		*pos2++ = *pos++;
	*pos2 = '\0';
	return value[valueindex];
}

/* simple xor encoding */
static void xcode_buf(char *buf, int len)
{
	int i;
	char *pos = enc;

	for (i = 0 ; i < len ; i++)
	{
		buf[i] ^= *pos++;
		if (*pos == 0)
			pos = enc;
	}
}

/* Return a sockaddrin for the given host (numeric or DNS) and port)
Returns the hostent in savehent if it is not NULL */
static int get_sockaddrin(char *host, int port, struct sockaddr_in *saddr, struct hostent **savehent)
{
	struct hostent *hent;
	char broadcast_t[] = {'2','5','5','.','2','5','5','.','2','5','5','.','2','5','5','\0'}; //255.255.255.255

	memset(saddr,0,sizeof(struct sockaddr_in));
	saddr->sin_family = AF_INET;
	saddr->sin_port = htons((unsigned short)port);
	if (host == NULL)
		saddr->sin_addr.s_addr = INADDR_ANY;
	else
		saddr->sin_addr.s_addr = inet_addr(host);
	
	if (saddr->sin_addr.s_addr == INADDR_NONE && strcmp(host,broadcast_t) != 0)
	{
		hent = gethostbyname(host);
		if (!hent)
			return 0;
		saddr->sin_addr.s_addr = *(u_long *)hent->h_addr_list[0];
	}
	if (savehent != NULL)
		*savehent = hent;
	return 1;
} 

static gsproduct_t *find_product(int gameid)
{
	int i;
	for (i = 0 ; i < numproducts ; i++)
		if (products[i].pid == gameid)
			return &products[i];
	return NULL;
}


/***********
Linked List Code
***********/


/*******
add_to_queue
*******/
static void add_to_queue(gsnode_t *t, gsnode_t *que)
{
        while(que->next)
                que=que->next;
        que->next = t;
        t->prev = que;
        t->next = NULL;
}

/*******
remove_from_queue

if NULL is given as first parameter, top list item is popped off

item that is removed is returned, or NULL if not found
*******/
static gsnode_t *remove_from_queue(gsnode_t *t, gsnode_t *que)
{
        
        if(!t) t = que->next;
        if(!t) return(NULL);
        t->prev->next = t->next;
        if(t->next)
                t->next->prev = t->prev;

        return(t);
}

#ifdef __cplusplus
}
#endif


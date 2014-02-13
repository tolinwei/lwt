#ifndef CHAN_H
	#define CHAN_H
#endif


typedef struct clist_head
{
	int id;
	//int snd_blocked;
	void* data;
	lwt_t thd;
	struct clist_head *next;

} clist_head, *clist_t;

typedef struct lwt_channel 
{
	int id;     /* channel's id*/
	/* sender’s data */
	//void *snd_data;
	int snd_cnt; 				/* number of sending threads */
	clist_t snd_thds;			/* list of those threads */

	/* receiver’s data */
	int rcv_blocked; 			/* if the receiver is blocked */
	lwt_t rcv_thd;	 			/* the receiver */
} lwt_channel, *lwt_chan_t;

int n_chan; //channel counter

clist_t tail_snd; 			//point to the end of sender's list


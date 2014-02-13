#include "lwt.h"
#include "channel.h"

void enqueue_snd(lwt_chan_t c, clist_t new)
{
	//printf("Entering enqueue!\n");
	//c->snd_cnt++;
	if(c->snd_thds == NULL) {
		c->snd_thds = new;
		tail_snd = new;
		return;
	} else {
		tail_snd->next = new;
		tail_snd = new;
		return;
	}
}

void dequeue_snd(lwt_chan_t c)
{
	if (c->snd_thds == NULL) {
		return;
	}
	if (c->snd_thds->next == NULL) {
		free(c->snd_thds);
		c->snd_thds = NULL;
		tail_snd = NULL;
		return;
	} else {
		clist_t temp = c->snd_thds;
		c->snd_thds = c->snd_thds->next;
		free(temp);
		return;
	}
}

/* Currently assume that sz is always 1. This function 
creates a channel referenced by the return value. 
The thread that creates the channels is the receiver 
of the channel. This function will use malloc to allocate
the memory for the channel. */

lwt_chan_t lwt_chan(int sz)
{
	LWT_INFO_NCHAN++;
	lwt_chan_t new = (lwt_chan_t)malloc(sizeof(lwt_channel));

	new->snd_cnt = sz;
	new->id = n_chan++;	
	new->snd_thds = NULL;
	new->rcv_blocked = 0;
	new->rcv_thd = NULL;
	return new;
}

/*Deallocate the channel only if no threads still 
have references to the channel. Threads have 
references to the channel if they are either the 
receiver or one of the multiple senders. */

void lwt_chan_deref(lwt_chan_t c)
{
	if (c->snd_thds == NULL && c->rcv_thd->state != _TCB_WAITING) {
		free(c); 	//will also set "rcv_thd" as NULL
		return;
	}
	printd("Cannot derefence the channel.\n");
	return;
}

int lwt_snd(lwt_chan_t c, void *data)
{
	//this channnel has benn lwt_chan_deref() before || data is NULL, illegal
	if (c->rcv_thd == NULL || data == NULL) {
		return -1;
	}
	
	//if receiver is NOT waiting, enqueue and yield
	clist_t new_sender = (clist_t)malloc(sizeof(clist_head));
	new_sender->thd = current_tcb;
	new_sender->data = data;
	new_sender->next = NULL;
	c->snd_cnt++;
	enqueue_snd(c, new_sender);

	//if receiver is waiting, there must be NO sender in the queue
	if (c->rcv_blocked == 1) {
		c->rcv_blocked = 0;
		return 0;
	}

	//block current thread
	while (c->rcv_blocked == 0) {
		lwt_yield(LWT_NULL);
	}
	c->rcv_blocked == 0;
	//receiver start recieves, unblock current sender
	return 0;
}

void *lwt_rcv(lwt_chan_t c)
{
	if (c->rcv_thd == NULL) {
		c->rcv_thd = current_tcb;
	}
	
	//if SENDERS' QUEUE is NOT empty, receive directly
	if (c->snd_thds != NULL) {
		//Important!
		c->rcv_blocked = 1;
		void *temp = c->snd_thds->data;

		c->snd_cnt--;
		dequeue_snd(c);
		return temp;	
	}
	
	c->rcv_blocked = 1;
	while (c->snd_thds == NULL) {
		//printf("Rcv blocking! (because of NO sender in queue)\n");
		lwt_yield(LWT_NULL);
	}
	//somebody sent, so it can receive
	void *temp = c->snd_thds->data;
	c->snd_cnt--;
	dequeue_snd(c);
	return temp;
}

/* This is equivalent to lwt snd except that a channel 
is sent over the channel (sending is sent over c). 
This is how senders are added to a channel: The thread 
that receives channel c is added to the senders of the 
channel. This is used for reference counting to determine 
when to deallocate the channel. */

void lwt_snd_chan(lwt_chan_t c, lwt_chan_t sending)
{

	//this channnel has benn lwt_chan_deref() before || data is NULL, illegal
	if (c->rcv_thd == NULL || sending == NULL) {
		return -1;
	}

	//if receiver is NOT waiting, enqueue and yield
	clist_t new_sender = (clist_t)malloc(sizeof(clist_head));
	new_sender->thd = current_tcb;
	new_sender->data = sending; ////////////////////
	new_sender->next = NULL;
	sending->rcv_thd = new_sender->thd; //add sender to the new channel's receiver
	c->snd_cnt++;
	enqueue_snd(c, new_sender);

	//if receiver is waiting, there must be NO sender in the queue
	if (c->rcv_blocked == 1) {
		c->rcv_blocked = 0;
		return 0;
	}

	//block current thread
	while (c->rcv_blocked == 0) {
		lwt_yield(LWT_NULL);
	}
	//receiver start recieves, unblock current sender
	return 0;

}

/* Same as for lwt rcv except a channel is sent over 
the channel. See lwt snd chan for an explanation. */

lwt_chan_t lwt_rcv_chan(lwt_chan_t c)
{
	if (c->rcv_thd == NULL) {
		c->rcv_thd = current_tcb;
	}
	
	//if SENDERS' QUEUE is NOT empty, receive directly
	if (c->snd_thds != NULL) {
		//wake up the sender, by changing the state of rcv_thd
		c->rcv_blocked = 1;
		lwt_chan_t temp = c->snd_thds->data;  ////////////////////
		c->snd_cnt--;
		dequeue_snd(c);
		return temp;	
	}

	c->rcv_blocked = 1;
	while (c->snd_thds == NULL) {
		lwt_yield(LWT_NULL);
	}

	//somebody sent, so it can receive
	lwt_chan_t temp = c->snd_thds->data;
	c->snd_cnt--;
	dequeue_snd(c);

	return temp;
}


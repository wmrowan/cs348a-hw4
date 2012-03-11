/*
 * NAME: queue.h
 */

extern queuesType     *quNew();
extern bool            quGetTop ();
extern void            quDispose ();
extern void            quInsertNode ();
extern void            quDeleteNode ();
extern queuesType     *pqStyleQuNew();

extern poolType       *queue ();

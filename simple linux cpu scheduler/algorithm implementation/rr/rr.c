#include <linux/module.h>
#include "../sched.h"

/*
 * HCPARK: my scheduling class: ONE_PROCESS_SCHED
 *
 */
#define MY_DEF_SLICE (125)

extern int mysched_used;
struct task_struct *task;
extern void (*set_class_my) (struct task_struct *p);
extern void resched_curr(struct rq* rq);
struct list_head *my_list;
int list_num = 0;
static void
check_preempt_curr_fifo(struct rq *rq, struct task_struct *p, int flags)
{
	/* we're never preempted */
	printk(KERN_INFO "MYMOD: check_preempt_curr_fifo CALLED\n");
}

static struct task_struct *
pick_next_task_fifo(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	//carefully use printk() in this function !!	
	//printk(KERN_DEBUG "MYMOD: pick_next_task_fifo CALLED\n");
	if(list_num < 1) return NULL;
  	struct sched_rt_entity *_next = NULL;
  	struct task_struct *p;
  	put_prev_task(rq, prev);
   	_next = list_entry(my_list->next, struct sched_rt_entity, run_list);
   	p = container_of(_next, struct task_struct, rt);
   	p->se.exec_start = rq_clock_task(rq);
   	//printk(KERN_DEBUG "MYMOD: pick_next_task_fifo return task p\n");
   	return p;
}

static void
enqueue_task_fifo(struct rq *rq, struct task_struct *p, int flags)
{
   struct sched_rt_entity *rt_entity = &p->rt;
   printk(KERN_INFO "MYMOD: enqueue_task_fifo CALLED task = %p\n", p);
   list_add_tail(&rt_entity->run_list, my_list);
	list_num++;
   add_nr_running(rq, 1);
   task = p;
}

static void
dequeue_task_fifo(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_INFO "MYMOD: dequeue_task_fifo CALLED\n");
	sub_nr_running(rq, 1);
	list_del_init(&p->rt.run_list);
	list_num--;
	task = NULL;
}

static void yield_task_fifo(struct rq *rq)
{
	printk(KERN_INFO "MYMOD: yield_task_fifo CALLED\n");
	BUG();
}

static void put_prev_task_fifo(struct rq *rq, struct task_struct *prev)
{
	//list_del_init(&prev->rt.run_list);
	//list_add_tail(&prev->rt.run_list, my_list);
}

static void task_tick_fifo(struct rq *rq, struct task_struct *curr, int queued)
{
	//do not use printk() in this function !!	
	//printk(KERN_DEBUG "MYMOD: task_tick_fifo CALLED\n");
	
	if(--curr->rt.time_slice)
		return;
	
	curr->rt.time_slice = MY_DEF_SLICE;
	
	list_del_init(&curr->rt.run_list);
	list_add_tail(&curr->rt.run_list, my_list);
	resched_curr(rq);
}

static void set_curr_task_fifo(struct rq *rq)
{
	printk(KERN_INFO "MYMOD: set_curr_task_fifo CALLED\n");
}

static void switched_to_fifo(struct rq *rq, struct task_struct *new)
{
	printk(KERN_INFO "MYMOD: switched_to_fifo CALLED new = %p\n", new);
}

static void
prio_changed_fifo(struct rq *rq, struct task_struct *p, int oldprio)
{
	printk(KERN_INFO "MYMOD: prio_changed_fifo CALLED\n");
}

static unsigned int
get_rr_interval_fifo(struct rq *rq, struct task_struct *task)
{
	printk(KERN_INFO "MYMOD: get_rr_interval_fifo CALLED\n");
	return 0;
}

static void update_curr_fifo(struct rq *rq)
{
}

const struct sched_class my_sched_class = {
	.next			= &idle_sched_class,

	.enqueue_task		= enqueue_task_fifo,
	.dequeue_task		= dequeue_task_fifo,
	.yield_task		= yield_task_fifo,

	.check_preempt_curr	= check_preempt_curr_fifo,

	.pick_next_task		= pick_next_task_fifo,
	.put_prev_task		= put_prev_task_fifo,


	.set_curr_task          = set_curr_task_fifo,
	.task_tick		= task_tick_fifo,

	.get_rr_interval	= get_rr_interval_fifo,

	.prio_changed		= prio_changed_fifo,
	.switched_to		= switched_to_fifo,
	.update_curr		= update_curr_fifo,
};

void myclass (struct task_struct *p) {
	printk(KERN_INFO "MYMOD: myclass CALLED\n");
	p->sched_class = &my_sched_class;
}

static int __init init_mysched(void)
{
	const struct sched_class *class;	
	my_list = kmalloc(sizeof(struct list_head), GFP_KERNEL);
	INIT_LIST_HEAD(my_list);
	mysched_used = 1;
	fair_sched_class.next = &my_sched_class;
	task = NULL;
	set_class_my = &myclass;
	for_each_class(class)
		printk(KERN_INFO "INIT_MOD: class = %p\n", class);
	return 0;
	
}

static void __exit exit_mysched(void)
{
	const struct sched_class *class;
	mysched_used = 0;
	task = NULL;
	fair_sched_class.next = &idle_sched_class;
	for_each_class(class)
		printk(KERN_INFO "EXIT_MOD: class = %p\n", class);

	return;
}

MODULE_AUTHOR("Lee YoungHun");
MODULE_DESCRIPTION("My_Scheduler");
MODULE_LICENSE("GPL");

module_init(init_mysched)
module_exit(exit_mysched)



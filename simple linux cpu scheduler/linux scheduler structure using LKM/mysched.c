#include <linux/module.h>
#include "../sched.h"

/*
 * HCPARK: my scheduling class: ONE_PROCESS_SCHED
 *
 */


extern int mysched_used;
struct task_struct *task;
extern void (*set_class_my) (struct task_struct *p);

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

	if(task!=NULL) {
		put_prev_task(rq, prev);
		//printk(KERN_DEBUG "MYMOD: pick_next_task_fifo return task p\n");
		return task;
	}
		
	return NULL;
}

static void
enqueue_task_fifo(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_INFO "MYMOD: enqueue_task_fifo CALLED task = %p\n", p);
	add_nr_running(rq, 1);

	task = p;
}

static void
dequeue_task_fifo(struct rq *rq, struct task_struct *p, int flags)
{
	printk(KERN_INFO "MYMOD: dequeue_task_fifo CALLED\n");
	sub_nr_running(rq, 1);

	task = NULL;
}

static void yield_task_fifo(struct rq *rq)
{
	printk(KERN_INFO "MYMOD: yield_task_fifo CALLED\n");
	BUG();
}

static void put_prev_task_fifo(struct rq *rq, struct task_struct *prev)
{

}

static void task_tick_fifo(struct rq *rq, struct task_struct *curr, int queued)
{
	//do not use printk() in this function !!	
	//printk(KERN_DEBUG "MYMOD: task_tick_fifo CALLED\n");
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



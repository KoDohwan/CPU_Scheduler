#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#define Non 1
#define Pre 2
#define Rr 3
#define Arrival 1
#define CPU 2
#define Priority 3

int ready_count = 0, waiting_count = 0, process_count = 0;

typedef struct node {
	struct node *pre;
	struct node *next;
	int process_ID;
	int CPU_burst_time;
	int IO_burst_time;
	int arrival_time;
	int priority;
	int flag;
	int waiting_time;
	int turnaround_time;
	int start;
	int IO_execution_time;
	int RR_execution_time;
	int RR_flag;
}list;

void node_initialize(list *head) {
	head->pre = NULL;
	head->next = NULL;
	head->process_ID = -1;
	head->CPU_burst_time = -1;
	head->IO_burst_time = -1;
	head->arrival_time = -1;
	head->priority = -1;
	head->start = -1;
	head->IO_execution_time = -1;
	head->RR_execution_time = -1;
	head->RR_flag = -1;
}

list *create_process(list **head) {
	list *temp = *head;
	list *new_node = (list *)malloc(sizeof(list));
	node_initialize(new_node);

	int flag = rand() % 10 + 1;

	process_count++;
	new_node->process_ID = process_count;
	new_node->CPU_burst_time = rand() % 9 + 1;
	new_node->IO_burst_time = (flag <= 4 && new_node->CPU_burst_time >= 2) ? rand() % 9 + 1 : 0;
	new_node->arrival_time = (ready_count == 0) ? 0 : rand() % 19 + 1;
	new_node->priority = rand() % 9 + 1;
	new_node->flag = 0;
	new_node->waiting_time = 0;
	new_node->turnaround_time = 0;
	new_node->start = (new_node->IO_burst_time != 0) ? rand() % (new_node->CPU_burst_time - 1) + 1 : 0;
	new_node->IO_execution_time = 0;
	new_node->RR_execution_time = 0;
	new_node->RR_flag = 0;

	if (*head == NULL) 
		*head = new_node;
	else {
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new_node;
		new_node->pre = temp;
	}
	return new_node;
}

void insert_node(list **head, list *node) {
	list *temp = *head;
	list *new_node = (list *)malloc(sizeof(list));
	node_initialize(new_node);

	new_node->process_ID = node->process_ID;
	new_node->arrival_time = node->arrival_time;
	new_node->CPU_burst_time = node->CPU_burst_time;
	new_node->IO_burst_time = node->IO_burst_time;
	new_node->priority = node->priority;
	new_node->flag = 0;
	new_node->waiting_time = 0;
	new_node->turnaround_time = 0;
	new_node->start = node->start;
	new_node->IO_execution_time = node->IO_execution_time;
	new_node->RR_execution_time = node->RR_execution_time;
	new_node->RR_flag = node->RR_flag;

	if (*head == NULL)
		*head = new_node;
	else {
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new_node;
		new_node->pre = temp;
	}

}

void delete_node(list **head, list *node) {
	if (node->next == NULL && node->pre == NULL)
		*head = NULL;
	else if (node->next == NULL)
		node->pre->next = NULL;
	else if (node->pre == NULL) {
		*head = node->next;
		node->next->pre = NULL;
	}
	else {
		node->pre->next = node->next;
		node->next->pre = node->pre;
	}
}

void print_menu() {
	printf("1. Create Process\n");
	printf("2. Current Processes\n");
	printf("3. Schedule\n");
	printf("4. Restart\n");
	printf("5. Exit\n");
	printf("Select Number : ");
}

void schedule_menu() {
	printf("1. FCFS\n");
	printf("2. Nonpreemptive SJF\n");
	printf("3. Preemptive SJF\n");
	printf("4. Nonpreemptive Priority\n");
	printf("5. Premeptive Priority\n");
	printf("6. RR(Time Quantum = 4)\n");
	printf("7. Exit\n");
	printf("Select Number : ");
}

void print_empty() {
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("*                                    EMPTY                                    *\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("\n");
}

void print_ready_queue(list *head) {
	printf("Ready Queue\n");
	if (ready_count == 0) {
		print_empty();
		return;
	}

	list *temp = head;
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("*  Process_ID *  CPU_burst_time *  I/O_burst_time *  Arrival_time *  Priority *\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	while (temp != NULL) {
		if (temp->process_ID <= 9) printf("*          P%d *", temp->process_ID);
		else printf("*         P%d *", temp->process_ID);
		printf("%16d *%16d *%14d *%10d *\n",temp->CPU_burst_time, temp->IO_burst_time, temp->arrival_time, temp->priority);
		temp = temp->next;
	}
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("\n");
}

void print_waiting_queue(list *head) {
	printf("Waiting Queue\n");
	if (waiting_count == 0) {
		print_empty();
		return;
	}

	list *temp = head;
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("*   Process_ID  *  Remain_CPU_burst_time  * I/0_finish_time *\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	while (temp != NULL) {
		if (temp->process_ID <= 9) printf("*           P%d  *", temp->process_ID);
		else printf("*          P%d  *", temp->process_ID);
		printf("%24d *%16d *\n", temp->CPU_burst_time, temp->arrival_time);
		temp = temp->next;
	}
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	printf("\n");
}

void print_evaluation(list *head, list *gantt_chart) {
	list *temp = head;
	int sum = 0;
	printf("Average Waiting Time : ");
	while (temp != NULL) {
		if (temp->process_ID != 0) {
			sum += temp->waiting_time;
			/*if (temp->next != NULL)
				printf("%d + ", temp->waiting_time);
			else
				printf("%d ) / %d = %.2fns\n", temp->waiting_time, ready_count, (double)sum / (double)ready_count);*/
		}
		if (temp->next == NULL)
			printf("%.2fns\n", (double)sum / (double)ready_count);
		temp = temp->next;
	}
	
	temp = head;
	sum = 0;
	printf("Average Turnaround Time : ");
	while (temp != NULL) {
		if (temp->process_ID != 0) {
			sum += temp->turnaround_time;
			/*if (temp->next != NULL)
				printf("%d + ", temp->turnaround_time);
			else
				printf("%d ) / %d = %.2fns\n", temp->turnaround_time, ready_count, (double)sum / (double)ready_count);*/
		}
		if (temp->next == NULL)
			printf("%.2fns\n", (double)sum / (double)ready_count);
		temp = temp->next;
	}
	
	sum = 0;
	int sum2 = 0;
	temp = head;
	list *temp2 = gantt_chart;
	while (temp != NULL) {
		sum += temp->CPU_burst_time;
		temp = temp->next;
	}
	while (temp2 != NULL) {
		sum2 += temp2->CPU_burst_time;
		temp2 = temp2->next;
	}
	printf("CPU Utilization : (%d / %d) * 100 = %.2f%%\n\n", sum, sum2, (double)sum / (double)sum2 * 100);
}

void gotoxy(int x, int y) {
	printf("\033[%d;%df", y, x);
	fflush(stdout);
}

void print_chart(list *head) {
	list *temp = head;
	int time = 0;
	int start;
	int end;

	while (temp != NULL) {
		
		start = time;
		time += temp->CPU_burst_time;
		list *temp2 = temp;
		
		while (temp2->next != NULL) {
			if (temp2->process_ID == temp2->next->process_ID) {
				time += temp2->next->CPU_burst_time;
				temp2 = temp2->next;
				}
			else break;
		}

		end = time;
		printf("%dns ~ %dns : ", start, end);
		if (temp->process_ID != 0) printf("CPU is processing P%d\n", temp->process_ID);
		else printf("CPU is Idle\n");

		temp = temp2->next;
	}
	printf("\n");
}

list *find_min(list *head, int type) {
	list *temp = head;
	list *result = NULL;
	
	int min = INT_MAX;
	int max = INT_MIN;

	if (type == Arrival) {
		while (temp != NULL) {
			if (min > temp->arrival_time) {
				result = temp;
				min = temp->arrival_time;
			}
			temp = temp->next;
		}
	}
	else if (type == CPU) {
		while (temp != NULL) {
			if (min > temp->CPU_burst_time) {
				result = temp;
				min = temp->CPU_burst_time;
			}
			temp = temp->next;
		}
	}
	else if (type == Priority) {
		while (temp != NULL) {
			if (min > temp->priority) {
				result = temp;
				min = temp->priority;
			}
			temp = temp->next;
		}
	}
	return result;
}

void set_temp_queue(list **head, list *node) {
	while (node != NULL) {
		insert_node(head, node);
		node = node->next;
	}
}

list *make_idle(int length) {
	list *new_node = (list *)malloc(sizeof(list));
	node_initialize(new_node);

	new_node->process_ID = 0;
	new_node->CPU_burst_time = length;

	return new_node;
}

list *find_index(list *ready_queue, list *result) {
	list *temp = ready_queue;
	while (temp != NULL) {
		if (temp->process_ID == result->process_ID)
			break;
		if (temp->next == NULL)
			break;
		temp = temp->next;
	}
	return temp;
}

void calc_waiting_time(list *temp, list *result, int time) {
	if (result->flag == 0) 
		temp->waiting_time += (time - result->arrival_time);
	if (result->flag == 1) 
		temp->waiting_time = (time - (temp->CPU_burst_time - result->CPU_burst_time) - temp->arrival_time);
}

void calc_turnaround_time(list *temp, list *result, int time) {
	if (result->flag == 0) 
		temp->turnaround_time = (time - temp->arrival_time);
}

list *temp_waiting_queue = NULL;

void queue_initialize(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp = memory;
	while (temp != NULL) {
		temp->waiting_time = 0;
		temp->turnaround_time = 0;
		temp = temp->next;
	}

	temp_waiting_queue = NULL;
	*waiting_queue = NULL;
	waiting_count = 0;
	*gantt_chart = NULL;
}

void io_work(list **ready_queue, list **waiting_queue, list **gantt_chart, list *head, int *time, int type) {
	switch (type) {
		case Non:
			if (head->flag == 0) {
				int burst_time = head->CPU_burst_time;
				int start = head->start;

				head->CPU_burst_time = start;
				insert_node(gantt_chart, head);
				
				head->flag = 1;
				(*time) += head->CPU_burst_time;

				head->CPU_burst_time = burst_time - start;
				head->arrival_time = *time + head->IO_burst_time;

				head->IO_burst_time = 0;

				insert_node(waiting_queue, head);
				waiting_count++;
				delete_node(ready_queue, head);

				insert_node(&temp_waiting_queue, head);
			}
			break;
		case Pre:
			if (head->flag == 0) {
				if (head->IO_execution_time == head->start) {
					head->arrival_time = *time + head->IO_burst_time;

					head->IO_burst_time = 0;
					insert_node(waiting_queue, head);
					waiting_count++;
					delete_node(ready_queue, head);
					head->flag = 1;
					head->RR_execution_time = 0;

					insert_node(&temp_waiting_queue, head);
				}
			}
			break;
		default: break;
	}
}

void memory_to_ready_queue(list **temp_memory, list **ready_queue, int time) {
	list *temp = *temp_memory;
	while (temp != NULL) {
		if (temp->arrival_time <= time) {
			delete_node(temp_memory, temp);
			insert_node(ready_queue, temp);
		}
		temp = temp->next;
	}
}

void waiting_queue_to_ready_queue(list **waiting_memory, list **ready_queue, int time) {
	list *temp = *waiting_memory;
	while (temp != NULL) {
		if (temp->arrival_time <= time) {
			delete_node(waiting_memory, temp);
			//waiting_count--;
			insert_node(ready_queue, temp);
			temp->flag = 0;
		}
		temp->RR_execution_time = 0;
		temp = temp->next;
	}
}

void FCFS(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;
	
	int time = 0;
	int idle = 0;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = find_min(ready_queue, Arrival);
		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Non);
			}

			list *index = find_index(memory, result);
			calc_waiting_time(index, result, time);

			if (result->flag == 0) {
				delete_node(&ready_queue, result);
				insert_node(gantt_chart, result);
				time += result->CPU_burst_time;
			}

			calc_turnaround_time(index, result, time);
			
		}
		else {
			idle++;
			time++;
		}
	}
}

void N_SJF(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;

	int time = 0;
	int idle = 0;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = find_min(ready_queue, CPU);
		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Non);
			}

			list *index = find_index(memory, result);
			calc_waiting_time(index, result, time);

			if (result->flag == 0) {
				delete_node(&ready_queue, result);
				insert_node(gantt_chart, result);
				time += result->CPU_burst_time;
			}

			calc_turnaround_time(index, result, time);

		}
		else {
			idle++;
			time++;
		}
	}
}

void P_SJF(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;

	int time = 0;
	int idle = 0;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = find_min(ready_queue, CPU);
		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Pre);
			}

			if (result->flag == 0) {
				int temp_time = result->CPU_burst_time;
				result->CPU_burst_time = 1;
				
				insert_node(gantt_chart, result);
				result->IO_execution_time++;
				time++;
				result->CPU_burst_time = temp_time - 1;
				
				list *index = find_index(memory, result);

				if (result->CPU_burst_time == 0) {
					delete_node(&ready_queue, result);
					index->turnaround_time = (time - index->arrival_time);
					index->waiting_time = (index->turnaround_time - index->IO_burst_time - index->CPU_burst_time);
				}
				
			}

		}
		else {
			idle++;
			time++;
		}
	}
}

void N_Priority(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;

	int time = 0;
	int idle = 0;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = find_min(ready_queue, Priority);
		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Non);
			}

			list *index = find_index(memory, result);
			calc_waiting_time(index, result, time);

			if (result->flag == 0) {
				delete_node(&ready_queue, result);
				insert_node(gantt_chart, result);
				time += result->CPU_burst_time;
			}

			calc_turnaround_time(index, result, time);

		}
		else {
			idle++;
			time++;
		}
	}
}

void P_Priority(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;

	int time = 0;
	int idle = 0;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = find_min(ready_queue, Priority);
		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Pre);
			}

			if (result->flag == 0) {
				int temp_time = result->CPU_burst_time;
				result->CPU_burst_time = 1;

				insert_node(gantt_chart, result);
				result->IO_execution_time++;
				time++;
				result->CPU_burst_time = temp_time - 1;

				list *index = find_index(memory, result);

				if (result->CPU_burst_time == 0) {
					delete_node(&ready_queue, result);
					index->turnaround_time = (time - index->arrival_time);
					index->waiting_time = (index->turnaround_time - index->IO_burst_time - index->CPU_burst_time);
				}

			}

		}
		else {
			idle++;
			time++;
		}
	}
}

void RR(list *memory, list **waiting_queue, list **gantt_chart) {
	list *temp_memory = (list *)malloc(sizeof(list));
	temp_memory = NULL;
	set_temp_queue(&temp_memory, memory);

	list *ready_queue = (list *)malloc(sizeof(list));
	ready_queue = NULL;

	int count = 0;
	int time = 0;
	int idle = 0;
	const int time_quantum = 4;

	while (temp_memory != NULL || ready_queue != NULL || *waiting_queue != NULL) {
		memory_to_ready_queue(&temp_memory, &ready_queue, time);
		waiting_queue_to_ready_queue(waiting_queue, &ready_queue, time);

		list *result = NULL;
		int min = INT_MAX;
		list *temp = ready_queue;
		while (temp != NULL) {
			if (min > temp->arrival_time && temp->RR_flag == 0) {
				min = temp->arrival_time;
				result = temp;
			}
			else if (temp->RR_flag == 1 && time > temp->arrival_time + time_quantum) temp->RR_flag = 0;
			temp = temp->next;
		}


		if (result == NULL) {
			result = find_min(ready_queue, Arrival);
		}

		if (result != NULL) {
			if (idle != 0) {
				insert_node(gantt_chart, make_idle(idle));
				idle = 0;
			}

			if (result->IO_burst_time != 0) {
				io_work(&ready_queue, waiting_queue, gantt_chart, result, &time, Pre);
			}

			if (result->flag == 0) {
				int temp_time = result->CPU_burst_time;
				result->CPU_burst_time = 1;

				insert_node(gantt_chart, result);
				time++;
				result->CPU_burst_time = temp_time - 1;

				result->IO_execution_time++;
				result->RR_execution_time++;

				list *index = find_index(memory, result);

				if (result->RR_execution_time % time_quantum == 0 && result->RR_execution_time != index->CPU_burst_time && result->RR_execution_time != 0 && result->CPU_burst_time != 0) {
					result->RR_flag = 1;
					result->arrival_time = time;
					continue;
				}

				if (result->CPU_burst_time == 0) {
					delete_node(&ready_queue, result);
					index->turnaround_time = (time - index->arrival_time);
					index->waiting_time = (index->turnaround_time - index->IO_burst_time - index->CPU_burst_time);
				}

			}

		}
		else {
			idle++;
			time++;
		}
	}
}

void print_terminal(list *memory, list *waiting_queue, list *gantt_chart) {
	system("clear");
	print_ready_queue(memory);
	print_waiting_queue(waiting_queue);
	print_chart(gantt_chart);
	print_evaluation(memory, gantt_chart);
}

void scheduling(list *memory) {
	int n;
	int flag = 1;

	list *waiting_queue = (list *)malloc(sizeof(list));
	node_initialize(waiting_queue);
	waiting_queue = NULL;

	list *gantt_chart = (list *)malloc(sizeof(list));
	node_initialize(gantt_chart);
	gantt_chart = NULL;

	while (flag) {
		schedule_menu();
		scanf("%d", &n);

		switch (n) {
			case 1:
				FCFS(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 2:
				N_SJF(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 3:
				P_SJF(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 4:
				N_Priority(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 5:
				P_Priority(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 6:
				RR(memory, &waiting_queue, &gantt_chart);
				print_terminal(memory, temp_waiting_queue, gantt_chart);
				break;
			case 7:
				flag = 0;
				system("clear");
				break;
			default:
				continue;
		}
		
		queue_initialize(memory, &waiting_queue, &gantt_chart);
	}
}

int main() {
	list *memory = (list *)malloc(sizeof(list));
	node_initialize(memory);
	memory = NULL;

	srand(time(NULL));
	list *temp;
	int i, j, n, num;
	int flag = 1;

	while (flag) {
		print_menu();
		scanf("%d", &n);

		switch (n) {
		case 1:
			system("clear");
			printf("How many? ");
			scanf("%d", &num);
			system("clear");

			for (i = 1; i <= num; i++) {
				temp = create_process(&memory);
				ready_count++;
				printf("Process P%d created!!!\n", temp->process_ID);
			}
			printf("\n");
			break;
		case 2:
			system("clear");
			print_ready_queue(memory);
			break;
		case 3:
			system("clear");
			if (ready_count == 0) {
				printf("Ready Queue is Empty. Create Process Fiirst!!!\n");
				break;
			}
			scheduling(memory);
			break;
		case 4:
			node_initialize(memory);
			memory = NULL;
			ready_count = 0;
			waiting_count = 0;
			process_count = 0;
			system("clear");
			printf("Restarted!!!\n\n");
			break;
		case 5:
			flag = 0;
			break;
		default:
			system("clear");
			continue;
		}
	}


	return 0;
}
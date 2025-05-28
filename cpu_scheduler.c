#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESS 10
#define MAX_BURST 20
#define MAX_PRIORITY 5


typedef struct {
    int pid;
    int arrival_time;
    int total_cpu_burst;
    int priority;
    
    int cpu_executed;
    int waiting_time;
    int turnaround_time;
} Process;

typedef struct {
    int pid;
    int start_time;
    int end_time;
} GanttEntry;

Process processes[MAX_PROCESS];
Process original_processes[MAX_PROCESS]; // 평가용 원본 복사
int process_count = 5; // main 함수에서 예시 5개

GanttEntry gantt_chart[1000];
int gantt_size = 0;

// 간트 차트 기록 함수
void AddGanttEntry(int pid, int start, int end) {
    if (gantt_size > 0 && gantt_chart[gantt_size - 1].pid == pid && gantt_chart[gantt_size - 1].end_time == start) {
        gantt_chart[gantt_size - 1].end_time = end;
    }
    else {
        gantt_chart[gantt_size].pid = pid;
        gantt_chart[gantt_size].start_time = start;
        gantt_chart[gantt_size].end_time = end;
        gantt_size++;
    }
}

void PrintGanttChart() {
    printf("\n== 간트 차트 ==\n|");
    for (int i = 0; i < gantt_size; i++) {
        printf(" P%d |", gantt_chart[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < gantt_size; i++) {
        printf("    %d", gantt_chart[i].end_time);
    }
    printf("\n\n");
}

// 원본 프로세스(초기화 용도)
void InitProcesses() {
    for (int i = 0; i < process_count; i++) {
        processes[i] = original_processes[i];
        processes[i].cpu_executed = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
    }
    gantt_size = 0;
}


// FCFS
void FCFS() {
    InitProcesses();
    int time = 0;
    for (int i = 0; i < process_count; i++) {
        if (time < processes[i].arrival_time) time = processes[i].arrival_time;
        processes[i].waiting_time = time - processes[i].arrival_time;
        AddGanttEntry(processes[i].pid, time, time + processes[i].total_cpu_burst);
        time += processes[i].total_cpu_burst;
        processes[i].turnaround_time = time - processes[i].arrival_time;
    }
    printf("FCFS 스케줄링 \n");
    PrintGanttChart();
}

// Non-preemptive SJF
void NonPreemptiveSJF() {
    InitProcesses();
    int completed = 0, time = 0;
    int finished[MAX_PROCESS] = { 0 };

    while (completed < process_count) {
        int idx = -1;
        int min_burst = 100;
        for (int i = 0; i < process_count; i++) {
            if (!finished[i] && processes[i].arrival_time <= time && processes[i].total_cpu_burst < min_burst) {
                min_burst = processes[i].total_cpu_burst;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        processes[idx].waiting_time = time - processes[idx].arrival_time;
        AddGanttEntry(processes[idx].pid, time, time + processes[idx].total_cpu_burst);
        time += processes[idx].total_cpu_burst;
        processes[idx].turnaround_time = time - processes[idx].arrival_time;
        finished[idx] = 1;
        completed++;
    }
    printf("Non-preemptive SJF 스케줄링\n");
    PrintGanttChart();
}

// Non-preemptive Priority 
void NonPreemptivePriority() {
    InitProcesses();
    int completed = 0, time = 0;
    int finished[MAX_PROCESS] = { 0 };

    while (completed < process_count) {
        int idx = -1;
        int highest_priority = 100;
        for (int i = 0; i < process_count; i++) {
            if (!finished[i] && processes[i].arrival_time <= time && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        processes[idx].waiting_time = time - processes[idx].arrival_time;
        AddGanttEntry(processes[idx].pid, time, time + processes[idx].total_cpu_burst);
        time += processes[idx].total_cpu_burst;
        processes[idx].turnaround_time = time - processes[idx].arrival_time;
        finished[idx] = 1;
        completed++;
    }
    printf("Non-preemptive Priority 스케줄링\n");
    PrintGanttChart();
}
// Preemptive SJF 
void PreemptiveSJF() {
    InitProcesses();
    int completed = 0, time = 0;
    int remaining[MAX_PROCESS];
    int last_pid = -1;

    for (int i = 0; i < process_count; i++) {
        remaining[i] = processes[i].total_cpu_burst;
    }

    while (completed < process_count) {
        int idx = -1;
        int min_remain = 100;
        for (int i = 0; i < process_count; i++) {
            if (processes[i].arrival_time <= time && remaining[i] > 0 && remaining[i] < min_remain) {
                min_remain = remaining[i];
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }

        if (last_pid != idx) {
            if (last_pid != -1)
                AddGanttEntry(last_pid, time - 1, time); 
            AddGanttEntry(idx, time, time + 1);
        }
        else {
            if (gantt_size > 0 && gantt_chart[gantt_size - 1].pid == idx && gantt_chart[gantt_size - 1].end_time == time)
                gantt_chart[gantt_size - 1].end_time++;
            else
                AddGanttEntry(idx, time, time + 1);
        }

        remaining[idx]--;
        time++;
        last_pid = idx;

        if (remaining[idx] == 0) {
            processes[idx].turnaround_time = time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].total_cpu_burst;
            completed++;
        }
    }
    printf("Preemptive SJF 스케줄링\n");
    PrintGanttChart();
}
// Preemptive Priority
void PreemptivePriority() {
    InitProcesses();
    int completed = 0, time = 0;
    int remaining[MAX_PROCESS];
    int last_pid = -1;

    for (int i = 0; i < process_count; i++) {
        remaining[i] = processes[i].total_cpu_burst;
    }

    while (completed < process_count) {
        int idx = -1;
        int highest_priority = 100;
        for (int i = 0; i < process_count; i++) {
            if (processes[i].arrival_time <= time && remaining[i] > 0 && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }

        if (last_pid != idx) {
            if (last_pid != -1)
                AddGanttEntry(last_pid, time - 1, time);
            AddGanttEntry(idx, time, time + 1);
        }
        else {
            if (gantt_size > 0 && gantt_chart[gantt_size - 1].pid == idx && gantt_chart[gantt_size - 1].end_time == time)
                gantt_chart[gantt_size - 1].end_time++;
            else
                AddGanttEntry(idx, time, time + 1);
        }

        remaining[idx]--;
        time++;
        last_pid = idx;

        if (remaining[idx] == 0) {
            processes[idx].turnaround_time = time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].total_cpu_burst;
            completed++;
        }
    }
    printf("Preemptive Priority 스케줄링\n");
    PrintGanttChart();
}

// 결과 출력 함수
void PrintResults() {
    printf("PID | Arrival | Burst | Priority | Waiting | Turnaround\n");
    for (int i = 0; i < process_count; i++) {
        printf("P%d  | %7d | %5d | %8d | %7d | %10d\n",
            processes[i].pid,
            processes[i].arrival_time,
            processes[i].total_cpu_burst,
            processes[i].priority,
            processes[i].waiting_time,
            processes[i].turnaround_time);
    }
    printf("\n");
}

int main() {
    // 프로세스 예시 (PID, Arrival, Burst, Priority)
        original_processes[0] = (Process){ 1, 0, 5, 2 };
        original_processes[1] = (Process){ 2, 1, 3, 1 };
        original_processes[2] = (Process){ 3, 2, 8, 4 };
        original_processes[3] = (Process){ 4, 3, 6, 2 };
        original_processes[4] = (Process){ 5, 4, 2, 3 };

    printf("FCFS\n");
    FCFS();
    PrintResults();

    printf("Non-preemptive SJF\n");
    NonPreemptiveSJF();
    PrintResults();

    printf("Preemptive SJF\n");
    PreemptiveSJF();
    PrintResults();

    printf("Non-preemptive Priority\n");
    NonPreemptivePriority();
    PrintResults();

    printf("Preemptive Priority\n");
    PreemptivePriority();
    PrintResults();
    return 0;
}

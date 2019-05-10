//
//  scheduler.cpp
//  p1
//
//  Created by Jiajun Peng on 1/24/19.
//  Copyright © 2019 Jiajun Peng. All rights reserved.
//

#include <vector>
#include <fstream>
#include <iostream>
#include "thread.h"
using namespace std;

mutex mutex1;
cv cv_r, cv_s;

int max_disk_queue = 0, num_files = 0, current_task_num = 0, count_requester = 0;
vector<int> disk_queue;

vector<std::vector<int>> input;

void requester(void *a) {
    mutex1.lock();
    int r = count_requester++;
    
    int count = 0;
    while (count < (int)input[r].size()) {
        while (current_task_num >= max_disk_queue) {
            cv_r.wait(mutex1);
        }
        
        int track = input[r][count];
        disk_queue[r] = track;
        cout << "requester " << r << " send track " << track << endl;
        ++current_task_num;
        ++count;
        cv_s.signal();

        while (disk_queue[r] != -1) {
            cv_r.wait(mutex1);
        }
    }
    
    --count_requester;
    max_disk_queue = (count_requester >= max_disk_queue) ? max_disk_queue : count_requester;
    cv_s.signal();
    mutex1.unlock();
}

int next_request(int track) {
    int min_dist = 2000, requester = -1;
    for (int i = 0; i < num_files; ++i) {
        if (disk_queue[i] != -1) {
            int temp = abs(disk_queue[i] - track);
            if (temp < min_dist) {
                min_dist = temp;
                requester = i;
            }
        }
    }
    return requester;
}

void service(void *a) {
    mutex1.lock();
    int current_track = 0;
    while (true) {           ///// current task num !=0 not good when max and requester = 1
        while (current_task_num != max_disk_queue) {
            cv_s.wait(mutex1);
        }
        
        int r = next_request(current_track);
        if (r < 0) break;
        current_track = disk_queue[r];
        disk_queue[r] = -1;
        cout << "service requester " << r << " track " << current_track << endl;
        --current_task_num;
        cv_r.broadcast();
    }
    mutex1.unlock();
}

void disk_scheduler(void *a) {
    for (int i = 0; i < num_files; ++i) {
        thread((thread_startfunc_t) requester, (void *) 0);
    }
    thread((thread_startfunc_t) service, (void *) 0);
}

int main() {
    input.resize(5);
    input[0] = {785};
    input[1] = {350,914,589};
    input[2] = {827,567,21};
    input[3] = {302,230};
    input[4] = {631,11};

    num_files = 5;
    max_disk_queue = 2;
    max_disk_queue = min(max_disk_queue, num_files);
    disk_queue.resize(num_files, -1);
    cpu::boot(1, (thread_startfunc_t) disk_scheduler, 0, false, false, 1);
    return 0;
}

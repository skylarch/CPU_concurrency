//
//  main.cpp
//  p1
//
//  Created by Skylar Chen on 1/30/19.
//  Copyright © 2019 Skylar Chen. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include "thread.h"

using std::pair;
using std::cout;
using std::cin;
using std::vector;
using std::ifstream;
using std::string;
using std::min;
using std::endl;

int MAX_DISK_QUEUE = 0;
int ACT_REQ = 0;
int num = 0;
vector< pair<int, int> > Q;
vector<bool> *F;
int CUR_ID = -1;
std::vector<std::vector<int>> INPUT;

mutex reqid;
mutex queue;
mutex print;

cv request;
cv service;

void scheduler(void *a);
void requester(void *a);
void servicer(void *a);

void scheduler(void *a){
    for(int i = 0; i < num; i++){
        thread req ((thread_startfunc_t) requester, (void *) 0);
    }
    thread ser ((thread_startfunc_t) servicer, (void *) 0);
}

void requester(void *a){
    reqid.lock();
    CUR_ID+=1;
    int id = CUR_ID;
    reqid.unlock();
    
    std::vector<int> inp = INPUT[id];
    for(int i = 0; i < (int)inp.size(); i++){
        int track = inp[i];
        queue.lock();
        while(((int)Q.size() >= min(MAX_DISK_QUEUE, ACT_REQ))
              || (*F)[id] == false){
            request.wait(queue);
        }
        
        pair<int, int> p(id, track);
        Q.push_back(p);
        
        print.lock();
        cout << id << " " << track << endl;           //*******optimize?
        print.unlock();
        
        (*F)[id] = false;
        service.signal();
        queue.unlock();
    }
    
    queue.lock();
    while(!(*F)[id]){
        request.wait(queue);
    }
    ACT_REQ--;
    service.signal();
    queue.unlock();
}

void servicer(void *a){
    int head = 0;
    
    while(true){
        queue.lock();
        while((int)Q.size() < min(MAX_DISK_QUEUE, ACT_REQ)
              || (int)Q.size()==0){
            service.wait(queue);
        }
        if(!Q.empty()){
            int short_index = 0;
            for(int i = 0; i < (int)Q.size(); i++){
                if(abs(Q[i].second-head) < abs(Q[short_index].second - head)){
                    short_index = i;
                }
            }
            head = Q[short_index].second;
            
            print.lock();
            cout << Q[short_index].first << " " << head << endl;
            print.unlock();
            
            (*F)[Q[short_index].first] = true;
            Q.erase(Q.begin()+short_index);
            request.broadcast();
        }
        queue.unlock();
    }
}

int main(int argc, char **args){
    INPUT.resize(5);
    INPUT[0] = {785};
    INPUT[1] = {350,914,589};
    INPUT[2] = {827,567,21};
    INPUT[3] = {302,230};
    INPUT[4] = {631,11};

    MAX_DISK_QUEUE = 3;
    ACT_REQ = 5;
    num = ACT_REQ;
    F = new vector<bool>(ACT_REQ, true);
    
    cpu::boot(1, (thread_startfunc_t) scheduler, (void *)5, true, true, 2);
    return 0;
}

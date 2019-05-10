#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm> 
#include "thread.h"


using std::cout;
using std::endl;

mutex mutex1;
cv cv1;  //whether to service or request
cv cv2;

std::vector<std::pair<int,int>> queue = {};
unsigned int max_queue_cap = 3;
unsigned int active_requesters = 5;

std::vector<bool> active = {};
int cur_track = 0;

std::vector<std::vector<int>> input;






void requester(void *a){
    intptr_t num_req = (intptr_t) a;
    mutex1.lock();
    
    while(!input[num_req].empty()){
    
    while(queue.size() >= max_queue_cap || active[num_req]){
        
        cv1.wait(mutex1);
    }

    
    cout << "requester "<< num_req << " track " << input[num_req].front()<<endl;

    active[num_req] = true;
    std::pair<int,int> temp(num_req,input[num_req].front());
    queue.push_back(temp);
    input[num_req].erase(input[num_req].begin());
    cv2.signal();
    
    }
    mutex1.unlock();
}
void server(void* a){
    while(max_queue_cap > 0){
    mutex1.lock();
    
    while(queue.size() < max_queue_cap){
        
        cv2.wait(mutex1);
    }

    

    //choose which track to service
    int min = std::abs(queue.front().second - cur_track);
    std::vector<std::pair<int,int>>:: iterator iter = queue.begin();
    for(auto it = queue.begin(); it!= queue.end(); ++it)
    {
        int diff = std::abs(it -> second - cur_track);
        if(diff < min){
            iter = it;
            min = diff;
        }
    }
    cur_track = iter->second;

    cout << "service requester " <<iter->first << " track " << iter->second << endl;

    active[iter ->first] = false;
    if(input[iter->first].empty())
    {
        active_requesters--;
        max_queue_cap = std::min(max_queue_cap,active_requesters);
        cout << "max_queue_cap = "<<max_queue_cap <<endl; 
    }
    

    queue.erase(iter);
    cv1.broadcast();
    mutex1.unlock();
    }
}



void parent(void *a)
{   
    //start requester threads  
    intptr_t num_thr = (intptr_t) a;
    thread* t[num_thr];
    for(int i = 0; i < num_thr; i++)
    {
        t[i] = new thread((thread_startfunc_t) requester, (void *) i);
    }
    thread t_s((thread_startfunc_t) server, (void *) "test message");

}

int main()
{   
    input.resize(5);
    input[0] = {785};
    input[1] = {350,914,589};
    input[2] = {827,567,21};
    input[3] = {302,230};
    input[4] = {631,11};


    
    max_queue_cap = 3;
    active_requesters = 5;
    input.resize(5);
    active.resize(5);
    for(int i = 0; i < 5; i++)
    {
        active[i] = false;
    }
    
    
    cpu::boot(1, (thread_startfunc_t) parent, (void *)5, false, false,  0);
}
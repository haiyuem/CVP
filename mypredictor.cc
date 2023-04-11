#include <inttypes.h>
#include "cvp.h"


// #include "mypredictor.h"
#include <iostream>
#include <map>
#include <list>

using namespace std;

//This algorithm implements a load history table, with 256 cache entries and LRU evict policy. Query by the load instruction's PC. Fully associative. 

//Each cache entry stores the last two values accessed by the instruction
typedef pair<uint64_t, pair<uint64_t, uint64_t>> CACHE_ENTRY;
//queue to hold entries and identify LRU entry
list<CACHE_ENTRY> load_history;
//map to keep reference from PC to entry and to save access time
map<uint64_t, list<CACHE_ENTRY>::iterator> pc_map_to_dq;
//map the last 10000 seq no to pc. We need PC to update the predictor but it is not provided, so 
// uint64_t seq_no_to_pc[10000];
int size = 64;

void display()
{
 
    // Iterate in the deque and print
    // all the elements in it
    for (auto it = load_history.begin(); it != load_history.end(); it++)
        cout << hex << it->first << " " <<  it->second.first << " " << it->second.second <<endl;
 
    cout << endl;
}

PredictionResult getPrediction(const PredictionRequest& req)
{
	PredictionResult result;
	// display();
	if (req.is_candidate){
		//is load, put seq_no <-> pc mapping into array for updatePredictor use
		// int seq_no_mod = (int)req.seq_no % 10000;
		// seq_no_to_pc[seq_no_mod] = req.pc;
		// map<uint64_t, list<CACHE_ENTRY>::iterator> it = pc_map_to_dq.find(req.pc);
		if (pc_map_to_dq.find(req.pc) != pc_map_to_dq.end()){
		//if this pc already has load history && 2 history entries match
		// if ((it != pc_map_to_dq.end()) && (it->second.first == it->second.second)){
			list<CACHE_ENTRY>::iterator entry = pc_map_to_dq[req.pc];
			if (entry->second.first == entry->second.second){
				result.predicted_value = entry->second.second;
				result.speculate = true;
				return result;
			}
			
		}
	}
	result.predicted_value = 0xdeadbeef;
	result.speculate = false;
	return result;
}

void
updatePredictor (uint64_t seq_no, uint64_t pc, uint64_t actual_addr, uint64_t actual_value, uint64_t actual_latency)
{
	// display();
	if ((actual_addr != 0xdeadbeef) && (actual_value != 0xdeadbeef)){
		// int seq_no_mod = (int)seq_no % 10000;
		// uint64_t pc = seq_no_to_pc[seq_no_mod];
		//update load history
		// list<CACHE_ENTRY>::iterator it = pc_map_to_dq.find(pc);
		if (pc_map_to_dq.find(pc) != pc_map_to_dq.end()){
			list<CACHE_ENTRY>::iterator entry = pc_map_to_dq[pc];
			//shift load history
			entry->second.first = entry->second.second;
			entry->second.second = actual_value;
			//update posentryion in the queue
			CACHE_ENTRY val = *entry;
			load_history.erase(entry);
			load_history.push_front(val);
			pc_map_to_dq[pc] = load_history.begin();
			
			// cout << "update load history (existing) " << hex << pc << " " << entry->second.first << " " << entry->second.second << endl;
		} else {
			//make new entry
			pair<uint64_t, uint64_t> new_pair = make_pair(0xdeadbeef, actual_value);
			CACHE_ENTRY new_entry = make_pair(pc, new_pair);
			//evict LRU entry if load history is full
			if (load_history.size() >= size){
				// delete LRU entry
				CACHE_ENTRY last = load_history.back();
				load_history.pop_back();
				pc_map_to_dq.erase(last.first); //erase key
			}
			load_history.push_front(new_entry);
			pc_map_to_dq[pc] = load_history.begin();
			// cout << "update load history (non existing) " << hex << pc << " " << actual_value << endl;
		}
	}	
}

void
speculativeUpdate (uint64_t seq_no,	// dynamic micro-instruction # (starts at 0 and increments indefinitely)
		   bool eligible,	// true: instruction is eligible for value prediction. false: not eligible.
		   uint8_t prediction_result,	// 0: incorrect, 1: correct, 2: unknown (not revealed)
		   // Note: can assemble local and global branch history using pc, next_pc, and insn.
		   uint64_t
		   pc, uint64_t next_pc, InstClass insn, uint8_t piece,
		   // Note: up to 3 logical source register specifiers, up to 1 logical destination register specifier.
		   // 0xdeadbeef means that logical register does not exist.
		   // May use this information to reconstruct architectural register file state (using log. reg. and value at updatePredictor()).
		   uint64_t src1, uint64_t src2, uint64_t src3, uint64_t dst)
{

}

void
beginPredictor (int argc_other, char **argv_other)
{
}

void
endPredictor ()
{
}

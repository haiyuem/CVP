#include <inttypes.h>
#include "cvp.h"


// #include "mypredictor.h"
#include <iostream>
#include <map>

using namespace std;

//store 2 load history
std::map<uint64_t, pair<uint64_t, uint64_t>> load_history;
uint64_t seq_no_to_pc[1000];

PredictionResult getPrediction(const PredictionRequest& req)
{
	PredictionResult result;
	if (req.is_candidate){
		//is load, put seq_no <-> pc mapping into array for updatePredictor use
		int seq_no_mod = (int)req.seq_no % 100;
		seq_no_to_pc[seq_no_mod] = req.pc;
		std::map<uint64_t, pair<uint64_t, uint64_t>>::iterator it = load_history.find(req.pc);
		//if this pc already has load history && 2 history entries match
		if ((it != load_history.end()) && (it->second.first == it->second.second)){
			result.predicted_value = it->second.second;
			result.speculate = true;
			return result;
		}
	}
	result.predicted_value = 0xdeadbeef;
	result.speculate = false;
	return result;
}

void
updatePredictor (uint64_t
		 seq_no,
		 uint64_t
		 actual_addr, uint64_t actual_value, uint64_t actual_latency)
{
	if ((actual_addr != 0xdeadbeef) && (actual_value != 0xdeadbeef)){
		int seq_no_mod = (int)seq_no % 100;
		uint64_t pc = seq_no_to_pc[seq_no_mod];
		//update load history
		std::map<uint64_t, pair<uint64_t, uint64_t>>::iterator it = load_history.find(pc);
		if (it != load_history.end()){
			//shift load history
			it->second.first = it->second.second;
			it->second.second = actual_value;
			// cout << "update load history (existing) " << hex << pc << " " << it->second.first << " " << it->second.second << endl;
		} else {
			pair<uint64_t, uint64_t> new_pair = make_pair(0xdeadbeef, actual_value);
			load_history.insert(std::pair<uint64_t, pair<uint64_t, uint64_t>>(pc, new_pair));
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

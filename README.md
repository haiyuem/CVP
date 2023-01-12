
# CVP for Intel64
This page only documents changes from the original CVP infrastructure. For original documentation, please see [CVP page](https://github.com/haiyuem/CVP/tree/master).

## Predictor
This branch uses a simpler self-written predictor. It remembers the last two values seen by the load instruction and only predict when the last two values match. For the award-winning predictor that comes with the CVP simulator, see branch [intel64_winnerpred](https://github.com/haiyuem/CVP/tree/intel64_winnerpred). 
Besides the predictor, other changes on the simulator are the same as the winnerpred branch.

## Changes for Intel64 traces
This study uses [PIN tool](https://github.com/haiyuem/pintool) to generate traces that are fed into the CVP simulator. Becasue PIN tool runs on a Intel64 machine, all instructions are Intel64 and thus require some changes on the original simulator. 

### New Trace Format
```Trace Format :
Inst PC 		    - 8 bytes
Next Inst PC 	- 8 bytes (next insn or branch tar)
Inst Type			- 1 byte
If load/storeInst
  Effective Address 		- 8 bytes
  Access Size (one reg)   - 1 byte
  If load: 
     Mem Val   - 8 bytes
If branch
   Taken 				- 1 byte
Num Input Regs 			- 1 byte
Input Reg Names 			- 1 byte each
Num Output Regs 			- 1 byte
Output Reg Names			- 1 byte each
```

### Simulator Changes
- All trace format changes in [trace_reader](lib/cvp_trace_reader.h)
- Intel64 has different instruction lengths, CVP cannot blindly use pc+4 for next_pc
	- Add br_taken to indicate if branch is truely taken instead of comparing pc and pc+4, changes in [struct db_t in trace_reader](lib/cvp_trace_reader.h), [bp.cc](lib/bp.cc) and [bp.h](lib/bp.h)
	- Read the next pc as non-branch target
	-  In 
- Intel64 uses 1 byte to represent reg id, change number of regs to 256 (1 byte) and flag at 257th reg in [uarchsim.h](lib/uarchsim.h)
	- The new simulator does not differentiate int and fp regs, assuming everything is int
- Intel64 uses fused instructions: one mov instruction can do both load and ALU, so the output reg does not hold the immediate loaded value (as assumed in original simulator design), and we need to predict the loaded value, not the output reg value
	- Use mem_load_val instead of D out reg value in [uarchsim](lib/uarchsim.cc) and [trace_reader](lib/cvp_trace_reader.h) 
- Need to split fused instructions into two parts
	- Splitting is done in trace generation
	- Labeled as piece 0 and piece 1, using original infrastructure for fp insts
	- Both pieces of the splitted instruction has the same PC, same output reg, but change the input reg for the ALU inst in load-ALU fused inst to be the output reg of the inst
		- Preserve reg dependency, so that the ALU inst's output reg ready cycle does not overwrite the load's ready cycle
		- Under current infrastructure, the ready cycle of D output reg should always be the max cycle of current ready cycle and newly calculated ready cycle ([changes](https://github.com/eric-rotenberg/CVP/compare/master...haiyuem:CVP:intel64_winnerpred#diff-c4faaeb8327ecf2717410934e4034bc98c857d12d60a59f19fa6ad6d7c2eed73R365))
- [Parameter changes](lib/parameters.cc): 
	- Turn off prefetch 
	- Reduce L2 size to gauge more prediction benefits (optional)

## Key Features of CVP
Just for documentation. 

- Simulator only accepts max 3 input regs and 1 output reg per each instruction
	- This is constrained in trace generation in PIN
- 16 ALU lanes, 8 ldst lanes
- How cycles are calculated: fetch_cycle (ins fetch) + pipeline latency, reg dependency, speculation
	- correct prediction: set early release time for out reg
- Speculate: knows result directly, if wrong prediction, set fetch_cycle <- last ins retire cycle (wait till everything is done) 


> Written with [StackEdit](https://stackedit.io/).

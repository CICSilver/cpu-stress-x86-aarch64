## Instruction
- Get CPU performance scores for floating-point and integer operations on x86 and aarch64 platforms. The program is now compatible with both Windows and Linux operating systems.
- To test X86 with SSE, uncomment lines 101-106 and 74-76 in cou_stress.cpp. Then, comment lines 70-73 and 95-99.
## Install
#### Windows 
run
```bash
    g++ cpu_stress.cpp -o cpu_stress
```
only test g++ version 13.1.0 on windows 11.
#### Linux
run
```bash
    g++ -pthread -std=c++11 -o cpu_stress
```
only test g++ version 5.5.0 and 7.5.0 on Ubuntu 18.04/aarch64.

## Usage
    cpu_stress [OPTIONS]
    
    Options:
        -f, -float
            run float test both single and multi-core
        -a, -all
            run float and int test both single and multi-core
        

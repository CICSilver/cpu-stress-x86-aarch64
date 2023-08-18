## Instruction
- get cpu float and int performance score on x86/aarch64 platform, now can run on windows and linux.
- If you need to test X86 with SSE, you need to uncomment cou_stress.cpp:101-106 lines and 74-76 lines, then comment 70-73 and 95-99 lines.
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
        

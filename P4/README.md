## Time Driven Simulation (TDS): slotted aloha protocol (full version)

  Time driven simulation (TDS) example
  
  The system model is explained in the slides of the slotted aloha protocol specification included with the Practice 2 material (basic slotted-aloha). 
  This is the model implemented in the simulation.

  **Basic version** includes (saloha-basic):
    - Implementation of the slotted-aloha protocol with traces to turn ON and OFF
    - Deterministic CRA and just this one (so the basic version has only one stochastic process, the arrival process)
    - Minimal statistics 
  
  **Full version** includes (saloha-full):
    - It includes many CRA algorithms to do comparison of algorithms (this version has 2 stochastic processes: arrival and waiting time of CRA)
    - It includes a full computation of statistics and a complete final report at the output to measure the system behavior. This implies that it also adds statistics updates all along the code where the measures need to be taken.


  ## To compile and run with Visual Studio code
  
  The files inside the folder .vscode need updates to run in your computer.

  1.- Update the compiler (gcc, gdb, MSYS2_PATH) path inside the settings.json, and launch.json according to where you have the c installed.

  2.- The program arguments are specified in the file launch.json. Change the args to adjust the run to the desired filenames (and path folder of these files). Right now the arguments are ./log/in-ref ./log/out. This means the current program arguments assume the in file (in-ref) is in the log folder and it provides the output of the program in the out file in this same log folder. Change as you prefer to have the in and out files.
  
  3.- The other files in .vscode do not need any change

  When the code is compiled it creates a saloha.exe file (inside the build folder), and as mentioned, the input and output files are where the launch.json arguments indicate.

  ## Running in terminal

  The code provides a make file (mymakefile) to compile in command line in a terminal. When compiled, the compilation files, and executable, are placed in mybuild folder. See inside mymakefile for how to use it.
  
  ## Notes

  The building folders (build or mybuild) should not be included/committed in github if you use a group github repository to share runs and updates.
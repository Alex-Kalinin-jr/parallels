Small console application for investigation of mechanisms and methods of parallel/concurrent programming.
There are 3 tasks for development in concurrency and parallelism:

Salesman problem solving: now developed using standard ***lock_guards and mutexes***.

System linear equations with using of Gauss method: now developed using ***std::async, std::future and threading***.

Matrix multiplication Vinogradov algorithm: now developed in 2 forms:
parallel computations with ***threading and std::call_once*** and pipeline computation efforts with ***std::future, std::packaged_task and threading***.

There are time measurements for linear and parallel equations for the same tasks. You can try to change and adjust algorithms classes for improvement of behaviour, computation time, emulation of races-effects, etc. Algotihms are implemented conforming to SOLID:
- ***lib/s21_gauss_algorithms.cc***.
- ***lib/s21_graph_algorithms.cc***.
- ***lib/s21_vinograd_algorithms.cc***.

Investigate ***Makefile*** for building, testing and checks.

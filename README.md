*This project has been created as part of the 42 curriculum by maaugust.*

<div align="center">
  <img src="https://raw.githubusercontent.com/rfs-hybrid/42-Common-Core/main/assets/covers/cover-philosophers-bonus.png" alt="Philosophers Cover" width="100%" />
</div>

<div align="center">
  <h1>🍝 Philosophers: I never thought philosophy would be so deadly</h1>
  <img src="https://img.shields.io/badge/Language-C-blue" />
  <img src="https://img.shields.io/badge/Grade-125%2F100-success" />
  <img src="https://img.shields.io/badge/Norminette-Passing-success" />
</div>

---

## 💡 Description
**Philosophers** is a classic concurrency and synchronization project in the 42 curriculum. The purpose of this project is to learn the basics of threading a process, explore the use of mutexes, and understand inter-process communication (IPC) using semaphores.

The simulation is based on the famous Edsger Dijkstra's *Dining Philosophers Problem*. A set number of philosophers sit at a round table with a large bowl of spaghetti in the middle. They alternate between eating, thinking, and sleeping. However, to eat, a philosopher must hold two forks (one on their left and one on their right). The challenge is to write a highly optimized algorithm that prevents deadlocks, avoids data races, and ensures no philosopher dies of starvation, all while managing strict CPU timing constraints.

---

## 📑 Features

### 🔹 Mandatory Features
* **Threads & Mutexes:** Each philosopher is represented by a separate thread within a single process.
* **Resource Protection:** Each fork's state is protected by a mutex to prevent philosophers from duplicating forks or experiencing data races.
* **Optimized Monitoring:** A highly responsive monitor thread continuously checks for philosopher deaths and meal quotas, ensuring any death is reported within 10ms of it occurring.

### 🚀 Bonus Features
* **Processes & Semaphores:** The entire simulation architecture is rebuilt. Each philosopher is represented by a completely separate child process. Therefore, memory variables aren't shared between them.
* **Centralized Forks:** Forks are no longer tied to specific seats; instead, they are placed in the middle of the table, represented by a single counting semaphore (`sem_t`) managed by the operating system.
* **Advanced IPC (Inter-Process Communication):** Utilizes named semaphores (`sem_open`) for logging (`print`), start synchronization (`ready`), and meal tracking (`full`) across isolated memory spaces.

---

## 🧠 Algorithm & Data Structure

### Mandatory Algorithm: Resource Hierarchy
To prevent deadlocks (where every philosopher grabs their left fork and waits forever for their right fork), the mandatory implementation uses a **Resource Hierarchy** solution:
* **Odd-numbered philosophers** lock their right fork first, then their left fork.
* **Even-numbered philosophers** lock their left fork first, then their right fork.
* This slight desynchronization mathematically guarantees that at least one philosopher will always be able to pick up both forks, eliminating circular wait deadlocks.

### Bonus Algorithm: The Waiter Semaphore
In the process-based bonus part, forks are represented by a single semaphore. To prevent deadlocks in this environment, a **Waiter Semaphore** (Dijkstra's approach) is implemented.
* A `waiter` semaphore is initialized to `total_philosophers / 2` (with a fallback to 1 for the single-philosopher edge case). 
* Before a philosopher can attempt to grab forks from the table, they must successfully pass through the `waiter` semaphore.
* Because each philosopher requires exactly 2 forks to eat, limiting the concurrent diners to `N / 2` mathematically guarantees that every philosopher who passes the waiter will immediately secure both forks. This completely eliminates the starvation trap where everyone grabs 1 fork and waits forever for a second one.

### The "Thundering Herd" & Synchronization Barriers
A hidden trap in scaling this simulation to 200 philosophers is the severe OS scheduling latency required to spawn 200 concurrent entities. The two architectures handle this very differently:
* **Mandatory (Lightweight Threads):** `pthread_create` spawns threads within the *same* virtual memory space, meaning overhead is incredibly low. To ensure all 200 threads start at the exact same microsecond, a simple shared-memory **Spinlock** is used. All threads wait in a `while (data->start_time == 0)` loop until the monitor thread flips the variable to the current timestamp.
* **Bonus (Heavyweight Processes):** Calling `fork()` to create 200 isolated memory spaces takes the OS kernel a significant amount of time (often 50ms-100ms). If the simulation started immediately, Philosopher 1 would finish eating before Philosopher 200 was even created. To fix this, the parent process calculates a **Future Start Time** (scaled dynamically by the philosopher count), forks the children, and posts a `ready` semaphore. The children wake up and wait for the real-time clock to catch up to this future timestamp, creating a flawless synchronization barrier independent of CPU speed.

### Output Serialization: The "Death Grip"
Both versions prioritize thread/process-safe printing to ensure logs never overlap. In the bonus version, a unique "Death Grip" logic is applied: if a philosopher process detects starvation, it locks the `print` semaphore, prints the death message, and *intentionally never unlocks it*. This guarantees that no other process can print delayed, out-of-order messages while the parent process handles the `SIGKILL` shutdown sequence.

---

## 🛠️ Instructions

### 📦 Installation
To compile the project, run the following commands in the root of the repository.

* To build the **mandatory** executable (`philo`):
```bash
cd philo && make
```

* To build the **bonus** executable (`philo_bonus`):
```bash
cd philo_bonus && make
```

### 🧹 Cleaning
* `make clean`: Removes the compiled object files (`.o`).
* `make fclean`: Removes object files and the executables.
* `make re`: Performs a clean re-build.

### 💻 Usage
Both the mandatory and bonus executables take the exact same arguments:

```bash
./philo <number_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [number_of_times_each_philosopher_must_eat]
```

---

## 🎬 Visual Demos & Testing

The following tests demonstrate the program's reliability and strict adherence to the project's core requirements. These scenarios confirm the absence of deadlocks, precise death reporting (<10ms), and robust argument parsing.

### 1. The Single Philosopher Trap
**Command:** `./philo 1 800 200 200`
A single philosopher only has access to one fork. They should pick it up, be unable to eat, and accurately die at the 800ms mark.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/fb04af77-3596-49a9-b84e-bec0c8b1edad" alt="Test 1 Mandatory"> | <img src="https://github.com/user-attachments/assets/91ae96cd-2d30-41d7-b8d2-78b314f87b06" alt="Test 1 Bonus"> |

</div>

### 2. Standard Survival (Odd Number)
**Command:** `./philo 5 800 200 200`
With plenty of time to eat and sleep, no philosopher should die. The resource hierarchy perfectly manages the odd number of forks.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/cd257509-bcc6-45af-a35d-b9694690ba9d" alt="Test 2 Mandatory"> | <img src="https://github.com/user-attachments/assets/447c49b1-0dd3-4df6-bed7-9138656cdbd0" alt="Test 2 Bonus"> |

</div>

### 3. Meal Quota Reached
**Command:** `./philo 5 800 200 200 7`
No philosopher should die, and the simulation must cleanly exit the exact moment all philosophers have eaten at least 7 times.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/80f86754-0935-4901-89d3-1638b742615e" alt="Test 3 Mandatory"> | <img src="https://github.com/user-attachments/assets/6cf5d203-967b-45bb-a468-77e77bf4b973" alt="Test 3 Bonus"> |

</div>

### 4. Tight Survival Margin (Even Number)
**Command:** `./philo 4 410 200 200`
A highly synchronized test. No philosopher should die, proving that the desynchronization algorithms prevent fork starvation.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/a8320f65-e2cf-4bc4-b0c0-cb56e1e020e5" alt="Test 4 Mandatory"> | <img src="https://github.com/user-attachments/assets/ebe11262-42d4-4267-8342-615bc396d53e" alt="Test 4 Bonus"> |

</div>

### 5. Fork Stealing & Contention (Even Number)
**Command:** `./philo 4 410 200 100`
Because the sleep time is significantly lower than the eating time, a philosopher might wake up and try to steal a fork from someone who has been waiting longer. The internal scheduling logic prevents this contention, allowing all to survive.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/5ab9e326-a1d8-4aed-a00b-cba71214bc7b" alt="Test 5 Mandatory"> | <img src="https://github.com/user-attachments/assets/d3885ec4-a8c4-4389-beb9-9b712dcbd160" alt="Test 5 Bonus"> |

</div>

### 6. Inevitable Death
**Command:** `./philo 4 310 200 100`
A philosopher should die. The program detects and prints the death message precisely at the time limit.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/e028919c-ae73-4bff-ba6a-db61e97d04ea" alt="Test 6 Mandatory"> | <img src="https://github.com/user-attachments/assets/ad04df27-1663-4fde-ae9b-a57a036a8340" alt="Test 6 Bonus"> |

</div>

### 7. Strict Timing Constraints
**Command:** `./philo 5 601 200 200 7`
This pushes the threading limits. The philosophers have exactly 1ms of leeway before starving, yet the simulation perfectly survives to 7 meals.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/f166a594-874b-418c-9a35-c820356d11e3" alt="Test 7 Mandatory"> | <img src="https://github.com/user-attachments/assets/078cea08-3e03-45b4-83a2-6c5260272665" alt="Test 7 Bonus"> |

</div>

### 8. Max Capacity Stress Test (Even Number)
**Command:** `./philo 200 800 200 200`
Testing the upper limits. The program spawns and manages 200 concurrent threads (or processes) seamlessly, proving perfect symmetry in resource allocation.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/81065848-838a-4e9d-92e4-d129848aa68c" alt="Test 8 Mandatory"> | <img src="https://github.com/user-attachments/assets/a494e720-e00b-4a3c-8b98-5b9cef0d7f2f" alt="Test 8 Bonus"> |

</div>

### 9. Max Capacity Stress Test (Odd Number)
**Command:** `./philo 199 800 200 200`
Testing the limits with an odd number of philosophers. The program must flawlessly handle the asymmetrical "odd-man-out" delay at massive scale without deadlocking or dropping performance.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/72e61cb0-6037-42f6-8e3e-98b9b8cd4a46" alt="Test 9 Mandatory"> | <img src="https://github.com/user-attachments/assets/24c95118-a59f-41ac-ab2d-a5aee5e6819d" alt="Test 9 Bonus"> |

</div>

### 10. Parser & Error Handling
Robust input validation. The program safely catches invalid arguments, non-numeric characters, and missing parameters without memory leaks.

<div align="center">

| Mandatory (`philo`) | Bonus (`philo_bonus`) |
| :---: | :---: |
| <img src="https://github.com/user-attachments/assets/de63b9dd-139b-4bb6-aac3-05820c49d675" alt="Test 10 Mandatory"> | <img src="https://github.com/user-attachments/assets/f11fc777-cf78-425d-9718-ce10fa43039c" alt="Test 10 Bonus"> |

</div>

---

## 🧪 Advanced Testing Tools

Concurrency bugs like data races and deadlocks can be notoriously difficult to track down because they happen inconsistently depending on the CPU scheduler. 

### 1. Data Race Detection (Helgrind / Thread Sanitizer)
A program can run perfectly 99 times and crash on the 100th due to a hidden data race. It is strictly forbidden to have data races in this project. You should thoroughly test your mandatory code using either Valgrind's Helgrind tool or GCC's Thread Sanitizer.

**Using Helgrind:**
```bash
valgrind --tool=helgrind ./philo 5 800 200 200
```
*(Note: Helgrind may sometimes report false positives regarding `usleep` inside POSIX implementations, but your actual shared variables must be race-free).*

**Using Thread Sanitizer (Add to Makefile):**
Add `-fsanitize=thread -g` to your `CFLAGS`, recompile, and run the program normally.

### 2. Memory Leak Verification (Valgrind)
Ensure all heap-allocated memory (philosopher arrays, mutex matrices) is properly freed, especially during early termination errors (e.g., if a mutex fails to initialize halfway through).

```bash
valgrind --leak-check=full --show-leak-kinds=all ./philo 5 800 200 200 7
```

> **⏳ Warning: Testing Tool Overhead** > Tools like Valgrind, Helgrind, and Thread Sanitizer (`-fsanitize=thread`) inject massive computational overhead into your program to monitor memory and thread states. This will severely slow down execution and distort your simulation's timing. If your philosophers die during tight-margin tests (like `5 800 200 200`) while running these tools, **do not panic**. This is completely expected behavior. Use these tools strictly to verify memory safety and trace data races, not to test timing accuracy.

### 3. Third-Party Testers & Visualizers
* [dantonik / philosophers-tester](https://github.com/dantonik/42-philosophers-tester) - A highly rigorous bash script that automates dozens of stress tests and tight timing edge cases.
* [nafuka11 / philosophers-visualizer](https://github.com/nafuka11/philosophers-visualizer-v2) - The updated, modern web-based visualizer by nafuka11. Simply redirect your standard output to a text file, upload it to the visualizer, and watch your philosophers' states in real-time to spot overlapping logs or starvation anomalies.

> **⚠️ A Note on Hardware Limits & Strict Testers:** > When pushing extreme edge cases (e.g., `5 601 200 200`) or running high-capacity tests (e.g., 200 philosophers), you are entirely at the mercy of your Operating System's CPU scheduler. The higher the number of threads/processes, the higher the context-switching latency and memory overhead. For massive amounts of philosophers, the `time_to_die` margin must organically be increased just to account for this physical hardware delay.  
> Third-party testers like the **dantonik** script are notoriously rigid with their timing margins. If your completely valid code occasionally fails a tight stress test on a virtual machine, WSL, or an older CPU, it is perfectly normal. It typically indicates an OS scheduling limitation rather than a logical flaw in your concurrency architecture!

---

### 🚨 The Norm
Moulinette relies on a program called `norminette` to check if your files comply with the 42 Norm. Every single `.c` and `.h` file must pass this check. If there is a norm error, you will receive a 0.

**The 42 Header:**
Before writing any code, every file must start with the standard 42 header. `norminette` will automatically fail any file missing this specific signature.
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:04:33 by maaugust          #+#    #+#             */
/*   Updated: 2025/12/31 17:27:14 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

Run the following command in the root of your repository to check all your files at once:
```bash
norminette -R CheckForbiddenSourceHeader philo/ philo_bonus/
```

---

## 📚 Resources & References

**System Manuals:**
* `man pthread_create`, `man pthread_join`, `man pthread_detach` - Essential manuals for thread lifecycle management.
* `man pthread_mutex_init`, `man pthread_mutex_lock`, `man pthread_mutex_unlock` - Manuals detailing shared memory protection and locks.
* `man sem_open`, `man sem_wait`, `man sem_post` - Crucial reading for named IPC semaphore states (Bonus).
* `man fork`, `man kill`, `man waitpid` - Manuals detailing child process spawning and termination (Bonus).
* `man gettimeofday`, `man usleep` - Manuals for microsecond epoch calculations and execution pausing.

**Articles & Guides:**
* **[Mutex vs Semaphore (GeeksforGeeks)](https://www.geeksforgeeks.org/operating-systems/mutex-vs-semaphore/):** A clear conceptual breakdown of the difference between locking a resource (mandatory) and counting resources (bonus).
* **[Dining Philosophers Problem (Wikipedia)](https://en.wikipedia.org/wiki/Dining_philosophers_problem):** The theoretical background of the project and an overview of Dijkstra's solutions.
* **[The Dining Philosophers (Medium)](https://medium.com/@jalal92/the-dining-philosophers-7157cc05315):** An excellent 42-specific walkthrough of thread management and mutex implementation.
* **[Philosophers 42 Guide: The Dining Philosophers Problem (Medium)](https://medium.com/@ruinadd/philosophers-42-guide-the-dining-philosophers-problem-893a24bc0fe2):** A thorough guide covering the logic and architecture for the 42 project.
* **[Dining Philosophers Problem 42 Project Guide (Medium)](https://medium.com/@denaelgammal/dining-philosophers-problem-42-project-guide-mandatory-part-a20fb8dc530e):** A step-by-step breakdown of the mandatory multithreading portion of the project.

**Video Tutorials:**
* **[CodeVault - Multithreading in C](https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2):** A super practical introduction to X-Window programming in C using the MiniLibX API.
* **[CodeVault - Unix Processes in C](https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY):** An absolute goldmine playlist for understanding forks, pipes, and wait functions.
* **[Oceano - The dining Philosophers in C: threads, race conditions and deadlocks](https://www.youtube.com/watch?v=zOpzGHwJ3MU):** An extensive two-hour deep dive into coding the philosophers project from scratch, explaining race conditions, deadlocks, and synchronization.

**42 Standards:**
* **[42 Norm V4](https://cdn.intra.42.fr/pdf/pdf/96987/en.norm.pdf):** The strict coding standard for 42 C projects.
* **[Official 42 Norminette Repository](https://github.com/42School/norminette):** The open-source linter enforcing the strict 42 coding standard.

### 🤖 AI Usage & Transparency
In the spirit of transparency and the learning objectives of the 42 curriculum, here is how AI tools were utilized during this project:

* **Repository Review:** AI was used to conduct a structural audit of the repository, verifying that the subdirectories and file locations adhered to the strict submission guidelines.
* **Documentation & Formatting:** Assisted in formatting this `README.md` layout, ensuring it met all subject requirements (including the italicized header), generating the dual-table test evaluation structure, and drafting the Doxygen comments for the source files.
* **Zero Code Generation:** **No core concurrency logic was generated by AI**. All thread management, process forking, mutex locking protocols, semaphore coordination, and deadlock prevention algorithms were 100% manually researched and written. This strict boundary was maintained to ensure a fundamental, hands-on mastery of POSIX threads and IPC synchronization.

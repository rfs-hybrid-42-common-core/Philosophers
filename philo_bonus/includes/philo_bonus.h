/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:07:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/02 21:48:23 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- Internal Libraries --------------------------- */
# include <fcntl.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/* ---------------------------- Semaphore Names ----------------------------- */

/**
 * @def SEM_PRINT
 * @brief Name of the semaphore used to serialize output operations.
 */
# define SEM_PRINT	"/philo_print"

/**
 * @def SEM_MEAL
 * @brief Base name for the philosopher-specific meal semaphores.
 */
# define SEM_MEAL	"/philo_meal_"

/**
 * @def SEM_FULL
 * @brief Name of the semaphore used to count how many philosophers are full.
 */
# define SEM_FULL	"/philo_full"

/**
 * @def SEM_READY
 * @brief Name of the semaphore used to synchronize the start of all processes.
 */
# define SEM_READY	"/philo_ready"

/**
 * @def SEM_WAITER
 * @brief Name of the semaphore used to limit concurrent diners to prevent 
 * deadlocks.
 */
# define SEM_WAITER	"/philo_waiter"

/**
 * @def SEM_FORKS
 * @brief Name of the semaphore representing the pool of available forks.
 */
# define SEM_FORKS	"/philo_forks"

/* ========================================================================== */
/* ANSI COLOR CODES                                                           */
/* ========================================================================== */

/* --------------------------------- Reset ---------------------------------- */
# define RESET				"\x1b[0m"

/* ------------------------------ Text Styles ------------------------------- */
# define BOLD_ON			"\x1b[1m"
# define DIM_ON				"\x1b[2m"
# define ITALIC_ON			"\x1b[3m"
# define UNDERLINE_ON		"\x1b[4m"
# define BLINK_ON			"\x1b[5m"
# define REVERSE_ON			"\x1b[7m"
# define HIDDEN_ON			"\x1b[8m"
# define STRIKETHROUGH_ON	"\x1b[9m"

# define BOLD_OFF			"\x1b[22m"
# define DIM_OFF			"\x1b[22m"
# define ITALIC_OFF			"\x1b[23m"
# define UNDERLINE_OFF		"\x1b[24m"
# define BLINK_OFF			"\x1b[25m"
# define REVERSE_OFF		"\x1b[27m"
# define HIDDEN_OFF			"\x1b[28m"
# define STRIKETHROUGH_OFF	"\x1b[29m"

/* --------------------------- Foreground Colors ---------------------------- */
# define FG_BLACK			"\x1b[30m"
# define FG_RED				"\x1b[31m"
# define FG_GREEN			"\x1b[32m"
# define FG_YELLOW			"\x1b[33m"
# define FG_BLUE			"\x1b[34m"
# define FG_MAGENTA			"\x1b[35m"
# define FG_CYAN			"\x1b[36m"
# define FG_WHITE			"\x1b[37m"
# define FG_DEFAULT			"\x1b[39m"

/* ------------------------ Bright Foreground Colors ------------------------ */
# define BFG_BLACK			"\x1b[90m"
# define BFG_RED			"\x1b[91m"
# define BFG_GREEN			"\x1b[92m"
# define BFG_YELLOW			"\x1b[93m"
# define BFG_BLUE			"\x1b[94m"
# define BFG_MAGENTA		"\x1b[95m"
# define BFG_CYAN			"\x1b[96m"
# define BFG_WHITE			"\x1b[97m"

/* --------------------------- Background Colors ---------------------------- */
# define BG_BLACK			"\x1b[40m"
# define BG_RED				"\x1b[41m"
# define BG_GREEN			"\x1b[42m"
# define BG_YELLOW			"\x1b[43m"
# define BG_BLUE			"\x1b[44m"
# define BG_MAGENTA			"\x1b[45m"
# define BG_CYAN			"\x1b[46m"
# define BG_WHITE			"\x1b[47m"
# define BG_DEFAULT			"\x1b[49m"

/* ------------------------ Bright Background Colors ------------------------ */
# define BBG_BLACK			"\x1b[100m"
# define BBG_RED			"\x1b[101m"
# define BBG_GREEN			"\x1b[102m"
# define BBG_YELLOW			"\x1b[103m"
# define BBG_BLUE			"\x1b[104m"
# define BBG_MAGENTA		"\x1b[105m"
# define BBG_CYAN			"\x1b[106m"
# define BBG_WHITE			"\x1b[107m"

/* ========================================================================== */
/* TYPEDEFS & DATA STRUCTURES                                                 */
/* ========================================================================== */

/**
 * @typedef t_data
 * @brief Forward declaration of the main data structure.
 * @details Allows for circular referencing between the philosopher node and 
 * the central data struct without triggering compiler warnings.
 */
typedef struct s_data	t_data;

/**
 * @enum e_print_code
 * @brief Enumeration for all print messages and error codes.
 * @details Centralizes the status and error outputs for the simulation.
 * @var NUM_ARGS    Error: Incorrect number of arguments.
 * @var POS_ARGS    Error: Arguments must be positive integers.
 * @var MALLOC      Error: Memory allocation failed.
 * @var FORK        Error: Process fork failed.
 * @var SLEEP       Error: System sleep interruption/failure.
 * @var TH_CREATE   Error: Thread creation failed.
 * @var TH_DETACH   Error: Thread detaching failed.
 * @var TH_JOIN     Error: Thread joining failed.
 * @var TH_INVALID  Error: Unknown thread operation requested.
 * @var SEM_OPEN    Error: Semaphore opening failed.
 * @var SEM_CLOSE   Error: Semaphore closing failed.
 * @var SEM_POST    Error: Semaphore post failed.
 * @var SEM_WAIT    Error: Semaphore wait failed.
 * @var SEM_UNLINK  Error: Semaphore unlink failed.
 * @var SEM_INVALID Error: Unknown semaphore operation requested.
 * @var PHILO_FORK  Status: Philosopher has taken a fork.
 * @var PHILO_EAT   Status: Philosopher is eating.
 * @var PHILO_SLEEP Status: Philosopher is sleeping.
 * @var PHILO_THINK Status: Philosopher is thinking.
 * @var PHILO_DEAD  Status: Philosopher died.
 */
typedef enum e_print_code
{
	NUM_ARGS,
	POS_ARGS,
	MALLOC,
	FORK,
	SLEEP,
	TH_CREATE,
	TH_DETACH,
	TH_JOIN,
	TH_INVALID,
	SEM_OPEN,
	SEM_CLOSE,
	SEM_POST,
	SEM_WAIT,
	SEM_UNLINK,
	SEM_INVALID,
	PHILO_FORK,
	PHILO_EAT,
	PHILO_SLEEP,
	PHILO_THINK,
	PHILO_DEAD
}	t_print_code;

/**
 * @enum e_thread_op
 * @brief Operations for safe_thread wrapper.
 * @details Used to route the safe_thread wrapper to the correct POSIX 
 * thread function.
 * @var CREATE        Creates a new thread.
 * @var JOIN          Joins a terminating thread.
 * @var DETACH        Detaches a thread.
 * @var INVALID_TH_OP Triggers an error for an unknown operation.
 */
typedef enum e_thread_op
{
	CREATE,
	JOIN,
	DETACH,
	INVALID_TH_OP
}	t_th_op;

/**
 * @enum e_sem_op
 * @brief Operations for safe_sem wrapper.
 * @details Used to route the safe_sem wrapper to the correct POSIX 
 * semaphore function.
 * @var POST           Posts (increments) a semaphore.
 * @var WAIT           Waits (decrements) a semaphore.
 * @var INVALID_SEM_OP Triggers an error for an unknown operation.
 */
typedef enum e_sem_op
{
	POST,
	WAIT,
	INVALID_SEM_OP
}	t_sem_op;

/**
 * @struct s_philo
 * @brief Represents a single philosopher process.
 * @details Holds individual state data, unique named semaphores, and thread 
 * information for a philosopher's internal death monitor.
 * @var philo_id    The philosopher's unique ID (1 to total_philos).
 * @var meals_eaten The number of meals this philosopher has consumed.
 * @var last_meal   Timestamp (in ms) of the start of the last meal.
 * @var pid         The Process ID of the philosopher (child process).
 * @var meal        Named semaphore unique to this philosopher (protects data).
 * @var monitor     The Thread ID of the internal death monitor thread.
 * @var data        Pointer to the shared simulation data.
 */
typedef struct s_philo
{
	long		philo_id;
	long		meals_eaten;
	int64_t		last_meal;
	pid_t		pid;
	sem_t		*meal;
	pthread_t	monitor;
	t_data		*data;
}	t_philo;

/**
 * @struct s_data
 * @brief Shared data structure containing simulation rules and resources.
 * @var total_philos  Total number of philosophers and forks.
 * @var time_to_die   Maximum time (in ms) a philosopher can survive without
 * eating.
 * @var time_to_eat   Time (in ms) it takes to eat.
 * @var time_to_sleep Time (in ms) spent sleeping.
 * @var total_meals   Minimum number of meals per philosopher (-1 if infinite).
 * @var start_time    Timestamp (in ms) when the simulation officially begins.
 * @var print         Semaphore used to serialize output operations.
 * @var full          Semaphore used to count how many philosophers are full.
 * @var ready         Semaphore used to synchronize the start of all processes.
 * @var waiter        Semaphore used to limit concurrent diners (deadlock
 * prevent).
 * @var forks         Semaphore representing the pool of available forks.
 * @var philos        Array of philosopher structures.
 */
typedef struct s_data
{
	long		total_philos;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	long		total_meals;
	int64_t		start_time;
	sem_t		*print;
	sem_t		*full;
	sem_t		*ready;
	sem_t		*waiter;
	sem_t		*forks;
	t_philo		*philos;
}	t_data;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------- Process & Thread Routines ------------------------ */
void	dining(t_philo *philo);
void	*monitor_philo_status(void *arg);
void	monitor_philo_meals(t_data *data);

/* ----------------------------- Initialization ----------------------------- */
void	init(t_data *data, int argc, char **argv);

/* --------------------------- Simulation & Logic --------------------------- */
void	simulation(t_data *data);

/* ----------------------------- Safe Wrappers ------------------------------ */
bool	safe_thread(pthread_t *th, void *(*f)(void *), void *data, t_th_op op);
void	safe_sem(t_data *data, sem_t *sem, t_sem_op op);
void	*safe_malloc(t_data *data, size_t size);

/* ---------------------------- Output & Cleanup ---------------------------- */
void	print_message(t_philo *philo, t_print_code code);
void	safe_print(t_philo *philo, t_print_code code);
void	exit_error(t_data *data, t_print_code code);
void	close_semaphores(t_data *data);
void	cleanup(t_data *data);

/* ------------------------------- Utilities -------------------------------- */
long	ft_atol(char *str);
int64_t	ft_gettimeofday_ms(void);
int		ft_msleep(long msec);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:07:04 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 02:12:48 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- Internal Libraries --------------------------- */
# include <pthread.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

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
 * @typedef t_mtx
 * @brief Alias for pthread_mutex_t.
 * @details Keeps the code clean and concise, reducing the visual clutter of 
 * the standard POSIX type during frequent mutex declarations.
 */
typedef pthread_mutex_t	t_mtx;

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
 * @details Centralizes the status and error outputs for the simulation to 
 * ensure clean, consistent messaging and localized error handling.
 * @var NUM_ARGS    Error: Incorrect number of arguments.
 * @var POS_ARGS    Error: Arguments must be positive integers.
 * @var MALLOC      Error: Memory allocation failed.
 * @var SLEEP       Error: System sleep interruption/failure.
 * @var TH_CREATE   Error: Thread creation failed.
 * @var TH_JOIN     Error: Thread joining failed.
 * @var TH_DETACH   Error: Thread detaching failed.
 * @var TH_INVALID  Error: Unknown thread operation requested.
 * @var MTX_INIT    Error: Mutex initialization failed.
 * @var MTX_LOCK    Error: Mutex locking failed.
 * @var MTX_UNLOCK  Error: Mutex unlocking failed.
 * @var MTX_DESTROY Error: Mutex destruction failed.
 * @var MTX_INVALID Error: Unknown mutex operation requested.
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
	SLEEP,
	TH_CREATE,
	TH_JOIN,
	TH_DETACH,
	TH_INVALID,
	MTX_INIT,
	MTX_LOCK,
	MTX_UNLOCK,
	MTX_DESTROY,
	MTX_INVALID,
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
 * @enum e_mutex_op
 * @brief Operations for safe_mutex wrapper.
 * @details Used to route the safe_mutex wrapper to the correct POSIX 
 * mutex function.
 * @var INIT           Initializes a new mutex.
 * @var LOCK           Locks a mutex.
 * @var UNLOCK         Unlocks a mutex.
 * @var DESTROY        Destroys a mutex.
 * @var INVALID_MTX_OP Triggers an error for an unknown operation.
 */
typedef enum e_mutex_op
{
	INIT,
	LOCK,
	UNLOCK,
	DESTROY,
	INVALID_MTX_OP
}	t_mtx_op;

/**
 * @struct s_philo
 * @brief Represents a single philosopher.
 * @details Holds individual state data, resource access indexes, and thread 
 * information for a philosopher.
 * @var philo_id    The philosopher's unique ID (1 to total_philos).
 * @var meals_eaten The number of meals this philosopher has consumed.
 * @var last_meal   Timestamp (in ms) of the start of the last meal.
 * @var fork_a      Index of the first fork to lock (resource hierarchy).
 * @var fork_b      Index of the second fork to lock.
 * @var meal_mtx    Mutex protecting `last_meal` and `meals_eaten`.
 * @var thread      The thread ID associated with this philosopher.
 * @var data        Pointer to the shared simulation data.
 */
typedef struct s_philo
{
	long		philo_id;
	long		meals_eaten;
	int64_t		last_meal;
	long		fork_a;
	long		fork_b;
	t_mtx		meal_mtx;
	pthread_t	thread;
	t_data		*data;
}	t_philo;

/**
 * @struct s_data
 * @brief Shared data structure containing simulation rules and resources.
 * @details Serves as the central state manager, accessible by all philosopher 
 * threads and the monitor thread
 * @var total_philos  Total number of philosophers and forks.
 * @var time_to_die   Maximum time (in ms) a philosopher can survive without 
 * eating.
 * @var time_to_eat   Time (in ms) it takes to eat.
 * @var time_to_sleep Time (in ms) spent sleeping.
 * @var total_meals   Minimum number of meals per philosopher (-1 if infinite).
 * @var philos_ready  Counter for the number of threads ready to start.
 * @var start_time    Timestamp (in ms) when the simulation officially begins.
 * @var is_over       Flag indicating if the simulation should stop (death or 
 * full).
 * @var print_mtx     Mutex for ensuring thread-safe printing.
 * @var status_mtx    Mutex for reading/writing the `is_over` flag.
 * @var ready_mtx     Mutex for protecting `philos_ready` and `start_time`.
 * @var forks_mtx     Array of mutexes representing the forks.
 * @var philos        Array of philosopher structures.
 * @var monitor       The thread ID for the monitor.
 */
typedef struct s_data
{
	long		total_philos;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	long		total_meals;
	long		philos_ready;
	int64_t		start_time;
	bool		is_over;
	t_mtx		print_mtx;
	t_mtx		status_mtx;
	t_mtx		ready_mtx;
	t_mtx		*forks_mtx;
	t_philo		*philos;
	pthread_t	monitor;
}	t_data;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ---------------------------- Thread Routines ----------------------------- */
void	*dining(void *arg);
void	*monitor(void *arg);

/* ----------------------------- Initialization ----------------------------- */
void	init(t_data *data, int argc, char **argv);

/* --------------------------- Simulation & Logic --------------------------- */
bool	handle_single_philo(t_data *data, t_philo *philo);

/* ----------------------------- Safe Wrappers ------------------------------ */
bool	safe_thread(pthread_t *th, void *(*f)(void *), void *data, t_th_op op);
void	safe_mutex(t_data *data, t_mtx *mutex, t_mtx_op op, long count);
void	*safe_malloc(t_data *data, size_t size);

/* ---------------------------- Output & Cleanup ---------------------------- */
void	print_message(t_philo *philo, t_print_code code);
void	safe_print(t_philo *philo, t_print_code code);
void	exit_error(t_data *data, t_print_code code, long count);
void	destroy_mutexes(t_data *data, long count);

/* ------------------------------- Utilities -------------------------------- */
long	ft_atol(char *str);
int64_t	ft_gettimeofday_ms(void);
int		ft_msleep(t_data *data, long msec);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 15:04:25 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/02 21:32:52 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn static void wait_all_philos(t_data *data)
 * @brief Waits for all philosopher processes to terminate.
 * @details Ensures no zombie processes are left behind by reaping the exit 
 * status of every child process created.
 * @param data Pointer to the shared data.
 */
static void	wait_all_philos(t_data *data)
{
	long	i;

	i = -1;
	while (++i < data->total_philos)
		waitpid(data->philos[i].pid, NULL, 0);
}

/**
 * @fn static void kill_all_philos(t_data *data)
 * @brief Sends SIGKILL to all philosopher processes.
 * @details Used to abruptly stop the simulation when a philosopher dies or 
 * an error occurs.
 * @param data Pointer to the shared data.
 */
static void	kill_all_philos(t_data *data)
{
	long	i;

	i = -1;
	while (++i < data->total_philos)
		kill(data->philos[i].pid, SIGKILL);
}

/**
 * @fn static pid_t start_meal_monitor(t_data *data)
 * @brief Forks a dedicated monitoring process for meals.
 * @details Isolates the meal monitoring logic into its own separate process. 
 * If the fork succeeds, the child process executes the meal monitoring routine 
 * to track when all philosophers are full. If the fork fails, it cleanly 
 * aborts the simulation by terminating all running philosopher processes and 
 * exiting safely to prevent resource leaks or zombie processes.
 * @param data Pointer to the main data structure.
 * @return     The PID of the newly created meal monitor process.
 */
static pid_t	start_meal_monitor(t_data *data)
{
	pid_t	meal_pid;

	meal_pid = fork();
	if (meal_pid == -1)
	{
		kill_all_philos(data);
		wait_all_philos(data);
		exit_error(data, FORK);
	}
	if (meal_pid == 0)
		monitor_philo_meals(data);
	return (meal_pid);
}

/**
 * @fn static void start_processes(t_data *data)
 * @brief Forks a child process for each philosopher.
 * @details Loops through the total number of philosophers and forks a new 
 * process for each. Each child process runs the `dining` routine. If a fork 
 * fails, it immediately catches the error, sends a SIGKILL to all previously 
 * created philosopher processes, reaps their zombie states with `waitpid`, 
 * and cleanly exits the program to prevent resource leaks or undefined 
 * behavior.
 * @param data Pointer to the main data structure.
 */
static void	start_processes(t_data *data)
{
	pid_t	philo_pid;
	long	i;
	long	j;

	i = -1;
	while (++i < data->total_philos)
	{
		philo_pid = fork();
		if (philo_pid == -1)
		{
			j = -1;
			while (++j < i)
				kill(data->philos[j].pid, SIGKILL);
			j = -1;
			while (++j < i)
				waitpid(data->philos[j].pid, NULL, 0);
			exit_error(data, FORK);
		}
		if (philo_pid == 0)
			dining(&data->philos[i]);
		data->philos[i].pid = philo_pid;
	}
}

/**
 * @fn void simulation(t_data *data)
 * @brief Orchestrates the main simulation flow in the parent process.
 * @details 
 * 1. Starts all philosopher processes.
 * 2. Fires the `ready` semaphore to wake all child processes simultaneously.
 * 3. If a meal limit is set, forks a dedicated monitoring process.
 * 4. Blocks on `waitpid(-1)` waiting for ANY child process to terminate 
 * (due to death, full meals, or manual kill).
 * 5. Kills and reaps all child processes (philosophers + monitor) to ensure 
 * a clean exit without zombies or leaks.
 * @param data Pointer to the main data structure.
 */
void	simulation(t_data *data)
{
	pid_t	meal_pid;
	long	i;

	meal_pid = -1;
	data->start_time = ft_gettimeofday_ms() + (data->total_philos * 2);
	start_processes(data);
	i = -1;
	while (++i < data->total_philos)
		safe_sem(data, data->ready, POST);
	if (data->total_meals != -1)
		meal_pid = start_meal_monitor(data);
	waitpid(-1, NULL, 0);
	if (ft_msleep(5) != 0)
		exit_error(data, SLEEP);
	kill_all_philos(data);
	if (meal_pid != -1)
	{
		kill(meal_pid, SIGKILL);
		waitpid(meal_pid, NULL, 0);
	}
	wait_all_philos(data);
}

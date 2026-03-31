/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:04:33 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:10:22 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @fn bool handle_single_philo(t_philo *philo, t_data *data)
 * @brief Handles the specific edge case of a single philosopher.
 * @details A single philosopher takes one fork, waits for `time_to_die`, and 
 * then dies. This function handles the locking, printing, and sleeping for 
 * that scenario.
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 * @return      Always returns false to stop the routine.
 */
bool	handle_single_philo(t_data *data, t_philo *philo)
{
	safe_mutex(data, &data->forks_mtx[philo->fork_a], LOCK, data->total_philos);
	safe_print(philo, PHILO_FORK);
	if (ft_msleep(data, data->time_to_die) != 0)
		exit_error(data, SLEEP, data->total_philos);
	safe_mutex(data, &data->forks_mtx[philo->fork_a], UNLOCK,
		data->total_philos);
	return (false);
}

/**
 * @fn static void cleanup(t_data *data)
 * @brief Cleans up resources before the program exits.
 * @details Waits for a brief moment to ensure threads have settled, then 
 * destroys all mutexes and frees allocated memory for philosophers and fork 
 * locks.
 * @param data Pointer to the main data structure containing all resources.
 */
static void	cleanup(t_data *data)
{
	if (ft_msleep(data, 1) != 0)
		exit_error(data, SLEEP, data->total_philos);
	destroy_mutexes(data, data->total_philos);
	free(data->philos);
	free(data->forks_mtx);
}

/**
 * @fn static void simulation(t_data *data)
 * @brief Launches the simulation threads.
 * @details Creates a thread for each philosopher to run the `dining` routine. 
 * Then creates the monitor thread. Finally, it joins the monitor thread 
 * (waiting for it to finish) and then joins all philosopher threads.
 * @param data Pointer to the main data structure.
 */
static void	simulation(t_data *data)
{
	long	i;

	i = -1;
	while (++i < data->total_philos)
	{
		if (!safe_thread(&data->philos[i].thread, &dining,
				&data->philos[i], CREATE))
			exit_error(data, TH_CREATE, data->total_philos);
	}
	if (!safe_thread(&data->monitor, &monitor, data, CREATE))
		exit_error(data, TH_CREATE, data->total_philos);
	if (!safe_thread(&data->monitor, NULL, NULL, JOIN))
		exit_error(data, TH_JOIN, data->total_philos);
	i = -1;
	while (++i < data->total_philos)
		if (!safe_thread(&data->philos[i].thread, NULL, NULL, JOIN))
			exit_error(data, TH_JOIN, data->total_philos);
}

/**
 * @fn int main(int argc, char **argv)
 * @brief Main entry point of the Philosophers program.
 * @details Orchestrates the execution flow: initializes data and parses 
 * arguments, runs the simulation, and performs cleanup upon completion.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int	main(int argc, char **argv)
{
	t_data	data;

	init(&data, argc, argv);
	simulation(&data);
	cleanup(&data);
	return (EXIT_SUCCESS);
}

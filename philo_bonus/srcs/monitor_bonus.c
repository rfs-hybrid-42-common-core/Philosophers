/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 15:18:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/02 21:38:47 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn void *monitor_philo_status(void *arg)
 * @brief Thread routine that monitors the death status of a single philosopher.
 * @details Runs as a thread inside each philosopher's child process. It 
 * continuously checks if the philosopher has starved. If death is detected, 
 * it prints the message and exits the process with `EXIT_FAILURE`.  This exit 
 * is detected by the parent's `waitpid`, triggering the global shutdown.
 * @param arg Void pointer to the philosopher structure.
 * @return    NULL.
 */
void	*monitor_philo_status(void *arg)
{
	t_philo	*philo;
	int64_t	now;
	int64_t	last;

	philo = (t_philo *)arg;
	while (true)
	{
		now = ft_gettimeofday_ms();
		safe_sem(philo->data, philo->meal, WAIT);
		last = philo->last_meal;
		safe_sem(philo->data, philo->meal, POST);
		if (now - last > (int64_t)philo->data->time_to_die)
		{
			safe_print(philo, PHILO_DEAD);
			exit(EXIT_FAILURE);
		}
		usleep(500);
	}
	return (NULL);
}

/**
 * @fn void monitor_philo_meals(t_data *data)
 * @brief Routine for the dedicated meal monitoring process.
 * @details Runs in a separate child process. Waits for `total_philos` signals 
 * on the `full` semaphore. Once all philosophers have eaten enough, it exits 
 * with `EXIT_SUCCESS`. This exit is detected by the parent's `waitpid`, 
 * triggering the global shutdown.
 * @param data Pointer to the main data structure.
 */
void	monitor_philo_meals(t_data *data)
{
	long	i;

	i = -1;
	while (++i < data->total_philos)
		safe_sem(data, data->full, WAIT);
	close_semaphores(data);
	free(data->philos);
	exit(EXIT_SUCCESS);
}

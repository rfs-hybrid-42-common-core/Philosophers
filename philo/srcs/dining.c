/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dining.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:43:32 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/02 21:24:34 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @fn static void release_forks(t_data *data, t_philo *philo)
 * @brief Unlocks both forks held by a philosopher.
 * @details Only attempts to unlock if there is more than one philosopher.
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 */
static void	release_forks(t_data *data, t_philo *philo)
{
	if (data->total_philos > 1)
	{
		safe_mutex(data, &data->forks_mtx[philo->fork_a], UNLOCK,
			data->total_philos);
		safe_mutex(data, &data->forks_mtx[philo->fork_b], UNLOCK,
			data->total_philos);
	}
}

/**
 * @fn static bool pick_forks(t_data *data, t_philo *philo)
 * @brief Attempts to lock two forks for the philosopher.
 * @details Implements a resource hierarchy solution to prevent deadlocks:
 * - Odd philosophers lock fork B then A.
 * - Even philosophers lock fork A then B.
 * - Delegates single philosopher logic to `handle_single_philo`.
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 * @return      true if forks were successfully picked and simulation is 
 * running, false otherwise.
 */
static bool	pick_forks(t_data *data, t_philo *philo)
{
	long	fork_a;
	long	fork_b;
	bool	is_over;

	if (data->total_philos == 1)
		return (handle_single_philo(data, philo));
	fork_a = philo->fork_a;
	fork_b = philo->fork_b;
	if (philo->philo_id % 2 == 1)
	{
		safe_mutex(data, &data->forks_mtx[fork_b], LOCK, data->total_philos);
		safe_print(philo, PHILO_FORK);
		safe_mutex(data, &data->forks_mtx[fork_a], LOCK, data->total_philos);
	}
	else
	{
		safe_mutex(data, &data->forks_mtx[fork_a], LOCK, data->total_philos);
		safe_print(philo, PHILO_FORK);
		safe_mutex(data, &data->forks_mtx[fork_b], LOCK, data->total_philos);
	}
	safe_print(philo, PHILO_FORK);
	safe_mutex(data, &data->status_mtx, LOCK, data->total_philos);
	is_over = data->is_over;
	safe_mutex(data, &data->status_mtx, UNLOCK, data->total_philos);
	return (!is_over);
}

/**
 * @fn static bool eating_sleeping(t_data *data, t_philo *philo)
 * @brief Executes the eating and sleeping phases.
 * @details Checks if the philosopher died while waiting for forks *before* 
 * updating the `last_meal` timestamp. This prevents the "Aliasing" issue 
 * where a starved philosopher resets their timer and fools the monitor.
 * 1. Checks death condition (current_time - last_meal >= time_to_die).
 * 2. Updates `last_meal` and `meals_eaten`.
 * 3. Prints EAT message and sleeps for `time_to_eat`.
 * 4. Releases forks.
 * 5. Prints SLEEP message and sleeps for `time_to_sleep`.
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 * @return      true if the cycle completed successfully, false if died or 
 * stopped.
 */
static bool	eating_sleeping(t_data *data, t_philo *philo)
{
	safe_mutex(data, &philo->meal_mtx, LOCK, data->total_philos);
	if (ft_gettimeofday_ms() - philo->last_meal >= (int64_t)data->time_to_die)
	{
		safe_mutex(data, &philo->meal_mtx, UNLOCK, data->total_philos);
		return (false);
	}
	philo->last_meal = ft_gettimeofday_ms();
	philo->meals_eaten++;
	safe_mutex(data, &philo->meal_mtx, UNLOCK, data->total_philos);
	safe_print(philo, PHILO_EAT);
	if (ft_msleep(data, data->time_to_eat) != 0)
		exit_error(data, SLEEP, data->total_philos);
	release_forks(data, philo);
	safe_print(philo, PHILO_SLEEP);
	if (ft_msleep(data, data->time_to_sleep) != 0)
		exit_error(data, SLEEP, data->total_philos);
	return (true);
}

/**
 * @fn static bool philo_routine(t_data *data, t_philo *philo)
 * @brief Executes one full cycle of the philosopher's life.
 * @details 
 * 1. Checks if simulation is over.
 * 2. Picks forks.
 * 3. Performs eating and sleeping (via helper).
 * 4. Thinks (with calculated delay for odd number of philosophers to prevent 
 * starvation).
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 * @return      true to continue, false to stop.
 */
static bool	philo_routine(t_data *data, t_philo *philo)
{
	bool	is_over;
	long	time_to_think;

	safe_mutex(data, &data->status_mtx, LOCK, data->total_philos);
	is_over = data->is_over;
	safe_mutex(data, &data->status_mtx, UNLOCK, data->total_philos);
	if (is_over)
		return (false);
	if (!pick_forks(data, philo) || !eating_sleeping(data, philo))
	{
		release_forks(data, philo);
		return (false);
	}
	safe_print(philo, PHILO_THINK);
	if (data->total_philos % 2 != 0)
	{
		time_to_think = (data->time_to_eat * 2) - data->time_to_sleep;
		if (time_to_think < 0)
			time_to_think = 0;
		if (time_to_think > 0 && ft_msleep(data, time_to_think * 0.95) != 0)
			exit_error(data, SLEEP, data->total_philos);
		usleep(500);
	}
	return (true);
}

/**
 * @fn void *dining(void *arg)
 * @brief The main thread entry point for a philosopher.
 * @details 
 * 1. Signals readiness to the monitor.
 * 2. Waits for the global `start_time` synchronization.
 * 3. Even-numbered philosophers delay start to desynchronize forks.
 * 4. Loops `philo_routine` until simulation ends.
 * @param arg Void pointer to the philosopher structure.
 * @return    NULL.
 */
void	*dining(void *arg)
{
	t_data	*data;
	t_philo	*philo;

	philo = (t_philo *)arg;
	data = philo->data;
	safe_mutex(data, &data->ready_mtx, LOCK, data->total_philos);
	data->philos_ready++;
	safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
	while (true)
	{
		safe_mutex(data, &data->ready_mtx, LOCK, data->total_philos);
		if (data->start_time != 0)
		{
			safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
			break ;
		}
		safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
		usleep(50);
	}
	if (philo->philo_id % 2 == 0 && ft_msleep(data, 10) != 0)
		exit_error(data, SLEEP, data->total_philos);
	while (philo_routine(data, philo))
		;
	return (NULL);
}

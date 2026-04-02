/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dining_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:43:32 by maaugust          #+#    #+#             */
/*   Updated: 2026/04/02 20:51:31 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn static void release_forks(t_data *data)
 * @brief Releases acquired resources after eating.
 * @details Posts to the forks semaphore twice and the waiter semaphore once, 
 * allowing other philosophers to enter the eating queue.
 * @param data Pointer to shared data.
 */
static void	release_forks(t_data *data)
{
	safe_sem(data, data->forks, POST);
	safe_sem(data, data->forks, POST);
	safe_sem(data, data->waiter, POST);
}

/**
 * @fn static void pick_forks(t_data *data, t_philo *philo)
 * @brief Acquires resources necessary for eating.
 * @details 
 * 1. Waits on `waiter` semaphore (limits table access to N-1).
 * 2. Waits on `forks` semaphore twice.
 * 3. Prints fork acquisition messages.
 * @param data  Pointer to shared data.
 * @param philo Pointer to philosopher.
 */
static void	pick_forks(t_data *data, t_philo *philo)
{
	safe_sem(data, data->waiter, WAIT);
	safe_sem(data, data->forks, WAIT);
	safe_print(philo, PHILO_FORK);
	safe_sem(data, data->forks, WAIT);
	safe_print(philo, PHILO_FORK);
}

/**
 * @fn static void eating_sleeping(t_data *data, t_philo *philo)
 * @brief Handles the eating and sleeping phases.
 * @details 
 * 1. Locks `meal` semaphore to update `last_meal`.
 * 2. performs a strict death check: if the philosopher died while waiting 
 * for forks, it releases resources and exits immediately.
 * 3. Updates meal counters and posts `full` if applicable.
 * 4. Prints status, sleeps for `time_to_eat`, releases forks, then sleeps.
 * @param data  Pointer to shared data.
 * @param philo Pointer to philosopher.
 */
static void	eating_sleeping(t_data *data, t_philo *philo)
{
	safe_sem(data, philo->meal, WAIT);
	if (ft_gettimeofday_ms() - philo->last_meal >= data->time_to_die)
	{
		safe_sem(data, philo->meal, POST);
		release_forks(data);
		safe_print(philo, PHILO_DEAD);
		exit(EXIT_FAILURE);
	}
	philo->last_meal = ft_gettimeofday_ms();
	philo->meals_eaten++;
	if (data->total_meals != -1 && philo->meals_eaten >= data->total_meals)
	{
		safe_sem(data, data->full, POST);
		data->total_meals = -1;
	}
	safe_sem(data, philo->meal, POST);
	safe_print(philo, PHILO_EAT);
	if (ft_msleep(data->time_to_eat) != 0)
		exit_error(data, SLEEP);
	release_forks(data);
	safe_print(philo, PHILO_SLEEP);
	if (ft_msleep(data->time_to_sleep) != 0)
		exit_error(data, SLEEP);
}

/**
 * @fn static void philo_routine(t_data *data, t_philo *philo)
 * @brief Executes one cycle of eating, sleeping, and thinking.
 * @details 
 * 1. Acquires `waiter` semaphore to prevent deadlock (limits 
 * concurrent eaters).
 * 2. Acquires two forks from the global pool.
 * 3. Eats (updates `last_meal`, signals `full` if quota reached).
 * 4. Sleeps.
 * 5. Thinks (with calculated delay for odd number of philosophers to prevent 
 * starvation).
 * @param data  Pointer to the shared data.
 * @param philo Pointer to the philosopher.
 */
static void	philo_routine(t_data *data, t_philo *philo)
{
	long	time_to_think;

	pick_forks(data, philo);
	eating_sleeping(data, philo);
	safe_print(philo, PHILO_THINK);
	if (data->total_philos % 2 != 0)
	{
		time_to_think = (data->time_to_eat * 2) - data->time_to_sleep;
		if (time_to_think < 0)
			time_to_think = 0;
		if (time_to_think > 0 && ft_msleep(time_to_think * 0.95) != 0)
			exit_error(data, SLEEP);
		if (usleep(500) != 0)
			exit_error(data, SLEEP);
	}
}

/**
 * @fn void dining(t_philo *philo)
 * @brief The main routine for a philosopher child process.
 * @details 
 * 1. Blocks on the `ready` semaphore until the parent finishes all forks.
 * 2. Initializes the local survival timer (`last_meal`).
 * 3. Starts the monitoring thread for this specific process.
 * 4. Handles the single philosopher edge case.
 * 5. Staggers even-numbered philosophers to reduce initial contention.
 * 6. Enters the infinite `philo_routine` loop.
 * @param philo Pointer to the philosopher structure.
 */
void	dining(t_philo *philo)
{
	philo->pid = 0;
	safe_sem(philo->data, philo->data->ready, WAIT);
	while (ft_gettimeofday_ms() < philo->data->start_time)
		usleep(100);
	philo->last_meal = philo->data->start_time;
	if (!safe_thread(&philo->monitor, &monitor_philo_status, philo, CREATE))
		exit_error(philo->data, TH_CREATE);
	if (philo->data->total_philos == 1)
	{
		safe_sem(philo->data, philo->data->forks, WAIT);
		safe_print(philo, PHILO_FORK);
		safe_sem(philo->data, philo->data->forks, WAIT);
	}
	if (philo->philo_id % 2 == 0 && ft_msleep(10) != 0)
		exit_error(philo->data, SLEEP);
	while (true)
		philo_routine(philo->data, philo);
}

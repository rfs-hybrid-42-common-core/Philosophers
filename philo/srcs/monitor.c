/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 15:18:03 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:25:48 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @fn static void wait_until_all_philos_ready(t_data *data)
 * @brief Waits until all philosopher threads have initialized.
 * @details Spins with a small sleep until `philos_ready` equals total count.
 * @param data Pointer to the shared data.
 */
static void	wait_until_all_philos_ready(t_data *data)
{
	while (true)
	{
		safe_mutex(data, &data->ready_mtx, LOCK, data->total_philos);
		if (data->philos_ready == data->total_philos)
		{
			safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
			break ;
		}
		safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
		if (usleep(50) != 0)
			exit_error(data, SLEEP, data->total_philos);
	}
}

/**
 * @fn static void initialize_timer(t_data *data)
 * @brief Synchronizes start time and initializes philosopher timers.
 * @details 
 * 1. Sets all `last_meal` timestamps to current time to prevent premature 
 * death at T=0.
 * 2. Sets the global `start_time` to release philosopher threads.
 * 3. Waits a "Grace Period" (time_to_die / 2) to let threads stabilize.
 * @param data Pointer to the shared data.
 */
static void	initialize_timer(t_data *data)
{
	int64_t	start_time;
	long	i;

	start_time = ft_gettimeofday_ms();
	i = -1;
	while (++i < data->total_philos)
	{
		safe_mutex(data, &data->philos[i].meal_mtx, LOCK, data->total_philos);
		data->philos[i].last_meal = start_time;
		safe_mutex(data, &data->philos[i].meal_mtx, UNLOCK, data->total_philos);
	}
	safe_mutex(data, &data->ready_mtx, LOCK, data->total_philos);
	data->start_time = start_time;
	safe_mutex(data, &data->ready_mtx, UNLOCK, data->total_philos);
	if (data->total_philos > 1)
		ft_msleep(data, data->time_to_die / 2);
}

/**
 * @fn static bool check_full_philos(t_data *data, long full_philos)
 * @brief Checks if the meal goal has been reached.
 * @details If all philosophers have eaten at least `total_meals` times, 
 * sets the `is_over` flag to true.
 * @param data        Pointer to the shared data.
 * @param full_philos Count of philosophers who have reached the meal goal.
 * @return            true if goal reached, false otherwise.
 */
static bool	check_full_philos(t_data *data, long full_philos)
{
	if (data->total_meals != -1 && full_philos == data->total_philos)
	{
		safe_mutex(data, &data->status_mtx, LOCK, data->total_philos);
		data->is_over = true;
		safe_mutex(data, &data->status_mtx, UNLOCK, data->total_philos);
		return (true);
	}
	return (false);
}

/**
 * @fn static bool check_simulation_state(t_data *data)
 * @brief Optimized single-pass monitor check.
 * @details Iterates through all philosophers once to check both:
 * 1. Starvation: If `now - last_meal > time_to_die`.
 * 2. Meal Count: If `meals_eaten >= total_meals`.
 * Merging these loops reduces mutex contention by 50%.
 * @param data Pointer to the shared data.
 * @return     true if simulation should stop (death or full), false otherwise.
 */
static bool	check_simulation_state(t_data *data)
{
	int64_t	now;
	long	full_philos;
	long	i;

	now = ft_gettimeofday_ms();
	full_philos = 0;
	i = -1;
	while (++i < data->total_philos)
	{
		safe_mutex(data, &data->philos[i].meal_mtx, LOCK, data->total_philos);
		if (now - data->philos[i].last_meal > (int64_t)data->time_to_die)
		{
			safe_mutex(data, &data->philos[i].meal_mtx, UNLOCK,
				data->total_philos);
			safe_print(&data->philos[i], PHILO_DEAD);
			return (true);
		}
		if (data->total_meals != -1
			&& data->philos[i].meals_eaten >= data->total_meals)
			full_philos++;
		safe_mutex(data, &data->philos[i].meal_mtx, UNLOCK, data->total_philos);
	}
	return (check_full_philos(data, full_philos));
}

/**
 * @fn void *monitor(void *arg)
 * @brief The main monitor thread routine.
 * @details 
 * 1. Waits for readiness.
 * 2. Initializes timers and grace period.
 * 3. Loops with a tuned `usleep(500)` to balance responsiveness (checking 
 * death and meal quotas) and CPU usage (allowing philosophers to run).
 * @param arg Void pointer to the data structure.
 * @return    NULL.
 */
void	*monitor(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	wait_until_all_philos_ready(data);
	initialize_timer(data);
	while (true)
	{
		if (check_simulation_state(data))
			break ;
		if (usleep(500) != 0)
			exit_error(data, SLEEP, data->total_philos);
	}
	return (NULL);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safety.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 14:10:11 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:03:57 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @fn bool safe_thread(pthread_t *th, void *(*f)(void *), void *arg, 
 * t_th_op op)
 * @brief Wrapper for pthread functions to handle errors.
 * @details Abstraction for create, join, and detach operations.
 * @param th  Pointer to the pthread_t variable.
 * @param f   Function pointer for thread routine (create only).
 * @param arg Argument for the thread routine (create only).
 * @param op  Operation type (CREATE, JOIN, DETACH).
 * @return    true on success, false on failure.
 */
bool	safe_thread(pthread_t *th, void *(*f)(void *), void *arg, t_th_op op)
{
	int	ret;

	ret = -1;
	if (op == CREATE)
		ret = pthread_create(th, NULL, f, arg);
	else if (op == JOIN)
		ret = pthread_join(*th, NULL);
	else if (op == DETACH)
		ret = pthread_detach(*th);
	return (ret == 0);
}

/**
 * @fn void safe_mutex(t_mtx *mutex, t_mtx_op op, t_data *data, long count)
 * @brief Wrapper for mutex functions with error handling.
 * @details Handles INIT, LOCK, UNLOCK, and DESTROY operations. Exits the 
 * program if a mutex operation fails.
 * @param data  Pointer to data (for cleanup on error).
 * @param mutex Pointer to the mutex.
 * @param op    Operation type.
 * @param count Count for cleanup (number of philos initialized so far).
 */
void	safe_mutex(t_data *data, t_mtx *mutex, t_mtx_op op, long count)
{
	int	ret;

	ret = -1;
	if (op == INIT)
		ret = pthread_mutex_init(mutex, NULL);
	else if (op == LOCK)
		ret = pthread_mutex_lock(mutex);
	else if (op == UNLOCK)
		ret = pthread_mutex_unlock(mutex);
	else if (op == DESTROY)
		ret = pthread_mutex_destroy(mutex);
	else
		exit_error(data, MTX_INVALID, count);
	if (ret != 0)
	{
		if (op == INIT)
			exit_error(data, MTX_INIT, count);
		else if (op == LOCK)
			exit_error(data, MTX_LOCK, count);
		else if (op == UNLOCK)
			exit_error(data, MTX_UNLOCK, count);
		else if (op == DESTROY)
			exit_error(data, MTX_DESTROY, count);
	}
}

/**
 * @fn void *safe_malloc(t_data *data, size_t size)
 * @brief Safe memory allocation wrapper.
 * @details Calls malloc and checks for NULL return. If malloc fails, it 
 * automatically triggers the program's error termination sequence.
 * @param data Pointer to data (for cleanup on error).
 * @param size Bytes to allocate.
 * @return     Pointer to allocated memory (never returns NULL).
 */
void	*safe_malloc(t_data *data, size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		exit_error(data, MALLOC, 0);
	return (ptr);
}

/**
 * @fn void safe_print(t_philo *philo, t_print_code code)
 * @brief Thread-safe wrapper for printing simulation events.
 * @details Implements a "Print Lock First" strategy.
 * 1. Locks `print_mtx` *before* checking status. This serializes threads 
 * waiting to print, preventing them from spamming the critical `status_mtx`.
 * 2. Locks `status_mtx` briefly to check/update the `is_over` flag.
 * 3. Prints the message if the simulation is active (or if reporting death).
 * @param philo Pointer to the philosopher.
 * @param code  Message code.
 */
void	safe_print(t_philo *philo, t_print_code code)
{
	t_data	*data;

	data = philo->data;
	safe_mutex(data, &data->print_mtx, LOCK, data->total_philos);
	safe_mutex(data, &data->status_mtx, LOCK, data->total_philos);
	if (data->is_over && code != PHILO_DEAD)
	{
		safe_mutex(data, &data->status_mtx, UNLOCK, data->total_philos);
		safe_mutex(data, &data->print_mtx, UNLOCK, data->total_philos);
		return ;
	}
	if (code == PHILO_DEAD)
		data->is_over = true;
	safe_mutex(data, &data->status_mtx, UNLOCK, data->total_philos);
	print_message(philo, code);
	safe_mutex(data, &data->print_mtx, UNLOCK, data->total_philos);
}

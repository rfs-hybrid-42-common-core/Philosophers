/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 17:10:24 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 14:15:01 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn static void unlink_semaphores(t_data *data)
 * @brief Removes named semaphores from the operating system.
 * @details This function should ONLY be called by the parent process at the 
 * very end of the simulation. It ensures the names (e.g., "/philo_forks") 
 * are removed so future runs do not link to old/stale semaphores.
 * @param data Pointer to the data structure.
 */
static void	unlink_semaphores(t_data *data)
{
	if (sem_unlink(SEM_PRINT) != 0 || sem_unlink(SEM_FULL) != 0
		|| sem_unlink(SEM_READY) != 0 || sem_unlink(SEM_WAITER) != 0
		|| sem_unlink(SEM_FORKS) != 0)
		exit_error(data, SEM_UNLINK);
}

/**
 * @fn void close_semaphores(t_data *data)
 * @brief Closes the process-local semaphore descriptors.
 * @details Safely checks if each semaphore was successfully opened (not 
 * SEM_FAILED) before attempting to close it, preventing crashes during 
 * partial initialization failures. Closing a semaphore only decrements its 
 * reference count in the calling process and does NOT remove the name from 
 * the OS. This makes it completely safe for child processes to call on 
 * error/exit without breaking the simulation for others.
 * @param data Pointer to the main data structure.
 */
void	close_semaphores(t_data *data)
{
	long	i;

	if ((data->print != SEM_FAILED && sem_close(data->print) != 0)
		|| (data->full != SEM_FAILED && sem_close(data->full) != 0)
		|| (data->ready != SEM_FAILED && sem_close(data->ready) != 0)
		|| (data->waiter != SEM_FAILED && sem_close(data->waiter) != 0)
		|| (data->forks != SEM_FAILED && sem_close(data->forks) != 0))
		exit_error(data, SEM_CLOSE);
	if (data->philos)
	{
		i = -1;
		while (++i < data->total_philos)
			if (data->philos[i].meal != NULL
				&& data->philos[i].meal != SEM_FAILED
				&& sem_close(data->philos[i].meal) != 0)
				exit_error(data, SEM_CLOSE);
	}
}

/**
 * @fn void cleanup(t_data *data)
 * @brief Performs the final resource cleanup for the parent process.
 * @details 
 * 1. Unlinks semaphores (removes names from OS).
 * 2. Closes parent's open descriptors.
 * 3. Frees memory.
 * @param data Pointer to the main data structure.
 */
void	cleanup(t_data *data)
{
	unlink_semaphores(data);
	close_semaphores(data);
	free(data->philos);
}

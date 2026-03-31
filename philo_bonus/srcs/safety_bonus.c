/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safety_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 14:10:11 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:52:32 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

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
 * @fn void safe_sem(t_data *data, sem_t *sem, t_sem_op op)
 * @brief Wrapper for semaphore functions with error handling.
 * @details Handles POST and WAIT operations. Exits the program if a semaphore 
 * operation fails.
 * @param data Pointer to data (for cleanup on error).
 * @param sem  Pointer to the semaphore.
 * @param op   Operation type (POST or WAIT).
 */
void	safe_sem(t_data *data, sem_t *sem, t_sem_op op)
{
	int	ret;

	ret = -1;
	if (op == POST)
		ret = sem_post(sem);
	else if (op == WAIT)
		ret = sem_wait(sem);
	else
		exit_error(data, SEM_INVALID);
	if (ret != 0)
	{
		if (op == POST)
			exit_error(data, SEM_POST);
		else if (op == WAIT)
			exit_error(data, SEM_WAIT);
	}
}

/**
 * @fn void *safe_malloc(t_data *data, size_t size)
 * @brief Safe memory allocation wrapper.
 * @details Calls malloc and checks for NULL return. If malloc fails, it 
 * automatically triggers the program's error termination sequence.
 * @param size Bytes to allocate.
 * @param data Pointer to data (for cleanup on error).
 * @return     Pointer to allocated memory (never returns NULL).
 */
void	*safe_malloc(t_data *data, size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		exit_error(data, MALLOC);
	return (ptr);
}

/**
 * @fn void safe_print(t_philo *philo, t_print_code code)
 * @brief Thread/Process-safe wrapper for printing simulation events.
 * @details Locks the `print` semaphore to ensure messages don't overlap. 
 * Crucial Logic: If the message is `PHILO_DEAD`, the semaphore is NOT 
 * unlocked. This creates a "death grip" that prevents any other process from 
 * printing subsequent messages, ensuring the death log is the final output.
 * @param philo Pointer to the philosopher.
 * @param code  Message code.
 */
void	safe_print(t_philo *philo, t_print_code code)
{
	safe_sem(philo->data, philo->data->print, WAIT);
	print_message(philo, code);
	if (code != PHILO_DEAD)
		safe_sem(philo->data, philo->data->print, POST);
}

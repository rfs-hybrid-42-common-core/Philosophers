/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:13:07 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:38:15 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn void exit_error(t_data *data, t_print_code code)
 * @brief Handles fatal errors by printing a message and cleaning up locally.
 * @details Closes open semaphore descriptors and frees memory before exiting. 
 * DOES NOT unlink semaphores, ensuring other processes remain unaffected. 
 * Guards against recursion during semaphore closing errors.
 * @param data Pointer to data structure.
 * @param code Error code.
 */
void	exit_error(t_data *data, t_print_code code)
{
	print_message(NULL, code);
	if (data->philos)
	{
		if (code != SEM_CLOSE && code != SEM_UNLINK)
			close_semaphores(data);
		free(data->philos);
	}
	exit(EXIT_FAILURE);
}

/**
 * @fn long ft_atol(char *str)
 * @brief Converts a string to a long integer.
 * @details Robust conversion that handles spacing, signs and letters.
 * @param str The string to convert.
 * @return    The converted value, or -1 if invalid/overflow.
 */
long	ft_atol(char *str)
{
	long	res;
	long	prev;

	res = 0;
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '-')
		return (-1);
	if (*str == '+')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		prev = res;
		res = res * 10 + (*str++ - '0');
		if (res < prev)
			return (-1);
	}
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str)
		return (-1);
	return (res);
}

/**
 * @fn int64_t ft_gettimeofday_ms(void)
 * @brief Gets the current time of day in milliseconds.
 * @details Uses the `gettimeofday` system call to fetch the current epoch 
 * time, converting both the seconds and microseconds into a single 64-bit 
 * integer representing total milliseconds.
 * @return Current timestamp in ms, or -1 on error.
 */
int64_t	ft_gettimeofday_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return ((int64_t)tv.tv_sec * 1000LL + (int64_t)tv.tv_usec / 1000LL);
}

/**
 * @fn int ft_msleep(long msec)
 * @brief Precise sleep function.
 * @details Sleeps for the specified duration in milliseconds. Loops with short 
 * sleeps for precision. Note: In the bonus, we do not need to check an 
 * `is_over` flag here because the parent process uses `SIGKILL` to interrupt 
 * sleeps.
 * @param msec Duration to sleep in milliseconds.
 * @return     0 on success.
 */
int	ft_msleep(long msec)
{
	int64_t	start;
	int64_t	now;

	start = ft_gettimeofday_ms();
	if (start == -1)
		return (usleep(msec * 1000));
	now = start;
	while (now - start < (int64_t)msec)
	{
		usleep(500);
		now = ft_gettimeofday_ms();
	}
	return (0);
}

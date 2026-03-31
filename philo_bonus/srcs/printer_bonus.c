/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 14:04:47 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 02:34:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn static void thread_sem_message(t_print_code code)
 * @brief Prints error messages specific to thread or semaphore failures.
 * @details Centralizes all threading and semaphore-related error prints. It is 
 * called by `error_message` when the print code falls within the concurrency 
 * error range, keeping the main error function clean.
 * @param code The error code representing the specific failure type.
 */
static void	thread_sem_message(t_print_code code)
{
	if (code == TH_CREATE)
		printf("║                 FAILED TO CREATE THREAD                 ║\n");
	else if (code == TH_DETACH)
		printf("║                 FAILED TO DETACH THREAD                 ║\n");
	else if (code == TH_JOIN)
		printf("║                  FAILED TO JOIN THREAD                  ║\n");
	else if (code == TH_INVALID)
		printf("║                 INVALID THREAD OPERATION                ║\n");
	else if (code == SEM_OPEN)
		printf("║                 FAILED TO OPEN SEMAPHORE                ║\n");
	else if (code == SEM_CLOSE)
		printf("║                FAILED TO CLOSE SEMAPHORE                ║\n");
	else if (code == SEM_POST)
		printf("║                 FAILED TO POST SEMAPHORE                ║\n");
	else if (code == SEM_WAIT)
		printf("║                 FAILED TO WAIT SEMAPHORE                ║\n");
	else if (code == SEM_UNLINK)
		printf("║                FAILED TO UNLINK SEMAPHORE               ║\n");
	else
		printf("║               INVALID SEMAPHORE OPERATION               ║\n");
}

/**
 * @fn static void error_message(t_print_code code)
 * @brief Main error reporting function.
 * @details Prints a formatted error box based on the error code provided.
 * Also prints usage examples if the error is related to arguments.
 * @param code The error code.
 */
static void	error_message(t_print_code code)
{
	printf(BOLD_ON FG_RED BLINK_ON);
	printf("\n╔═════════════════════════════════════════════════════════╗\n");
	if (code == NUM_ARGS)
		printf("║                WRONG NUMBER OF ARGUMENTS                ║\n");
	else if (code == POS_ARGS)
		printf("║         ALL ARGUMENTS MUST BE POSITIVE INTEGERS         ║\n");
	else if (code == MALLOC)
		printf("║                 MEMORY ALLOCATION FAILED                ║\n");
	else if (code == FORK)
		printf("║                   FORK PROCESS FAILED                   ║\n");
	else if (code == SLEEP)
		printf("║                   SLEEP ERROR DETECTED                  ║\n");
	else if (code >= TH_CREATE && code <= SEM_INVALID)
		thread_sem_message(code);
	printf("╚═════════════════════════════════════════════════════════╝\n");
	printf(RESET);
	if (code != NUM_ARGS && code != POS_ARGS)
		return ;
	printf(BOLD_ON FG_GREEN "\nUsage:" BOLD_OFF "\t./philo " ITALIC_ON
		"number_of_philosophers time_to_die time_to_eat time_to_sleep "
		ITALIC_OFF "[" ITALIC_ON "number_of_times_each_philosopher_must_eat"
		ITALIC_OFF "]\n");
	printf(BOLD_ON FG_YELLOW"\nExamples:\n" BOLD_OFF "\t./philo 5 800 200 200\n"
		"\t./philo 5 800 200 200 7\n\n" RESET);
}

/**
 * @fn static void philo_message(t_print_code code, size_t id)
 * @brief Prints a state change message for a philosopher.
 * @details Handles formatting for Taken Fork, Eating, Sleeping, Thinking, and 
 * Died.
 * @param code The state code.
 * @param id   The ID of the philosopher.
 */
static void	philo_message(t_print_code code, size_t id)
{
	if (code == PHILO_FORK)
		printf("%zu has taken a fork" RESET "\n", id);
	else if (code == PHILO_EAT)
		printf("%zu is eating" RESET "\n", id);
	else if (code == PHILO_SLEEP)
		printf("%zu is sleeping" RESET "\n", id);
	else if (code == PHILO_THINK)
		printf("%zu is thinking" RESET "\n", id);
	else
		printf("%zu died" RESET "\n", id);
}

/**
 * @fn void print_message(t_philo *philo, t_print_code code)
 * @brief Thread-safe function to print simulation messages.
 * @details Checks the error code range to delegate to `error_message` if 
 * necessary. Otherwise, calculates the timestamp and prints the philosopher's 
 * state with color coding.
 * @param philo Pointer to the philosopher (or NULL for generic errors).
 * @param code  The type of message to print.
 */
void	print_message(t_philo *philo, t_print_code code)
{
	static const char	*colors[] = {
		FG_WHITE,
		FG_GREEN,
		FG_YELLOW,
		FG_BLUE,
		FG_MAGENTA,
		FG_CYAN
	};
	const size_t		color_count = sizeof(colors) / sizeof(colors[0]);
	const char			*text_color;

	if (code >= NUM_ARGS && code <= SEM_INVALID)
	{
		error_message(code);
		return ;
	}
	else if (code == PHILO_DEAD)
		text_color = FG_RED;
	else
		text_color = colors[philo->philo_id % color_count];
	printf(BOLD_ON "%s[%ld]\t" BOLD_OFF, text_color,
		ft_gettimeofday_ms() - philo->data->start_time);
	philo_message(code, philo->philo_id);
}

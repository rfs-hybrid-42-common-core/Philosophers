/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 00:18:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:09:47 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * @fn static void mutexes_init(t_data *data)
 * @brief Initializes all mutexes used in the simulation.
 * @details Initializes global mutexes (print, status, ready) and individual 
 * mutexes for forks and philosopher meal data.
 * @param data Pointer to the main data structure.
 */
static void	mutexes_init(t_data *data)
{
	long	i;

	safe_mutex(data, &data->print_mtx, INIT, 0);
	safe_mutex(data, &data->status_mtx, INIT, 0);
	safe_mutex(data, &data->ready_mtx, INIT, 0);
	i = -1;
	while (++i < data->total_philos)
	{
		if (pthread_mutex_init(&data->forks_mtx[i], NULL) != 0)
			exit_error(data, MTX_INIT, i);
		if (pthread_mutex_init(&data->philos[i].meal_mtx, NULL) != 0)
		{
			pthread_mutex_destroy(&data->forks_mtx[i]);
			exit_error(data, MTX_INIT, i);
		}
	}
}

/**
 * @fn static void philo_data(t_data *data, t_philo *philo, long index,
 * long total)
 * @brief Sets initial values for a single philosopher.
 * @details Assigns ID, meal counters, and determines the left/right fork 
 * indices.
 * @param data  Pointer to the shared data structure.
 * @param philo Pointer to the specific philosopher structure to initialize.
 * @param index The index of the philosopher (0 to total-1).
 * @param total Total number of philosophers (used for fork logic).
 */
static void	philo_data(t_data *data, t_philo *philo, long index, long total)
{
	memset(philo, 0, sizeof(t_philo));
	philo->philo_id = index + 1;
	philo->meals_eaten = 0;
	philo->last_meal = 0;
	philo->fork_a = index;
	philo->fork_b = (index + 1) % total;
	philo->data = data;
}

/**
 * @fn static void parse_args(t_data *data, int argc, char **argv)
 * @brief Validates and parses command-line arguments in a single pass.
 * @details Ensures the correct number of arguments are provided and that 
 * all arguments are positive integers. Populates the main data structure 
 * with these values and initializes the base simulation state.
 * @param data Pointer to the shared data structure.
 * @param argc Argument count from the command line.
 * @param argv Argument vector from the command line.
 */
static void	parse_args(t_data *data, int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		print_message(NULL, NUM_ARGS);
		exit(EXIT_FAILURE);
	}
	data->total_philos = ft_atol(argv[1]);
	data->time_to_die = ft_atol(argv[2]);
	data->time_to_eat = ft_atol(argv[3]);
	data->time_to_sleep = ft_atol(argv[4]);
	data->total_meals = -1;
	if (argc == 6)
		data->total_meals = ft_atol(argv[5]);
	if (data->total_philos <= 0 || data->time_to_die <= 0
		|| data->time_to_eat <= 0 || data->time_to_sleep <= 0
		|| (argc == 6 && data->total_meals <= 0))
	{
		print_message(NULL, POS_ARGS);
		exit(EXIT_FAILURE);
	}
	data->philos_ready = 0;
	data->start_time = 0;
	data->is_over = false;
	data->philos = NULL;
	data->forks_mtx = NULL;
}

/**
 * @fn void init(t_data *data, int argc, char **argv)
 * @brief Parses arguments and initializes the main data structure.
 * @details Performs parsing of arguments, allocates memory for philosophers 
 * and mutexes, and calls sub-initialization functions. Handles memory 
 * allocation errors.
 * @param data Pointer to the main data structure.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
void	init(t_data *data, int argc, char **argv)
{
	long	i;

	parse_args(data, argc, argv);
	data->philos = safe_malloc(data, sizeof(t_philo) * data->total_philos);
	data->forks_mtx = safe_malloc(data, sizeof(t_mtx) * data->total_philos);
	i = -1;
	while (++i < data->total_philos)
		philo_data(data, &data->philos[i], i, data->total_philos);
	mutexes_init(data);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 00:18:20 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 14:21:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn static int number_of_digits(int num)
 * @brief Helper function to count the digits in an integer.
 * @details Used to calculate exactly how much memory to allocate for the 
 * semaphore name string when appending the philosopher's ID.
 * @param num The number to count.
 * @return    The number of digits.
 */
static int	number_of_digits(int num)
{
	int	count;

	count = 1;
	while (num >= 10)
	{
		count++;
		num /= 10;
	}
	return (count);
}

/**
 * @fn static char *generate_meal_sem_name(t_data *data, long index)
 * @brief Generates a unique name for a philosopher's meal semaphore.
 * @details Concatenates the base name defined in `SEM_MEAL` with the 
 * philosopher's specific index (e.g., "/philo_meal_1").
 * This is crucial because `sem_open` requires unique names to create 
 * distinct semaphores for each process.
 * @param data  Pointer to shared data (used for error handling).
 * @param index The philosopher's index (0 to total_philos - 1).
 * @return      A dynamically allocated string containing the unique name.
 */
static char	*generate_meal_sem_name(t_data *data, long index)
{
	char	*name;
	long	tmp;
	int		name_len;
	int		index_len;

	name_len = -1;
	while (SEM_MEAL[++name_len])
		;
	index_len = number_of_digits(index);
	name = safe_malloc(data, sizeof(char) * (name_len + index_len + 1));
	if (!name)
		return (NULL);
	name[name_len + index_len] = '\0';
	tmp = -1;
	while (++tmp < name_len)
		name[tmp] = SEM_MEAL[tmp];
	while (index_len-- > 0)
	{
		name[name_len + index_len] = (index % 10) + '0';
		index /= 10;
	}
	return (name);
}

/**
 * @fn static void philo_data(t_data *data, t_philo *philo, long index)
 * @brief Initializes individual philosopher data.
 * @details Generates a unique named semaphore (e.g., "/philo_meal_1") 
 * to protect this specific philosopher's `last_meal` variable.
 * Unlinks the name immediately after opening so it cleans up on exit.
 * @param data  Pointer to shared data.
 * @param philo Pointer to the philosopher to init.
 * @param index The philosopher's index.
 */
static void	philo_data(t_data *data, t_philo *philo, long index)
{
	char	*name;

	memset(philo, 0, sizeof(t_philo));
	philo->philo_id = index + 1;
	philo->meals_eaten = 0;
	philo->last_meal = 0;
	philo->data = data;
	name = generate_meal_sem_name(data, index);
	if (!name)
		exit_error(data, MALLOC);
	sem_unlink(name);
	philo->meal = sem_open(name, O_CREAT, 0644, 1);
	if (philo->meal == SEM_FAILED)
	{
		free(name);
		exit_error(data, SEM_OPEN);
	}
	if (sem_unlink(name) != 0)
	{
		free(name);
		exit_error(data, SEM_UNLINK);
	}
	free(name);
}

/**
 * @fn static void parse_args(t_data *data, int argc, char **argv)
 * @brief Validates and parses command-line arguments in a single pass.
 * @details Ensures the correct number of arguments are provided and that 
 * all arguments are positive integers. Populates the main data structure 
 * with these values and initializes the base simulation state, including 
 * safely setting semaphores to SEM_FAILED.
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
	data->start_time = 0;
	data->print = SEM_FAILED;
	data->full = SEM_FAILED;
	data->ready = SEM_FAILED;
	data->waiter = SEM_FAILED;
	data->forks = SEM_FAILED;
}

/**
 * @fn void init(t_data *data, int argc, char **argv)
 * @brief Parses arguments and initializes the main data structure.
 * @details Performs parsing of arguments, allocates and zeroes memory for 
 * philosophers, initializes individual philosopher data, and opens the 
 * global semaphores used for the simulation.
 * @param data Pointer to the main data structure.
 * @param argc Argument count.
 * @param argv Argument vector.
 */
void	init(t_data *data, int argc, char **argv)
{
	long	i;

	parse_args(data, argc, argv);
	data->philos = safe_malloc(data, sizeof(t_philo) * data->total_philos);
	memset(data->philos, 0, sizeof(t_philo) * data->total_philos);
	i = -1;
	while (++i < data->total_philos)
		philo_data(data, &data->philos[i], i);
	sem_unlink(SEM_PRINT);
	sem_unlink(SEM_FULL);
	sem_unlink(SEM_READY);
	sem_unlink(SEM_WAITER);
	sem_unlink(SEM_FORKS);
	data->print = sem_open(SEM_PRINT, O_CREAT, 0644, 1);
	data->full = sem_open(SEM_FULL, O_CREAT, 0644, 0);
	data->ready = sem_open(SEM_READY, O_CREAT, 0644, 0);
	data->waiter = sem_open(SEM_WAITER, O_CREAT, 0644, data->total_philos - 1);
	data->forks = sem_open(SEM_FORKS, O_CREAT, 0644, data->total_philos);
	if (data->print == SEM_FAILED || data->full == SEM_FAILED
		|| data->ready == SEM_FAILED || data->waiter == SEM_FAILED
		|| data->forks == SEM_FAILED)
		exit_error(data, SEM_OPEN);
}

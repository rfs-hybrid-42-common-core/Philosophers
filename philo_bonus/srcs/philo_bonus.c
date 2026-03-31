/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 01:04:33 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/31 01:47:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

/**
 * @fn int main(int argc, char **argv)
 * @brief Main entry point of the Philosophers Bonus program.
 * @details Orchestrates the flow: initializes data/semaphores and parses 
 * arguments, hands control to the simulation module, and performs cleanup.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return     EXIT_SUCCESS on success, EXIT_FAILURE on error.
 */
int	main(int argc, char **argv)
{
	t_data	data;

	init(&data, argc, argv);
	simulation(&data);
	cleanup(&data);
	return (EXIT_SUCCESS);
}

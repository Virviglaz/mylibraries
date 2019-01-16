#ifndef NEURON_H
#define NEURON_H

#include <stdint.h>

typedef double f_t;
typedef uint16_t u_t;

typedef struct
{
	u_t layer, num, in_count;
	f_t ** in;
	f_t * w, * out;
}neuron_t;

typedef struct
{
	neuron_t ** neurons;
	u_t neurons_count;
}layer_t;

typedef struct
{
	u_t layer_count, last_layer_neurons_count;
	u_t * neurons_per_layer;
}net_params_t;

typedef struct
{
	layer_t ** layers;
	u_t layers_count;
	net_params_t params;
}net_t;

typedef struct
{
	unsigned int * mem_used;
	net_t * (*create_net)(u_t * neurons_per_layer, u_t layer_count, f_t * input, u_t input_count);
	void (*release_net)(net_t * net);
	f_t * (*get_result)(net_t * net);
	f_t (*weight_get_func)(u_t layer, u_t num);
	f_t (*result_calc_func)(f_t input);
	void (*update_weights)(void);
}neutron_network_t;

neutron_network_t * get_network(void);
void set_network(neutron_network_t * network);

#endif //NEURON_H

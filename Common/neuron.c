#include "neuron.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

static f_t rand_f (u_t layer, u_t neuron, u_t input)
{
	return (f_t)rand() / RAND_MAX - 0.5;
}

static f_t simple_trigger_func(f_t in)
{
#include <math.h>
	return 1 / (1 + pow(2.718, -in));
}

static net_t * _net_in_use;
static neutron_network_t * _network;
static size_t mem_used = 0;

static void * _malloc(size_t size)
{
	mem_used += size;
	return malloc(size);
}

static f_t * get_weights(u_t layer, u_t num, u_t len)
{
	f_t * weights = _malloc(len * sizeof(f_t));

	for (u_t i = 0; i != len; i++)
	{
		weights[i] = _network->weight_get_func(layer, num, i);
		//printf("Weight L=%5.1u%5.1u%5.1u: %f\n", layer, num, i, weights[i]);
	}

	return weights;
}

static neuron_t * create_neuron(u_t layer_num, u_t neuron_num, u_t input_count)
{
	neuron_t * neuron = _malloc(sizeof(neuron_t));

	neuron->in_count = input_count;
	neuron->layer = layer_num;
	neuron->num = neuron_num;
	neuron->w = get_weights(layer_num, neuron_num, input_count);
	neuron->in = _malloc(input_count * sizeof(f_t*));

	if (layer_num != _net_in_use->params.layer_count - 1)
		neuron->out = _malloc(sizeof(f_t));

	return neuron;
}

static layer_t * create_layer(u_t layer_num, u_t neuron_count, u_t input_count)
{
	layer_t * layer = _malloc(sizeof(layer_t));

	layer->neurons = _malloc(neuron_count * sizeof(neuron_t*));

	for (u_t i = 0; i != neuron_count; i++)
		layer->neurons[i] = create_neuron(layer_num, i, input_count);

	layer->neurons_count = neuron_count;
	return layer;
}

static void interconnect_layers(layer_t * src_layer, layer_t * dest_layer)
{
	for (u_t i = 0; i != dest_layer->neurons_count; i++)
	{
		neuron_t * neuron = dest_layer->neurons[i];

		if (src_layer->neurons_count != neuron->in_count)
		{
			printf("Error! Source layer out count = %u, dest neuron input count = %u\n", src_layer->neurons_count, neuron->in_count);
			return;
		}

		for (u_t j = 0; j != src_layer->neurons_count; j++)
			neuron->in[j] = src_layer->neurons[j]->out;
	}
}

static net_t * create_net(u_t * neurons_per_layer, u_t layer_count, f_t * input, u_t input_count)
{
	net_t * net = _malloc(sizeof(net_t));

	mem_used = 0;
	_net_in_use = net;

	net->layers = _malloc(layer_count * sizeof(layer_t*));

	net->layers_count = layer_count;

	net->params.layer_count = layer_count;
	net->params.last_layer_neurons_count = neurons_per_layer[layer_count - 1];
	net->params.neurons_per_layer = neurons_per_layer;

	for (u_t i = 0; i != layer_count; i++)
		net->layers[i] = create_layer(i, neurons_per_layer[i], i == 0 ? input_count : neurons_per_layer[i]);

	/* First layer assign inputs */
	for (u_t i = 0; i != neurons_per_layer[0]; i++)
		for (u_t j = 0; j != input_count; j++)
			net->layers[0]->neurons[i]->in[j] = &input[j];

	/* Other layer assign inputs */
	for (u_t i = 1; i != layer_count; i++)
		interconnect_layers(net->layers[i - 1], net->layers[i]);

	return net;
}

static void release_net(net_t * net)
{
	for (u_t i = 0; i != net->layers_count; i++)
	{
		layer_t * layer = net->layers[i];
		
		for (u_t j = 0; j != layer->neurons_count; j++)
		{
			neuron_t * neuron = layer->neurons[j];

			free(neuron->w);
			free(neuron->in);
			if (i != _net_in_use->params.layer_count - 1)
				free(neuron->out);
			free(neuron);
		}
		free(layer);
	}
	free(net);

	_net_in_use = 0;
}

static void update_weights(void)
{
	for (u_t i = 0; i != _net_in_use->layers_count; i++)
		for (u_t j = 0; j != _net_in_use->layers[i]->neurons_count; j++)
			for (u_t k = 0; k != _net_in_use->layers[i]->neurons[j]->in_count; k++)
				_net_in_use->layers[i]->neurons[j]->w[k] = _network->weight_get_func(i, j, k);
}

static void proccess_one_neuron(neuron_t * neuron)
{
	f_t result = 0;
	for (u_t i = 0; i != neuron->in_count; i++)
		result += *neuron->in[i] * neuron->w[i];

	*neuron->out = _network->trigger_func(result);
}

static f_t * get_result(net_t * net)
{
	f_t * result = _malloc(_net_in_use->params.last_layer_neurons_count * sizeof(f_t));

	if (net)
		_net_in_use = net;

	/* Last layer assign outputs */
	for (u_t i = 0; i != _net_in_use->params.last_layer_neurons_count; i++)
		_net_in_use->layers[_net_in_use->params.layer_count - 1]->neurons[i]->out = &result[i];

	for (u_t i = 0; i != _net_in_use->layers_count; i++)
		for (u_t j = 0; j != _net_in_use->params.neurons_per_layer[i]; j++)
		{
			proccess_one_neuron(_net_in_use->layers[i]->neurons[j]);
			//printf("Neuron L=%3.1u%3.1u: %f\n", i, j, *_net_in_use->layers[i]->neurons[j]->out);
		}

	return result;
}

neutron_network_t * get_network(void)
{
	const static neutron_network_t network = {
		.create_net = create_net,
		.release_net = release_net,
		.get_result = get_result,
		.update_weights = update_weights,
		.trigger_func = simple_trigger_func,
		.mem_used = &mem_used,
		.weight_get_func = rand_f,
	};

	_network = (void*)&network;

	return _network;
}


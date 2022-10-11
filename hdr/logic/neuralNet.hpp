#pragma once

//Dependencies
#include "../resources.hpp"
#include "player.hpp"

namespace engine {
	extern const gs::Vec2f weightRange; 
	extern const gs::Vec2f biasRange; 

	struct Neuron {
		std::vector<float> weights; 
		// Stored value after a predition to be used for rendering. 
		float value; 

		Neuron(); 
	};
	struct Layer {
		std::vector<Neuron> neurons; 
		std::vector<float> biases;

		Layer(); 
	};

	class NeuralNet {
	public:
		// This neural-net has 3 layers. An input, hidden, and output layer. 
		Layer layers[3];

		NeuralNet();

		// Generates the neurons for the adjustable layers of the network. 
		void createNetwork(int numOfInputs, int numOfHiddenNeurons); 
		// Sets all of the weights and biases to random values within their 
		// respective ranges. 
		void randomize(); 
		// Slightly changes some of the weights and biases. Note: This function 
		// acts more like a mutation function. 
		void evolve(); 
		// Generates a prediction based on the given inputs.
		float predict(const std::vector<float>& inputs); 

		// Constrains values to a certain range. Note: This activation function
		// is a sigmoid function. 
		static float activation(float value); 
		static float randRange(gs::Vec2f bounds); 
		// Generates value between 2 bounds. Used for generating weights and 
		// biases. 
		static float randRange(float lowerBound, float upperBound); 
	private:
		// Performs dot product on 2 vectors. 
		static float dot(const std::vector<float>& v1, const std::vector<float>& v2); 
	};

	extern std::vector<std::pair<Player, NeuralNet>> npcs; 
	// Keeps track of the npc that has made it the furthest. 
	extern int bestNpcIndex; 
	extern int prvsBestNpcIndex; 
	extern bool endGenerationEarly; 

	// Calculates the last xpos that a player is safe on. 
	gs::Vec2f calculateLastSafePosition(const Player& player); 
	bool calculateJumpDeathStatus(const Player& player); 
	int calculateFlyingVerticalObsticleHeight(gs::Vec2f safePosition); 
	std::pair<Player, NeuralNet>* getFarthestNpc();

	void initNpcs(); 
	void spawnNpcs(); 
	// Uses best npc to mutate all other npcs. 
	void evolveNpcs();
	void updateNpcs();
	void resetSimulation(); 
	void prepareMap(int mapIndex); 
}
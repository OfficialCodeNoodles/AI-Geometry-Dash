#include "../../hdr/logic/neuralNet.hpp"
#include "../../hdr/logic/collision.hpp"
#include "../../hdr/logic/map.hpp"

namespace engine {
	const gs::Vec2f weightRange = gs::Vec2f(-1.0f, 1.0f);
	const gs::Vec2f biasRange = gs::Vec2f(-1.0f, 1.0f);

	Neuron::Neuron() : value(0.0f) {
	}
	Layer::Layer() {
	}

	NeuralNet::NeuralNet() {
	}

	void NeuralNet::createNetwork(int numOfInputs, int numOfHiddenNeurons) {
		// Returns the number of neurons in each layer of the neural-net. 
		auto getNeuronCount = [&](int layer) -> int {
			return layer == 0 ? numOfInputs : (layer == 1 ? numOfHiddenNeurons
				: 1);
		};

		for (int layerIndex = 0; layerIndex < 3; layerIndex++) {
			Layer& layer = layers[layerIndex];
			Neuron newNeuron;

			// Each neuron gets weights corresponding to the number of neurons
			// in the next layer. 
			newNeuron.weights = std::vector<float>(getNeuronCount(layerIndex + 1));

			layer.neurons = std::vector<Neuron>(
				getNeuronCount(layerIndex), newNeuron
			);
			// Each layer has biases for the neurons in the next layer. 
			layer.biases = std::vector<float>(getNeuronCount(layerIndex + 1));
		}
	}
	void NeuralNet::randomize() {
		for (Layer& layer : layers) {
			for (Neuron& neuron : layer.neurons) {
				for (float& weight : neuron.weights)
					weight = randRange(weightRange.x, weightRange.y);
			}

			for (float& bias : layer.biases)
				bias = randRange(biasRange.x, biasRange.y);
		}
	}
	void NeuralNet::evolve() {
		// How far the evolution can mutate values. Note: The higher the 
		// quotient the slower the evolution. 
		const float evolutionQuotient = 3.0f;
		// How often values are evolved. Note: The higher the rate the less 
		// often the values are mutated. 
		const int evolutionRate = 4;

		for (Layer& layer : layers) {
			for (Neuron& neuron : layer.neurons) {
				for (float& weight : neuron.weights) {
					if (rand() % evolutionRate == 0) {
						weight += randRange(
							weightRange.x / evolutionQuotient,
							weightRange.y / evolutionQuotient
						);
						// Constrains all wwights to a certain range. 
						gs::util::clamp(&weight, weightRange.x, weightRange.y);
					}
				}
			}

			for (float& bias : layer.biases) {
				if (rand() % evolutionRate == 0) {
					bias += randRange(
						biasRange.x / evolutionQuotient,
						biasRange.y / evolutionQuotient
					);
					// Constrains all biases to a certain range. 
					gs::util::clamp(&bias, biasRange.x, biasRange.y);
				}
			}
		}
	}
	float NeuralNet::predict(const std::vector<float>& inputs) {
		float prediction = 0.0f;

		try {
			const int hiddenLayerNeuronCount = layers[1].neurons.size();
			std::vector<float> neuronValues(hiddenLayerNeuronCount);

			// Sets the values of the input neurons to the given inputs. 
			for (int inputIndex = 0; inputIndex < inputs.size(); inputIndex++) {
				float inputValue = inputs[inputIndex];

				if (inputIndex == 0)
					inputValue /= 10.0f;

				layers[0].neurons[inputIndex].value = activation(inputValue);
			}

			for (int hiddenNeuronIndex = 0; hiddenNeuronIndex <
				hiddenLayerNeuronCount; hiddenNeuronIndex++
			) {
				// Value of current hidden layer neuron. 
				float& neuronValue = neuronValues[hiddenNeuronIndex];
				// Weights of the current hidden layer. 
				std::vector<float> weights;

				for (const Neuron& neuron : layers[0].neurons)
					weights.push_back(neuron.weights[hiddenNeuronIndex]);

				// Performs the base dot product operation on the inputs and
				// their corresponding weights. 
				neuronValue = dot(inputs, weights);
				// Adds bias to value. 
				neuronValue += layers[0].biases[hiddenNeuronIndex];
				// Normalizes neuron value. 
				neuronValue = activation(neuronValue);
				layers[1].neurons[hiddenNeuronIndex].value = neuronValue;
			}

			// Weights of values for the output layer. 
			std::vector<float> weights;

			for (const Neuron& neuron : layers[1].neurons)
				weights.push_back(neuron.weights[0]);

			prediction = dot(neuronValues, weights);
			prediction += layers[1].biases[0];
			layers[2].neurons[0].value = activation(prediction);
		}
		catch (const std::string& error) {
			std::cout << error << std::endl;
		}

		// Normalizes the final prediction.  
		return activation(prediction);
	}
	float NeuralNet::activation(float value) {
		// Sigmoid function. 
		return 1.0f / (1.0f + std::exp(-value));
	}
	float NeuralNet::randRange(gs::Vec2f bounds) {
		return randRange(bounds.x, bounds.y); 
	}
	float NeuralNet::randRange(float lowerBound, float upperBound) {
		float value = static_cast<float>(rand() % 1000) / 1000.0f;
		value *= upperBound - lowerBound;
		value += lowerBound;
		return value;
	}
	float NeuralNet::dot(
		const std::vector<float>& v1, const std::vector<float>& v2
	) {
		// Cannot compute a dot product on unequally sized vectors. 
		if (v1.size() != v2.size())
			throw "NeuralNet Error: Cannot perform dot product on unequally \
				sized vectors";

		float product = 0.0f;

		for (int componentIndex = 0; componentIndex < v1.size();
			componentIndex++
		)
			product += v1[componentIndex] * v2[componentIndex];

		return product;
	}

	std::vector<std::pair<Player, NeuralNet>> npcs;
	int bestNpcIndex = 0;
	int prvsBestNpcIndex = -1; 
	bool endGenerationEarly = true; 

	gs::Vec2f calculateLastSafePosition(const Player& player) {
		gs::Vec2f safePosition = player.position; 
		// Used to simulate the next steps of an npc. 
		bool initiallyOnGround = player.onGround; 
		Player clone = player;

		clone.isClone = true;

		for (int step = 0; step < 180; step++) {
			const bool onJumpOrb = Tile::getCollisionType(
				currentMap->getTile(gs::Vec2i(
					std::round(player.position.x),
					std::round(player.position.y)
				))
			) == Tile::CollisionType::JumpOrb;

			clone.jump = onJumpOrb && initiallyOnGround; 
			clone.update();

			if (!clone.onGround && !clone.isFlying)
				step--; 

			// Is safe if npc is on the ground or flying while not being hit.  
			if ((clone.onGround || player.isFlying) && clone.respawnTicks == 0)
				safePosition = clone.position;
			else if (clone.respawnTicks != 0)
				break;
		}

		return safePosition;
	}
	bool calculateJumpDeathStatus(const Player& player) {
		Player clone = player;

		clone.isClone = true;

		if (!clone.isFlying && clone.onGround) {
			for (int step = 0; step < 180; step++) {
				clone.jump = true; 
				clone.update();

				if (clone.onGround)
					break;

				if (clone.respawnTicks != 0)
					return true;
			}
		}

		return false; 
	}
	int calculateFlyingVerticalObsticleHeight(gs::Vec2f safePosition) {
		int verticalDistance = 0;
		gs::Vec2i tileBase = gs::Vec2i(
			std::round(safePosition.x) + 1.0f,
			std::round(safePosition.y)
		);

		for (int ypos = tileBase.y; ypos >= 0; ypos--) {
			const Tile::Id currentTile = currentMap->getTile(gs::Vec2i(
				tileBase.x, ypos
			));
			const Tile::CollisionType currentCollisionType = 
				Tile::getCollisionType(currentTile); 

			if (currentCollisionType == Tile::CollisionType::Solid ||
				currentCollisionType == Tile::CollisionType::Spike
			) 
				verticalDistance++; 
		}

		return verticalDistance; 
	}
	std::pair<Player, NeuralNet>* getFarthestNpc() {
		std::pair<Player, NeuralNet>* nearestNpc = nullptr;
		float distance = 0.0f;

		for (auto& pair : npcs) {
			Player& npc = pair.first;

			if (npc.position.x > distance) {
				nearestNpc = &pair;
				distance = npc.position.x;
			}
		}

		return nearestNpc;
	}
	void initNpcs() {
		npcs.clear();

		bestNpcIndex = 0;
		farthestDistance = 0.0f;

		// Generates 19 npcs for all of the other textures. 
		for (int npcIndex = 0; npcIndex < 19; npcIndex++) {
			Player newNpc;

			newNpc.textureIndex = 19 - npcIndex;
			newNpc.primaryHue = 120.0f;
			newNpc.secondaryHue = 180.0f;
			newNpc.isNpc = true;

			npcs.push_back(std::pair<Player, NeuralNet>(newNpc, NeuralNet()));
		}

		for (auto& pair : npcs) {
			NeuralNet& neuralNet = pair.second;

			// Creates a network with 2 inputs and 3 hidden neurons. 
			neuralNet.createNetwork(5, 2);
			neuralNet.randomize();
		}

		spawnNpcs();
	}
	void spawnNpcs() {
		for (int npcIndex = 0; npcIndex < npcs.size(); npcIndex++) {
			std::pair<Player, NeuralNet>& pair = npcs[npcIndex];
			Player& npc = pair.first;

			npc.spawn();
			npc.position.x = npcIndex * 0.5f;
			npc.position.y = currentMap->size.y - 2;
		}
	}
	void evolveNpcs() {
		for (int npcIndex = 0; npcIndex < npcs.size(); npcIndex++) {
			std::pair<Player, NeuralNet>& pair = npcs[npcIndex];
			Player& npc = pair.first;
			NeuralNet& neuralNet = pair.second;

			if (npcIndex != bestNpcIndex) {
				// Copys data from the best player. 
				neuralNet = npcs[bestNpcIndex].second;
				neuralNet.evolve();
			}
		}
	}
	void updateNpcs() {
		// Value needed to be crossed in order to jump. 
		const float jumpingThreshold = 0.5f;

		for (int npcIndex = 0; npcIndex < npcs.size(); npcIndex++) {
			std::pair<Player, NeuralNet>& pair = npcs[npcIndex];
			Player& npc = pair.first;
			NeuralNet& neuralNet = pair.second;

			const gs::Vec2f safePosition = calculateLastSafePosition(npc);
			const bool onJumpOrb = Tile::getCollisionType(
				currentMap->getTile(gs::Vec2i(
					std::round(npc.position.x),
					std::round(npc.position.y)
				))
			) == Tile::CollisionType::JumpOrb;
			const float flyingVerticalObsticleHeight = static_cast<float>(
				calculateFlyingVerticalObsticleHeight(safePosition)
			); 

			float prediction = neuralNet.predict(
				{
					-std::min(safePosition.x - npc.position.x, 8.0f),
					static_cast<float>(npc.isFlying) * 2.0f,
					npc.isFlying ? flyingVerticalObsticleHeight : 0.0f, 
					static_cast<float>(onJumpOrb) * 2.0f,
					static_cast<float>(calculateJumpDeathStatus(npc)) * 1.0f
				}
			);

			// Npc will only jump of the prediction crosses the threshold. 
			if (prediction > jumpingThreshold)
				npc.jump = npc.onGround || npc.isFlying || onJumpOrb;

			npc.update();

			// Finds the farthest npc. 
			if (npc.position.x > farthestDistance) {
				farthestDistance = npc.position.x;
				bestNpcIndex = npcIndex;
			}
		}

		int lastNpcAlive = 0; 
		int npcsAlive = 0;

		for (int pairIndex = 0; pairIndex < npcs.size(); pairIndex++) {
			auto& pair = npcs[pairIndex]; 
			Player& npc = pair.first;

			if (npc.respawnTicks == 0) {
				lastNpcAlive = pairIndex; 
				npcsAlive++;
			}
		}

		// If all npcs are dead then start the next generation. 
		if (npcsAlive == 0 || (endGenerationEarly && npcsAlive == 1 &&
			lastNpcAlive == prvsBestNpcIndex
		)) {
			spawnNpcs();
			evolveNpcs();
			generation++;
			prvsBestNpcIndex = bestNpcIndex; 
		}
	}
	void resetSimulation() {
		initNpcs();
		attempt = 1;
		generation = 1;
	}
	void prepareMap(int mapIndex) {
		currentMap = &maps[mapIndex];
		resetSimulation();
		player.spawn();
	}
}
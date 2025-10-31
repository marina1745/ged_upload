#include <iostream>
#include <random>
#include <time.h>

// Define a macro for easier access to a flattened 2D array
#define IDX(x, y, w) ((x) + (y) * (w))

// Declare functions
void printArray(float* field, uint64_t width, uint64_t height);
void smoothArray(float* field, int64_t width, int64_t height);

int main()
{
	// uint64_t is equivalent to unsigned long long int meaning a unsigned 64bit integer
	uint64_t width = 16, height = 16;

	// Allocate memory on the heap for the 2D array
	float* field = new float[width * height];

	// Seed the static random number generator with the current time
	srand((unsigned) time(nullptr));

	// Iterate over the array to fill it with random values
	// y in the outer loop, x in the inner loop for better cache usage
	for (uint64_t y = 0; y < height; y++)
		for (uint64_t x = 0; x < width; x++)
		{
			// Generate a random value in [0, RAND_MAX] and map it to [0, 1]
			field[IDX(x, y, width)] = rand() / (float) RAND_MAX;
		}

	// Output the unmodified array
	printArray(field, width, height);

	// Run the smoothing filter	
	smoothArray(field, width, height);

	// Output the modified array
	std::cout << std::endl;
	printArray(field, width, height);

	// Free the allocated memory
	delete[] field;

	return EXIT_SUCCESS;
}

void printArray(float* field, uint64_t width, uint64_t height)
{
	for (uint64_t y = 0; y < height; y++)
	{
		for (uint64_t x = 0; x < width; x++)
			std::cout << field[IDX(x, y, width)] << " ";
		
		std::cout << std::endl;
	}
}

void smoothArray(float* field, int64_t width, int64_t height)
{
	// Allocate a temporary new array
	float* tmp_field = new float[width * height];

	for (int64_t y = 0; y < height; y++)
		for (int64_t x = 0; x < width; x++)
		{
			// Start with the value at the current position
			float value = field[IDX(x, y, width)];

			// Add values around the current position, clamped to [0, length - 1]
			value += field[IDX(std::min(x + 1, width - 1),                           y, width)];
			value += field[IDX(std::min(x + 1, width - 1),        std::max(y - 1, 0ll), width)];
			value += field[IDX(                         x,        std::max(y - 1, 0ll), width)];
			value += field[IDX(      std::max(x - 1, 0ll),        std::max(y - 1, 0ll), width)];
			value += field[IDX(      std::max(x - 1, 0ll),                           y, width)];
			value += field[IDX(      std::max(x - 1, 0ll), std::min(y + 1, height - 1), width)];
			value += field[IDX(                         x, std::min(y + 1, height - 1), width)];
			value += field[IDX(std::min(x + 1, width - 1), std::min(y + 1, height - 1), width)];

			// Save the computed value in the temporary array
			tmp_field[IDX(x, y, width)] = value / 9.0f;
		}

	// Copy the values of the temporary array to the array we weant to modify
	for (uint64_t y = 0; y < height; y++)
		for (uint64_t x = 0; x < width; x++)
			field[IDX(x, y, width)] = tmp_field[IDX(x, y, width)];

	// Free the memory of our temporary array
	delete[] tmp_field;
}
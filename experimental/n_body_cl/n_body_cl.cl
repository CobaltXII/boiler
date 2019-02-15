__stringify
(
	__kernel void n_body_cl(float timestep, float softening, int n, __global float4* state1, __global float4* state2)
	{
		// Get the index of the particle to be processed.

		int global_id = get_global_id(0);

		// Get the state of the particle that will be updated in this function.
		// The state holds the position and the velocity of the particle.

		float4 particle = state1[global_id];

		// Create two floating point numbers to hold the accumulated acceleration
		// of this worker's particle.

		float acceleration_x = 0.0f;
		float acceleration_y = 0.0f;

		// Iterate through each particle.

		for (int j = 0; j < n; j++)
		{
			// Get the opposing particle.

			float4 other_particle = state1[j];

			// Find the component distance between this worker's particle and
			// the other particle.

			float dx = other_particle.x - particle.x;
			float dy = other_particle.y - particle.y;

			// Calculate the inverse distance between this worker's particle
			// and the other particle.

			float inverse_distance = rsqrt(dx * dx + dy * dy + softening);

			// Calculate the inverse distance between this worker's particle
			// and the other particle cubed.

			float inverse_distance_3 =
			(
				inverse_distance *
				inverse_distance *
				inverse_distance
			);

			// Accumulate acceleration.

			acceleration_x += dx * inverse_distance_3;
			acceleration_y += dy * inverse_distance_3;
		}

		// Increment the current velocity.

		particle.z += timestep * acceleration_x;
		particle.w += timestep * acceleration_y;

		// Increment the current position.

		particle.x += particle.z;
		particle.y += particle.w;

		// Write the new position to memory.

		state2[global_id] = particle;
	}
);
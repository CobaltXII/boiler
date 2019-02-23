__stringify
(
	__kernel void verlet_fluid(float dt, float bounds, float r, float viscosity, int n, __global float3* old_state, __global float3* this_state, __global float3* next_state, float mouse_x, float mouse_y, int mouse_down)
	{
		// Get the index of the particle to be processed.

		int global_id = get_global_id(0);

		// Get the state of the particle that will be updated in this function.
		// The state holds the position and the velocity of the particle.

		float3 particle = this_state[global_id];

		// Reset density.

		particle.z = 0.0f;

		// Iterate through each particle.

		for (int j = 0; j < n; j++)
		{
			if (global_id == j)
			{
				continue;
			}

			// Get the opposing particle.

			float3 other_particle = this_state[j];

			// Find the component distance between this worker's particle and
			// the other particle.

			float dx = particle.x - other_particle.x;
			float dy = particle.y - other_particle.y;

			// Get the distance squared.

			float d_2 = dx * dx + dy * dy;

			// Check if the two particles intersect.

			if (d_2 < r * r * 4.0f)
			{
				// Get the distance.

				float d = sqrt(d_2);

				// Get the overlap.

				float overlap = (d - r * 2.0f) / viscosity;

				// Displace this worker's particle.

				particle.x -= dx / d * overlap / 2.0f * dt;
				particle.y -= dy / d * overlap / 2.0f * dt;

				// Increase this worker's particle's density.

				particle.z += fabs(overlap);
			}
		}

		// Mouse attraction.

		if (mouse_down != 0)
		{
			float mr = 1024.0f;

			float mdx = particle.x - mouse_x;
			float mdy = particle.y - mouse_y;

			if (mdx * mdx + mdy * mdy < mr * mr)
			{
				float md = sqrt(mdx * mdx + mdy * mdy);

				float overlap = (md - mr * 2.0f) / 512.0f;

				particle.x -= mdx / md * overlap * dt;
				particle.y -= mdy / md * overlap * dt;
			}
		}

		// Do Verlet integration.

		float vx = particle.x - old_state[global_id].x;
		float vy = particle.y - old_state[global_id].y;

		next_state[global_id].x = particle.x + vx;
		next_state[global_id].y = particle.y + vy;

		// Apply gravity.

		next_state[global_id].y += 0.1f * dt;

		// Save old state.

		old_state[global_id].x = particle.x;
		old_state[global_id].y = particle.y;

		// Retain density.

		next_state[global_id].z = particle.z * 2.0f;

		// Do boundary constaints.

		if (next_state[global_id].x < -bounds * 1.5f)
		{
			next_state[global_id].x = -bounds * 1.5f;
		}
		else if (next_state[global_id].x > bounds * 1.5f)
		{
			next_state[global_id].x = bounds * 1.5f;
		}

		if (next_state[global_id].y < -bounds)
		{
			next_state[global_id].y = -bounds;
		}
		else if (next_state[global_id].y > bounds)
		{
			next_state[global_id].y = bounds;
		}
	}
);